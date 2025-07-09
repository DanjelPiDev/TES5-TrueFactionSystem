#include "Managers/EnvironmentManager.h"
#include "Globals.h"


namespace NPE {
    bool EnvironmentManager::IsPlayerInDarkArea(RE::Actor *player) { return GetPlayerDarknessFactor(player) > 0.5f; }

    bool EnvironmentManager::IsPlayerNearLightSource(RE::Actor *player, float radius) {
        if (!player) return false;
        auto *cell = player->GetParentCell();
        if (!cell) return false;
        RE::NiPoint3 pos = player->GetPosition();

        bool found = false;
        cell->ForEachReferenceInRange(pos, radius, [&](RE::TESObjectREFR &ref) {
            if (ref.As<RE::TESObjectLIGH>() && ref.As<RE::TESObjectLIGH>()->data.radius > 0) {
                found = true;
                return RE::BSContainer::ForEachResult::kStop;
            }
            return RE::BSContainer::ForEachResult::kContinue;
        });
        return found;
    }

    bool EnvironmentManager::IsInLineOfSight(RE::Actor *npc, RE::Actor *player) {
        if (!npc || !player) {
            return false;
        }

        bool hasLineOfSight = false;
        bool isDetected = npc->HasLineOfSight(player->AsReference(), hasLineOfSight);
        if (isDetected) {
            spdlog::debug("NPC has line of sight to player");
        }
        return isDetected;
    }

    bool EnvironmentManager::IsNightTime() {
        RE::Sky *sky = RE::Sky::GetSingleton();
        if (!sky) return false;
        float currentTime = sky->currentGameHour;
        return (currentTime < 6.0f) || (currentTime > 18.0f);
    }

    bool EnvironmentManager::IsInFieldOfView(RE::Actor *npc, RE::Actor *player, float fieldOfViewDegrees) {
        if (!npc || !player) {
            return false;
        }

        float distance = npc->GetPosition().GetDistance(player->GetPosition());
        if (distance > DETECTION_RADIUS) {
            return false;
        }

        // Get the NPC's rotation in radians
        float npcRotationZ = npc->data.angle.z;
        RE::NiPoint3 npcForward(std::cos(npcRotationZ), std::sin(npcRotationZ), 0.0f);

        RE::NiPoint3 npcToPlayer = player->GetPosition() - npc->GetPosition();
        float length = npcToPlayer.Length();
        if (length > 0.0f) {
            npcToPlayer /= length;
        }

        float dotProduct = npcForward.Dot(npcToPlayer);
        dotProduct = std::clamp(dotProduct, -1.0f, 1.0f);

        // Field of view factor (cosine of half of the FOV in radians)
        float fovCosine = std::cos((fieldOfViewDegrees / 2.0f) * (M_PI / 180.0f));

        return dotProduct >= fovCosine;
    }

    bool EnvironmentManager::IsBadWeather(RE::TESWeather *weather) {
        // Check if the weather is rainy, foggy, or cloudy (bad for visibility)
        if (weather) {
            if (weather->data.flags & RE::TESWeather::WeatherDataFlag::kRainy ||
                weather->data.flags & RE::TESWeather::WeatherDataFlag::kCloudy ||
                weather->data.flags & RE::TESWeather::WeatherDataFlag::kSnow) {
                return true;
            }
        }
        return false;
    }

    float EnvironmentManager::AdjustProbabilityByDistance(float detectionProbability, float distance,
                                                          float maxDistance) {
        float distanceFactor = 1.0f - (distance / maxDistance);  // Scaled by 1 (near) to 0 (far)
        return detectionProbability * distanceFactor;
    }

    void EnvironmentManager::CheckHoursPassed(RE::Actor *npc, RE::Actor *player, RE::TESFaction *faction) {
        RE::FormID npcID = npc->GetFormID();
        float currentInGameHours = RE::Calendar::GetSingleton()->GetHoursPassed();

        if (recognizedNPCs.find(npcID) != recognizedNPCs.end()) {
            NPCDetectionData &detectionData = recognizedNPCs[npcID];

            float timeSinceLastDetected = currentInGameHours - detectionData.lastDetectedTime;

            if (timeSinceLastDetected > TIME_TO_LOSE_DETECTION) {
                recognizedNPCs.erase(npcID);
                if (playerDisguiseStatus.GetDisguiseValue(faction) > 5.0f) {
                    player->AddToFaction(faction, 1);
                }
            }
        }
    }

    float EnvironmentManager::GetEnvironmentalDetectionModifier(RE::Actor *player) {
        if (this->IsPlayerInDarkArea(player)) {
            return 0.5f;  // Reduce detection in dark areas
        }
        if (this->IsPlayerNearLightSource(player, 5.0f)) {
            return 1.5f;  // Increase detection near light sources
        }
        return 1.0f;
    }

    float EnvironmentManager::GetPlayerDarknessFactor(RE::Actor *player) {
        if (!player) {
            return 0.0f;  // safety: fully lit
        }

        auto *cell = player->GetParentCell();
        if (cell && cell->IsInteriorCell()) {
            // Interior lighting: sum RGB ambient and map to [0...1]
            auto *lighting = cell->GetLighting();
            float totalAmbient = lighting->ambient.red + lighting->ambient.green + lighting->ambient.blue;
            float norm = std::clamp(totalAmbient / 300.0f, 0.0f, 1.0f);

            return 1.0f - norm;  // 0 = bright, 1 = very dark
        }

        // --- Exterior: time of day, weather, nearby lights
        float factor = 0.0f;
        auto *sky = RE::Sky::GetSingleton();
        float hour = sky ? sky->currentGameHour : 12.0f;

        // map [6h...18h]->0, [0...6] and [18...24]->1, with linear fades
        if (hour < 6.0f) {
            factor = 1.0f;
        } else if (hour < 12.0f) {
            factor = 1.0f - (hour - 6.0f) / 6.0f;
        } else if (hour < 18.0f) {
            factor = (hour - 12.0f) / 6.0f;
        } else {
            factor = 1.0f;
        }

        if (IsBadWeather(sky ? sky->currentWeather : nullptr)) {
            factor = std::min(1.0f, factor + 0.3f);
        }
        if (IsPlayerNearLightSource(player, 50.0f)) {
            factor = std::max(0.0f, factor - 0.5f);
        }

        return std::clamp(factor, 0.0f, 1.0f);
    }

}