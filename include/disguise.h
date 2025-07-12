#pragma once
#include "Faction.h"
#include "Combat.h"
#include "Actor/NPEActor.h"

#include "Misc/Config.h"


namespace NPE {
    // Detection and probability functions
    /**
     * @brief Calculate the probability of detection based on disguise value.
     *
     * @param disguiseValue The disguise value of the actor.
     * @return float The probability of being detected.
     */
    float GetDetectionProbability(float disguiseValue);

    /**
     * @brief Adjust the detection probability based on distance from NPC.
     *
     * @param detectionProbability The initial detection probability.
     * @param distance The distance between the actor and the NPC.
     * @param maxDistance The maximum detection range.
     * @return float The adjusted detection probability.
     */
    float AdjustProbabilityByDistance(float detectionProbability, float distance, float maxDistance);

    // Data serialization functions
    /**
     * @brief Save the current NPC detection data for serialization.
     *
     * @param a_intfc The serialization interface.
     */
    void SaveDetectionData(SKSE::SerializationInterface* a_intfc);

    /**
     * @brief Load NPC detection data from serialized data.
     *
     * @param a_intfc The serialization interface.
     */
    void LoadDetectionData(SKSE::SerializationInterface* a_intfc);

    /**
     * @brief Remove the player from all relevant factions.
     *
     * @param player The player actor.
     */
    void RemovePlayerFromAllFactions(RE::Actor* player);

    /**
     *
     */
    float GetDisguiseValueForFaction(RE::TESFaction* faction);

    /**
     *
     */
    float GetDisguiseBonusValueForFaction(RE::TESFaction* faction);

    /**
     *
     */
    float GetRaceBonusValueForFaction(RE::TESFaction* faction);
}