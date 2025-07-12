#include "Managers/DisguiseManager.h"
#include "Globals.h"


namespace NPE {

    void DisguiseManager::UpdateDisguiseValue(RE::Actor *actor) {
        // Retieve all factions, where the player is already a member of (To avoid adding/remove quest related factions unnecessarily)
        if (!_initialized) {
            for (auto *f : allFactions) {
                if (actor->IsInFaction(f)) _originalFactions.insert(f);
            }
            _initialized = true;
        }

        constexpr const char *COVERED_FACE_TAG = "npeCoveredFace";
        std::unordered_map<RE::TESFaction *, float> factionDisguiseValues;

        // Gather the factions by armor tags
        std::vector<RE::TESFaction *> factions = GetFactionsByArmorTags(actor);
        for (auto &possible : allFactions) {
            if (actor->IsInFaction(possible) && !factionDisguiseValues.count(possible)) {
                factionDisguiseValues[possible] = 0.0f;
            }
        }

        if (factions.empty()) {
            playerDisguiseStatus.Clear();
            RemovePlayerFromAllFactions(actor);
            detectionManager.CheckNPCDetection(actor);
            return;
        }

        this->ClearArmorDisguiseValues(actor);

        // Iterate through armor slots and calculate values per faction
        for (const auto &slot : armorBipedSlots) {
            RE::TESObjectARMO *armor = actor->GetWornArmor(slot.slot);
            if (!armor) continue;

            for (RE::TESFaction *faction : factions) {
                if (!faction) continue;

                std::string factionTag = GetTagForFaction(faction);
                if (factionTag.empty()) continue;

                // Check if the armor has a keyword for the faction
                if (armor->HasKeywordString(factionTag)) {
                    // Add armor piece value to the faction
                    factionDisguiseValues[faction] += slot.weight;

                    // Check if the armor covers the face and add a bonus
                    if (armor->HasKeywordString(COVERED_FACE_TAG)) {
                        factionDisguiseValues[faction] += 12;
                    }
                }
            }
        }

        this->AddArmorSetBonus(actor);
        // Process each faction's disguise value
        for (auto &[faction, disguiseValue] : factionDisguiseValues) {
            disguiseValue += playerDisguiseStatus.GetRaceBonusValue(faction);
            disguiseValue += playerDisguiseStatus.GetBonusValue(faction);
            disguiseValue = std::clamp(disguiseValue, 0.0f, 100.0f);

            playerDisguiseStatus.SetDisguiseValue(faction, disguiseValue);

            std::string factionTag = GetTagForFaction(faction);

            // Add or remove the actor from factions based on disguise value
            if (!actor->IsInFaction(faction) && disguiseValue > ADD_TO_FACTION_THRESHOLD) {
                actor->AddToFaction(faction, 1);
            } else if (disguiseValue <= ADD_TO_FACTION_THRESHOLD) {
                // Only remove from faction if the player was NOT originally in it
                if (!_originalFactions.count(faction)) {
                    actor->AddToFaction(faction, -1);
                    playerDisguiseStatus.RemoveDisguiseValue(faction);
                }
            }

        }

        detectionManager.CheckNPCDetection(actor);
    }


    void DisguiseManager::ClearArmorDisguiseValues(RE::Actor *actor) {
        for (const auto &[factionTag, factionID] : factionArmorKeywords) {
            RE::TESFaction *faction = RE::TESForm::LookupByID<RE::TESFaction>(factionID);
            if (faction) {
                playerDisguiseStatus.SetBonusValue(faction, 0.0f);
                playerDisguiseStatus.SetDisguiseValue(faction, 0.0f);
            }
        }
    }

    float DisguiseManager::GetDisguiseValueForFaction(RE::TESFaction *faction) {
        return playerDisguiseStatus.GetDisguiseValue(faction);
    }

    float DisguiseManager::GetRaceBonusValueForFaction(RE::TESFaction *faction) {
        return playerDisguiseStatus.GetRaceBonusValue(faction);
    }

    float DisguiseManager::GetDisguiseBonusValueForFaction(RE::TESFaction *faction) {
        return playerDisguiseStatus.GetBonusValue(faction);
    }

    void DisguiseManager::AddArmorSetBonus(RE::Actor *actor) {
        for (const auto &[factionTag, factionID] : factionArmorKeywords) {
            RE::TESFaction *faction = RE::TESForm::LookupByID<RE::TESFaction>(factionID);
            if (!faction) continue;

            float bonus = 0.0f;
            for (const auto &slotInfo : armorBipedSlots) {
                auto armor = actor->GetWornArmor(slotInfo.slot);
                if (armor && armor->HasKeywordString(factionTag)) {
                    bonus += ComputeSlotWeight(slotInfo.slot);
                }
            }
            bonus = std::clamp(bonus, 0.0f, 100.0f);

            playerDisguiseStatus.SetBonusValue(faction, bonus);
        }
    }
}