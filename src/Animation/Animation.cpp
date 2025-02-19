#include "Animation/Animation.h"
#include "Animation/PointDefinition.h"
#include "AssociatedData.h"
#include "TLogger.h"

using namespace TracksAD;

namespace Animation {

PointDefinition* TryGetPointData(BeatmapAssociatedData& beatmapAD, PointDefinition*& anon,
                                 rapidjson::Value const& customData, std::string_view pointName, PointType type) {
  PointDefinition* pointData = nullptr;

  auto customDataItr = customData.FindMember(pointName.data());
  if (customDataItr == customData.MemberEnd()) return pointData;
  rapidjson::Value const& pointString = customDataItr->value;

  switch (pointString.GetType()) {
  case rapidjson::kNullType:
    return pointData;
  case rapidjson::kStringType: {
    auto itr = beatmapAD.pointDefinitions.find(pointString.GetString());
    if (itr != beatmapAD.pointDefinitions.end()) {
      pointData = new PointDefinition(*itr->second, type);
    } else {
      TLogger::Logger.warn("Could not find point definition {}", pointString.GetString());
    }
    break;
  }
  default:
    anon = new PointDefinition(pointString, type);
    pointData = anon;
  }

  return pointData;
}

} // namespace Animation