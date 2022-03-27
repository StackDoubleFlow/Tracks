#include "THooks.h"
#include "beatsaber-hook/shared/utils/hooking.hpp"
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

using namespace TracksAD;
using namespace CustomJSONData;
using namespace GlobalNamespace;

SafePtr<System::Action_1<System::Threading::Tasks::Task*>> callbackOther;
SafePtr<System::Func_2<System::Threading::Tasks::Task_1<GlobalNamespace::IReadonlyBeatmapData*>*, GlobalNamespace::IReadonlyBeatmapData*>> callback;
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


    auto *beatmapData = reinterpret_cast<CustomBeatmapData*>(self->get_transformedBeatmapData());
    auto& ad = getBeatmapAD(beatmapData->customData);
    for (auto& [name, track] : ad.tracks) {
        track.ResetVariables();
    }
    ad.leftHanded = playerSpecificSettings->leftHanded;
    clearEventADs();
}

MAKE_HOOK_MATCH(GameplayCoreSceneSetupData_GetTransformedBeatmapDataAsync,
                &GameplayCoreSceneSetupData::GetTransformedBeatmapDataAsync,
                System::Threading::Tasks::Task_1<::GlobalNamespace::IReadonlyBeatmapData*>*, GameplayCoreSceneSetupData *self) {
    auto ret = GameplayCoreSceneSetupData_GetTransformedBeatmapDataAsync(self);

    gameplayCoreSceneSetupData = self;

    return ret->ContinueWith(callback.operator System::Func_2<System::Threading::Tasks::Task_1<GlobalNamespace::IReadonlyBeatmapData *> *, GlobalNamespace::IReadonlyBeatmapData *> *const());
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

    std::function<GlobalNamespace::IReadonlyBeatmapData*(System::Threading::Tasks::Task_1<::GlobalNamespace::IReadonlyBeatmapData*>*)>func = [](System::Threading::Tasks::Task_1<::GlobalNamespace::IReadonlyBeatmapData*>* t) {
        auto beatmap = t->get_Result();

        TLogger::GetLogger().debug("Casting %p", beatmap);
        TLogger::GetLogger().debug("Casting %s", il2cpp_utils::ClassStandardName(reinterpret_cast<Il2CppObject*>(beatmap)->klass).c_str());

        auto *beatmapData = il2cpp_utils::cast<CustomBeatmapData>(beatmap);
        TLogger::GetLogger().debug("Crashing %p", beatmapData);
        CRASH_UNLESS(beatmapData);

        auto& ad = getBeatmapAD(beatmapData->customData);
        for (auto& [name, track] : ad.tracks) {
            track.ResetVariables();
        }
        ad.leftHanded = gameplayCoreSceneSetupData->playerSpecificSettings->leftHanded;
        clearEventADs();

        return beatmap;
    };

    callback = il2cpp_utils::MakeDelegate<
            System::Func_2<System::Threading::Tasks::Task_1<GlobalNamespace::IReadonlyBeatmapData*>*, GlobalNamespace::IReadonlyBeatmapData*>*>(
            func
    );

    std::function<void(System::Threading::Tasks::Task*)> func2 = [](System::Threading::Tasks::Task*) {
        auto beatmap = gameplayCoreSceneSetupData->transformedBeatmapData;

        TLogger::GetLogger().debug("Casting %p", beatmap);
        TLogger::GetLogger().debug("Casting %s", il2cpp_utils::ClassStandardName(reinterpret_cast<Il2CppObject*>(beatmap)->klass).c_str());

        auto *beatmapData = il2cpp_utils::cast<CustomBeatmapData>(beatmap);
        TLogger::GetLogger().debug("Crashing %p", beatmapData);
        CRASH_UNLESS(beatmapData);

        auto& ad = getBeatmapAD(beatmapData->customData);
        for (auto& [name, track] : ad.tracks) {
            track.ResetVariables();
        }
        ad.leftHanded = gameplayCoreSceneSetupData->playerSpecificSettings->leftHanded;
        clearEventADs();
    };

    callbackOther = il2cpp_utils::MakeDelegate<
            System::Action_1<System::Threading::Tasks::Task*>*>(
            func2
    );

    // TODO: Fix

    INSTALL_HOOK(logger, LevelScenesTransitionSetupDataSO_BeforeScenesWillBeActivatedAsync);
//    INSTALL_HOOK(logger, StandardLevelScenesTransitionSetupDataSO_Init);
//    INSTALL_HOOK(logger, GameplayCoreSceneSetupData_GetTransformedBeatmapDataAsync);
}
TInstallHooks(InstallStandardLevelScenesTransitionSetupDataSOHooks);