#pragma once
#include <cstddef>
#include <utility>
#include <variant>

#include "Vector.h"
#include "beatsaber-hook/shared/config/rapidjson-utils.hpp"
#include "Easings.h"
#include "../Hash.h"
#include "UnityEngine/Color.hpp"
#include "beatsaber-hook/shared/rapidjson/include/rapidjson/document.h"
#include "../bindings.h"

extern Tracks::ffi::FFIJsonValue const* convert_rapidjson(rapidjson::Value const& value);

class PointDefinitionW {
public:
  explicit PointDefinitionW(rapidjson::Value const& value, Tracks::ffi::WrapBaseValueType type,
                            Tracks::ffi::BaseProviderContext* internal_tracks_context) {
    auto* json = convert_rapidjson(value);

    internalPointDefinition = Tracks::ffi::tracks_make_base_point_definition(json, type, internal_tracks_context);
    this->internal_tracks_context = internal_tracks_context;
  }

  PointDefinitionW(Tracks::ffi::BasePointDefinition const* pointDefinition, Tracks::ffi::BaseProviderContext* context)
      : internalPointDefinition(pointDefinition), internal_tracks_context(context) {}

  PointDefinitionW(PointDefinitionW const& other) = default;
  explicit PointDefinitionW(std::nullptr_t) : internalPointDefinition(nullptr) {};

  Tracks::ffi::WrapBaseValue Interpolate(float time) const {
    bool last;
    return Interpolate(time, last);
  }

  Tracks::ffi::WrapBaseValue Interpolate(float time, bool& last) const {
    auto result = Tracks::ffi::tracks_interpolate_base_point_definition(internalPointDefinition, time, &last,
                                                                        internal_tracks_context);

    return result;
  }
  NEVector::Vector3 InterpolateVec3(float time, bool& last) const {
    auto result = Interpolate(time, last);
    return { result.value.vec3.x, result.value.vec3.y, result.value.vec3.z };
  }

  NEVector::Quaternion InterpolateQuaternion(float time, bool& last) const {
    auto result = Interpolate(time, last);
    return { result.value.quat.x, result.value.quat.y, result.value.quat.z, result.value.quat.w };
  }

  float InterpolateLinear(float time, bool& last) const {
    auto result = Interpolate(time, last);
    return result.value.float_v;
  }

  NEVector::Vector4 InterpolateVector4(float time, bool& last) const {
    auto result = Interpolate(time, last);
    return { result.value.vec4.x, result.value.vec4.y, result.value.vec4.z, result.value.vec4.w };
  }

  NEVector::Vector3 InterpolateVec3(float time) const {
    bool last;
    return InterpolateVec3(time, last);
  }

  NEVector::Quaternion InterpolateQuaternion(float time) const {
    bool last;
    return InterpolateQuaternion(time, last);
  }

  float InterpolateLinear(float time) const {
    bool last;
    return InterpolateLinear(time, last);
  }

  NEVector::Vector4 InterpolateVector4(float time) const {
    bool last;
    return InterpolateVector4(time, last);
  }

  uintptr_t count() const {
    return Tracks::ffi::tracks_base_point_definition_count(internalPointDefinition);
  }

  bool hasBaseProvider() const {
    return Tracks::ffi::tracks_base_point_definition_has_base_provider(internalPointDefinition);
  }

  operator Tracks::ffi::BasePointDefinition const*() const {
    return internalPointDefinition;
  }

  // operator Tracks::ffi::BasePointDefinition*() {
  //   return internalPointDefinition;
  // }

private:
  constexpr PointDefinitionW() = default;

  Tracks::ffi::BasePointDefinition const* internalPointDefinition;
  Tracks::ffi::BaseProviderContext* internal_tracks_context;
};

class PointDefinitionManager {
public:
  std::unordered_map<std::string, rapidjson::Value const*, TracksAD::string_hash, TracksAD::string_equal> pointData;

  void AddPoint(std::string const& pointDataName, rapidjson::Value const& pointData);
};