#pragma once
#include <utility>

#include "beatsaber-hook/shared/config/rapidjson-utils.hpp"
#include "Easings.h"
#include "Track.h"
#include "../Hash.h"

struct PointData {
    sbo::small_vector<float, 5> pointDatas;
    float time;
    Functions easing = Functions::easeLinear;
    bool smooth = false;

    PointData(std::span<float> point, float time, Functions easing = Functions::easeLinear, bool smooth = false) : pointDatas(point.begin(), point.end()), time(time), easing{easing}, smooth{smooth} {};
    PointData(sbo::small_vector<float, 5> point, float time, Functions easing = Functions::easeLinear, bool smooth = false) : pointDatas(std::move(point)), time(time), easing{easing}, smooth{smooth} {};


    [[nodiscard]] NEVector::Vector4 toVector4() const {
        // reaxt did a _color "_color":[[1,1,1,0]]
        // when it should be "_color":[[1,1,1,0,0]]
        if (pointDatas.size() >= 4)
            return {pointDatas[0], pointDatas[1], pointDatas[2], pointDatas[4]};
        else
            return {};
    }

    [[nodiscard]] constexpr NEVector::Vector3 toVector3() const {
        if (pointDatas.size() >= 3)
            return {pointDatas[0], pointDatas[1], pointDatas[2]};
        else
            return {};
    }

    [[nodiscard]] constexpr float toFloat() const {
        if (!pointDatas.empty())
            return pointDatas[0];
        else
            return {};
    }
};

class PointDefinition {
public:
    PointDefinition(const rapidjson::Value& value);
    [[nodiscard]] NEVector::Vector3 Interpolate(float time, bool &last) const;
    [[nodiscard]] NEVector::Quaternion InterpolateQuaternion(float time, bool &last) const;
    [[nodiscard]] float InterpolateLinear(float time, bool &last) const;
    [[nodiscard]] NEVector::Vector4 InterpolateVector4(float time, bool &last) const;

    static const PointDefinition EMPTY_POINT;

    [[nodiscard]] bool isSingle() const;
private:
    constexpr PointDefinition() = default;


    /// <summary>
    /// Does most of the interpolation magic between points
    /// </summary>
    /// <param name="time">time.</param>
    /// <param name="pointL">If returned false, will be the point with data and no interpolation. If true, will interpolate to pointR in normalTime.</param>
    /// <param name="pointR">If returned true, will interpolate from pointL to pointR in normalTime.</param>
    /// <param name="normalTime">interpolation time.</param>
    /// <param name="l">left value index</param>
    /// <param name="r">right value index</param>
    /// <returns>True if not interpolating between two values</returns>
    bool InterpolateRaw(float time, PointData const *&pointL, PointData const *&pointR, float &normalTime,
                        int &l, int &r, bool &last) const;

    constexpr void SearchIndex(float time, int& l, int& r) const;
    sbo::small_vector<PointData, 8> points;
};

class PointDefinitionManager {
public:
    std::unordered_map<std::string, PointDefinition, TracksAD::string_hash, TracksAD::string_equal> pointData;

    void AddPoint(std::string const& pointDataName, PointDefinition const& pointData);
    void AddPoint(std::string const& pointDataName, PointDefinition&& pointData);
};