#pragma once

#include "Main.h"
#include "Faction.h"
#include "RE/B/BGSBipedObjectForm.h"


namespace NPE {
    /**
     * @brief Constant values for the weight of different armor pieces.
     *
     * These values represent how much each type of armor contributes to the disguise.
     */
    constexpr float CHEST_WEIGHT = 30.0f;    /// Weight for chest armor.
    constexpr float HELMET_WEIGHT = 18.0f;   /// Weight for helmet.
    constexpr float GLOVES_WEIGHT = 4.0f;    /// Weight for gloves.
    constexpr float FOREARMS_WEIGHT = 8.0f;  /// Weight for forearm armor.
    constexpr float SHOES_WEIGHT = 5.0f;     /// Weight for shoes/boots.
    constexpr float CIRCLET_WEIGHT = 1.0f;   /// Weight for circlets.
    constexpr float HAIR_WEIGHT = 8.0f;      /// Weight for hair.

    /**
     * @brief Struct to represent an armor slot and its corresponding weight.
     *
     * This structure holds the armor slot type and the weight contribution to disguise.
     */
    struct ArmorSlot {
        RE::BGSBipedObjectForm::BipedObjectSlot slot;  /// The specific armor slot.
        float weight;                                  /// The disguise weight value for the slot.
    };

    extern const std::vector<ArmorSlot> armorBipedSlots;

    /**
     *
     */
    bool AddKeywordToArmor(RE::TESObjectARMO* armor, RE::BGSKeyword* keyword);

    /**
     *
     */
    bool RemoveKeywordFromArmor(RE::TESObjectARMO* armor, RE::BGSKeyword* keyword);

    /**
     *
     */
    RE::BGSKeyword* GetKeywordByEditorID(RE::BSFixedString keywordEditorID);
}