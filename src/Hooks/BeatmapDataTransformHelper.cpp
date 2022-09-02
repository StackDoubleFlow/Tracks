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

void TracksAD::readBeatmapDataAD(CustomJSONData::CustomBeatmapData *beatmapData) {
    static auto *customObstacleDataClass = classof(CustomJSONData::CustomObstacleData *);
    static auto *customNoteDataClass = classof(CustomJSONData::CustomNoteData *);

    BeatmapAssociatedData &beatmapAD = getBeatmapAD(beatmapData->customData);
    bool v2 = beatmapData->v2orEarlier;

    CJDLogger::Logger.fmtLog<Paper::LogLevel::INF>("Reading beatmap ad");
    Paper::Logger::Backtrace(CJDLogger::Logger.tag, 20);

    if (beatmapAD.valid) {
        return;
    }

    if (beatmapData->customData->value) {
        rapidjson::Value const& customData = *beatmapData->customData->value;

        PointDefinitionManager pointDataManager;
        auto pointDefinitionsIt = customData.FindMember(v2 ? Constants::V2_POINT_DEFINITIONS.data() : Constants::POINT_DEFINITIONS.data());

        if (pointDefinitionsIt != customData.MemberEnd()) {
            const rapidjson::Value &pointDefinitions = pointDefinitionsIt->value;
            for (rapidjson::Value::ConstValueIterator itr = pointDefinitions.Begin(); itr != pointDefinitions.End(); itr++) {
                if (v2) {
                    std::string pointName = (*itr)[Constants::V2_NAME.data()].GetString();
                    PointDefinition pointData((*itr)[Constants::V2_POINTS.data()]);
                    CJDLogger::Logger.fmtLog<Paper::LogLevel::INF>("Constructed point {}", pointName);
                    pointDataManager.AddPoint(pointName, pointData);
                } else {
                    for (auto const& [name, pointDataVal] : pointDefinitionsIt->value.GetObject()) {
                        PointDefinition pointData(pointDataVal);
                        CJDLogger::Logger.fmtLog<Paper::LogLevel::INF>("Constructed point {}", name.GetString());
                        pointDataManager.AddPoint(name.GetString(), pointData);
                    }
                }
            }
        }
        TLogger::GetLogger().debug("Setting point definitions");
        beatmapAD.pointDefinitions = pointDataManager.pointData;
    }
    auto &tracks = beatmapAD.tracks;



    auto doForObjects = [&](auto&& objects) {
        for (auto *beatmapObjectData: objects) {
            if (!beatmapObjectData) continue;

            CustomJSONData::JSONWrapper *customDataWrapper;
            if (beatmapObjectData->klass == customObstacleDataClass) {
                auto obstacleData =
                        (CustomJSONData::CustomObstacleData *) beatmapObjectData;
                customDataWrapper = obstacleData->customData;
            } else if (beatmapObjectData->klass == customNoteDataClass) {
                auto noteData =
                        (CustomJSONData::CustomNoteData *) beatmapObjectData;
                customDataWrapper = noteData->customData;
            } else {
                continue;
            }

            if (customDataWrapper->value) {
                rapidjson::Value const &customData = *customDataWrapper->value;
                BeatmapObjectAssociatedData &ad = getAD(customDataWrapper);
                TracksVector tracksAD;

                auto trackIt = customData.FindMember(v2 ? Constants::V2_TRACK.data() : Constants::TRACK.data());
                if (trackIt != customData.MemberEnd()) {
                    rapidjson::Value const &tracksObject = trackIt->value;


                    switch (tracksObject.GetType()) {
                        case rapidjson::Type::kArrayType: {
                            if (tracksObject.Empty())
                                break;

                            for (auto &trackElement: tracksObject.GetArray()) {
                                Track *track = &tracks.try_emplace(trackElement.GetString(), v2).first->second;
                                tracksAD.emplace_back(track);
                            }
                            break;
                        }
                        case rapidjson::Type::kStringType: {
                            Track *track = &tracks.try_emplace(tracksObject.GetString(), v2).first->second;
                            tracksAD.emplace_back(track);
                            break;
                        }

                        default: {
                            TLogger::GetLogger().error("Tracks object is not an array or a string, what? Why?");
                            break;
                        }
                    }
                }

                ad.tracks = tracksAD;
            }

        }
    };
    auto notes = beatmapData->GetBeatmapItemsCpp<NoteData*>();
    auto obstacles = beatmapData->GetBeatmapItemsCpp<ObstacleData*>();

    doForObjects(notes);
    doForObjects(obstacles);

    for (auto const& customEventData : beatmapData->GetBeatmapItemsCpp<CustomJSONData::CustomEventData*>()) {
        if(!customEventData) continue;
        LoadTrackEvent(customEventData, beatmapAD, beatmapData->v2orEarlier);
    }

    beatmapAD.valid = true;
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
//    INSTALL_HOOK(logger, BeatmapDataTransformHelper_CreateTransformedBeatmapData);
}
TInstallHooks(InstallBeatmapDataTransformHelperHooks)