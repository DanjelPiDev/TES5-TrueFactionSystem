#pragma once

namespace NPE {
    enum class NPCAlertness { RELAXED, SUSPICIOUS, ALERTED };

    class NPEActor {
    public:
        explicit NPEActor(RE::Actor* a_actor);
        RE::Actor* Get();

        void StartCombatWith(RE::Actor* target);
        bool IsHostileTo(RE::Actor* target);
        bool IsSuspicious();
        void SetHostile(RE::Actor* target);

        // Methods to get and set the NPC's alertness
        NPCAlertness GetAlertness() const;
        void SetAlertness(NPCAlertness newAlertness);
        void UpdateAlertnessBasedOnCombat();
        void RemoveFromFaction(RE::TESFaction* faction);
    private:
        RE::Actor* actor;
        NPCAlertness alertness = NPCAlertness::RELAXED;
    };
}