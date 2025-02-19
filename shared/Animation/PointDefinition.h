#pragma once
#include <utility>
#include <variant>

#include "beatsaber-hook/shared/config/rapidjson-utils.hpp"
#include "Easings.h"
#include "Track.h"
#include "../Hash.h"
#include "UnityEngine/Color.hpp"
#include "beatsaber-hook/shared/rapidjson/include/rapidjson/document.h"
#include "tracks-rs/shared/bindings.h"

extern Tracks::BaseProviderContext* internal_tracks_context;

enum class PointType {
  Float,
  Vector3,
  Vector4,
  Quaternion
};

class PointDefinition {
public:
  explicit PointDefinition(rapidjson::Value const& value, PointType type) {
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    value.Accept(writer);

    auto jsonString = buffer.GetString();

    switch (type) {
      case PointType::Float:
        internalPointDefinition = Tracks::tracks_make_float_point_definition(jsonString, internal_tracks_context);
        break;
      case PointType::Vector3:
        internalPointDefinition = Tracks::tracks_make_vector3_point_definition(jsonString, internal_tracks_context);
        break;
      case PointType::Vector4:
        internalPointDefinition = Tracks::tracks_make_vector4_point_definition(jsonString, internal_tracks_context);
        break;
      case PointType::Quaternion:
        internalPointDefinition = Tracks::tracks_make_quat_point_definition(jsonString, internal_tracks_context);
        break;
    }
  }

  NEVector::Vector3 Interpolate(float time, bool& last) const {
    auto result = Tracks::tracks_interpolate_vector3(std::get<Tracks::Vector3PointDefinition const*>(internalPointDefinition), time, internal_tracks_context);
    last = result.is_last;
    return {result.value.x, result.value.y, result.value.z}; 
  }

  NEVector::Quaternion InterpolateQuaternion(float time, bool& last) const {
    auto result = Tracks::tracks_interpolate_quat(std::get<Tracks::QuaternionPointDefinition const*>(internalPointDefinition), time, internal_tracks_context);
    last = result.is_last;
    return {result.value.x, result.value.y, result.value.z, result.value.w};
  }

  float InterpolateLinear(float time, bool& last) const {
    auto result = Tracks::tracks_interpolate_float(std::get<Tracks::FloatPointDefinition const*>(internalPointDefinition), time, internal_tracks_context);
    last = result.is_last;
    return result.value;
  }
  
  NEVector::Vector4 InterpolateVector4(float time, bool& last) const {
    auto result = Tracks::tracks_interpolate_vector4(std::get<Tracks::Vector4PointDefinition const*>(internalPointDefinition), time, internal_tracks_context);
    last = result.is_last;
    return {result.value.x, result.value.y, result.value.z, result.value.w};
  }

  NEVector::Vector3 Interpolate(float time) const {
    bool last;
    return Interpolate(time, last);
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
    uintptr_t count = -1;
    if (count == -1 && std::holds_alternative<Tracks::FloatPointDefinition const*>(internalPointDefinition)) {
      count = Tracks::tracks_float_count(std::get<Tracks::FloatPointDefinition const*>(internalPointDefinition));
    }
    if (count == -1 && std::holds_alternative<Tracks::Vector3PointDefinition const*>(internalPointDefinition)) {
      count = Tracks::tracks_vector3_count(std::get<Tracks::Vector3PointDefinition const*>(internalPointDefinition));
    }
    if (count == -1 && std::holds_alternative<Tracks::Vector4PointDefinition const*>(internalPointDefinition)) {
      count = Tracks::tracks_vector4_count(std::get<Tracks::Vector4PointDefinition const*>(internalPointDefinition));
    }
    if (count == -1 && std::holds_alternative<Tracks::QuaternionPointDefinition const*>(internalPointDefinition)) {
      count = Tracks::tracks_quat_count(std::get<Tracks::QuaternionPointDefinition const*>(internalPointDefinition));
    }
    return count;
  }

  bool hasBaseProvider() const {
    if (std::holds_alternative<Tracks::FloatPointDefinition const*>(internalPointDefinition)) {
      return Tracks::tracks_float_has_base_provider(std::get<Tracks::FloatPointDefinition const*>(internalPointDefinition));
    }
    if (std::holds_alternative<Tracks::Vector3PointDefinition const*>(internalPointDefinition)) {
      return Tracks::tracks_vector3_has_base_provider(std::get<Tracks::Vector3PointDefinition const*>(internalPointDefinition));
    }
    if (std::holds_alternative<Tracks::Vector4PointDefinition const*>(internalPointDefinition)) {
      return Tracks::tracks_vector4_has_base_provider(std::get<Tracks::Vector4PointDefinition const*>(internalPointDefinition));
    }
    if (std::holds_alternative<Tracks::QuaternionPointDefinition const*>(internalPointDefinition)) {
      return Tracks::tracks_quat_has_base_provider(std::get<Tracks::QuaternionPointDefinition const*>(internalPointDefinition));
    }
    return false;
  }

private:
  constexpr PointDefinition() = default;

  std::variant<Tracks::FloatPointDefinition const*, Tracks::Vector3PointDefinition const*, Tracks::Vector4PointDefinition const*, Tracks::QuaternionPointDefinition const*> internalPointDefinition;
};

class PointDefinitionManager {
public:
  std::unordered_map<std::string, const rapidjson::Value*, TracksAD::string_hash, TracksAD::string_equal> pointData;

  void AddPoint(std::string const& pointDataName, const rapidjson::Value& pointData);
};