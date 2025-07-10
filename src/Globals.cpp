#include "Globals.h"



namespace NPE {
    float TIME_TO_LOSE_DETECTION = 2.0f;
    float DETECTION_THRESHOLD = 0.61f;
    float DETECTION_RADIUS = 400.0f;

    std::atomic<bool> backgroundTaskRunning{false};
    std::unique_ptr<std::thread> backgroundTaskThread;
    std::mutex recognizedNPCsMutex;

    std::vector<RE::TESFaction*> filteredFactions;
    std::unordered_map<RE::FormID, RE::BSFixedString> factionEditorIDCache;

    DetectionManager& detectionManager = DetectionManager::GetInstance();
    DisguiseManager& disguiseManager = DisguiseManager::GetInstance();
    EnvironmentManager& environmentManager = EnvironmentManager::GetInstance();
    NPCAlertnessManager& npcAlertnessManager = NPCAlertnessManager::GetInstance();
    EquipEventHandler& equipEventHandler = EquipEventHandler::GetInstance();

    std::vector<std::pair<std::string, RE::FormID>> factionArmorKeywords = {{"npeBanditFaction", 0x0001BCC0},
                                                                            {"npeImperialFaction", 0x0002BF9A},
                                                                            {"npeBladesFaction", 0x00072834},
                                                                            {"npeCompanionsFaction", 0x00048362},
                                                                            {"npeDawnstarFaction", 0x0002816E},
                                                                            {"npeFalkreathFaction", 0x00028170},
                                                                            {"npeForswornFaction", 0x00043599},
                                                                            {"npeMarkarthFaction", 0x0002816C},
                                                                            {"npeMorthalFaction", 0x0002816D},
                                                                            {"npeNightingaleFaction", 0x000DEED6},
                                                                            {"npeRiftenFaction", 0x0002816B},
                                                                            {"npeSolitudeFaction", 0x00029DB0},
                                                                            {"npeStormcloaksFaction", 0x00028849},
                                                                            {"npeWhiterunFaction", 0x000267EA},
                                                                            {"npeWindhelmFaction", 0x000267E3},
                                                                            {"npeWinterholdFaction", 0x00014217},
                                                                            {"npeThalmorFaction", 0x00039F26},
                                                                            {"npeSilverHandFaction", 0x000AA0A4},
                                                                            {"npeThievesGuildFaction", 0x00029DA9}};
    
    const std::vector<RE::BGSBipedObjectForm::BipedObjectSlot> allArmorSlots = {
        RE::BGSBipedObjectForm::BipedObjectSlot::kHead,     RE::BGSBipedObjectForm::BipedObjectSlot::kBody,
        RE::BGSBipedObjectForm::BipedObjectSlot::kHands,    RE::BGSBipedObjectForm::BipedObjectSlot::kFeet,
        RE::BGSBipedObjectForm::BipedObjectSlot::kForearms, RE::BGSBipedObjectForm::BipedObjectSlot::kCirclet,
        RE::BGSBipedObjectForm::BipedObjectSlot::kHair};

    std::unordered_map<RE::FormID, std::string> factionFormIDToTagMap = {
        {0x0001BCC0, "npeBanditFaction"},      {0x0002BF9A, "npeImperialFaction"}, {0x00072834, "npeBladesFaction"},
        {0x00048362, "npeCompanionsFaction"},  {0x0002816E, "npeDawnstarFaction"}, {0x00028170, "npeFalkreathFaction"},
        {0x00043599, "npeForswornFaction"},    {0x0002816C, "npeMarkarthFaction"}, {0x0002816D, "npeMorthalFaction"},
        {0x000DEED6, "npeNightingaleFaction"}, {0x0002816B, "npeRiftenFaction"},   {0x00029DB0, "npeSolitudeFaction"},
        {0x00028849, "npeStormcloaksFaction"}, {0x000267EA, "npeWhiterunFaction"}, {0x000267E3, "npeWindhelmFaction"},
        {0x00014217, "npeWinterholdFaction"},  {0x00039F26, "npeThalmorFaction"},  {0x000AA0A4, "npeSilverHandFaction"},
        {0x00029DA9, "npeThievesGuildFaction"}};

    std::unordered_map<RE::FormID, NPCDetectionData> recognizedNPCs;
    PlayerDisguiseStatus playerDisguiseStatus;
    std::vector<ArmorKeywordData> savedArmorKeywordAssociations;
    RE::TESDataHandler* dataHandler = RE::TESDataHandler::GetSingleton();

    const std::vector<ArmorSlot> armorBipedSlots = {
        {RE::BGSBipedObjectForm::BipedObjectSlot::kBody, CHEST_WEIGHT},
        {RE::BGSBipedObjectForm::BipedObjectSlot::kHands, GLOVES_WEIGHT},
        {RE::BGSBipedObjectForm::BipedObjectSlot::kForearms, FOREARMS_WEIGHT},
        {RE::BGSBipedObjectForm::BipedObjectSlot::kCirclet, CIRCLET_WEIGHT},
        {RE::BGSBipedObjectForm::BipedObjectSlot::kFeet, SHOES_WEIGHT},
        {RE::BGSBipedObjectForm::BipedObjectSlot::kHead, HELMET_WEIGHT},
        {RE::BGSBipedObjectForm::BipedObjectSlot::kHair, HAIR_WEIGHT}};

    std::unordered_map<std::string, std::unordered_map<std::string, int>> factionRaceData;

    float ComputeSlotWeight(RE::BGSBipedObjectForm::BipedObjectSlot slot) {
        float baseWeight = 0.0f;
        for (const auto& si : armorBipedSlots) {
            if (si.slot == slot) {
                baseWeight = si.weight;
                break;
            }
        }

        // Normalize the weight based on player level
        // Higher level players have more effective disguises
        RE::PlayerCharacter* player = RE::PlayerCharacter::GetSingleton();
        float level = player ? static_cast<float>(player->GetLevel()) : 1.0f;
        level = std::clamp(level, 1.0f, 100.0f);

        float normalized = level / 100.0f;
        float curveFactor = std::pow(normalized, 1.5f);

        return baseWeight * (1.0f + curveFactor);
    }

    float GetTimeToLoseDetection() { return TIME_TO_LOSE_DETECTION; }
    void SetTimeToLoseDetection(float v) { TIME_TO_LOSE_DETECTION = v; }

    float GetDetectionThreshold() { return DETECTION_THRESHOLD; }
    void SetDetectionThreshold(float v) { DETECTION_THRESHOLD = v; }

    float GetDetectionRadius() { return DETECTION_RADIUS; }
    void SetDetectionRadius(float v) { DETECTION_RADIUS = v; }
}