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
    extern std::atomic<bool> backgroundTaskRunning;
    extern std::unique_ptr<std::thread> backgroundTaskThread;

    // Protected by a mutex to ensure thread safety (especially for recognizedNPCs)
    extern std::mutex recognizedNPCsMutex;

    extern std::vector<RE::TESFaction*> filteredFactions;
    extern std::unordered_map<RE::FormID, RE::BSFixedString> factionEditorIDCache;

    class DetectionManager;
    class DisguiseManager;
    class EnvironmentManager;
    class NPCAlertnessManager;
    class EquipEventHandler;

    extern DetectionManager& detectionManager;
    extern DisguiseManager& disguiseManager;
    extern EnvironmentManager& environmentManager;
    extern NPCAlertnessManager& npcAlertnessManager;
    extern EquipEventHandler& equipEventHandler;

    extern std::vector<std::pair<std::string, RE::FormID>> factionArmorKeywords;
    extern const std::vector<RE::BGSBipedObjectForm::BipedObjectSlot> allArmorSlots;
    extern std::unordered_map<RE::FormID, std::string> factionFormIDToTagMap;

    extern std::unordered_map<RE::FormID, NPCDetectionData> recognizedNPCs;
    extern PlayerDisguiseStatus playerDisguiseStatus;
    extern std::vector<ArmorKeywordData> savedArmorKeywordAssociations;
    extern RE::TESDataHandler* dataHandler;

    extern std::unordered_map<std::string, std::unordered_map<std::string, int>> factionRaceData;

    extern const std::vector<ArmorSlot> armorBipedSlots;

    extern float TIME_TO_LOSE_DETECTION;
    extern float DETECTION_THRESHOLD;
    extern float DETECTION_RADIUS;

    extern bool USE_FOV_CHECK;
    extern bool USE_LINE_OF_SIGHT_CHECK;

    float GetTimeToLoseDetection();
    void SetTimeToLoseDetection(float v);

    float GetDetectionThreshold();
    void SetDetectionThreshold(float v);

    float GetDetectionRadius();
    void SetDetectionRadius(float v);

    bool GetUseFOVCheck();
    void SetUseFOVCheck(bool v);

    bool GetUseLineOfSightCheck();
    void SetUseLineOfSightCheck(bool v);

    float ComputeSlotWeight(RE::BGSBipedObjectForm::BipedObjectSlot slot);
}