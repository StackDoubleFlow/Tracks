#include "AssociatedData.h"
#include "Animation/Animation.h"
#include "Animation/PointDefinition.h"
#include "Animation/Track.h"
#include "custom-json-data/shared/CustomBeatmapData.h"
#include "TLogger.h"

using namespace TracksAD;

namespace TracksAD {

BeatmapObjectAssociatedData& getAD(CustomJSONData::JSONWrapper* customData) {
  std::any& ad = customData->associatedData['T'];
  if (!ad.has_value()) ad = std::make_any<BeatmapObjectAssociatedData>();
  return std::any_cast<BeatmapObjectAssociatedData&>(ad);
}

BeatmapAssociatedData& getBeatmapAD(CustomJSONData::JSONWrapper* customData) {
  std::any& ad = customData->associatedData['T'];
  if (!ad.has_value()) ad = std::make_any<BeatmapAssociatedData>();
  return std::any_cast<BeatmapAssociatedData&>(ad);
}

static std::unordered_map<CustomJSONData::CustomEventData const*, CustomEventAssociatedData> eventDataMap;

::CustomEventAssociatedData& getEventAD(CustomJSONData::CustomEventData const* customData) {
  return eventDataMap[customData];
}

void clearEventADs() {
  eventDataMap.clear();
}

inline bool IsStringProperties(std::string_view n) {
  using namespace TracksAD::Constants;
  return n != V2_TRACK && n != V2_DURATION && n != V2_EASING && n != TRACK && n != DURATION && n != EASING &&
         n != REPEAT;
}

AnimateTrackData::AnimateTrackData(BeatmapAssociatedData& beatmapAD, rapidjson::Value const& customData,
                                   Properties& trackProperties) {
  for (auto const& member : customData.GetObject()) {
    char const* name = member.name.GetString();
    if (IsStringProperties(name)) {
      Property* property = trackProperties.FindProperty(name);
      if (property) {
        PointDefinition* anonPointDef = nullptr;
        PointType type;
        switch (property->type) {
          case PropertyType::linear: {
            type = PointType::Float;
            break;
          }
          case PropertyType::vector3: {
            type = PointType::Vector3;
            break;
          }
          case PropertyType::quaternion: {
            type = PointType::Quaternion;
            break;
          }
          case PropertyType::vector4: {
            type = PointType::Vector4;
            break;
          }
        }
        auto pointData = Animation::TryGetPointData(beatmapAD, anonPointDef, customData, name, type);

        if (anonPointDef) beatmapAD.anonPointDefinitions.emplace(anonPointDef);

        this->properties.emplace_back(property, pointData);
      } else {
        TLogger::Logger.warn("Could not find track property with name {}", name);
      }
    }
  }
}

AssignPathAnimationData::AssignPathAnimationData(BeatmapAssociatedData& beatmapAD, rapidjson::Value const& customData,
                                                 PathProperties& trackPathProperties) {
  for (auto const& member : customData.GetObject()) {
    char const* name = member.name.GetString();
    if (IsStringProperties(name)) {
      PathProperty* property = trackPathProperties.FindProperty(name);
      if (property) {
        PointDefinition* anonPointDef = nullptr;
        PointType type;
        switch (property->type) {
          case PropertyType::linear: {
            type = PointType::Float;
            break;
          }
          case PropertyType::vector3: {
            type = PointType::Vector3;
            break;
          }
          case PropertyType::quaternion: {
            type = PointType::Quaternion;
            break;
          }
          case PropertyType::vector4: {
            type = PointType::Vector4;
            break;
          }
        }
        auto pointData = Animation::TryGetPointData(beatmapAD, anonPointDef, customData, name, type);
        if (anonPointDef) beatmapAD.anonPointDefinitions.emplace(anonPointDef);

        pathProperties.emplace_back(property, pointData);
      } else {
        TLogger::Logger.warn("Could not find track path property with name {}", name);
      }
    }
  }
}

constexpr static float getFloat(rapidjson::Value const& value) {
  switch (value.GetType()) {
  case rapidjson::kStringType:
    return std::stof(value.GetString());
  case rapidjson::kNumberType:
    return value.GetFloat();
  default:
    throw std::runtime_error(&"Not valid type in JSON doc "[value.GetType()]);
  }
}

void LoadTrackEvent(CustomJSONData::CustomEventData const* customEventData, TracksAD::BeatmapAssociatedData& beatmapAD,
                    bool v2) {
  auto typeHash = customEventData->typeHash;

#define TYPE_GET(jsonName, varName) static auto jsonNameHash_##varName = std::hash<std::string_view>()(jsonName);

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

  auto& eventAD = getEventAD(customEventData);

  if (eventAD.parsed) return;

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
        TLogger::Logger.debug("Track in array is not a string, why?");
        continue;
      }

      tracks.emplace_back(beatmapAD.getTrack(track.GetString()));
    }
  } else if (trackJSON.IsString()) {
    tracks.emplace_back(beatmapAD.getTrack(trackJSON.GetString()));
  } else {
    TLogger::Logger.debug("Track object is not a string or array, why?");
    eventAD.type = EventType::unknown;
    return;
  }

  eventAD.tracks = std::move(tracks);
  auto durationIt =
      eventData.FindMember((v2 ? TracksAD::Constants::V2_DURATION : TracksAD::Constants::DURATION).data());
  auto easingIt = eventData.FindMember((v2 ? TracksAD::Constants::V2_EASING : TracksAD::Constants::EASING).data());
  auto repeatIt = eventData.FindMember(TracksAD::Constants::REPEAT.data());

  eventAD.duration = durationIt != eventData.MemberEnd() ? getFloat(durationIt->value) : 0;
  eventAD.repeat = eventAD.duration > 0 && repeatIt != eventData.MemberEnd() ? repeatIt->value.GetInt() : 0;
  eventAD.easing =
      easingIt != eventData.MemberEnd() ? FunctionFromStr(easingIt->value.GetString()) : Functions::easeLinear;

  for (auto const& track : eventAD.tracks) {
    auto& properties = track->properties;
    auto& pathProperties = track->pathProperties;

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

void readBeatmapDataAD(CustomJSONData::CustomBeatmapData* beatmapData) {
  static auto* customObstacleDataClass = classof(CustomJSONData::CustomObstacleData*);
  static auto* customNoteDataClass = classof(CustomJSONData::CustomNoteData*);
  static auto* customSliderDataClass = classof(CustomJSONData::CustomSliderData*);

  BeatmapAssociatedData& beatmapAD = getBeatmapAD(beatmapData->customData);
  bool v2 = beatmapData->v2orEarlier;

  CJDLogger::Logger.fmtLog<Paper::LogLevel::INF>("Reading beatmap ad");
  Paper::Logger::Backtrace(CJDLogger::Logger.tag, 20);

  if (beatmapAD.valid) {
    return;
  }

  beatmapAD.v2 = v2;

  if (beatmapData->customData->value) {
    rapidjson::Value const& customData = *beatmapData->customData->value;

    PointDefinitionManager pointDataManager;
    auto pointDefinitionsIt =
        customData.FindMember(v2 ? Constants::V2_POINT_DEFINITIONS.data() : Constants::POINT_DEFINITIONS.data());

    if (pointDefinitionsIt != customData.MemberEnd()) {
      rapidjson::Value const& pointDefinitions = pointDefinitionsIt->value;
      for (rapidjson::Value::ConstValueIterator itr = pointDefinitions.Begin(); itr != pointDefinitions.End(); itr++) {
        if (v2) {
          std::string pointName = (*itr)[Constants::V2_NAME.data()].GetString();
          CJDLogger::Logger.fmtLog<Paper::LogLevel::INF>("Added point {}", pointName);
          pointDataManager.AddPoint(pointName, (*itr)[Constants::V2_POINTS.data()]);
        } else {
          for (auto const& [name, pointDataVal] : pointDefinitionsIt->value.GetObject()) {
            CJDLogger::Logger.fmtLog<Paper::LogLevel::INF>("Added point {}", name.GetString());
            pointDataManager.AddPoint(name.GetString(), pointDataVal);
          }
        }
      }
    }
    TLogger::Logger.debug("Setting point definitions");
    beatmapAD.pointDefinitions = pointDataManager.pointData;
  }

  for (auto* beatmapObjectData : beatmapData->beatmapObjectDatas) {
    if (!beatmapObjectData) continue;

    CustomJSONData::JSONWrapper* customDataWrapper;
    if (beatmapObjectData->klass == customObstacleDataClass) {
      auto obstacleData = (CustomJSONData::CustomObstacleData*)beatmapObjectData;
      customDataWrapper = obstacleData->customData;
    } else if (beatmapObjectData->klass == customNoteDataClass) {
      auto noteData = (CustomJSONData::CustomNoteData*)beatmapObjectData;
      customDataWrapper = noteData->customData;
    } else if (beatmapObjectData->klass == customSliderDataClass) {
      auto sliderData = (CustomJSONData::CustomSliderData*)beatmapObjectData;
      customDataWrapper = sliderData->customData;
    } else {
      continue;
    }

    if (customDataWrapper->value) {
      rapidjson::Value const& customData = *customDataWrapper->value;
      BeatmapObjectAssociatedData& ad = getAD(customDataWrapper);
      TracksVector tracksAD;

      auto trackIt = customData.FindMember(v2 ? Constants::V2_TRACK.data() : Constants::TRACK.data());
      if (trackIt != customData.MemberEnd()) {
        rapidjson::Value const& tracksObject = trackIt->value;

        switch (tracksObject.GetType()) {
        case rapidjson::Type::kArrayType: {
          if (tracksObject.Empty()) break;

          for (auto& trackElement : tracksObject.GetArray()) {
            tracksAD.emplace_back(beatmapAD.getTrack(trackElement.GetString()));
          }
          break;
        }
        case rapidjson::Type::kStringType: {
          tracksAD.emplace_back(beatmapAD.getTrack(tracksObject.GetString()));
          break;
        }

        default: {
          TLogger::Logger.error("Tracks object is not an array or a string, what? Why?");
          break;
        }
        }
      }

      ad.tracks = tracksAD;
    }
  }

  for (auto const& customEventData : beatmapData->customEventDatas) {
    if (!customEventData) continue;
    LoadTrackEvent(customEventData, beatmapAD, beatmapData->v2orEarlier);
  }

  beatmapAD.valid = true;
}

} // namespace TracksAD