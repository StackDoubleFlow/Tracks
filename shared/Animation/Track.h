#pragma once
#include <cstdint>
#include <map>
#include <optional>
#include <string>
#include "../Vector.h"
#include "../sv/small_vector.h"
#include "PointDefinition.h"
#include "UnityEngine/GameObject.hpp"

#include "beatsaber-hook/shared/utils/typedefs-wrappers.hpp"

#include <chrono>

#include "../bindings.h"

namespace Events {
struct AnimateTrackContext;
}

struct PropertyW;
struct PathPropertyW;

using PropertyNames = Tracks::ffi::PropertyNames;

struct TimeUnit {
  Tracks::ffi::CTimeUnit time;

  constexpr TimeUnit(Tracks::ffi::CTimeUnit time) : time(time) {}
  constexpr TimeUnit() = default;
  constexpr TimeUnit(TimeUnit const&) = default;

  [[nodiscard]] constexpr operator Tracks::ffi::CTimeUnit() const {
    return time;
  }

  // get seconds
  constexpr uint64_t get_seconds() const {
    return time._0;
  }

  // get nanoseconds
  constexpr uint64_t get_nanoseconds() const {
    return time._1;
  }

  constexpr bool operator<(TimeUnit o) const {
    return get_seconds() < o.get_seconds() ||
           (o.get_seconds() == get_seconds() && get_nanoseconds() < o.get_nanoseconds());
  }

  constexpr bool operator>(TimeUnit o) const {
    return get_seconds() > o.get_seconds() ||
           (o.get_seconds() == get_seconds() && get_nanoseconds() > o.get_nanoseconds());
  }

  constexpr bool operator==(TimeUnit o) const {
    return get_seconds() == o.get_seconds() && get_nanoseconds() == o.get_nanoseconds();
  }

  constexpr bool operator!=(TimeUnit o) const {
    return get_seconds() != o.get_seconds() || get_nanoseconds() != o.get_nanoseconds();
  }

  constexpr bool operator<=(TimeUnit o) const {
    return o == *this || *this < o;
  }

  constexpr bool operator>=(TimeUnit o) const {
    return o == *this || *this > o;
  }
};

struct PropertyW {
  Tracks::ffi::ValueProperty const* property;

  constexpr PropertyW() = default;
  constexpr PropertyW(Tracks::ffi::ValueProperty const* property) : property(property) {}

  operator Tracks::ffi::ValueProperty const*() const {
    return property;
  }
  operator bool() const {
    return property != nullptr;
  }

  [[nodiscard]] Tracks::ffi::WrapBaseValueType GetType() const {
    return Tracks::ffi::property_get_type(property);
  }
  [[nodiscard]] Tracks::ffi::CValueProperty GetValue() const {
    return Tracks::ffi::property_get_value(property);
  }

  [[nodiscard]] std::optional<NEVector::Quaternion> GetQuat() const {
    auto value = GetValue();
    if (!value.value.has_value) return std::nullopt;
    if (value.value.value.ty != Tracks::ffi::WrapBaseValueType::Quat) return std::nullopt;
    auto v = value.value.value.value;
    return NEVector::Quaternion{ v.quat.x, v.quat.y, v.quat.z, v.quat.w };
  }
  [[nodiscard]] std::optional<NEVector::Vector3> GetVec3() const {
    auto value = GetValue();
    if (!value.value.has_value) return std::nullopt;
    if (value.value.value.ty != Tracks::ffi::WrapBaseValueType::Vec3) return std::nullopt;
    auto v = value.value.value.value;
    return NEVector::Vector3{ v.vec3.x, v.vec3.y, v.vec3.z };
  }
  [[nodiscard]] std::optional<NEVector::Vector4> GetVec4() const {
    auto value = GetValue();
    if (!value.value.has_value) return std::nullopt;
    if (value.value.value.ty != Tracks::ffi::WrapBaseValueType::Vec4) return std::nullopt;
    auto v = value.value.value.value;
    return NEVector::Vector4{ v.vec4.x, v.vec4.y, v.vec4.z, v.vec4.w };
  }
  [[nodiscard]] std::optional<float> GetFloat() const {
    auto value = GetValue();
    if (!value.value.has_value) return std::nullopt;
    if (value.value.value.ty != Tracks::ffi::WrapBaseValueType::Float) return std::nullopt;
    return value.value.value.value.float_v;
  }
};

struct PathPropertyW {
  Tracks::ffi::PathProperty* property;

  constexpr PathPropertyW() = default;
  constexpr PathPropertyW(Tracks::ffi::PathProperty* property) : property(property) {}
  operator Tracks::ffi::PathProperty*() const {
    return property;
  }
  operator Tracks::ffi::PathProperty const*() const {
    return property;
  }
  operator bool() const {
    return property != nullptr;
  }

  Tracks::ffi::WrapBaseValue Interpolate(float time, bool& last, Tracks::ffi::BaseProviderContext* context) const {
    auto result = Tracks::ffi::path_property_interpolate(property, time, context);
    last = result.has_value;
    return result.value;
  }
  NEVector::Vector3 InterpolateVec3(float time, bool& last, Tracks::ffi::BaseProviderContext* context) const {
    auto result = Interpolate(time, last, context);
    return { result.value.vec3.x, result.value.vec3.y, result.value.vec3.z };
  }
  NEVector::Vector4 InterpolateVec4(float time, bool& last, Tracks::ffi::BaseProviderContext* context) const {
    auto result = Interpolate(time, last, context);
    return { result.value.vec4.x, result.value.vec4.y, result.value.vec4.z, result.value.vec4.w };
  }
  NEVector::Quaternion InterpolateQuat(float time, bool& last, Tracks::ffi::BaseProviderContext* context) const {
    auto result = Interpolate(time, last, context);
    return { result.value.quat.x, result.value.quat.y, result.value.quat.z, result.value.quat.w };
  }

  float InterpolateLinear(float time, bool& last, Tracks::ffi::BaseProviderContext* context) const {
    auto result = Interpolate(time, last, context);
    return result.value.float_v;
  }

  [[nodiscard]] Tracks::ffi::WrapBaseValueType GetType() const {
    return Tracks::ffi::path_property_get_type(property);
  }

  [[nodiscard]] float GetTime() const {
    return Tracks::ffi::path_property_get_time(property);
  }

  void SetTime(float time) const {
    Tracks::ffi::path_property_set_time(property, time);
  }

  void Finish() const {
    Tracks::ffi::path_property_finish(property);
  }

  void Init(std::optional<PointDefinitionW> newPointData) const {
    Tracks::ffi::path_property_init(property, newPointData.value_or(PointDefinitionW(nullptr)));
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

  operator bool() const {
    return track != nullptr;
  }

  [[nodiscard]] PropertyW GetProperty(std::string_view name) const {
    auto prop = Tracks::ffi::track_get_property(track, name.data());
    return PropertyW(prop);
  }
  [[nodiscard]] PropertyW GetPropertyNamed(Tracks::ffi::PropertyNames name) const {
    auto prop = Tracks::ffi::track_get_property_by_name(track, name);
    return PropertyW(prop);
  }

  [[nodiscard]] PathPropertyW GetPathProperty(std::string_view name) const {
    auto prop = Tracks::ffi::track_get_path_property(track, name.data());
    return PathPropertyW(prop);
  }
  [[nodiscard]] PathPropertyW GetPathPropertyNamed(Tracks::ffi::PropertyNames name) const {
    auto prop = Tracks::ffi::track_get_path_property_by_name(track, name);
    return PathPropertyW(prop);
  }

  void RegisterGameObject(UnityEngine::GameObject* gameObject) const {
    Tracks::ffi::track_register_game_object(track, Tracks::ffi::GameObject{ .ptr = gameObject });
  }

  void RegisterProperty(std::string_view id, PropertyW property) {
    Tracks::ffi::track_register_property(track, id.data(), const_cast<Tracks::ffi::ValueProperty*>(property.property));
  }
  void RegisterPathProperty(std::string_view id, PathPropertyW property) const {
    Tracks::ffi::track_register_path_property(track, id.data(), property);
  }

  [[nodiscard]] Tracks::ffi::CPropertiesMap GetPropertiesMap() const {
    return Tracks::ffi::track_get_properties_map(track);
  }

  [[nodiscard]] Tracks::ffi::CPathPropertiesMap GetPathPropertiesMap() const {
    return Tracks::ffi::track_get_path_properties_map(track);
  }

  [[nodiscard]] std::string GetName() const {
    return Tracks::ffi::track_get_name(track);
  }

  [[nodiscard]] std::span<UnityEngine::GameObject* const> GetGameObjects() const {
    static_assert(sizeof(UnityEngine::GameObject*) == sizeof(Tracks::ffi::GameObject),
                  "Tracks wrapper and GameObject pointer do not match size!");
    std::size_t count = 0;
    auto const* ptr = Tracks::ffi::track_get_game_objects(track, &count);
    const auto *castedPtr = reinterpret_cast<UnityEngine::GameObject *const*>(ptr);

    return std::span<UnityEngine::GameObject* const>(castedPtr, count);
  }
};
