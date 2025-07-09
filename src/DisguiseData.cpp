#include "DisguiseData.h"



namespace NPE {
    void PlayerDisguiseStatus::SetDisguiseValue(RE::TESFaction* faction, float value) {
        if (factionDisguiseMap.find(faction) != factionDisguiseMap.end()) {
            factionDisguiseMap[faction].disguiseValue = value;
        } else {
            factionDisguiseMap[faction] = {faction, value};
        }
    }

    float PlayerDisguiseStatus::GetDisguiseValue(RE::TESFaction* faction) const {
        auto it = factionDisguiseMap.find(faction);
        if (it != factionDisguiseMap.end()) {
            return it->second.disguiseValue;
        }
        return 0.0f;
    }

    void PlayerDisguiseStatus::SetBonusValue(RE::TESFaction* faction, float bonus) {
        factionDisguiseMap[faction].faction = faction;
        factionDisguiseMap[faction].bonusValue = bonus;
    }

    float PlayerDisguiseStatus::GetBonusValue(RE::TESFaction* faction) const {
        auto it = factionDisguiseMap.find(faction);
        if (it != factionDisguiseMap.end()) {
            return it->second.bonusValue;
        }
        return 0.0f;
    }

    void PlayerDisguiseStatus::RemoveDisguiseValue(RE::TESFaction* faction) { factionDisguiseMap.erase(faction); }

    float PlayerDisguiseStatus::GetRaceBonusValue(RE::TESFaction* faction) const {
        auto it = factionDisguiseMap.find(faction);
        if (it != factionDisguiseMap.end()) {
            return it->second.raceDisguiseBonus;
        }
        return 0.0f;
    }

    void PlayerDisguiseStatus::Clear() { factionDisguiseMap.clear(); }

    void PlayerDisguiseStatus::Save(SKSE::SerializationInterface* a_intfc) {
        std::uint32_t count = static_cast<std::uint32_t>(factionDisguiseMap.size());
        a_intfc->WriteRecordData(&count, sizeof(count));
        for (auto& [faction, info] : factionDisguiseMap) {
            RE::FormID id = faction->GetFormID();
            a_intfc->WriteRecordData(&id, sizeof(id));
            a_intfc->WriteRecordData(&info.disguiseValue, sizeof(info.disguiseValue));
            a_intfc->WriteRecordData(&info.bonusValue, sizeof(info.bonusValue));
            a_intfc->WriteRecordData(&info.raceDisguiseBonus, sizeof(info.raceDisguiseBonus));
        }
    }

    void PlayerDisguiseStatus::SetRaceBonusValue(RE::TESFaction* faction, float bonus) {
        factionDisguiseMap[faction].faction = faction;
        factionDisguiseMap[faction].raceDisguiseBonus = bonus;
    }

    void PlayerDisguiseStatus::Load(SKSE::SerializationInterface* a_intfc) {
        std::uint32_t count;
        if (!a_intfc->ReadRecordData(&count, sizeof(count))) {
            spdlog::warn("PlayerDisguiseStatus::Load failed to read count");
            return;
        }
        factionDisguiseMap.clear();
        for (std::uint32_t i = 0; i < count; ++i) {
            RE::FormID id;
            DisguiseData info;
            if (!a_intfc->ReadRecordData(&id, sizeof(id)) ||
                !a_intfc->ReadRecordData(&info.disguiseValue, sizeof(info.disguiseValue)) ||
                !a_intfc->ReadRecordData(&info.bonusValue, sizeof(info.bonusValue)) ||
                !a_intfc->ReadRecordData(&info.raceDisguiseBonus, sizeof(info.raceDisguiseBonus))) {
                spdlog::warn("PlayerDisguiseStatus::Load incomplete data at index {}", i);
                return;
            }
            // Resolve faction pointer
            if (RE::TESForm* form = RE::TESForm::LookupByID(id)) {
                if (auto* faction = form->As<RE::TESFaction>()) {
                    info.faction = faction;
                    factionDisguiseMap[faction] = info;
                }
            }
        }
    }
}
