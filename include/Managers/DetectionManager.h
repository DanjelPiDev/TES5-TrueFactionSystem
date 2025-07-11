#pragma once
#include "Config.h"
#include "Actor/NPEActor.h"

#include "NpcDetectionData.h"
#include "Faction.h"
#include "Managers/EnvironmentManager.h"


namespace NPE {
    class DetectionManager {
    public:
        static DetectionManager &GetInstance() {
            static DetectionManager instance;
            return instance;
        }
        /**
         * @brief Check if the player is detected by NPCs based on disguise value.
         *
         * @param actor The actor (usually the player) being detected by NPCs.
         */
        void CheckNPCDetection(RE::Actor *player);
        /**
         * @brief Check if an NPC recognizes the player based on faction and disguise.
         *
         * @param npc The NPC checking for the player.
         * @param player The player actor.
         * @param faction The faction being checked for disguise.
         * @return true If the NPC recognizes the player.
         * @return false If the NPC does not recognize the player.
         */
        bool NPCRecognizesPlayer(RE::Actor *npc, RE::Actor *player, RE::TESFaction *faction);
        /**
         * @brief Calculate the probability of detection based on disguise value.
         *
         * @param disguiseValue The disguise value of the actor.
         * @return float The probability of being detected.
         */
        float GetDetectionProbability(float disguiseValue);
        void CheckHoursPassed(RE::Actor *npc, RE::Actor *player, RE::TESFaction *faction);
        void StartCombat(RE::Actor *npc, RE::Actor *player, RE::TESFaction *npcFaction);
        bool DetectCrimeWhileDisguised(RE::Actor *npc, RE::Actor *player);

        void TriggerInvestigateLastKnownPosition(RE::Actor *npc, const RE::NiPoint3 &loc);

        void Save(SKSE::SerializationInterface *a_intfc);
        void Load(SKSE::SerializationInterface *a_intfc);
    private:
        DetectionManager() = default;
        /**
         * @brief Adjust the detection probability based on distance from NPC.
         *
         * @param detectionProbability The initial detection probability.
         * @param distance The distance between the actor and the NPC.
         * @param maxDistance The maximum detection range.
         * @return float The adjusted detection probability.
         */
        float AdjustProbabilityByDistance(float detectionProbability, float distance, float maxDistance);
    };

}