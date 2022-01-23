#pragma once
#include "beatsaber-hook/shared/config/rapidjson-utils.hpp"
#include "Easings.h"
#include "Track.h"
#include "../Hash.h"

struct PointData {
    NEVector::Vector2 linearPoint = NEVector::Vector2(0, 0);
    NEVector::Vector4 point = NEVector::Vector4(0, 0, 0, 0);
    NEVector::Vector5 vector4Point = NEVector::Vector5(0, 0, 0, 0, 0);
    Functions easing = Functions::easeLinear;
    bool smooth = false;

    PointData(NEVector::Vector4 point, Functions easing = Functions::easeLinear, bool smooth = false) : point{point}, easing{easing}, smooth{smooth} {};
    PointData(NEVector::Vector2 point, Functions easing = Functions::easeLinear) : linearPoint{point}, easing{easing} {};
    PointData(NEVector::Vector5 point, Functions easing = Functions::easeLinear) : vector4Point{point}, easing{easing} {};
};

class PointDefinition {
public:
    PointDefinition(const rapidjson::Value& value);
    [[nodiscard]] NEVector::Vector3 Interpolate(float time) const;
    [[nodiscard]] NEVector::Quaternion InterpolateQuaternion(float time) const;
    [[nodiscard]] float InterpolateLinear(float time) const;
    [[nodiscard]] NEVector::Vector4 InterpolateVector4(float time) const;

    static const PointDefinition EMPTY_POINT;

private:
    constexpr PointDefinition() = default;

    constexpr void SearchIndex(float time, PropertyType propertyType, int& l, int& r) const;
    sbo::small_vector<PointData, 8> points;
};

class PointDefinitionManager {
public:
    std::unordered_map<std::string, PointDefinition, TracksAD::string_hash, TracksAD::string_equal> pointData;

    void AddPoint(std::string const& pointDataName, PointDefinition const& pointData);
};