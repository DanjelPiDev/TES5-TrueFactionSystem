#include "Managers/DetectionManager.h"
#include "Globals.h"


namespace NPE {
    /**
     * Check if NPCs detect the player based the player's disguise value
     * @param **player** The player actor
     */
    void DetectionManager::CheckNPCDetection(RE::Actor *player) {
        RE::TESObjectCELL *currentCell = player->GetParentCell();
        if (!currentCell) {
            // Player is not in a cell (e.g. in the main menu), return
            return;
        }

        bool playerDetected = false;
        float currentInGameHours = RE::Calendar::GetSingleton()->GetHoursPassed();
        float detectionRadius = DETECTION_RADIUS; // Default is 400.0f

        std::vector<std::future<bool>> detectionFutures;

        // Look for NPCs in the player's detection range
        currentCell->ForEachReferenceInRange(player->GetPosition(), detectionRadius, [&](RE::TESObjectREFR &ref) {
            RE::Actor *npc = skyrim_cast<RE::Actor *>(&ref);
            /*
             * Check if the reference is an actor and not the player
             * The Player also needs to be in the NPC's line of sight and field of view
             */
            if (npc && npc != player) {
                detectionFutures.push_back(std::async(std::launch::async, [&, npc]() {
                    // Iterate through factions to check disguise values
                    for (const auto &[factionName, faction] : GetRelevantFactions()) {
                        float disguiseValue = playerDisguiseStatus.GetDisguiseValue(faction) +
                                              playerDisguiseStatus.GetBonusValue(faction);

                        if (disguiseValue > 0.0f) {
                            this->CheckHoursPassed(npc, player, faction);

                            bool isInLineOfSight = environmentManager.IsInLineOfSight(npc, player);
                            // TODO: Rewrite this to use the field of view check in a more efficient way
                            // bool isInFieldOfView = environmentManager.IsInFieldOfView(npc, player);

                            float distance = std::abs(player->GetPosition().GetDistance(npc->GetPosition()));

                            // Calculate detection probability
                            float detectionProbability = GetDetectionProbability(disguiseValue);
                            detectionProbability =
                                AdjustProbabilityByDistance(detectionProbability, distance, detectionRadius);

                            if (isInLineOfSight && this->NPCRecognizesPlayer(npc, player, faction) ||
                                this->DetectCrimeWhileDisguised(npc, player)) {
                                this->StartCombat(npc, player, faction);
                                recognizedNPCs[npc->GetFormID()] = {npc->GetFormID(), currentInGameHours};
                                // NPC detected the player
                                std::lock_guard<std::mutex> lk(NPE::recognizedNPCsMutex);
                                recognizedNPCs[npc->GetFormID()] = {npc->GetFormID(), currentInGameHours};
                                return true;
                            }
                        }
                    }
                    return false;  // NPC did not detect the player
                }));
            }

            return RE::BSContainer::ForEachResult::kContinue;  // Continue with the next reference
        });

        for (auto &future : detectionFutures) {
            if (future.get()) {
                playerDetected = true;
                break;
            }
        }

        if (playerDetected) {
            return;
        }
    }

    void DetectionManager::TriggerInvestigateLastKnownPosition(RE::Actor *npc, const RE::NiPoint3 &lastKnownPos) {
        if (!npc || npc->IsDead() || npc->IsInCombat()) return;

        static constexpr RE::FormID kXMarkerFormID = 0x0001F66E;
        auto *markerBase = RE::TESForm::LookupByID<RE::TESBoundObject>(kXMarkerFormID);
        if (!markerBase) return;

        auto markerPtr = RE::PlayerCharacter::GetSingleton()->PlaceObjectAtMe(markerBase, false);
        if (!markerPtr) return;

        RE::TESObjectREFR *marker = markerPtr.get();

        marker->MoveTo(RE::PlayerCharacter::GetSingleton());
        marker->SetPosition(lastKnownPos);

        npc->MoveTo(marker);

        marker->Disable();
        marker->IsMarkedForDeletion();
    }

    bool DetectionManager::NPCRecognizesPlayer(RE::Actor *npc, RE::Actor *player, RE::TESFaction *faction) {
        float playerDisguiseValue = playerDisguiseStatus.GetDisguiseValue(faction);
        float distance = abs(npc->GetPosition().GetDistance(player->GetPosition()));

        if (distance > DETECTION_RADIUS) {
            return false;
        }

        float recognitionProbability = (DETECTION_RADIUS - distance) / DETECTION_RADIUS;
        recognitionProbability *= (100.0f - playerDisguiseValue) / 100.0f;

        float distanceFactor = 1.0f / (1.0f + std::exp((distance - DETECTION_RADIUS) * 0.1f));
        recognitionProbability *= distanceFactor;

        int npcLevel = npc->GetLevel();
        int playerLevel = player->GetLevel();
        int levelDifference = abs(npcLevel - playerLevel);

        float levelFactor = 1.0f + 0.02f * std::log(1 + levelDifference);
        recognitionProbability *= levelFactor;

        // Add any environmental modifiers for detection (lighting, sneaking, etc.)
        recognitionProbability += environmentManager.GetEnvironmentalDetectionModifier(player);

        RE::FormID npcID = npc->GetFormID();
        float currentInGameHours = RE::Calendar::GetSingleton()->GetHoursPassed();

        if (recognizedNPCs.find(npcID) != recognizedNPCs.end()) {
            NPCDetectionData &detectionData = recognizedNPCs[npcID];

            float timeSinceLastDetected = currentInGameHours - detectionData.lastDetectedTime;

            if (timeSinceLastDetected < TIME_TO_LOSE_DETECTION) {
                recognitionProbability += 0.25f;
            } else {
                recognizedNPCs.erase(npcID);
            }
        }

        spdlog::info("NPC {} – Dist: {:.1f}, Disguise: {:.1f}, Prob: {:.3f}", npc->GetName(), distance,
                     playerDisguiseValue, recognitionProbability);

        if (recognitionProbability >= 1.0f) {
            return true;
        }

        recognitionProbability = std::clamp(recognitionProbability, 0.0f, 1.0f);

        // Detection check using rng
        if (recognitionProbability >= DETECTION_THRESHOLD) {
            this->TriggerSuspiciousIdle(npc);
            static thread_local std::mt19937 gen(std::random_device{}());
            std::uniform_real_distribution<float> dist(0.0f, 1.0f);
            if (dist(gen) <= recognitionProbability) {
                return true;
            }
        }

        return false;
    }

    void DetectionManager::TriggerSuspiciousIdle(RE::Actor *npc) {
        static RE::FormID idleFormID = 0x000977ED;  // StudyIdle for now
    
        RE::TESIdleForm *idleForm = RE::TESForm::LookupByID<RE::TESIdleForm>(idleFormID);
        if (idleForm && npc) {
            RE::stl::zstring startState = "StudyIdle";
            RE::stl::zstring endState = "StudyIdle";
            npc->PlayAnimation(startState, endState);
        } else {
            spdlog::error("Failed to play animation.");
        }
    }

    bool DetectionManager::DetectCrimeWhileDisguised(RE::Actor* npc, RE::Actor* player) {
        int detectionLevel = player->RequestDetectionLevel(npc);
        bool isLockpicking = player->IsLockpick();
        // bool isStealing = player->PickUpObject();

        if (detectionLevel > 1 && isLockpicking) {
            if (!player->IsSneaking()) {
                return true;
            }
        }
        return false;
    }

    float DetectionManager::GetDetectionProbability(float disguiseValue) { return abs(100.0f - disguiseValue); }

    void DetectionManager::CheckHoursPassed(RE::Actor *npc, RE::Actor *player, RE::TESFaction *faction) {
        if (!npc || !player || !faction) {
            return;
        }

        const RE::FormID npcID = npc->GetFormID();
        const float currentHours = RE::Calendar::GetSingleton()->GetHoursPassed();

        std::lock_guard<std::mutex> lk(NPE::recognizedNPCsMutex);
        auto it = recognizedNPCs.find(npcID);
        if (it == recognizedNPCs.end()) {
            return;
        }

        float lastTime = it->second.lastDetectedTime;
        float elapsed = currentHours - lastTime;

        if (elapsed < 0.0f) {
            elapsed = 0.0f;
        }

        // If it's been long enough, forget detection and restore faction
        if (elapsed >= TIME_TO_LOSE_DETECTION) {
            recognizedNPCs.erase(it);

            float disguiseVal = playerDisguiseStatus.GetDisguiseValue(faction);
            if (disguiseVal > ADD_TO_FACTION_THRESHOLD) {
                player->AddToFaction(faction, 1);
            }
        }
    }

    float DetectionManager::AdjustProbabilityByDistance(float detectionProbability, float distance, float maxDistance) {
        float distanceFactor = 1.0f / (1.0f + std::exp((distance - maxDistance) * 0.1f));
        return detectionProbability * distanceFactor;
    }

    void DetectionManager::StartCombat(RE::Actor *npc, RE::Actor *player, RE::TESFaction *npcFaction) {
        // TODO: Rework
        if (!npc || !player) {
            return;
        }

        if (npcFaction) {
            player->AddToFaction(npcFaction, -1);
            npc->UpdateCombat();
        }
    }

    void DetectionManager::Save(SKSE::SerializationInterface *a_intfc) {
        std::uint32_t count = static_cast<std::uint32_t>(recognizedNPCs.size());
        a_intfc->WriteRecordData(&count, sizeof(count));
        // Write each NPC's FormID and detection struct
        for (auto &[npcID, data] : recognizedNPCs) {
            a_intfc->WriteRecordData(&npcID, sizeof(npcID));
            a_intfc->WriteRecordData(&data, sizeof(data));
        }
    }

    void DetectionManager::Load(SKSE::SerializationInterface *a_intfc) {
        std::uint32_t count;
        if (!a_intfc->ReadRecordData(&count, sizeof(count))) {
            spdlog::warn("DetectionManager::Load failed to read count");
            return;
        }
        recognizedNPCs.clear();
        for (std::uint32_t i = 0; i < count; ++i) {
            RE::FormID npcID;
            NPCDetectionData data;
            if (!a_intfc->ReadRecordData(&npcID, sizeof(npcID)) || !a_intfc->ReadRecordData(&data, sizeof(data))) {
                spdlog::warn("DetectionManager::Load incomplete data at index {}", i);
                return;
            }
            recognizedNPCs[npcID] = data;
        }
    }
}