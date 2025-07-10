#include <filesystem>
#include <fstream>

#include "Race.h"
#include "Globals.h"

// Get nlohmann/json from: https://github.com/nlohmann/json
#include "nlohmann/json.hpp"



namespace NPE {
    const float raceFactionBonus = 0.0f;

    const std::string raceFactionFilePath = "tfs_definitions/race_faction.json";

    void LoadJsonData() {
        std::filesystem::path dllPath = GetCurrentDLLPath();
        dllPath = dllPath.parent_path();

        std::filesystem::path filePath = dllPath / "tfs_definitions" / "race_faction.json";

        if (!std::filesystem::exists(filePath)) {
            spdlog::error("JSON file '{}' does not exist", filePath.string());
            return;
        }

        std::ifstream raceFactionFile(filePath);
        if (!raceFactionFile.is_open()) {
            spdlog::error("Failed to open JSON file '{}'", filePath.string());
            return;
        }

        nlohmann::json raceFactionJson;
        try {
            raceFactionFile >> raceFactionJson;
        } catch (const nlohmann::json::parse_error &e) {
            spdlog::error("Failed to parse JSON '{}': {}", filePath.string(), e.what());
            return;
        }

        if (!raceFactionJson.contains("factions") || !raceFactionJson["factions"].is_object()) {
            spdlog::error("JSON '{}' missing or invalid 'factions' object", filePath.string());
            return;
        }

        // Expected categories
        const std::array<std::string, 4> categories = {"best_fit", "possible", "unlikely", "impossible"};
        for (auto &[factionTag, raceList] : raceFactionJson["factions"].items()) {
            if (!raceList.is_object()) {
                spdlog::warn("Skipping faction '{}': entry is not an object", factionTag);
                continue;
            }

            std::unordered_map<std::string, int> raceMap;
            for (auto &category : categories) {
                if (!raceList.contains(category) || !raceList[category].is_array()) {
                    spdlog::warn("Faction '{}' missing or invalid '{}' array", factionTag, category);
                    continue;
                }
                int value = 0;
                if (category == "best_fit")
                    value = 20;
                else if (category == "possible")
                    value = 10;
                else if (category == "unlikely")
                    value = 0;
                else if (category == "impossible")
                    value = -80;

                for (auto &race : raceList[category]) {
                    if (!race.is_string()) {
                        spdlog::warn("Skipping non-string race in faction '{}', category '{}'", factionTag, category);
                        continue;
                    }
                    raceMap[race.get<std::string>()] = value;
                }
            }
            factionRaceData[factionTag] = std::move(raceMap);
        }

        spdlog::info("Loaded JSON data from '{}' ({} factions)", filePath.string(), factionRaceData.size());
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