#include "Animation/Animation.h"
#include "Animation/PointDefinition.h"
#include "AssociatedData.h"
#include "TLogger.h"

using namespace TracksAD;

namespace Animation {

PointDefinitionW TryGetPointData(BeatmapAssociatedData& beatmapAD, rapidjson::Value const& customData,
                                 std::string_view pointName, Tracks::ffi::WrapBaseValueType type) {
  PointDefinitionW pointData = nullptr;

  auto customDataItr = customData.FindMember(pointName.data());
  if (customDataItr == customData.MemberEnd()) return pointData;
  rapidjson::Value const& pointString = customDataItr->value;

  switch (pointString.GetType()) {
  case rapidjson::kNullType:
    return pointData;
  case rapidjson::kStringType: {

    auto itr = beatmapAD.pointDefinitions.find(pointString.GetString());
    if (itr == beatmapAD.pointDefinitions.end()) {
      TLogger::Logger.warn("Could not find point definition {}", pointString.GetString());
    } else {
      pointData = itr->second;
    }

    break;
  }
  default:
    auto json = convert_rapidjson(pointString);
    auto baseProviderContext = Tracks::ffi::tracks_context_get_base_provider_context(beatmapAD.internal_tracks_context);
    auto pointDataAnon = Tracks::ffi::tracks_make_base_point_definition(json, type, baseProviderContext);
    pointData = Tracks::ffi::tracks_context_add_point_definition(beatmapAD.internal_tracks_context, pointDataAnon);
  }

  return pointData;
}

} // namespace Animation