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

    beatmapAD.valid = true;
}

void InstallBeatmapDataLoaderHooks(Logger &logger) {
//    INSTALL_HOOK(logger, GetBeatmapDataFromBeatmapSaveData);
}

TInstallHooks(InstallBeatmapDataLoaderHooks);