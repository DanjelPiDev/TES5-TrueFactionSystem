#include "Globals.h"



namespace NPE {
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
    RE::TESDataHandler* g_dataHandler = RE::TESDataHandler::GetSingleton();

    const std::vector<ArmorSlot> armorSlotsSlot = {
        {RE::BGSBipedObjectForm::BipedObjectSlot::kBody, CHEST_WEIGHT},
        {RE::BGSBipedObjectForm::BipedObjectSlot::kHands, GLOVES_WEIGHT},
        {RE::BGSBipedObjectForm::BipedObjectSlot::kForearms, FOREARMS_WEIGHT},
        {RE::BGSBipedObjectForm::BipedObjectSlot::kCirclet, CIRCLET_WEIGHT},
        {RE::BGSBipedObjectForm::BipedObjectSlot::kFeet, SHOES_WEIGHT},
        {RE::BGSBipedObjectForm::BipedObjectSlot::kHead, HELMET_WEIGHT},
        {RE::BGSBipedObjectForm::BipedObjectSlot::kHair, HAIR_WEIGHT}};

    std::unordered_map<std::string, std::unordered_map<std::string, int>> factionRaceData;
}