#pragma once
#include "PointDefinition.h"
#include "Easings.h"
#include "Track.h"
#include "PointDefinition.h"

namespace GlobalNamespace {
class BeatmapData;
}

namespace TracksAD {
struct BeatmapAssociatedData;
}

namespace Animation {

PointDefinitionW TryGetPointData(TracksAD::BeatmapAssociatedData& beatmapAD, rapidjson::Value const& customData,
                                 std::string_view pointName, Tracks::ffi::WrapBaseValueType type);

#pragma region track_utils

[[nodiscard]] static constexpr std::optional<NEVector::Vector3>
MirrorVectorNullable(std::optional<NEVector::Vector3> const& vector) {
  if (!vector) {
    return vector;
  }

  auto modifiedVector = *vector;
  modifiedVector.x *= -1;

  return modifiedVector;
}

[[nodiscard]] constexpr static std::optional<NEVector::Quaternion>
MirrorQuaternionNullable(std::optional<NEVector::Quaternion> const& quaternion) {
  if (!quaternion) {
    return quaternion;
  }

  auto modifiedVector = *quaternion;

  return NEVector::Quaternion(modifiedVector.x, modifiedVector.y * -1, modifiedVector.z * -1, modifiedVector.w);
}

} // namespace Animation