#include "THooks.h"
#include "beatsaber-hook/shared/utils/hooking.hpp"

#include "custom-types/shared/delegate.hpp"

#include "custom-json-data/shared/CustomBeatmapData.h"

#include "GlobalNamespace/StandardLevelScenesTransitionSetupDataSO.hpp"
#include "GlobalNamespace/GameplayCoreSceneSetupData.hpp"
#include "GlobalNamespace/IDifficultyBeatmap.hpp"
#include "GlobalNamespace/PlayerSpecificSettings.hpp"

#include "System/Threading/Tasks/Task.hpp"
#include "System/Threading/Tasks/Task_1.hpp"
#include "System/Action_1.hpp"
#include "System/Func_2.hpp"

#include "AssociatedData.h"
#include "custom-json-data/shared/CustomBeatmapSaveDatav3.h"
#include "StaticHolders.hpp"

using namespace TracksAD;
using namespace CustomJSONData;
using namespace GlobalNamespace;

SafePtr<System::Action_1<System::Threading::Tasks::Task*>> callbackOther;
GameplayCoreSceneSetupData * gameplayCoreSceneSetupData;

//#define MakeDelegate(DelegateType, ...) (il2cpp_utils::MakeDelegate<DelegateType>(classof(DelegateType), __VA_ARGS__))

MAKE_HOOK_MATCH(StandardLevelScenesTransitionSetupDataSO_Init, &StandardLevelScenesTransitionSetupDataSO::Init,
                void, StandardLevelScenesTransitionSetupDataSO *self, ::StringW gameMode,
                ::GlobalNamespace::IDifficultyBeatmap* difficultyBeatmap, ::GlobalNamespace::IPreviewBeatmapLevel* previewBeatmapLevel,
                ::GlobalNamespace::OverrideEnvironmentSettings* overrideEnvironmentSettings,
                ::GlobalNamespace::ColorScheme* overrideColorScheme, ::GlobalNamespace::GameplayModifiers* gameplayModifiers,
                ::GlobalNamespace::PlayerSpecificSettings* playerSpecificSettings, ::GlobalNamespace::PracticeSettings* practiceSettings,
                ::StringW backButtonText, bool useTestNoteCutSoundEffects, bool startPaused) {
    StandardLevelScenesTransitionSetupDataSO_Init(self, gameMode, difficultyBeatmap, previewBeatmapLevel, overrideEnvironmentSettings, overrideColorScheme,
                                                  gameplayModifiers, playerSpecificSettings, practiceSettings, backButtonText, useTestNoteCutSoundEffects, startPaused);

    TracksStatic::bpmController = nullptr;

    clearEventADs();
    TLogger::GetLogger().debug("Got beatmap %s", il2cpp_utils::ClassStandardName(reinterpret_cast<Il2CppObject*>(difficultyBeatmap)->klass).c_str());
}

MAKE_HOOK_MATCH(LevelScenesTransitionSetupDataSO_BeforeScenesWillBeActivatedAsync,
                &LevelScenesTransitionSetupDataSO::BeforeScenesWillBeActivatedAsync,
                System::Threading::Tasks::Task*, LevelScenesTransitionSetupDataSO *self) {
    auto ret = LevelScenesTransitionSetupDataSO_BeforeScenesWillBeActivatedAsync(self);
//    auto beatmap = gameplayCoreSceneSetupData->transformedBeatmapData;
//
//    TLogger::GetLogger().debug("Casting %p", beatmap);
//    TLogger::GetLogger().debug("Casting %s", il2cpp_utils::ClassStandardName(reinterpret_cast<Il2CppObject*>(beatmap)->klass).c_str());
//
//    auto *beatmapData = il2cpp_utils::cast<CustomBeatmapData>(beatmap);
//    TLogger::GetLogger().debug("Crashing %p", beatmapData);
//    CRASH_UNLESS(beatmapData);
//
//    auto& ad = getBeatmapAD(beatmapData->customData);
//    for (auto& [name, track] : ad.tracks) {
//        track.ResetVariables();
//    }
//    ad.leftHanded = gameplayCoreSceneSetupData->playerSpecificSettings->leftHanded;
//    clearEventADs();

    gameplayCoreSceneSetupData = self->gameplayCoreSceneSetupData;

    return ret->ContinueWith(callbackOther.operator System::Action_1<System::Threading::Tasks::Task *> *const());
}



void InstallStandardLevelScenesTransitionSetupDataSOHooks(Logger& logger){

    std::function<void(System::Threading::Tasks::Task*)> func2 = [](System::Threading::Tasks::Task*) {
        IL2CPP_CATCH_HANDLER(
            auto beatmap = gameplayCoreSceneSetupData->transformedBeatmapData;

            TLogger::GetLogger().debug("Casting %p", beatmap);
            TLogger::GetLogger().debug("Casting %s", il2cpp_utils::ClassStandardName(reinterpret_cast<Il2CppObject*>(beatmap)->klass).c_str());

            auto *beatmapData = il2cpp_utils::cast<CustomBeatmapData>(beatmap);
            TLogger::GetLogger().debug("Crashing %p", beatmapData);
            CRASH_UNLESS(beatmapData);
            TLogger::GetLogger().debug("Did not crash");

            auto& ad = getBeatmapAD(beatmapData->customData);
            for (auto& [name, track] : ad.tracks) {
                track.ResetVariables();
            }
            ad.leftHanded = gameplayCoreSceneSetupData->playerSpecificSettings->leftHanded;
            clearEventADs();
            TLogger::GetLogger().debug("Reset tracks sucessfully");
        )
    };

    callbackOther = custom_types::MakeDelegate<
            System::Action_1<System::Threading::Tasks::Task*>*>(
            func2
    );



//    INSTALL_HOOK(logger, LevelScenesTransitionSetupDataSO_BeforeScenesWillBeActivatedAsync);
//    INSTALL_HOOK(logger, StandardLevelScenesTransitionSetupDataSO_Init);
//    INSTALL_HOOK(logger, GameplayCoreSceneSetupData_GetTransformedBeatmapDataAsync);
}
TInstallHooks(InstallStandardLevelScenesTransitionSetupDataSOHooks);