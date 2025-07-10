#include "Faction.h"
#include "Globals.h"


namespace NPE {
    // Only allow up to 20 mod added factions. I hope that's enough :D
    const std::vector<std::string> predefinedKeywords = {
        "npeFaction1",  "npeFaction2",  "npeFaction3",  "npeFaction4",  "npeFaction5",  "npeFaction6",  "npeFaction7",
        "npeFaction8",  "npeFaction9",  "npeFaction10", "npeFaction11", "npeFaction12", "npeFaction13", "npeFaction14",
        "npeFaction15", "npeFaction16", "npeFaction17", "npeFaction18", "npeFaction19", "npeFaction20"};

    std::unordered_map<RE::FormID, std::string> assignedKeywordsMap;


    std::vector<std::pair<std::string, RE::TESFaction *>> GetRelevantFactions() {
        std::vector<std::pair<std::string, RE::TESFaction *>> factions;

        RE::TESFaction *banditFaction = RE::TESForm::LookupByID<RE::TESFaction>(0x0001BCC0);
        if (banditFaction) {
            factions.push_back({"BanditFaction", banditFaction});
        }

        RE::TESFaction *imperialFaction = RE::TESForm::LookupByID<RE::TESFaction>(0x0002BF9A);
        if (imperialFaction) {
            factions.push_back({"ImperialFaction", imperialFaction});
        }

        RE::TESFaction *stormcloakFaction = RE::TESForm::LookupByID<RE::TESFaction>(0x00028849);
        if (stormcloakFaction) {
            factions.push_back({"StormcloakFaction", stormcloakFaction});
        }

        RE::TESFaction *bladesFaction = RE::TESForm::LookupByID<RE::TESFaction>(0x00072834);
        if (bladesFaction) {
            factions.push_back({"BladesFaction", bladesFaction});
        }

        RE::TESFaction *companionsFaction = RE::TESForm::LookupByID<RE::TESFaction>(0x00048362);
        if (companionsFaction) {
            factions.push_back({"CompanionsFaction", companionsFaction});
        }

        RE::TESFaction *dawnstarFaction = RE::TESForm::LookupByID<RE::TESFaction>(0x0002816E);
        if (dawnstarFaction) {
            factions.push_back({"DawnstarFaction", dawnstarFaction});
        }

        RE::TESFaction *falkreathFaction = RE::TESForm::LookupByID<RE::TESFaction>(0x00028170);
        if (falkreathFaction) {
            factions.push_back({"FalkreathFaction", falkreathFaction});
        }

        RE::TESFaction *forswornFaction = RE::TESForm::LookupByID<RE::TESFaction>(0x00043599);
        if (forswornFaction) {
            factions.push_back({"ForswornFaction", forswornFaction});
        }

        RE::TESFaction *markarthFaction = RE::TESForm::LookupByID<RE::TESFaction>(0x0002816C);
        if (markarthFaction) {
            factions.push_back({"MarkarthFaction", markarthFaction});
        }

        RE::TESFaction *morthalFaction = RE::TESForm::LookupByID<RE::TESFaction>(0x0002816D);
        if (morthalFaction) {
            factions.push_back({"MorthalFaction", morthalFaction});
        }

        RE::TESFaction *nightingaleFaction = RE::TESForm::LookupByID<RE::TESFaction>(0x000DEED6);
        if (nightingaleFaction) {
            factions.push_back({"NightingaleFaction", nightingaleFaction});
        }

        RE::TESFaction *riftenFaction = RE::TESForm::LookupByID<RE::TESFaction>(0x0002816B);
        if (riftenFaction) {
            factions.push_back({"RiftenFaction", riftenFaction});
        }

        RE::TESFaction *solitudeFaction = RE::TESForm::LookupByID<RE::TESFaction>(0x00029DB0);
        if (solitudeFaction) {
            factions.push_back({"SolitudeFaction", solitudeFaction});
        }

        RE::TESFaction *whiterunFaction = RE::TESForm::LookupByID<RE::TESFaction>(0x000267EA);
        if (whiterunFaction) {
            factions.push_back({"WhiterunFaction", whiterunFaction});
        }

        RE::TESFaction *windhelmFaction = RE::TESForm::LookupByID<RE::TESFaction>(0x000267E3);
        if (windhelmFaction) {
            factions.push_back({"WindhelmFaction", windhelmFaction});
        }

        RE::TESFaction *winterholdFaction = RE::TESForm::LookupByID<RE::TESFaction>(0x00014217);
        if (winterholdFaction) {
            factions.push_back({"WinterholdFaction", winterholdFaction});
        }

        RE::TESFaction *thalmorFaction = RE::TESForm::LookupByID<RE::TESFaction>(0x00039F26);
        if (thalmorFaction) {
            factions.push_back({"ThalmorFaction", thalmorFaction});
        }

        RE::TESFaction *silverHandFaction = RE::TESForm::LookupByID<RE::TESFaction>(0x000AA0A4);
        if (silverHandFaction) {
            factions.push_back({"SilverHandFaction", silverHandFaction});
        }

        for (const auto &[factionID, keyword] : assignedKeywordsMap) {
            RE::TESFaction *faction = RE::TESForm::LookupByID<RE::TESFaction>(factionID);
            if (faction) {
                std::string factionName = GetFactionEditorID(faction).c_str();
                factions.push_back({factionName, faction});
            }
        }

        return factions;
    }

    std::string GetTagForFaction(RE::TESFaction *faction) {
        if (faction) {
            auto it = factionFormIDToTagMap.find(faction->GetFormID());
            if (it != factionFormIDToTagMap.end()) {
                return it->second;
            }
        }
        return "";
    }

    RE::TESFaction *GetFactionByFormID(RE::FormID *formID) { return nullptr; }

    std::vector<RE::TESFaction *> GetFactionsByArmorTags(RE::Actor *actor) {
        std::set<RE::TESFaction *> factionsSet;

        for (auto slot : armorBipedSlots) {
            RE::TESObjectARMO *wornArmor = actor->GetWornArmor(slot.slot);
            if (wornArmor) {
                for (const auto &[tag, factionID] : factionArmorKeywords) {
                    if (wornArmor->HasKeywordString(tag)) {
                        RE::TESFaction *faction = RE::TESForm::LookupByID<RE::TESFaction>(factionID);
                        if (faction) {
                            factionsSet.insert(faction);
                        }
                    }
                }
            }
        }

        return std::vector<RE::TESFaction *>(factionsSet.begin(), factionsSet.end());
    }

    RE::TESFaction *GetFactionByArmorTag(RE::Actor *actor) {
        if (!actor) {
            return nullptr;
        }

        for (auto &slot : allArmorSlots) {
            RE::TESObjectARMO *wornArmor = actor->GetWornArmor(slot);
            if (wornArmor) {
                for (const auto &[tag, factionID] : factionArmorKeywords) {
                    if (wornArmor->HasKeywordString(tag)) {
                        return RE::TESForm::LookupByID<RE::TESFaction>(factionID);
                    }
                }
            }
        }

        return nullptr;
    }

    std::vector<RE::TESFaction *> GetFactionsForActor(RE::Actor *actor) {
        std::vector<RE::TESFaction *> factions;

        if (!actor) {
            return factions;
        }

        for (RE::TESFaction *faction : allFactions) {
            if (faction && actor->IsInFaction(faction)) {
                factions.push_back(faction);
            }
        }

        return factions;
    }

    std::vector<RE::TESFaction *> GetAllFactions() {
        std::vector<RE::TESFaction *> factions;

        for (RE::TESFaction *faction : allFactions) {
            if (strcmp(faction->GetName(), "") != 0) {
                factions.push_back(faction);
            }
        }

        return factions;
    }

    RE::BSFixedString GetFactionEditorID(RE::TESFaction *faction) {
        if (faction) {
            const char *editorID = faction->GetFormEditorID();

            // Check if editorID is valid
            if (editorID && strlen(editorID) > 0) {
                return RE::BSFixedString(editorID);
            } else {
                // Fallback: return the faction's form ID in hexadecimal as a string
                char formIDBuffer[10];  // enough space for "0x" + 8 hex digits + null terminator
                snprintf(formIDBuffer, sizeof(formIDBuffer), "0x%08X", faction->GetFormID());
                auto factionName = factionFormIDToTagMap.find(faction->GetFormID());
                if (factionName != factionFormIDToTagMap.end()) {
                    return RE::BSFixedString(factionName->second);
                }
                return RE::BSFixedString(formIDBuffer);
            }
        }
        return RE::BSFixedString("No FactionID found!");
    }

    RE::TESFaction *GetFactionByEditorID(RE::BSFixedString factionEditorID) {
        if (factionEditorID.empty()) {
            return nullptr;
        }

        for (RE::TESFaction *faction : allFactions) {
            if (faction && strcmp(factionEditorID.c_str(), faction->GetFormEditorID()) == 0) {
                return faction;
            }
        }

        return nullptr;
    }

    bool IsKeywordAssigned(const std::string keywordName) {
        for (const auto &entry : assignedKeywordsMap) {
            if (strcmp(entry.second.c_str(), keywordName.c_str()) == 0) {
                RE::DebugNotification("Keyword was already assigned!");
                return true;
            }
        }
        RE::DebugNotification("Keyword wasn't assigned!");
        return false;
    }

    RE::BGSKeyword *AssignPredefinedKeywordToFaction(RE::TESFaction *faction) {
        if (!faction) {
            return nullptr;
        }

        for (const std::string &keywordEditorID : predefinedKeywords) {
            // Check if the keyword is already assigned
            if (!IsKeywordAssigned(keywordEditorID)) {
                RE::DebugNotification("Trying to assign new Keyword!");
                const auto keyword = GetKeywordByEditorID(RE::BSFixedString(keywordEditorID));
                if (keyword) {
                    RE::DebugNotification("Found keyword!");
                    // Associate this keyword with the faction
                    assignedKeywordsMap[faction->GetFormID()] = keywordEditorID;
                    factionArmorKeywords.push_back({keywordEditorID, faction->GetFormID()});
                    return keyword;
                } else {
                    RE::DebugNotification(("Keyword not found: " + keywordEditorID).c_str());
                }
            }
        }

        return nullptr;
    }

    RE::BGSKeyword *AddModFaction(RE::TESFaction *faction) {
        if (!faction) {
            return nullptr;
        }

        RE::BGSKeyword *assignedKeyword = AssignPredefinedKeywordToFaction(faction);
        if (!assignedKeyword) {
            RE::DebugNotification("Failed to assign a keyword for this faction.");
            return nullptr;
        }

        RE::DebugNotification("Faction and corresponding keyword added successfully!");
        return assignedKeyword;
    }

    RE::BGSKeyword *HandleAddFactionFromMCM(RE::TESFaction *faction) {
        if (!faction) {
            return nullptr;
        }

        RE::BGSKeyword *newKeyword = AddModFaction(faction);
        if (newKeyword) {
            RE::DebugNotification("Faction and keyword assigned successfully!");
            return newKeyword;
        }
        RE::DebugNotification("Failed to add faction.");
        return nullptr;
    }

    bool RemoveFactionKeywordAssignment(RE::BSFixedString targetKeyword, RE::TESFaction *faction) {
        if (!faction) {
            return false;
        }

        auto it = assignedKeywordsMap.find(faction->GetFormID());
        if (it != assignedKeywordsMap.end() && it->second == targetKeyword.c_str()) {
            assignedKeywordsMap.erase(it);

            auto removeIt = std::remove_if(factionArmorKeywords.begin(), factionArmorKeywords.end(),
                                           [&targetKeyword](const std::pair<std::string, RE::FormID> &tag) {
                                               return tag.first == targetKeyword.c_str();
                                           });
            if (removeIt != factionArmorKeywords.end()) {
                factionArmorKeywords.erase(removeIt, factionArmorKeywords.end());
            }
            return true;
        }

        return false;
    }

    std::pair<std::vector<std::string>, std::vector<RE::TESFaction *>> GetAllAssignedFactionKeywordPairs() {
        std::vector<std::string> keywords;
        std::vector<RE::TESFaction *> factions;

        for (const auto &keyword : predefinedKeywords) {
            for (const auto &entry : assignedKeywordsMap) {
                if (entry.second == keyword) {
                    RE::TESFaction *faction = RE::TESForm::LookupByID<RE::TESFaction>(entry.first);
                    if (faction) {
                        keywords.push_back(keyword);
                        factions.push_back(faction);
                    }
                }
            }
        }

        return {keywords, factions};
    }

    std::vector<std::string> GetAssignedKeywords() {
        std::vector<std::string> keywords;
        auto pairs = GetAllAssignedFactionKeywordPairs();

        for (const auto &pair : pairs.first) {
            keywords.push_back(pair);
        }

        return keywords;
    }

    std::vector<RE::TESFaction *> GetAssignedFactions() {
        std::vector<RE::TESFaction *> factions;
        auto pairs = GetAllAssignedFactionKeywordPairs();

        for (const auto &faction : pairs.second) {
            factions.push_back(faction);
        }

        return factions;
    }
}