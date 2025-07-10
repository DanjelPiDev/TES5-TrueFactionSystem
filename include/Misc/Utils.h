#pragma once


namespace NPE {
    std::string GetCurrentDLLPath();
    float CalculateSlotWeight(RE::BGSBipedObjectForm::BipedObjectSlot slot, RE::Actor* actor);
}