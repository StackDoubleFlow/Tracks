#include "TLogger.h"
#include "THooks.h"
#include "AssociatedData.h"
#include "Animation/PointDefinition.h"
#include "beatsaber-hook/shared/utils/hooking.hpp"
#include "custom-json-data/shared/CustomBeatmapData.h"
#include "GlobalNamespace/BeatmapLineData.hpp"

#include "GlobalNamespace/BeatmapData.hpp"
#include "GlobalNamespace/BeatmapDataLoader.hpp"

#include "System/Linq/Enumerable.hpp"

using namespace GlobalNamespace;
using namespace TracksAD;

void TracksAD::readBeatmapDataAD(CustomJSONData::CustomBeatmapData *beatmapData) {
    static auto *customObstacleDataClass = classof(CustomJSONData::CustomObstacleData *);
    static auto *customNoteDataClass = classof(CustomJSONData::CustomNoteData *);

    BeatmapAssociatedData &beatmapAD = getBeatmapAD(beatmapData->customData);

    if (beatmapAD.valid) {
        return;
    }

    if (beatmapData->customData->value) {
        rapidjson::Value const& customData = *beatmapData->customData->value;

        PointDefinitionManager pointDataManager;
        auto pointDefinitionsIt = customData.FindMember("_pointDefinitions");

        if (pointDefinitionsIt != customData.MemberEnd()) {
            const rapidjson::Value &pointDefinitions = pointDefinitionsIt->value;
            for (rapidjson::Value::ConstValueIterator itr = pointDefinitions.Begin(); itr != pointDefinitions.End(); itr++) {
                std::string pointName = (*itr)["_name"].GetString();
                PointDefinition pointData((*itr)["_points"]);
                pointDataManager.AddPoint(pointName, pointData);
            }
        }
        TLogger::GetLogger().debug("Setting point definitions");
        beatmapAD.pointDefinitions = pointDataManager.pointData;
    }
    auto &tracks = beatmapAD.tracks;

    auto notes = beatmapData->GetBeatmapItemsCpp<NoteData*>();
    auto obstacles = beatmapData->GetBeatmapItemsCpp<ObstacleData*>();

    std::vector<BeatmapObjectData*> objects(notes.size() + obstacles.size());

    std::copy(notes.begin(), notes.end(), std::back_inserter(objects));
    std::copy(obstacles.begin(), obstacles.end(), std::back_inserter(objects));

    for (auto* beatmapObjectData : objects) {
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
            rapidjson::Value const& customData = *customDataWrapper->value;
            BeatmapObjectAssociatedData &ad = getAD(customDataWrapper);
            TracksVector tracksAD;

            auto trackIt = customData.FindMember("_track");
            if (trackIt != customData.MemberEnd()) {
                rapidjson::Value const& tracksObject = trackIt->value;


                switch (tracksObject.GetType()) {
                    case rapidjson::Type::kArrayType: {
                        if (tracksObject.Empty())
                            break;

                        for (auto &trackElement: tracksObject.GetArray()) {
                            Track *track = &tracks[trackElement.GetString()];
                            tracksAD.emplace_back(track);
                        }
                        break;
                    }
                    case rapidjson::Type::kStringType: {
                        Track *track = &tracks[tracksObject.GetString()];
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

    beatmapAD.valid = true;
}

MAKE_HOOK_MATCH(GetBeatmapDataFromBeatmapSaveData,
                &BeatmapDataLoader::GetBeatmapDataFromBeatmapSaveData, BeatmapData *, BeatmapSaveDataVersion3::BeatmapSaveData* beatmapSaveData,
                float startBpm, bool loadingForDesignatedEnvironment,
                ::GlobalNamespace::EnvironmentKeywords* environmentKeywords,
                ::GlobalNamespace::EnvironmentLightGroups* environmentLightGroups,
                ::GlobalNamespace::DefaultEnvironmentEvents* defaultEnvironmentEvents) {
    auto *result =
        reinterpret_cast<CustomJSONData::CustomBeatmapData *>(GetBeatmapDataFromBeatmapSaveData(
                beatmapSaveData, startBpm, loadingForDesignatedEnvironment, environmentKeywords,
            environmentLightGroups, defaultEnvironmentEvents));
    
    TracksAD::readBeatmapDataAD(result);

    return result;
}

void InstallBeatmapDataLoaderHooks(Logger &logger) {
//    INSTALL_HOOK(logger, GetBeatmapDataFromBeatmapSaveData);
}

TInstallHooks(InstallBeatmapDataLoaderHooks);