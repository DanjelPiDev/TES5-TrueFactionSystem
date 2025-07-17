#include "Hooks/FactionHook.h"


namespace NPE {
    FactionHook::AddToFactionFunc FactionHook::_originalAddToFaction = nullptr;

    void FactionHook::Install() {
        constexpr std::ptrdiff_t AddToFactionIndex = 0x5A;  // 0x2D0 / sizeof(void*)

        REL::Relocation<std::uintptr_t> vtbl{RE::VTABLE_Actor[0]};
        auto vtablePtr = reinterpret_cast<std::uintptr_t*>(vtbl.address());

        // Save original function using union trick
        union {
            std::uintptr_t raw;
            AddToFactionFunc method;
        } caster;

        caster.raw = vtablePtr[AddToFactionIndex];
        _originalAddToFaction = caster.method;

        spdlog::info("[HOOK] Installed AddToFaction VTable hook at index {}", AddToFactionIndex);
    }

    void FactionHook::HookedAddToFaction(RE::Actor* actor, RE::TESFaction* faction, std::int8_t rank) {
        if (actor && faction) {
            if (actor->IsPlayerRef()) {
                spdlog::info("[HOOK] Player added to faction: {}", faction->GetName());
            } else {
                spdlog::debug("[HOOK] {} added to faction: {}", actor->GetName(), faction->GetName());
            }
            // Call original function
            (actor->*_originalAddToFaction)(faction, rank);
        } else {
            spdlog::error("[HOOK] Actor is null or faction is null in HookedAddToFaction");
        }
    }
}