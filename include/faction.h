#pragma once
#include "DisguiseData.h"
#include "Main.h"


namespace NPE {
    /**
     * @brief Get the faction based on the actor's worn armor tags.
     *
     * @param actor The actor whose armor is being evaluated.
     * @return TESFaction* The corresponding faction if found, nullptr otherwise.
     */
    RE::TESFaction* GetFactionByArmorTag(RE::Actor* actor);

    /**
     * @brief Get a list of factions relevant to the current game context.
     *
     * @return std::vector<std::pair<std::string, TESFaction*>> A list of factions with their corresponding string tags.
     */
    std::vector<std::pair<std::string, RE::TESFaction*>> GetRelevantFactions();

    /**
     * @brief Get the tag associated with a given faction.
     *
     * @param faction The faction whose tag is being retrieved.
     * @return std::string The tag corresponding to the faction, or an empty string if not found.
     */
    std::string GetTagForFaction(RE::TESFaction* faction);

    /**
     * @brief Get a list of factions based on the armor tags worn by the actor.
     *
     * @param actor The actor whose armor is being checked for faction tags.
     * @return std::vector<TESFaction*> A list of factions based on the actor's worn armor.
     */
    std::vector<RE::TESFaction*> GetFactionsByArmorTags(RE::Actor* actor);

    /**
     *
     */
    std::vector<RE::TESFaction*> GetFactionsForActor(RE::Actor* actor);

    /**
     *
     */
    RE::TESFaction* GetFactionByEditorID(RE::BSFixedString factionEditorID);

    /**
     *
     */
    RE::BSFixedString GetFactionEditorID(RE::TESFaction* faction);

    /**
     *
     */
    RE::BGSKeyword* HandleAddFactionFromMCM(RE::TESFaction* faction);

    /**
     *
     */
    std::pair<std::vector<std::string>, std::vector<RE::TESFaction*>> GetAllAssignedFactionKeywordPairs();

    /**
     *
     */
    std::vector<std::string> GetAssignedKeywords();

    /**
     *
     */
    std::vector<RE::TESFaction*> GetAssignedFactions();

    /**
     *
     */
    std::vector<RE::TESFaction*> GetAllFactions();

    /**
     *
     */
    bool RemoveFactionKeywordAssignment(RE::BSFixedString targetKeyword, RE::TESFaction* faction);
}