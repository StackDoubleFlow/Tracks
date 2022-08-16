#include "THooks.h"
#include "beatsaber-hook/shared/utils/hooking.hpp"
#include "Animation/Events.h"

#include "AssociatedData.h"

#include "GlobalNamespace/BeatmapDataTransformHelper.hpp"
#include "GlobalNamespace/EnvironmentEffectsFilterPreset.hpp"
#include "GlobalNamespace/GameplayModifiers.hpp"

#include "custom-json-data/shared/CustomBeatmapData.h"

using namespace TracksAD;
using namespace GlobalNamespace;

constexpr static float getFloat(rapidjson::Value const& value) {
    switch (value.GetType()) {
        case rapidjson::kStringType:
            return std::stof(value.GetString());
        case rapidjson::kNumberType:
            return value.GetFloat();
        default:
            throw std::runtime_error(&"Not valid type in JSON doc " [ value.GetType()]);
    }
}

void LoadTrackEvent(CustomJSONData::CustomEventData const *customEventData, TracksAD::BeatmapAssociatedData &beatmapAD,
                    bool v2) {
    CRASH_UNLESS(beatmapAD.valid);

    auto typeHash = customEventData->typeHash;

#define TYPE_GET(jsonName, varName)                                \
    static auto jsonNameHash_##varName = std::hash<std::string_view>()(jsonName); \

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

    eventAD.parsed = true;

    rapidjson::Value const& eventData = *customEventData->data;

    eventAD.type = type;
    auto const& trackJSON = eventData[(v2 ? TracksAD::Constants::V2_TRACK : TracksAD::Constants::TRACK).data()];
    unsigned int trackSize = trackJSON.IsArray() ? trackJSON.Size() : 1;


    sbo::small_vector<Track*, 1> tracks;
    tracks.reserve(trackSize);

    if (trackJSON.IsArray()) {
        for (auto const& track : trackJSON.GetArray()) {
            if (!track.IsString()) {
                TLogger::GetLogger().debug("Track in array is not a string, why?");
                continue;
            }

            tracks.emplace_back(&beatmapAD.tracks.try_emplace(track.GetString(), v2).first->second);
        }
    } else if (trackJSON.IsString()) {
        tracks.emplace_back(&beatmapAD.tracks.try_emplace(trackJSON.GetString(), v2).first->second);
    } else {
        TLogger::GetLogger().debug("Track object is not a string or array, why?");
        eventAD.type = EventType::unknown;
        return;
    }

    eventAD.tracks = std::move(tracks);
    auto durationIt = eventData.FindMember((v2 ? TracksAD::Constants::V2_DURATION : TracksAD::Constants::DURATION).data());
    auto easingIt = eventData.FindMember((v2 ? TracksAD::Constants::V2_EASING : TracksAD::Constants::EASING).data());
    auto repeatIt = eventData.FindMember(TracksAD::Constants::REPEAT.data());

    eventAD.duration = durationIt != eventData.MemberEnd() ? getFloat(durationIt->value) : 0;
    eventAD.repeat = eventAD.duration > 0 && repeatIt != eventData.MemberEnd() ? repeatIt->value.GetInt() : 0;
    eventAD.easing = easingIt != eventData.MemberEnd() ? FunctionFromStr(easingIt->value.GetString()) : Functions::easeLinear;

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
}

MAKE_HOOK_MATCH(BeatmapDataTransformHelper_CreateTransformedBeatmapData,&BeatmapDataTransformHelper::CreateTransformedBeatmapData, GlobalNamespace::IReadonlyBeatmapData*,
                ::GlobalNamespace::IReadonlyBeatmapData* beatmapData,
                ::GlobalNamespace::IPreviewBeatmapLevel* beatmapLevel,
                ::GlobalNamespace::GameplayModifiers* gameplayModifiers,
                bool leftHanded, ::GlobalNamespace::EnvironmentEffectsFilterPreset environmentEffectsFilterPreset,
                ::GlobalNamespace::EnvironmentIntensityReductionOptions* environmentIntensityReductionOptions,
                ::GlobalNamespace::MainSettingsModelSO* mainSettingsModel
                ) {
    auto result = il2cpp_utils::cast<CustomJSONData::CustomBeatmapData>(
            BeatmapDataTransformHelper_CreateTransformedBeatmapData(beatmapData, beatmapLevel, gameplayModifiers, leftHanded,
                                                                    environmentEffectsFilterPreset, environmentIntensityReductionOptions,
                                                                    mainSettingsModel));

    auto &beatmapAD = TracksAD::getBeatmapAD(result->customData);

    if (!beatmapAD.valid) {
        TracksAD::readBeatmapDataAD(result);
    }


    for (auto const& customEventData : result->GetBeatmapItemsCpp<CustomJSONData::CustomEventData*>()) {
        if(!customEventData) continue;
        LoadTrackEvent(customEventData, beatmapAD, result->v2orEarlier);
    }

    return result->i_IReadonlyBeatmapData();
}

void InstallBeatmapDataTransformHelperHooks(Logger& logger) {
    INSTALL_HOOK(logger, BeatmapDataTransformHelper_CreateTransformedBeatmapData);
}
TInstallHooks(InstallBeatmapDataTransformHelperHooks)