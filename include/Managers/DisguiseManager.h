#pragma once
#include "Misc/Config.h"
#include "Faction.h"


namespace NPE {
    class DisguiseManager {
    public:
        static DisguiseManager &GetInstance() {
            static DisguiseManager instance;
            return instance;
        }

        /**
         * @brief Calculate the total disguise value for a given faction.
         *
         * @param faction The faction to evaluate disguise against.
         * @return float The computed disguise value (0.0 to 100.0 scale).
         */
        float GetDisguiseValueForFaction(RE::TESFaction *faction);

        /**
         * @brief Retrieve additional bonus disguise value from equipped armor sets.
         *
         * @param faction The faction context for bonus calculation.
         * @return float Bonus disguise value (0.0 to 100.0 scale).
         */
        float GetDisguiseBonusValueForFaction(RE::TESFaction *faction);

        /**
         * @brief Retrieve race-based bonus disguise value for a faction.
         *
         * @param faction The faction context for race bonus.
         * @return float Race bonus disguise value (0.0 to 100.0 scale).
         */
        float GetRaceBonusValueForFaction(RE::TESFaction *faction);

        /**
         * @brief Apply additional bonuses based on complete armor sets equipped by actor.
         *
         * @param actor Pointer to actor whose armor is evaluated.
         */
        void AddArmorSetBonus(RE::Actor *actor);

        /**
         * @brief Clear all current armor-based disguise values for actor.
         *
         * @param actor Pointer to actor whose disguise values are reset.
         */
        void ClearArmorDisguiseValues(RE::Actor *actor);

        /**
         * @brief Recalculate and update disguise values for the actor based on current state.
         *
         * @param actor Pointer to actor whose disguise is recalculated.
         */
        void UpdateDisguiseValue(RE::Actor *actor);
    private:
        DisguiseManager() = default;

        /**
         * @brief Initialize the manager with default faction data. 
           To keep track of original factions, otherwise people will be thrown out of their original factions when they un-equip armor 
           (Could break quests). 
           
           
           Example: Player is in the Thieves Guild, but when they un-equip armor, they are removed from the Thieves Guild faction.
         */
        std::unordered_set<RE::TESFaction *> _originalFactions;
        bool _initialized = false;
    };
}
