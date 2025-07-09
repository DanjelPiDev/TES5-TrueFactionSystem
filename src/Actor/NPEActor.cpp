# include "Actor/NPEActor.h"



namespace NPE {
    NPEActor::NPEActor(RE::Actor* a_actor) : actor(a_actor) {
        if (!actor) throw std::invalid_argument("Actor cannot be null");
    }

    RE::Actor* NPEActor::Get() { return actor; }

    NPCAlertness NPEActor::GetAlertness() const { return alertness; }

    void NPEActor::SetAlertness(NPCAlertness newAlertness) {
        if (alertness != newAlertness) {
            alertness = newAlertness;

            switch (alertness) {
                case NPCAlertness::RELAXED:
                    // NPC is relaxed, possibly decrease awareness? Maybe play an animation, or have them return to
                    // their patrol route
                    break;
                case NPCAlertness::SUSPICIOUS:
                    // NPC is suspicious, increase awareness, maybe play an animation
                    break;
                case NPCAlertness::ALERTED:
                    // NPC is fully alerted, they should be on high alert and potentially engage in combat (Or
                    // dialogue?)
                    // TODO: Implement StartCombatWith logic
                    // StartCombatWith(actor->GetTarget());
                    break;
            }
        }
    }

    void NPEActor::UpdateAlertnessBasedOnCombat() {
        if (actor->IsInCombat()) {
            SetAlertness(NPCAlertness::ALERTED);
        } else if (this->IsSuspicious()) {  // Assuming a method to check suspicion level
            SetAlertness(NPCAlertness::SUSPICIOUS);
        } else {
            SetAlertness(NPCAlertness::RELAXED);
        }
    }

    bool NPEActor::IsHostileTo(RE::Actor* target) { return false; }

    bool NPEActor::IsSuspicious() { return false; }

    void NPEActor::StartCombatWith(RE::Actor* target) {}

    // Method to set the target as hostile
    void NPEActor::SetHostile(RE::Actor* target) {}

    void NPEActor::RemoveFromFaction(RE::TESFaction* faction) { actor->AddToFaction(faction, -1); }

}