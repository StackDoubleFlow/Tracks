#include "AssociatedData.h"
#include "Animation/Animation.h"
#include "Animation/PointDefinition.h"
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
        auto pointData = Animation::TryGetPointData(beatmapAD, anonPointDef, customData, name);

        if (anonPointDef) beatmapAD.anonPointDefinitions.emplace(anonPointDef);

        this->properties.emplace_back(property, pointData);
      } else {
        TLogger::GetLogger().warning("Could not find track property with name %s", name);
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
        auto pointData = Animation::TryGetPointData(beatmapAD, anonPointDef, customData, name);
        if (anonPointDef) beatmapAD.anonPointDefinitions.emplace(anonPointDef);

        pathProperties.emplace_back(property, pointData);
      } else {
        TLogger::GetLogger().warning("Could not find track path property with name %s", name);
      }
    }
  }
}

} // namespace TracksAD