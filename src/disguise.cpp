#include "Disguise.h"
#include "Globals.h"


namespace NPE {
    float GetDisguiseValueForFaction(RE::TESFaction *faction) { return playerDisguiseStatus.GetDisguiseValue(faction); }

    float GetDisguiseBonusValueForFaction(RE::TESFaction *faction) {
        return playerDisguiseStatus.GetBonusValue(faction);
    }

    float GetRaceBonusValueForFaction(RE::TESFaction *faction) {
        return playerDisguiseStatus.GetRaceBonusValue(faction);
    }

    float GetDetectionProbability(float disguiseValue) { return abs(100.0f - disguiseValue); }

    float AdjustProbabilityByDistance(float detectionProbability, float distance, float maxDistance) {
        float distanceFactor = 1.0f - (distance / maxDistance);  // Scaled by 1 (near) to 0 (far)
        return detectionProbability * distanceFactor;
    }

    void RemovePlayerFromAllFactions(RE::Actor *player) {
        auto factions = GetRelevantFactions();
        for (auto &[factionName, faction] : factions) {
            if (player->IsInFaction(faction)) {
                spdlog::info("Removing player from faction (ALL): {}", GetFactionEditorID(faction).c_str());
                player->AddToFaction(faction, -1);
                spdlog::info("Removed player from faction (ALL): {}", GetFactionEditorID(faction).c_str());
            } else {
                spdlog::info("Player NOT in faction (ALL): {}", GetFactionEditorID(faction).c_str());
            }
        }
    }

    

    void SaveDetectionData(SKSE::SerializationInterface *a_intfc) {
        for (auto &[npcID, detectionData] : recognizedNPCs) {
            a_intfc->WriteRecord('NPCD', 1, &npcID, sizeof(npcID));
            a_intfc->WriteRecordData(&detectionData, sizeof(detectionData));
        }
    }

    void LoadDetectionData(SKSE::SerializationInterface *a_intfc) {
        std::uint32_t type;
        std::uint32_t version;
        std::uint32_t length;

        while (a_intfc->GetNextRecordInfo(type, version, length)) {
            RE::FormID npcID;
            NPCDetectionData detectionData;

            if (a_intfc->ReadRecordData(&npcID, sizeof(npcID))) {
                if (length == sizeof(detectionData)) {
                    if (a_intfc->ReadRecordData(&detectionData, sizeof(detectionData))) {
                        recognizedNPCs[npcID] = detectionData;
                    }
                } else {
                    RE::ConsoleLog::GetSingleton()->Print("Record length mismatch!");
                }
            }
        }
    }
}