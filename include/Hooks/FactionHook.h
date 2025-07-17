#pragma once

#include "RE/Skyrim.h"
#include "REL/Relocation.h"

namespace NPE {
    class FactionHook {
    public:
        static void Install();

    private:
        using AddToFactionFunc = void (RE::Actor::*)(RE::TESFaction*, std::int8_t);
        static AddToFactionFunc _originalAddToFaction;

        static void HookedAddToFaction(RE::Actor* actor, RE::TESFaction* faction, std::int8_t rank);
    };
}
