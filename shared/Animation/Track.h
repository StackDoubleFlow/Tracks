#pragma once
#include <map>
#include <string>
#include "../Vector.h"
#include "../sv/small_vector.h"
#include "Animation/PointDefinition.h"
#include "UnityEngine/GameObject.hpp"

#include "beatsaber-hook/shared/utils/typedefs-wrappers.hpp"

#include <chrono>

#include "tracks-rs/shared/bindings.h"

namespace Events {
struct AnimateTrackContext;
}

struct PropertyW;
struct PathPropertyW;

struct PropertyW {
  Tracks::ffi::ValueProperty const* property;

  constexpr PropertyW() = default;
  constexpr PropertyW(Tracks::ffi::ValueProperty const* property) : property(property) {}

  operator Tracks::ffi::ValueProperty const*() const {
    return property;
  }

  Tracks::ffi::WrapBaseValueType GetType() const {
    return Tracks::ffi::property_get_type(property);
  }

  Tracks::ffi::CValueProperty GetValue() const {
    return Tracks::ffi::property_get_value(property);
  }
};

struct PathPropertyW {
  Tracks::ffi::PathProperty* property;

  constexpr PathPropertyW() = default;
  constexpr PathPropertyW(Tracks::ffi::PathProperty* property) : property(property) {}
  operator Tracks::ffi::PathProperty const*() const {
    return property;
  }

  Tracks::ffi::WrapBaseValue Interpolate(float time, bool& last, Tracks::ffi::BaseProviderContext* context) const {
    auto result = Tracks::ffi::path_property_interpolate(property, time, context);
    last = result.has_value;
    return result.value;
  }
  NEVector::Vector3 InterpolateVec3(float time, bool& last, Tracks::ffi::BaseProviderContext* context) const {
    auto result = Interpolate(time, last, context);
    return {result.value.vec3.x, result.value.vec3.y, result.value.vec3.z};
  }
  NEVector::Vector4 InterpolateVec4(float time, bool& last, Tracks::ffi::BaseProviderContext* context) const {
    auto result = Interpolate(time, last, context);
    return {result.value.vec4.x, result.value.vec4.y, result.value.vec4.z, result.value.vec4.w};
  }
  NEVector::Quaternion InterpolateQuat(float time, bool& last, Tracks::ffi::BaseProviderContext* context) const {
    auto result = Interpolate(time, last, context);
    return {result.value.quat.x, result.value.quat.y, result.value.quat.z, result.value.quat.w};
  }

  float InterpolateLinear(float time, bool& last, Tracks::ffi::BaseProviderContext* context) const {
    auto result = Interpolate(time, last, context);
    return result.value.float_v;
  }


  Tracks::ffi::WrapBaseValueType GetType() const {
    return Tracks::ffi::path_property_get_type(property);
  }

  float GetTime() const {
    return Tracks::ffi::path_property_get_time(property);
  }

  void SetTime(float time) {
    Tracks::ffi::path_property_set_time(property, time);
  }

  void Finish() {
    Tracks::ffi::path_property_finish(property);
  }

  void Init(std::optional<PointDefinitionW> newPointData) {
    Tracks::ffi::path_property_init(property, newPointData.value_or(nullptr));
  }
};

struct TrackW {
  Tracks::ffi::Track* track;
  bool v2;

  constexpr TrackW() = default;
  constexpr TrackW(Tracks::ffi::Track* track, bool v2) : track(track), v2(v2) {}

  operator Tracks::ffi::Track*() const {
    return track;
  }

  PropertyW GetProperty(std::string_view name) const {
    auto prop = Tracks::ffi::track_get_property(track, name.data());
    return PropertyW(prop);
  }

  PathPropertyW GetPathProperty(std::string_view name) const {
    auto prop = Tracks::ffi::track_get_path_property(track, name.data());
    return PathPropertyW(prop);
  }
};
