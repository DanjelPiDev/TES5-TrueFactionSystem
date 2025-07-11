#pragma once

#include "Race.h"
#include "Disguise.h"
#include "Faction.h"
#include "Combat.h"


namespace NPE {
    // Papyrus keyword and faction utility functions for Skyrim modding API

    bool RegisterPapyrusFunctions(RE::BSScript::IVirtualMachine* vm);

    /**
     * @brief Adds a keyword to the specified armor.
     *
     * This function attempts to add the provided keyword to the given armor item. If the keyword
     * already exists, the function will return false.
     *
     * @param armor The armor to which the keyword will be added.
     * @param keyword The keyword to be added to the armor.
     * @return bool True if the keyword was successfully added, false otherwise.
     */
    bool PapyrusAddKeywordToArmor(RE::StaticFunctionTag*, RE::TESObjectARMO* armor, RE::BGSKeyword* keyword);

    /**
     * @brief Removes a keyword from the specified armor.
     *
     * This function attempts to remove the provided keyword from the given armor item. If the keyword
     * does not exist on the armor, the function will return false.
     *
     * @param armor The armor from which the keyword will be removed.
     * @param keyword The keyword to be removed from the armor.
     * @return bool True if the keyword was successfully removed, false otherwise.
     */
    bool PapyrusRemoveKeywordFromArmor(RE::StaticFunctionTag*, RE::TESObjectARMO* armor, RE::BGSKeyword* keyword);

    /**
     * @brief Retrieves a keyword by its editor ID.
     *
     * This function looks up a keyword in the game's data using its editor ID and returns the corresponding keyword
     * object.
     *
     * @param keyword The editor ID of the keyword to be retrieved.
     * @return RE::BGSKeyword* The keyword object if found, nullptr otherwise.
     */
    RE::BGSKeyword* PapyrusGetKeywordByEditorID(RE::StaticFunctionTag*, RE::BSFixedString keyword);

    /**
     * @brief Retrieves a list of factions for a given actor.
     *
     * This function returns a list of all factions that the specified actor belongs to.
     *
     * @param actor The actor whose factions will be retrieved.
     * @return std::vector<RE::TESFaction*> A vector containing pointers to all factions the actor belongs to.
     */
    std::vector<RE::TESFaction*> PapyrusGetFactionsForActor(RE::StaticFunctionTag*, RE::Actor* actor);

    /**
     * @brief Retrieves the disguise value for a specified faction.
     *
     * This function returns the player's disguise value for the specified faction, which may affect interactions with
     * NPCs.
     *
     * @param faction The faction for which the disguise value is being retrieved.
     * @return float The disguise value associated with the faction, or 0.0f if no disguise value exists.
     */
    float PapyrusGetDisguiseValueForFaction(RE::StaticFunctionTag*, RE::TESFaction* faction);

    /**
     * @brief Retrieves the editor ID for a specified faction.
     *
     * This function returns the editor ID (string identifier) of the specified faction.
     *
     * @param faction The faction whose editor ID will be retrieved.
     * @return RE::BSFixedString The editor ID of the faction.
     */
    RE::BSFixedString PapyrusGetFactionEditorID(RE::StaticFunctionTag*, RE::TESFaction* faction);

    /**
     * @brief Retrieves the disguise bonus value for a specified faction.
     *
     * This function returns the bonus disguise value the player has for the specified faction, which may be derived
     * from armor sets or other factors.
     *
     * @param faction The faction for which the disguise bonus value is being retrieved.
     * @return float The bonus disguise value associated with the faction, or 0.0f if no bonus exists.
     */
    float PapyrusGetDisguiseBonusValueForFaction(RE::StaticFunctionTag*, RE::TESFaction* faction);

    std::vector<RE::TESFaction*> PapyrusGetAllFactions(RE::StaticFunctionTag*);

    RE::BGSKeyword* PapyrusHandleAddFactionFromMCM(RE::StaticFunctionTag*, RE::TESFaction* faction);

    float PapyrusGetTimeToLoseDetection(RE::StaticFunctionTag*);

    void PapyrusSetTimeToLoseDetection(RE::StaticFunctionTag*, float time);

    float PapyrusGetInvestigationThreshold(RE::StaticFunctionTag*);

    void PapyrusSetInvestigationThreshold(RE::StaticFunctionTag*, float threshold);

    float PapyrusGetDetectionThreshold(RE::StaticFunctionTag*);

    void PapyrusSetDetectionThreshold(RE::StaticFunctionTag*, float threshold);

    float PapyrusGetDetectionRadius(RE::StaticFunctionTag*);

    void PapyrusSetDetectionRadius(RE::StaticFunctionTag*, float radius);

    bool PapyrusGetUseFOVCheck(RE::StaticFunctionTag*);

    void PapyrusSetUseFOVCheck(RE::StaticFunctionTag*, bool useFOVCheck);

    bool PapyrusGetUseLineOfSightCheck(RE::StaticFunctionTag*);

    void PapyrusSetUseLineOfSightCheck(RE::StaticFunctionTag*, bool useLineOfSightCheck);

    void PapyrusSetFOVAngle(RE::StaticFunctionTag*, float angle);

    float PapyrusGetFOVAngle(RE::StaticFunctionTag*);
}