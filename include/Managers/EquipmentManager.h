#pragma once
#include "Misc/Config.h"


namespace NPE {
    // Class for handling equip events
    class EquipEventHandler : public RE::BSTEventSink<RE::TESEquipEvent> {
    public:
        static EquipEventHandler &GetInstance() {
            static EquipEventHandler instance;
            return instance;
        }
        // Override method for processing equip events
        virtual RE::BSEventNotifyControl ProcessEvent(const RE::TESEquipEvent *evn,
                                                      RE::BSTEventSource<RE::TESEquipEvent> *dispatcher) override;

    private:
        EquipEventHandler() = default;
    };
}