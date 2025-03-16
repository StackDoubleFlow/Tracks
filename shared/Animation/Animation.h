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

[[nodiscard]]
static auto getCurrentTime() {
  return std::chrono::high_resolution_clock::now().time_since_epoch().count();
}

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
#pragma region property_utils

// Conversion functions
[[nodiscard]] constexpr static NEVector::Vector3 ToVector3(Tracks::ffi::WrapBaseValue const& val) {
  if (val.ty != Tracks::ffi::WrapBaseValueType::Vec3) return {};
  return { val.value.vec3.x, val.value.vec3.y, val.value.vec3.z };
}

[[nodiscard]] constexpr static NEVector::Vector4 ToVector4(Tracks::ffi::WrapBaseValue const& val) {
  if (val.ty != Tracks::ffi::WrapBaseValueType::Vec4) return {};
  return { val.value.vec4.x, val.value.vec4.y, val.value.vec4.z, val.value.vec4.w };
}

[[nodiscard]] constexpr static NEVector::Quaternion ToQuaternion(Tracks::ffi::WrapBaseValue const& val) {
  if (val.ty != Tracks::ffi::WrapBaseValueType::Quat) return {};
  return { val.value.quat.x, val.value.quat.y, val.value.quat.z, val.value.quat.w };
}

[[nodiscard]] constexpr static float ToFloat(Tracks::ffi::WrapBaseValue const& val) {
  if (val.ty != Tracks::ffi::WrapBaseValueType::Float) return {};
  return val.value.float_v;
}

// Base versions that return WrapBaseValue
[[nodiscard]]
constexpr static std::vector<Tracks::ffi::WrapBaseValue> getProperties(std::span<TrackW const> tracks,
                                                                       PropertyNames name, uint64_t time) {
  std::vector<Tracks::ffi::WrapBaseValue> properties;
  properties.reserve(tracks.size());
  for (auto const& track : tracks) {
    auto property = track.GetPropertyNamed(name);
    auto value = property.GetValue();
    if (!value.has_value) continue;
    properties.push_back(value.value);
  }

  return properties;
}

[[nodiscard]]
constexpr static std::vector<Tracks::ffi::WrapBaseValue>
getPathProperties(std::span<TrackW const> tracks, PropertyNames name, Tracks::ffi::BaseProviderContext* context,
                  uint64_t time) {
  std::vector<Tracks::ffi::WrapBaseValue> properties;
  bool last = false;

  properties.reserve(tracks.size());
  for (auto const& track : tracks) {
    auto property = track.GetPathPropertyNamed(name);
    bool tempLast;
    auto value = property.Interpolate(time, tempLast, context);
    last = last && tempLast;
    properties.push_back(value);
  }

  return properties;
}

// Macro to generate type-specific property getters
#define GENERATE_PROPERTY_GETTERS(ReturnType, Suffix, Conversion)                                                      \
  [[nodiscard]]                                                                                                        \
  constexpr static std::vector<ReturnType> getProperties##Suffix(std::span<TrackW const> tracks, PropertyNames name,   \
                                                                 uint64_t time) {                                      \
    std::vector<ReturnType> properties;                                                                                \
    properties.reserve(tracks.size());                                                                                 \
    for (auto const& track : tracks) {                                                                                 \
      auto property = track.GetPropertyNamed(name);                                                                    \
      auto value = property.GetValue();                                                                                \
      if (!value.has_value) continue;                                                                                  \
      properties.push_back(Conversion(value.value));                                                                   \
    }                                                                                                                  \
    return properties;                                                                                                 \
  }                                                                                                                    \
                                                                                                                       \
  [[nodiscard]]                                                                                                        \
  constexpr static std::vector<ReturnType> getPathProperties##Suffix(                                                  \
      std::span<TrackW const> tracks, PropertyNames name, Tracks::ffi::BaseProviderContext* context, uint64_t time) {  \
    std::vector<ReturnType> properties;                                                                                \
    bool last = false;                                                                                                 \
    properties.reserve(tracks.size());                                                                                 \
    for (auto const& track : tracks) {                                                                                 \
      auto property = track.GetPathPropertyNamed(name);                                                                \
      bool tempLast;                                                                                                   \
      auto value = property.Interpolate(time, tempLast, context);                                                      \
      last = last && tempLast;                                                                                         \
      properties.push_back(Conversion(value));                                                                         \
    }                                                                                                                  \
    return properties;                                                                                                 \
  }

// Generate specialized versions for different types
GENERATE_PROPERTY_GETTERS(NEVector::Vector3, Vec3, ToVector3)
GENERATE_PROPERTY_GETTERS(NEVector::Vector4, Vec4, ToVector4)
GENERATE_PROPERTY_GETTERS(NEVector::Quaternion, Quat, ToQuaternion)
GENERATE_PROPERTY_GETTERS(float, Float, ToFloat)

#pragma endregion // property_utils

} // namespace Animation