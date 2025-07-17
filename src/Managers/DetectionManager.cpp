#include "Managers/DetectionManager.h"
#include "Globals.h"


namespace NPE {

    /*
     * This one is used to handle the TESContainerChangedEvent, if the player steals from an NPC (or vice versa? Is that possible? Should look into it).
     * 
     * Probably refactor this into a separate class later, but for now it is fine.
    */
    DetectionManager::ContainerChangedEventHandler DetectionManager::_containerChangedHandler;

    void DetectionManager::RegisterEventHandlers() {
        if (auto evtSrc = RE::ScriptEventSourceHolder::GetSingleton()) {
            evtSrc->AddEventSink<RE::TESContainerChangedEvent>(&_containerChangedHandler);
            spdlog::info("ContainerChangedEventHandler registered in DetectionManager");
        }
    }

    RE::BSEventNotifyControl DetectionManager::ContainerChangedEventHandler::ProcessEvent(
        const RE::TESContainerChangedEvent *evn, RE::BSTEventSource<RE::TESContainerChangedEvent> *) {

        if (!evn) return RE::BSEventNotifyControl::kContinue;
        auto player = RE::PlayerCharacter::GetSingleton();
        if (!player) return RE::BSEventNotifyControl::kContinue;

        RE::TESObjectREFR *refr = nullptr;
        RE::Actor *npc = nullptr;
        // NPC -> Player
        if (evn->newContainer == player->GetFormID()) {
            refr = RE::TESForm::LookupByID<RE::TESObjectREFR>(evn->oldContainer);
        }
        // Player -> NPC
        else if (evn->oldContainer == player->GetFormID()) {
            refr = RE::TESForm::LookupByID<RE::TESObjectREFR>(evn->newContainer);
        }
        if (refr) npc = refr->As<RE::Actor>();
        if (!npc || npc == player) return RE::BSEventNotifyControl::kContinue;

        RE::TESFaction *faction = GetFactionByActor(npc);

        if (faction) {
            SKSE::GetTaskInterface()->AddTask([=] { DetectionManager::GetInstance().StartCombat(npc, player, faction); });
        }
        return RE::BSEventNotifyControl::kContinue;
    }

    /**
     * Check if NPCs detect the player based the player's disguise value
     * @param **player** The player actor
     */
    void DetectionManager::CheckNPCDetection(RE::Actor *player) {
        if (!NPE::MOD_ENABLED) return;

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

                            // Optional line of sight and field of view checks (User configurable)
                            bool useLineOfSight = NPE::USE_LINE_OF_SIGHT_CHECK;
                            bool useFOV = NPE::USE_FOV_CHECK;

                            bool passesLineOfSight = !useLineOfSight || environmentManager.IsInLineOfSight(npc, player);
                            bool passesFOV = !useFOV || environmentManager.IsInFieldOfView(npc, player, NPE::FOV_ANGLE);

                            bool visibilityRequired = useLineOfSight || useFOV;
                            bool passedVisibilityCheck = !visibilityRequired || (passesLineOfSight && passesFOV);

                            float distance = std::abs(player->GetPosition().GetDistance(npc->GetPosition()));

                            // Calculate detection probability
                            float detectionProbability = GetDetectionProbability(disguiseValue);
                            detectionProbability =
                                AdjustProbabilityByDistance(detectionProbability, distance, detectionRadius);
                          
                            if (passedVisibilityCheck && (this->NPCRecognizesPlayer(npc, player, faction) ||
                                                          this->DetectCrimeWhileDisguised(npc, player))) {
                                SKSE::GetTaskInterface()->AddTask([=] { this->StartCombat(npc, player, faction); });
                                // this->StartCombat(npc, player, faction);
                                std::lock_guard<std::mutex> lk(NPE::recognizedNPCsMutex);
                                recognizedNPCs[npc->GetFormID()] = {npc->GetFormID(), currentInGameHours, player->GetPosition()};
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
        if (!NPE::MOD_ENABLED || !npc || npc->IsDead() || npc->IsInCombat()) return;
        // AIPackage: npeInvestigatePlayerPosition: 0x04039821
        // Keyword: npeInvestigate: 0x0403BFBB, 
        // Activator: npeInvestigationActivator: 0x0403BFBC (Has Keyword npeInvestigate)

        constexpr char kPluginName[] = "TrueFactionSystem.esp";
        constexpr RE::FormID kActivatorFormLower = 0x003BFBC;  // Lower 3 Bytes-> CK: 0403BFBC -> 003BFBC
        constexpr RE::FormID kPackageFormLower = 0x0039821;    // Lower 3 Bytes-> CK: 04039821 -> 0039821

        RE::TESDataHandler* dataHandler = RE::TESDataHandler::GetSingleton();
        std::optional<uint8_t> modIndexOpt = dataHandler->GetLoadedModIndex(kPluginName);
        if (!modIndexOpt) {
            spdlog::warn("Mod '{}' not found in Load Order!", kPluginName);
            return;
        }
        std::uint8_t modIndex = *modIndexOpt;

        RE::FormID activatorFormID = (static_cast<RE::FormID>(modIndex) << 24) | kActivatorFormLower;
        RE::FormID packageFormID = (static_cast<RE::FormID>(modIndex) << 24) | kPackageFormLower;

        auto activatorBase = RE::TESForm::LookupByID<RE::TESBoundObject>(activatorFormID);
        if (!activatorBase) {
            spdlog::warn("Investigation-Activator not found (FormID {:08X})", activatorFormID);
            return;
        }

        auto markerPtr = RE::PlayerCharacter::GetSingleton()->PlaceObjectAtMe(activatorBase, false);
        if (!markerPtr) {
            spdlog::warn("Couldn't spawn Investigation-Marker.");
            return;
        }
        RE::TESObjectREFR *investigationMarker = markerPtr.get();
        investigationMarker->SetPosition(lastKnownPos);
        investigationMarker->MoveTo(RE::PlayerCharacter::GetSingleton());

        auto investigatePackage = RE::TESForm::LookupByID<RE::TESPackage>(packageFormID);
        if (!investigatePackage) {
            spdlog::warn("Investigation-Package not found (FormID {:08X})", packageFormID);
            return;
        }

        // Push the package to the actor (Does this really work? I don't know)
        npc->PutCreatedPackage(investigatePackage,
                               true,   // tempPackage: will be removed after completion
                               false,  // createdPackage: false, because it's a static one from CK 0x04039821
                               false   // allowFromFurniture: irrelevant for investigation
        );

        npc->EvaluatePackage(true, true);

        // Disable the marker after the package is assigned and done with it
        // investigationMarker->Disable();
        // investigationMarker->IsMarkedForDeletion();
    }

    bool DetectionManager::NPCRecognizesPlayer(RE::Actor *npc, RE::Actor *player, RE::TESFaction *faction) {
        if (!NPE::MOD_ENABLED) return false;

        float playerDisguiseValue = playerDisguiseStatus.GetDisguiseValue(faction);
        float distance = abs(npc->GetPosition().GetDistance(player->GetPosition()));

        if (distance > NPE::DETECTION_RADIUS) {
            return false;
        }

        float recognitionProbability = (NPE::DETECTION_RADIUS - distance) / NPE::DETECTION_RADIUS;
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

            if (timeSinceLastDetected < NPE::TIME_TO_LOSE_DETECTION) {
                recognitionProbability += 0.25f;
            } else {
                recognizedNPCs.erase(npcID);
            }
        }
        if (recognitionProbability >= 1.0f) {
            return true;
        }

        recognitionProbability = std::clamp(recognitionProbability, 0.0f, 1.0f);

        if (recognitionProbability >= NPE::INVESTIGATION_THRESHOLD) {
            SKSE::GetTaskInterface()->AddTask([=] { this->TriggerInvestigateLastKnownPosition(npc, player->GetPosition()); });
        }
        if (recognitionProbability >= NPE::DETECTION_THRESHOLD) {
            static thread_local std::mt19937 gen(std::random_device{}());
            std::uniform_real_distribution<float> dist(0.0f, 1.0f);
            if (dist(gen) <= recognitionProbability) {
                return true;
            }
        }

        return false;
    }

    bool DetectionManager::DetectCrimeWhileDisguised(RE::Actor* npc, RE::Actor* player) {
        int detectionLevel = player->RequestDetectionLevel(npc);
        bool isLockpicking = player->IsLockpick();
        // Stealing implemented via EventHandler (ContainerChangedEventHandler)

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
        if (elapsed >= NPE::TIME_TO_LOSE_DETECTION) {
            recognizedNPCs.erase(it);

            float disguiseVal = playerDisguiseStatus.GetDisguiseValue(faction);
            if (disguiseVal > NPE::ADD_TO_FACTION_THRESHOLD) {
                // No modded factions, so we can safely add the player to the faction (Mod support later on!)
                if (ALLOWED_FACTIONS.count(faction->GetFormID()) && ALLOWED_FACTIONS[faction->GetFormID()]) {
                    player->AddToFaction(faction, 1);
                }
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
        if (ALLOWED_FACTIONS.count(npcFaction->GetFormID()) && !ALLOWED_FACTIONS[npcFaction->GetFormID()]) {
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

    void DetectionManager::UnregisterEventHandlers() {
        if (auto evtSrc = RE::ScriptEventSourceHolder::GetSingleton()) {
            evtSrc->RemoveEventSink<RE::TESContainerChangedEvent>(&_containerChangedHandler);
            spdlog::info("ContainerChangedEventHandler unregistered in DetectionManager");
        } else {
            spdlog::warn("UnregisterEventHandlers: ScriptEventSourceHolder not available.");
        }
    }

}