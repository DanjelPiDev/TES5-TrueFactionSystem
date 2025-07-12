#pragma once
#include <atomic>
#include <memory>
#include <thread>
#include <mutex>

#include "Managers/NPCAlertnessManager.h"
#include "Managers/DetectionManager.h"
#include "Managers/DisguiseManager.h"
#include "Managers/EnvironmentManager.h"
#include "Managers/EquipmentManager.h"

#include "DisguiseData.h"
#include "NpcDetectionData.h"
#include "ArmorKeywordData.h"

namespace NPE {

    /* ========================================================================
     * Forward declarations of manager classes to avoid circular dependencies.
      ========================================================================*/
    class DetectionManager;
    class DisguiseManager;
    class EnvironmentManager;
    class NPCAlertnessManager;
    class EquipEventHandler;

    /* ========================================================================
    * Background Task Control
      ========================================================================*/
    extern std::atomic<bool> backgroundTaskRunning;
    extern std::unique_ptr<std::thread> backgroundTaskThread;
    extern std::mutex recognizedNPCsMutex;

    /* ========================================================================
    * Faction and Armor Data
      ========================================================================*/
    extern RE::TESDataHandler* dataHandler;
    extern std::vector<RE::TESFaction*> filteredFactions;
    extern std::unordered_map<RE::FormID, RE::BSFixedString> factionEditorIDCache;

    extern std::vector<std::pair<std::string, RE::FormID>> factionArmorKeywords;
    extern const std::vector<RE::BGSBipedObjectForm::BipedObjectSlot> allArmorSlots;
    extern std::unordered_map<RE::FormID, std::string> factionFormIDToTagMap;

    
    /* ========================================================================
    * Managers (Singletons References)
      ========================================================================*/
    extern DetectionManager& detectionManager;
    extern DisguiseManager& disguiseManager;
    extern EnvironmentManager& environmentManager;
    extern NPCAlertnessManager& npcAlertnessManager;
    extern EquipEventHandler& equipEventHandler;

    /* ========================================================================
    * Recognized NPCs and Player Disguise Status
      ========================================================================*/
    extern std::unordered_map<RE::FormID, NPCDetectionData> recognizedNPCs;
    extern PlayerDisguiseStatus playerDisguiseStatus;
    extern std::vector<ArmorKeywordData> savedArmorKeywordAssociations;
    
    extern std::unordered_map<std::string, std::unordered_map<std::string, int>> factionRaceData;
    extern const std::vector<ArmorSlot> armorBipedSlots;

    /* ========================================================================
    * Configuration Variables (MCM)
      ========================================================================*/
    extern float TIME_TO_LOSE_DETECTION;
    extern float INVESTIGATION_THRESHOLD;
    extern float DETECTION_THRESHOLD;
    extern float DETECTION_RADIUS;
    extern float FOV_ANGLE;

    extern bool USE_FOV_CHECK;
    extern bool USE_LINE_OF_SIGHT_CHECK;

    extern float NPC_LEVEL_THRESHOLD;
    extern float ADD_TO_FACTION_THRESHOLD;

    /* ========================================================================
    * Getters and Setters for Configuration Variables
      ========================================================================*/

    float GetTimeToLoseDetection();
    void SetTimeToLoseDetection(float v);

    float GetInvestigationThreshold();
    void SetInvestigationThreshold(float v);

    float GetDetectionThreshold();
    void SetDetectionThreshold(float v);

    float GetDetectionRadius();
    void SetDetectionRadius(float v);

    bool GetUseFOVCheck();
    void SetUseFOVCheck(bool v);

    bool GetUseLineOfSightCheck();
    void SetUseLineOfSightCheck(bool v);

    float GetFOVAngle();
    void SetFOVAngle(float v);

    float GetNPCLevelThreshold();
    void SetNPCLevelThreshold(float v);

    float GetAddToFactionThreshold();
    void SetAddToFactionThreshold(float v);
    
    /* ========================================================================
    * Utility Functions
      ========================================================================*/
    /**
     * @brief Computes the weight for a given armor slot based on player level and slot type.
     *
     * @param slot The armor slot to compute the weight for.
     * @return float The computed weight for the armor slot.
     */
    float ComputeSlotWeight(RE::BGSBipedObjectForm::BipedObjectSlot slot);
}