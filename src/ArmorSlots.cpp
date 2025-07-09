#include "ArmorSlots.h"
#include "Globals.h"
#include <memory>


namespace NPE {
    bool AddKeywordToArmor(RE::TESObjectARMO* armor, RE::BGSKeyword* keyword) {
        if (!armor || !keyword) {
            spdlog::error("Armor or keyword is nullptr");
            return false;
        }

        RE::BGSKeywordForm* keywordForm = armor->As<RE::BGSKeywordForm>();
        if (!keywordForm) {
            spdlog::warn("Armor does not have a keyword form");
            return false;
        }

        // Check if the keyword is already in the list
        for (uint32_t i = 0; i < keywordForm->numKeywords; i++) {
            if (keywordForm->keywords[i] == keyword) {
                spdlog::warn("Keyword already exists in armor");
                return false;  // Keyword already exists, don't add it again
            }
        }

        // Expand the keyword array and add the new keyword
        uint32_t newNumKeywords = keywordForm->numKeywords + 1;
        // For the memory management (Because the MCM menu is really slow)
        auto newKeywords = std::make_unique<RE::BGSKeyword*[]>(newNumKeywords);

        // Copy over existing keywords
        for (uint32_t i = 0; i < keywordForm->numKeywords; i++) {
            newKeywords[i] = keywordForm->keywords[i];
        }

        newKeywords[newNumKeywords - 1] = keyword;

        if (keywordForm->keywords) {
            delete[] keywordForm->keywords;
        }

        keywordForm->keywords = newKeywords.release();
        keywordForm->numKeywords = newNumKeywords;

        return true;
    }

    bool RemoveKeywordFromArmor(RE::TESObjectARMO* armor, RE::BGSKeyword* keyword) {
        if (!armor || !keyword) {
            spdlog::error("Armor or keyword is nullptr");
            return false;
        }

        RE::BGSKeywordForm* keywordForm = armor->As<RE::BGSKeywordForm>();
        if (!keywordForm || keywordForm->numKeywords == 0) {
            spdlog::warn("Armor does not have a keyword form");
            return false;
        }

        // Find the index of the keyword to be removed
        int32_t indexToRemove = -1;
        for (uint32_t i = 0; i < keywordForm->numKeywords; i++) {
            if (keywordForm->keywords[i] == keyword) {
                indexToRemove = i;
                break;
            }
        }

        // If keyword not found, return false
        if (indexToRemove == -1) {
            spdlog::warn("Keyword not found in armor");
            return false;
        }

        // If there's only one keyword, just clear the array
        if (keywordForm->numKeywords == 1) {
            delete[] keywordForm->keywords;
            keywordForm->keywords = nullptr;
            keywordForm->numKeywords = 0;
            return true;
        }

        uint32_t newNumKeywords = keywordForm->numKeywords - 1;
        // Same as above, for the memory management
        auto newKeywords = std::make_unique<RE::BGSKeyword*[]>(newNumKeywords);

        // Copy over the keywords except for the one to remove
        for (uint32_t i = 0, j = 0; i < keywordForm->numKeywords; i++) {
            if (i != indexToRemove) {
                newKeywords[j++] = keywordForm->keywords[i];
            }
        }

        delete[] keywordForm->keywords;

        keywordForm->keywords = newKeywords.release();
        keywordForm->numKeywords = newNumKeywords;

        return true;
    }

    RE::BGSKeyword* GetKeywordByEditorID(RE::BSFixedString keywordEditorID) {
        if (!g_dataHandler) {
            g_dataHandler = RE::TESDataHandler::GetSingleton();
        }
        const auto& allKeywords = g_dataHandler->GetFormArray<RE::BGSKeyword>();

        for (RE::BGSKeyword* keyword : allKeywords) {
            if (keyword && strcmp(keywordEditorID.c_str(), keyword->GetFormEditorID()) == 0) {
                return keyword;
            }
        }

        return nullptr;
    }
}