#include "THooks.h"
#include "beatsaber-hook/shared/utils/hooking.hpp"
#include "custom-json-data/shared/CustomBeatmapData.h"

#include "GlobalNamespace/StandardLevelScenesTransitionSetupDataSO.hpp"
#include "GlobalNamespace/IDifficultyBeatmap.hpp"
#include "GlobalNamespace/PlayerSpecificSettings.hpp"
#include "AssociatedData.h"

using namespace TracksAD;
using namespace CustomJSONData;
using namespace GlobalNamespace;

MAKE_HOOK_MATCH(StandardLevelScenesTransitionSetupDataSO_Init, &StandardLevelScenesTransitionSetupDataSO::Init, void, StandardLevelScenesTransitionSetupDataSO *self, StringW gameMode, IDifficultyBeatmap *difficultyBeatmap, IPreviewBeatmapLevel *previewBeatmapLevel, OverrideEnvironmentSettings *overrideEnvironmentSettings, ColorScheme *overrideColorScheme, GameplayModifiers *gameplayModifiers, PlayerSpecificSettings *playerSpecificSettings, PracticeSettings *practiceSettings, StringW backButtonText, bool useTestNoteCutSoundEffects) {
    StandardLevelScenesTransitionSetupDataSO_Init(self, gameMode, difficultyBeatmap, previewBeatmapLevel, overrideEnvironmentSettings, overrideColorScheme, gameplayModifiers, playerSpecificSettings, practiceSettings, backButtonText, useTestNoteCutSoundEffects);
    
    auto *beatmapData = reinterpret_cast<CustomBeatmapData*>(difficultyBeatmap->get_beatmapData());
    auto& ad = getBeatmapAD(beatmapData->customData);
    for (auto& [name, track] : ad.tracks) {
        track.ResetVariables();
    }
    ad.leftHanded = playerSpecificSettings->leftHanded;
    clearEventADs();
}

void InstallStandardLevelScenesTransitionSetupDataSOHooks(Logger& logger) {
    INSTALL_HOOK(logger, StandardLevelScenesTransitionSetupDataSO_Init);
}
TInstallHooks(InstallStandardLevelScenesTransitionSetupDataSOHooks);