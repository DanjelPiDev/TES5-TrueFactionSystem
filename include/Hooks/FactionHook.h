#pragma once

#include "RE/Skyrim.h"
#include "REL/Relocation.h"

namespace NPE {
    class FactionHook {
    public:
        static void Install();
        static void HookedAddToFaction(RE::Actor* actor, RE::TESFaction* faction, std::int8_t rank);
    private:
        using AddToFactionFunc = void (RE::Actor::*)(RE::TESFaction*, std::int8_t);
        static AddToFactionFunc _originalAddToFaction;
    };
}
