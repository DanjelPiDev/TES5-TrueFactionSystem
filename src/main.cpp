#include <cstdint>

#include "Main.h"
#include "Globals.h"


using namespace SKSE::stl;
using namespace SKSE;
using namespace RE;

std::chrono::steady_clock::time_point lastCheckTime;

std::chrono::steady_clock::time_point lastUpdateDisguiseCheckTime;
std::chrono::steady_clock::time_point lastCheckDetectionTime;
std::chrono::steady_clock::time_point lastRaceCheckTime;

static NPE::HitEventHandler hitEventHandler;

RE::TESDataHandler* dataHandler;
std::vector<RE::TESFaction*> allFactions;

enum : uint32_t {
    kRecordHeader = 'NPE1',         // Header: TIME_TO_LOSE_DETECTION, DETECTION_THRESHOLD, DETECTION_RADIUS, FOV_ANGLE, USE_FOV_CHECK, USE_LINE_OF_SIGHT_CHECK
    kRecordArmor = 'NPE2',          // ArmorKeywordData
    kRecordDetection = 'NPE3',      // recognizedNPCs
    kRecordDisguiseStatus = 'NPE4'  // PlayerDisguiseStatus
};

void StartBackgroundTask(Actor* player) {
    NPE::backgroundTaskRunning.store(true);

    NPE::backgroundTaskThread = std::make_unique<std::thread>([player]() {
        while (NPE::backgroundTaskRunning.load()) {
            if (player && player->IsPlayerRef()) {
                auto now = std::chrono::steady_clock::now();
                auto elapsed = now - lastCheckTime;

                auto elapsedDisguise = now - lastUpdateDisguiseCheckTime;
                auto elapsedDetection = now - lastCheckDetectionTime;
                auto elapsedRace = now - lastRaceCheckTime;

                if (elapsedDisguise >= NPE::UPDATE_DISGUISE_INTERVAL_SECONDS) {
                    NPE::disguiseManager.UpdateDisguiseValue(player);
                    NPE::CheckAndReAddPlayerToFaction(player);
                    lastUpdateDisguiseCheckTime = now;
                }
                if (elapsedDetection >= NPE::DETECTION_INTERVAL_SECONDS) {
                    NPE::detectionManager.CheckNPCDetection(player);
                    lastCheckDetectionTime = now;
                }
                if (elapsedRace >= NPE::RACE_CHECK_INTERVAL_SECONDS) {
                    NPE::InitRaceDisguiseBonus();
                    lastRaceCheckTime = now;
                }
                lastCheckTime = now;
            }
            std::this_thread::sleep_for(NPE::CHECK_INTERVAL_SECONDS);
        }
    });
}

void StopBackgroundTask() {
    NPE::backgroundTaskRunning.store(false);
    if (NPE::backgroundTaskThread && NPE::backgroundTaskThread->joinable()) {
        NPE::backgroundTaskThread->join();
    }
}
// Ensure StopBackgroundTask() runs when the DLL unloads (Like disabling the plugin by removing the dll, the task could still be in the memory?)
static BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID) {
    if (reason == DLL_PROCESS_DETACH) {
        StopBackgroundTask();
    }
    return TRUE;
}

static void SaveCallback(SKSE::SerializationInterface *intfc) {
    auto saveRecord = [&](uint32_t type, uint32_t version, auto &&writeFunc) {
        if (!intfc->OpenRecord(type, version)) {
            spdlog::error("SaveCallback: Failed to open record {:#X} v{}", type, version);
            return;
        }
        writeFunc();
    };

    // Header: floats and toggles
    saveRecord(kRecordHeader, 2, [&]() {
        intfc->WriteRecordData(&NPE::TIME_TO_LOSE_DETECTION, sizeof(float));
        intfc->WriteRecordData(&NPE::DETECTION_THRESHOLD, sizeof(float));
        intfc->WriteRecordData(&NPE::DETECTION_RADIUS, sizeof(float));
        intfc->WriteRecordData(&NPE::FOV_ANGLE, sizeof(float));
        intfc->WriteRecordData(&NPE::USE_FOV_CHECK, sizeof(bool));
        intfc->WriteRecordData(&NPE::USE_LINE_OF_SIGHT_CHECK, sizeof(bool));
    });

    // Armor-Keyword-Data
    saveRecord(kRecordArmor, 1, [&]() { NPE::Save(intfc); });

    // NPC-Detection-Data
    saveRecord(kRecordDetection, 1, [&]() { NPE::detectionManager.Save(intfc); });

    // Player-Disguise-Status
    saveRecord(kRecordDisguiseStatus, 1, [&]() { NPE::playerDisguiseStatus.Save(intfc); });
}

static void LoadCallback(SKSE::SerializationInterface *intfc) {
    uint32_t type, version, length;
    while (intfc->GetNextRecordInfo(type, version, length)) {
        switch (type) {
            case kRecordHeader: {
                if (version == 1) {
                    float tLose, tThresh;
                    if (intfc->ReadRecordData(&tLose, sizeof(tLose))) NPE::TIME_TO_LOSE_DETECTION = tLose;
                    if (intfc->ReadRecordData(&tThresh, sizeof(tThresh))) NPE::DETECTION_THRESHOLD = tThresh;
                    // Defaults for new variables (version 2)
                    NPE::DETECTION_RADIUS = 400.0f;
                    NPE::FOV_ANGLE = 120.0f;
                    NPE::USE_FOV_CHECK = true;
                    NPE::USE_LINE_OF_SIGHT_CHECK = true;
                } else if (version == 2) {
                    float tLose, tThresh, radius, angle;
                    bool fov, los;
                    if (intfc->ReadRecordData(&tLose, sizeof(tLose))) NPE::TIME_TO_LOSE_DETECTION = tLose;
                    if (intfc->ReadRecordData(&tThresh, sizeof(tThresh))) NPE::DETECTION_THRESHOLD = tThresh;
                    if (intfc->ReadRecordData(&radius, sizeof(radius))) NPE::DETECTION_RADIUS = radius;
                    if (intfc->ReadRecordData(&angle, sizeof(angle))) NPE::FOV_ANGLE = angle;
                    if (intfc->ReadRecordData(&fov, sizeof(fov))) NPE::USE_FOV_CHECK = fov;
                    if (intfc->ReadRecordData(&los, sizeof(los))) NPE::USE_LINE_OF_SIGHT_CHECK = los;
                }
                break;
            }
            case kRecordArmor:
                NPE::Load(intfc);
                break;
            case kRecordDetection:
                NPE::detectionManager.Load(intfc);
                break;
            case kRecordDisguiseStatus:
                NPE::playerDisguiseStatus.Load(intfc);
                break;
            default:
                break;
        }
    }
}

static void RevertCallback(SKSE::SerializationInterface *) {
    NPE::savedArmorKeywordAssociations.clear();
    NPE::recognizedNPCs.clear();
    NPE::playerDisguiseStatus.Clear();
}

std::vector<RE::TESFaction *> ConvertBSTArrayToVector(const RE::BSTArray<RE::TESFaction *> &bstArray) {
    std::vector<RE::TESFaction *> vector;

    for (std::uint32_t i = 0; i < bstArray.size(); ++i) {
        vector.push_back(bstArray[i]);
    }

    return vector;
}

static void InitializeGlobalData() {
    if (!dataHandler) {
        dataHandler = RE::TESDataHandler::GetSingleton();
    }
    const auto &bstFactions = dataHandler->GetFormArray<RE::TESFaction>();
    allFactions = ConvertBSTArrayToVector(bstFactions);

    NPE::filteredFactions.clear();
    for (RE::TESFaction *faction : allFactions) {
        if (faction && strcmp(faction->GetName(), "") != 0) {
            NPE::filteredFactions.push_back(faction);
        }
    }

    for (RE::TESFaction *faction : allFactions) {
        if (faction) {
            const char *editorID = faction->GetFormEditorID();
            if (editorID && strlen(editorID) > 0) {
                NPE::factionEditorIDCache[faction->GetFormID()] = RE::BSFixedString(editorID);
            } else {
                char buffer[32];
                snprintf(buffer, sizeof(buffer), "0x%08X", faction->GetFormID());
                NPE::factionEditorIDCache[faction->GetFormID()] = RE::BSFixedString(buffer);
            }
        }
    }
}

static void InitializeLogging() {
    auto path = logger::log_directory();
    if (!path) {
        report_and_fail("Unable to lookup SKSE logs directory.");
    }
    *path /= PluginDeclaration::GetSingleton()->GetName();
    *path += L".log";

    std::shared_ptr<spdlog::logger> log;
    log = std::make_shared<spdlog::logger>("Global", std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true));
    log->set_level(spdlog::level::debug);

    spdlog::set_default_logger(std::move(log));
    spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%n] [%l] [%t] [%s:%#] %v");
}

extern "C" [[maybe_unused]] __declspec(dllexport) bool SKSEPlugin_Load(const SKSE::LoadInterface *skse) {
    SKSE::Init(skse);
    SKSE::GetPapyrusInterface()->Register(NPE::RegisterPapyrusFunctions);

    auto serialization = SKSE::GetSerializationInterface();
    serialization->SetSaveCallback(SaveCallback);
    serialization->SetLoadCallback(LoadCallback);
    serialization->SetRevertCallback(RevertCallback);

    SKSE::GetMessagingInterface()->RegisterListener([](SKSE::MessagingInterface::Message *message) {
        if (message->type == SKSE::MessagingInterface::kDataLoaded) {
            InitializeLogging();

            spdlog::info("Loading in TFS...");
            spdlog::info("Loading in all Factions...");

            InitializeGlobalData();

            auto equipEventSource = RE::ScriptEventSourceHolder::GetSingleton();
            if (equipEventSource) {
                equipEventSource->AddEventSink(&NPE::equipEventHandler);
                spdlog::info("EquipEventHandler registered!");
            }

            auto hitEventSource = RE::ScriptEventSourceHolder::GetSingleton();
            if (hitEventSource) {
                hitEventSource->AddEventSink(&hitEventHandler);
                spdlog::info("HitEventHandler registered!");
            }

            Actor* player = PlayerCharacter::GetSingleton();
            if (player) {
                lastCheckTime = std::chrono::steady_clock::now();
                lastUpdateDisguiseCheckTime = lastCheckTime;
                lastCheckDetectionTime = lastCheckTime;
                lastRaceCheckTime = lastCheckTime;

                StartBackgroundTask(player);
            }

            spdlog::info("TFS successfully loaded!");
            spdlog::dump_backtrace();
            RE::ConsoleLog::GetSingleton()->Print("TFS successfully loaded!");
        }
    });

    return true;
}
