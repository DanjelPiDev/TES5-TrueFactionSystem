#include "Race.h"
#include "Globals.h"
// Get nlohmann/json from: https://github.com/nlohmann/json
#include "nlohmann/json.hpp"



namespace NPE {
    const float raceFactionBonus = 0.0f;

    const std::string raceFactionFilePath = "tfs_definitions/race_faction.json";

    void LoadJsonData() {
        std::string dllPath = GetCurrentDLLPath();
        dllPath = dllPath.substr(0, dllPath.find_last_of("\\/")) + "\\";

        std::ifstream raceFactionFile(dllPath + raceFactionFilePath);

        if (!raceFactionFile.is_open()) {
            spdlog::error("Failed to open one of the JSON files.");
            return;
        }

        nlohmann::json raceFactionJson;

        raceFactionFile >> raceFactionJson;

        // Parse the race-faction correlation data
        for (auto &[faction, raceList] : raceFactionJson["factions"].items()) {
            std::unordered_map<std::string, int> raceMap;
            for (auto &[fitType, races] : raceList.items()) {
                int value = 0;

                if (fitType == "best_fit")
                    value = 20;
                else if (fitType == "possible")
                    value = 10;
                else if (fitType == "unlikely")
                    value = 0;
                else if (fitType == "impossible")
                    value = -80;

                for (auto &race : races) {
                    raceMap[race] = value;
                }
            }
            factionRaceData[faction] = raceMap;
        }

        spdlog::info("Read in JSON data.");
        spdlog::info("factionRaceData size: {}", factionRaceData.size());
    }

    int RaceValueForFaction(const std::string &race, const std::string &faction) {
        // Check if faction exists
        if (factionRaceData.find(faction) == factionRaceData.end()) {
            spdlog::error("Faction not found.");
            return 0;
        }

        // Check if the race exists within that faction's data
        auto &raceMap = factionRaceData[faction];
        if (raceMap.find(race) != raceMap.end()) {
            return raceMap[race];
        } else {
            spdlog::error("Race not found in faction.");
            return 0;
        }
    }

    RE::TESRace *GetPlayerRace() {
        RE::PlayerCharacter *player = RE::PlayerCharacter::GetSingleton();
        if (player) {
            return player->GetRace();
        }
        return nullptr;
    }

    void InitRaceDisguiseBonus() {
        // Load race-faction data from JSON if not already loaded
        if (factionRaceData.empty()) {
            LoadJsonData();
        }

        RE::TESRace *playerRace = GetPlayerRace();
        if (!playerRace) {
            spdlog::error("Player race not found.");
            return;
        }

        std::string raceIdentifier = playerRace->GetFormEditorID();
        if (raceIdentifier.empty()) {
            spdlog::error("Player race does not have a valid EditorID.");
            return;
        }

        for (const auto &[factionTag, factionID] : factionArmorKeywords) {
            RE::TESFaction *faction = RE::TESForm::LookupByID<RE::TESFaction>(factionID);
            if (!faction) {
                continue;
            }
            // Check if the faction exists in the race-faction JSON data
            auto factionIt = factionRaceData.find(factionTag);
            if (factionIt != factionRaceData.end()) {
                const auto &raceMap = factionIt->second;
                // Check if the player's race exists in the faction's race list
                auto raceIt = raceMap.find(raceIdentifier);
                if (raceIt != raceMap.end()) {
                    int raceFactionBonus = raceIt->second;
                    playerDisguiseStatus.SetRaceBonusValue(faction, raceFactionBonus);
                } else {
                    playerDisguiseStatus.SetRaceBonusValue(faction, 0);
                }
            }
        }
    }

    bool IsPlayerInCorrectRace(RE::BSFixedString keyword) {
        RE::TESRace *playerRace = GetPlayerRace();
        RE::BGSKeyword **raceKeywords = playerRace->keywords;
        for (uint32_t i = 0; i < playerRace->numKeywords; i++) {
            if (raceKeywords[i]->GetFormEditorID() == keyword) {
                return true;
            }
        }
        return false;
    }
}