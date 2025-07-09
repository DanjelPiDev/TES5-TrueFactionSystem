#pragma once

#include "Race.h"
#include "Disguise.h"
#include "Faction.h"
#include "Combat.h"
#include "Papyrus.h"

#include "RE/B/BSFixedString.h"

#include "SKSE/Trampoline.h"


extern std::vector<RE::TESFaction*> g_allFactions;

// Function to start a background task for periodically updating disguise values and NPC detection
void StartBackgroundTask(RE::Actor* player);

void StopBackgroundTask();

void SaveCallback(SKSE::SerializationInterface* a_intfc);

void LoadCallback(SKSE::SerializationInterface* a_intfc);