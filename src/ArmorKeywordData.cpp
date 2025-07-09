#include "ArmorKeywordData.h"
#include "Globals.h"


namespace NPE {

    void SaveArmorKeywordDataCallback(SKSE::SerializationInterface* a_intfc) {
        std::uint32_t count = static_cast<std::uint32_t>(savedArmorKeywordAssociations.size());
        a_intfc->WriteRecordData(&count, sizeof(count));
        for (auto& entry : savedArmorKeywordAssociations) {
            a_intfc->WriteRecordData(&entry.armorID, sizeof(entry.armorID));
            a_intfc->WriteRecordData(&entry.keywordID, sizeof(entry.keywordID));
        }
    }

    void LoadArmorKeywordDataCallback(SKSE::SerializationInterface* a_intfc) {
        std::uint32_t count;
        if (!a_intfc->ReadRecordData(&count, sizeof(count))) {
            spdlog::warn("LoadArmorKeywordDataCallback failed to read count");
            return;
        }
        savedArmorKeywordAssociations.clear();
        for (std::uint32_t i = 0; i < count; ++i) {
            ArmorKeywordData entry;
            if (!a_intfc->ReadRecordData(&entry.armorID, sizeof(entry.armorID)) ||
                !a_intfc->ReadRecordData(&entry.keywordID, sizeof(entry.keywordID))) {
                spdlog::warn("LoadArmorKeywordDataCallback incomplete data at index {}", i);
                return;
            }
            savedArmorKeywordAssociations.push_back(entry);
        }
    }

    void RevertArmorKeywordDataCallback(SKSE::SerializationInterface* a_intfc) {
        savedArmorKeywordAssociations.clear();
    }

}