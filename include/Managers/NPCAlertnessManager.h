#pragma once
#include "Misc/Config.h"
#include "Actor/NPEActor.h"


namespace NPE {
    class NPCAlertnessManager {
    public:
        static NPCAlertnessManager &GetInstance() {
            static NPCAlertnessManager instance;
            return instance;
        }
        void UpdateNPCAlertness(RE::Actor *npc, bool recognizesPlayer);
        float GetNPCDetectionModifier(RE::Actor *npc);
        void AdjustDetectionProbabilityBasedOnAlertness(NPCAlertness alertness, float &detectionProbability);

    private:
        NPCAlertnessManager() = default;
    };
}
