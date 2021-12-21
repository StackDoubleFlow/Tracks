#include "THooks.h"
#include "beatsaber-hook/shared/utils/hooking.hpp"
#include "Animation/Events.h"

#include "AssociatedData.h"

#include "GlobalNamespace/BeatmapDataTransformHelper.hpp"
#include "GlobalNamespace/EnvironmentEffectsFilterPreset.hpp"

#include "custom-json-data/shared/CustomBeatmapData.h"

using namespace TracksAD;
using namespace GlobalNamespace;

void LoadTrackEvent(CustomJSONData::CustomEventData const* customEventData, TracksAD::BeatmapAssociatedData& beatmapAD) {
    static std::hash<std::string_view> stringViewHash;
    auto typeHash = stringViewHash(customEventData->type);

#define TYPE_GET(jsonName, varName)                                \
    static auto jsonNameHash_##varName = stringViewHash(jsonName); \

    TYPE_GET("AnimateTrack", AnimateTrack)
    TYPE_GET("AssignPathAnimation", AssignPathAnimation)

    EventType type;
    if (typeHash == jsonNameHash_AnimateTrack) {
        type = EventType::animateTrack;
    } else if (typeHash == jsonNameHash_AssignPathAnimation) {
        type = EventType::assignPathAnimation;
    } else {
        return;
    }

    auto &eventAD = getEventAD(customEventData);

    if (eventAD.parsed)
        return;

    rapidjson::Value& eventData = *customEventData->data;

    eventAD.type = type;
    auto const& trackJSON = eventData["_track"];
    unsigned int trackSize = trackJSON.IsArray() ? trackJSON.Size() : 1;


    std::vector<Track*> tracks;
    tracks.reserve(trackSize);

    if (trackJSON.IsArray()) {
        for (auto const& track : trackJSON.GetArray()) {
            if (!track.IsString()) {
                TLogger::GetLogger().debug("Track in array is not a string, why?");
                continue;
            }

            tracks.emplace_back(&beatmapAD.tracks[track.GetString()]);
        }
    } else if (trackJSON.IsString()) {
        tracks.emplace_back(&beatmapAD.tracks[trackJSON.GetString()]);
    } else {
        TLogger::GetLogger().debug("Track object is not a string or array, why?");
        eventAD.type = EventType::unknown;
        return;
    }

    eventAD.tracks = std::move(tracks);
    eventAD.duration = eventData.HasMember("_duration") ? eventData["_duration"].GetFloat() : 0;
    eventAD.easing = eventData.HasMember("_easing") ? FunctionFromStr(eventData["_easing"].GetString()) : Functions::easeLinear;

    for (auto const& track : eventAD.tracks) {
        auto &properties = track->properties;
        auto &pathProperties = track->pathProperties;

        switch (eventAD.type) {
            case EventType::animateTrack: {
                eventAD.animateTrackData.emplace_back(beatmapAD, eventData, properties);
                break;
            }
            case EventType::assignPathAnimation: {
                eventAD.assignPathAnimation.emplace_back(beatmapAD, eventData, pathProperties);
                break;
            }
            default:
                break;
        }
    }

    eventAD.parsed = true;
}

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


    for (auto const& customEventData : *result->customEventsData) {
        LoadTrackEvent(&customEventData, beatmapAD);
    }

    return reinterpret_cast<IReadonlyBeatmapData *>(result);
}

void InstallBeatmapDataTransformHelperHooks(Logger& logger) {
    INSTALL_HOOK(logger, BeatmapDataTransformHelper_CreateTransformedBeatmapData);
}
TInstallHooks(InstallBeatmapDataTransformHelperHooks)