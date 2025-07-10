#include "Misc/Utils.h"


namespace NPE {
    std::string GetCurrentDLLPath() {
        HMODULE hModule = NULL;
        if (GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                               reinterpret_cast<LPCSTR>(&GetCurrentDLLPath), &hModule)) {
            char dllPath[MAX_PATH];
            DWORD result = GetModuleFileNameA(hModule, dllPath, MAX_PATH);
            if (result != 0) {
                return std::string(dllPath);
            } else {
                spdlog::error("Failed to get DLL path.");
            }
        } else {
            spdlog::error("Failed to get DLL path.");
        }
        return "";
    }

    float CalculateSlotWeight(RE::BGSBipedObjectForm::BipedObjectSlot slot, RE::Actor* actor) {
        int level = actor->GetLevel();
        float base = 0.0f;
        switch (slot) {
            case RE::BGSBipedObjectForm::BipedObjectSlot::kBody:
                base = 25.0f;
                break;
            case RE::BGSBipedObjectForm::BipedObjectSlot::kHands:
                base = 10.0f;
                break;
            case RE::BGSBipedObjectForm::BipedObjectSlot::kFeet:
                base = 10.0f;
                break;
            case RE::BGSBipedObjectForm::BipedObjectSlot::kForearms:
                base = 5.0f;
                break;
            case RE::BGSBipedObjectForm::BipedObjectSlot::kCirclet:
                base = 3.0f;
                break;
            case RE::BGSBipedObjectForm::BipedObjectSlot::kHead:
                base = 15.0f;
                break;
            case RE::BGSBipedObjectForm::BipedObjectSlot::kHair:
                base = 2.0f;
                break;
            default:
                base = 5.0f;
                break;
        }
        // Weight is increased by 1% per level
        return base * (1.0f + 0.01f * level);
    }
}