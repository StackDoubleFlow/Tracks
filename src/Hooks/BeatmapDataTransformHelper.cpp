#include "THooks.h"
#include "beatsaber-hook/shared/utils/hooking.hpp"
#include "Animation/Events.h"

#include "AssociatedData.h"

#include "GlobalNamespace/BeatmapDataTransformHelper.hpp"
#include "GlobalNamespace/EnvironmentEffectsFilterPreset.hpp"

#include "custom-json-data/shared/CustomBeatmapData.h"

using namespace TracksAD;
using namespace GlobalNamespace;

MAKE_HOOK_MATCH(BeatmapDataTransformHelper_CreateTransformedBeatmapData,&BeatmapDataTransformHelper::CreateTransformedBeatmapData, GlobalNamespace::IReadonlyBeatmapData*,
                IReadonlyBeatmapData* beatmapData, GlobalNamespace::IPreviewBeatmapLevel* beatmapLevel,
                GlobalNamespace::GameplayModifiers* gameplayModifiers, GlobalNamespace::PracticeSettings* practiceSettings,
                bool leftHanded, GlobalNamespace::EnvironmentEffectsFilterPreset environmentEffectsFilterPreset,
                GlobalNamespace::EnvironmentIntensityReductionOptions* environmentIntensityReductionOptions,
                bool screenDisplacementEffectsEnabled) {
    auto result = (CustomJSONData::CustomBeatmapData*) BeatmapDataTransformHelper_CreateTransformedBeatmapData(beatmapData, beatmapLevel, gameplayModifiers, practiceSettings, leftHanded, environmentEffectsFilterPreset, environmentIntensityReductionOptions, screenDisplacementEffectsEnabled);

    auto &beatmapAD = TracksAD::getBeatmapAD(result->customData);

    if (!beatmapAD.valid) {
        TracksAD::readBeatmapDataAD(result);
    }


    auto &tracks = beatmapAD.tracks;

    for (auto const& customEventData : *result->customEventsData) {

        EventType type;
        if (customEventData.type == "AnimateTrack") {
            type = EventType::animateTrack;
        } else if (customEventData.type == "AssignPathAnimation") {
            type = EventType::assignPathAnimation;
        } else {
            continue;
        }

        auto &eventAD = getEventAD(&customEventData);
        rapidjson::Value& eventData = *customEventData.data;

        eventAD.type = type;
        eventAD.track = &beatmapAD.tracks[eventData["_track"].GetString()];
        eventAD.duration = eventData.HasMember("_duration") ? eventData["_duration"].GetFloat() : 0;
        eventAD.easing = eventData.HasMember("_easing") ? FunctionFromStr(eventData["_easing"].GetString()) : Functions::easeLinear;

        auto& properties = eventAD.track->properties;
        auto &pathProperties = eventAD.track->pathProperties;

        switch (eventAD.type) {
            case EventType::animateTrack:{
                eventAD.animateTrackData = AnimateTrackData(beatmapAD, eventData, properties);
                break;
            }
            case EventType::assignPathAnimation:{
                eventAD.assignPathAnimation = AssignPathAnimationData(beatmapAD, eventData, pathProperties);
                break;
            }
            default:
                break;
        }

    }

    return reinterpret_cast<IReadonlyBeatmapData *>(result);
}

void InstallBeatmapDataTransformHelperHooks(Logger& logger) {
    INSTALL_HOOK(logger, BeatmapDataTransformHelper_CreateTransformedBeatmapData);
}
TInstallHooks(InstallBeatmapDataTransformHelperHooks)