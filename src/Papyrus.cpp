#include "Papyrus.h"
#include "Globals.h"


namespace NPE {
    bool PapyrusAddKeywordToArmor(RE::StaticFunctionTag *, RE::TESObjectARMO *armor, RE::BGSKeyword *keyword) {
        return AddKeywordToArmor(armor, keyword);
    }

    bool PapyrusRemoveKeywordFromArmor(RE::StaticFunctionTag *, RE::TESObjectARMO *armor, RE::BGSKeyword *keyword) {
        return RemoveKeywordFromArmor(armor, keyword);
    }

    RE::BGSKeyword *PapyrusGetKeywordByEditorID(RE::StaticFunctionTag *, RE::BSFixedString keyword) {
        return GetKeywordByEditorID(keyword);
    }

    std::vector<RE::TESFaction *> PapyrusGetFactionsForActor(RE::StaticFunctionTag *, RE::Actor *actor) {
        return GetFactionsForActor(actor);
    }

    float PapyrusGetDisguiseValueForFaction(RE::StaticFunctionTag *, RE::TESFaction *faction) {
        return GetDisguiseValueForFaction(faction);
    }

    RE::BSFixedString PapyrusGetFactionEditorID(RE::StaticFunctionTag *, RE::TESFaction *faction) {
        return GetFactionEditorID(faction);
    }

    float PapyrusGetDisguiseBonusValueForFaction(RE::StaticFunctionTag *, RE::TESFaction *faction) {
        return GetDisguiseBonusValueForFaction(faction);
    }

    RE::BGSKeyword *PapyrusHandleAddFactionFromMCM(RE::StaticFunctionTag *, RE::TESFaction *faction) {
        return HandleAddFactionFromMCM(faction);
    }

    std::vector<std::string> PapyrusGetAssignedKeywords(RE::StaticFunctionTag *) { return GetAssignedKeywords(); }

    std::vector<RE::TESFaction *> PapyrusGetAssignedFactions(RE::StaticFunctionTag *) { return GetAssignedFactions(); }

    std::vector<RE::TESFaction *> PapyrusGetAllFactions(RE::StaticFunctionTag *) { return NPE::filteredFactions; }

    bool PapyrusRemoveFactionKeywordAssignment(RE::StaticFunctionTag *, RE::BSFixedString keyword,
                                               RE::TESFaction *faction) {
        return RemoveFactionKeywordAssignment(keyword, faction);
    }

    float PapyrusGetRaceBonusValue(RE::StaticFunctionTag *, RE::TESFaction *faction) {
        return GetRaceBonusValueForFaction(faction);
    }

    float PapyrusGetTimeToLoseDetection(RE::StaticFunctionTag *) { return GetTimeToLoseDetection(); }
    void PapyrusSetTimeToLoseDetection(RE::StaticFunctionTag *, float v) { SetTimeToLoseDetection(v); }

    float PapyrusGetInvestigationThreshold(RE::StaticFunctionTag *) { return GetInvestigationThreshold(); }
    void PapyrusSetInvestigationThreshold(RE::StaticFunctionTag *, float v) { SetInvestigationThreshold(v); }

    float PapyrusGetDetectionThreshold(RE::StaticFunctionTag *) { return GetDetectionThreshold(); }
    void PapyrusSetDetectionThreshold(RE::StaticFunctionTag *, float v) { SetDetectionThreshold(v); }

    float PapyrusGetDetectionRadius(RE::StaticFunctionTag *) { return GetDetectionRadius(); }
    void PapyrusSetDetectionRadius(RE::StaticFunctionTag *, float v) { SetDetectionRadius(v); }

    bool PapyrusGetUseFOVCheck(RE::StaticFunctionTag *) { return GetUseFOVCheck(); }
    void PapyrusSetUseFOVCheck(RE::StaticFunctionTag *, bool b) { SetUseFOVCheck(b); }

    bool PapyrusGetUseLineOfSightCheck(RE::StaticFunctionTag *) { return GetUseLineOfSightCheck(); }
    void PapyrusSetUseLineOfSightCheck(RE::StaticFunctionTag *, bool b) { SetUseLineOfSightCheck(b); }

    float PapyrusGetFOVAngle(RE::StaticFunctionTag *) { return GetFOVAngle(); }
    void PapyrusSetFOVAngle(RE::StaticFunctionTag *, float angle) { SetFOVAngle(angle); }

    // Function to bind the Papyrus function
    bool RegisterPapyrusFunctions(RE::BSScript::IVirtualMachine *vm) {
        vm->RegisterFunction("AddKeywordToArmor", "npeTFS_NativeFunctions", PapyrusAddKeywordToArmor);
        vm->RegisterFunction("RemoveKeywordFromArmor", "npeTFS_NativeFunctions", PapyrusRemoveKeywordFromArmor);
        vm->RegisterFunction("GetKeywordByEditorID", "npeTFS_NativeFunctions", PapyrusGetKeywordByEditorID);
        vm->RegisterFunction("GetFactionsForActor", "npeTFS_NativeFunctions", PapyrusGetFactionsForActor);
        vm->RegisterFunction("GetDisguiseValueForFaction", "npeTFS_NativeFunctions", PapyrusGetDisguiseValueForFaction);
        vm->RegisterFunction("GetDisguiseBonusValueForFaction", "npeTFS_NativeFunctions", PapyrusGetDisguiseBonusValueForFaction);
        vm->RegisterFunction("GetFactionEditorID", "npeTFS_NativeFunctions", PapyrusGetFactionEditorID);
        vm->RegisterFunction("HandleAddFactionFromMCM", "npeTFS_NativeFunctions", PapyrusHandleAddFactionFromMCM);
        vm->RegisterFunction("GetAllFactions", "npeTFS_NativeFunctions", PapyrusGetAllFactions);
        vm->RegisterFunction("GetAssignedKeywords", "npeTFS_NativeFunctions", PapyrusGetAssignedKeywords);
        vm->RegisterFunction("GetAssignedFactions", "npeTFS_NativeFunctions", PapyrusGetAssignedFactions);
        vm->RegisterFunction("RemoveFactionKeywordAssignment", "npeTFS_NativeFunctions", PapyrusRemoveFactionKeywordAssignment);
        vm->RegisterFunction("GetRaceBonusValueForFaction", "npeTFS_NativeFunctions", PapyrusGetRaceBonusValue);
        vm->RegisterFunction("GetTimeToLoseDetection", "npeTFS_NativeFunctions", PapyrusGetTimeToLoseDetection);
        vm->RegisterFunction("SetTimeToLoseDetection", "npeTFS_NativeFunctions", PapyrusSetTimeToLoseDetection);
        vm->RegisterFunction("GetInvestigationThreshold", "npeTFS_NativeFunctions", PapyrusGetInvestigationThreshold);
        vm->RegisterFunction("SetInvestigationThreshold", "npeTFS_NativeFunctions", PapyrusSetInvestigationThreshold);
        vm->RegisterFunction("GetDetectionThreshold", "npeTFS_NativeFunctions", PapyrusGetDetectionThreshold);
        vm->RegisterFunction("SetDetectionThreshold", "npeTFS_NativeFunctions", PapyrusSetDetectionThreshold);
        vm->RegisterFunction("GetDetectionRadius", "npeTFS_NativeFunctions", PapyrusGetDetectionRadius);
        vm->RegisterFunction("SetDetectionRadius", "npeTFS_NativeFunctions", PapyrusSetDetectionRadius);
        vm->RegisterFunction("GetUseFOVCheck", "npeTFS_NativeFunctions", PapyrusGetUseFOVCheck);
        vm->RegisterFunction("SetUseFOVCheck", "npeTFS_NativeFunctions", PapyrusSetUseFOVCheck);
        vm->RegisterFunction("GetUseLineOfSightCheck", "npeTFS_NativeFunctions", PapyrusGetUseLineOfSightCheck);
        vm->RegisterFunction("SetUseLineOfSightCheck", "npeTFS_NativeFunctions", PapyrusSetUseLineOfSightCheck);
        vm->RegisterFunction("GetFOVAngle", "npeTFS_NativeFunctions", PapyrusGetFOVAngle);
        vm->RegisterFunction("SetFOVAngle", "npeTFS_NativeFunctions", PapyrusSetFOVAngle);
        return true;
    }
}