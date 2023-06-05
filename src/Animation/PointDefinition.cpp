#include "Animation/PointDefinition.h"

#include <utility>
#include <numeric>
#include "Animation/Track.h"
#include "Animation/Easings.h"
#include "TLogger.h"
#include "custom-json-data/shared/CJDLogger.h"
#include "sombrero/shared/HSBColor.hpp"

using namespace NEVector;

const PointDefinition PointDefinition::EMPTY_POINT = PointDefinition();

constexpr Vector3 SmoothVectorLerp(std::span<PointData> const points, int a, int b, float time) {
    // Catmull-Rom Spline
    Vector3 p0 = a - 1 < 0 ? points[a].toVector3() : points[a - 1].toVector3();
    Vector3 p1 = points[a].toVector3();
    Vector3 p2 = points[b].toVector3();
    Vector3 p3 = b + 1 > points.size() - 1 ? points[b].toVector3() : points[b + 1].toVector3();

    float t = time;

    float tt = t * t;
    float ttt = tt * t;

    float q0 = -ttt + (2.0f * tt) - t;
    float q1 = (3.0f * ttt) - (5.0f * tt) + 2.0f;
    float q2 = (-3.0f * ttt) + (4.0f * tt) + t;
    float q3 = ttt - tt;

    Vector3 c = 0.5f * ((p0 * q0) + (p1 * q1) + (p2 * q2) + (p3 * q3));

    return c;
}

constexpr void PointDefinition::SearchIndex(float time, int &l, int &r) const {
    l = 0;
    r = points.size();

    while (l < r - 1) {
        int m = std::midpoint(l, r);
        float pointTime = points[m].time;

        if (pointTime < time) {
            l = m;
        } else {
            r = m;
        }
    }
}

struct TempPointData {
    sbo::small_vector<float, 5> copiedList;
    float time;
    Functions easing = Functions::easeLinear;
    bool spline = false;
    bool hsv = false;

    TempPointData(TempPointData &&) = default;

    TempPointData(sbo::small_vector<float, 5> copiedList, float time, Functions easing, bool spline)
            : copiedList(std::move(copiedList)),
              time(time),
              easing(easing),
              spline(spline) {}

    explicit TempPointData(sbo::small_vector<float, 5> copiedList, float time) : copiedList(std::move(copiedList)),
                                                                                 time(time) {}
};

PointDefinition::PointDefinition(const rapidjson::Value &value) {
    std::vector<TempPointData> tempPointDatas;
    sbo::small_vector<float, 5> alternateList;

    for (int i = 0; i < value.Size(); i++) {
        const rapidjson::Value &rawPoint = value[i];
        if (rawPoint.IsNull()) continue;

        // if [[...]]
        if (rawPoint.IsArray()) {
            sbo::small_vector<float, 5> copiedList;
            bool spline = false;
            Functions easing = Functions::easeLinear;
            bool hsv = false;

            for (int j = 0; j < rawPoint.Size(); j++) {
                const rapidjson::Value &rawPointItem = rawPoint[j];

                if (rawPointItem.IsNull())
                    continue;

                switch (rawPointItem.GetType()) {
                    case rapidjson::kNumberType:
                        copiedList.emplace_back(rawPointItem.GetFloat());
                        break;
                    case rapidjson::kStringType: {
                        std::string flag(rawPointItem.GetString());
                        if (flag.starts_with("ease")) {
                            easing = FunctionFromStr(flag);
                        } else if (flag == "splineCatmullRom") {
                            spline = true;
                        } else if (flag == "lerpHSV") {
                            hsv = true;
                        }
                        break;
                    }
                    default:
                        // TODO: Handle wrong types
                        break;
                }
            }

            float time = copiedList.back();
            copiedList.erase(copiedList.end() - 1); // remove time from list

            auto &p = tempPointDatas.emplace_back(copiedList, time, easing, spline);
            p.hsv = hsv;
        }
            // if [...]
        else if (rawPoint.IsNumber()) {
            alternateList.emplace_back(rawPoint.GetFloat());
        } else {
            TLogger::GetLogger().warning("Unknown point type: %i", rawPoint.GetType());
        }
    }


    // if [...]
    if (!alternateList.empty()) {
        tempPointDatas.emplace_back(std::move(alternateList), 0);
    }


    for (auto const &pointData: tempPointDatas) {
        auto const &copiedList = pointData.copiedList;
        auto time = pointData.time;
        Functions easing = pointData.easing;
        bool spline = pointData.spline;

        points.emplace_back(copiedList, time, easing, spline).hsv = pointData.hsv;
    }


    if (tempPointDatas.empty()) {
        using namespace rapidjson;

        StringBuffer sb;
        PrettyWriter<StringBuffer> writer(sb);
        value.Accept(writer);
        auto str = sb.GetString();

        TLogger::GetLogger().warning("Empty point data: %s", str);
    }
}

Vector3 PointDefinition::Interpolate(float time, bool &last) const {
    PointData const *pointL;
    PointData const *pointR;
    float normalTime;
    int l;
    int r;

    if (InterpolateRaw(time, pointL, pointR, normalTime, l, r, last)) {
        if (pointR->smooth) {
            return SmoothVectorLerp(points, l, r, normalTime);
        }
        return Vector3::LerpUnclamped(points[l].toVector3(), points[r].toVector3(), normalTime);
    }

    if (pointL) {
        return pointL->toVector3();
    }
    if (pointR) {
        return pointR->toVector3();
    }

    return NEVector::Vector3::zero();
}

Quaternion PointDefinition::InterpolateQuaternion(float time, bool &last) const {
    PointData const *pointL;
    PointData const *pointR;
    float normalTime;
    int l;
    int r;

    static auto Quaternion_SlerpUnclamped = il2cpp_utils::il2cpp_type_check::FPtrWrapper<&NEVector::Quaternion::SlerpUnclamped>::get();


    if (InterpolateRaw(time, pointL, pointR, normalTime, l, r, last)) {
        auto quat1 = pointL->toQuaternion();
        auto quat2 = pointR->toQuaternion();

        return Quaternion_SlerpUnclamped(quat1, quat2, normalTime);
    }

    if (pointL) {
        return pointL->toQuaternion();
    }
    if (pointR) {
        return pointR->toQuaternion();
    }

    return Quaternion::identity();
}

float PointDefinition::InterpolateLinear(float time, bool &last) const {
    PointData const *pointL;
    PointData const *pointR;
    float normalTime;
    int l;
    int r;

    if (InterpolateRaw(time, pointL, pointR, normalTime, l, r, last)) {
        return std::lerp(pointL->toFloat(), pointR->toFloat(), normalTime);
    }

    if (pointL) {
        return pointL->toFloat();
    }
    if (pointR) {
        return pointR->toFloat();
    }

    return 0;
}

Vector4 PointDefinition::InterpolateVector4(float time, bool &last) const {
    PointData const *pointL;
    PointData const *pointR;
    float normalTime;
    int l;
    int r;

    if (!InterpolateRaw(time, pointL, pointR, normalTime, l, r, last)) {
        if (pointL) {
            return pointL->toVector4();
        }
        if (pointR) {
            return pointR->toVector4();
        }

        return {0, 0, 0, 0};
    }

    // HSV Lerp
    if (pointR->hsv) {
        Sombrero::HSBColor pointLData(pointL->toColor());
        Sombrero::HSBColor pointRData(pointR->toColor());

        Vector4 pointLV4(pointLData.h, pointLData.s, pointLData.b, pointLData.a);
        Vector4 pointRV4(pointRData.h, pointRData.s, pointRData.b, pointRData.a);

        auto result = Vector4::LerpUnclamped(pointLV4, pointRV4, normalTime);

        auto rgbResult = Sombrero::HSBColor(result.x, result.y, result.z, result.w).ToColor();

        return {rgbResult.r, rgbResult.g, rgbResult.b, normalTime};
    }

    // normal lerp
    return Vector4::LerpUnclamped(pointL->toVector4(), pointR->toVector4(), normalTime);
}

bool PointDefinition::InterpolateRaw(float time, PointData const *&pointL, PointData const *&pointR, float &normalTime,
                                     int &l, int &r, bool &lastB) const {

    pointL = nullptr;
    pointR = nullptr;
    normalTime = 0;
    l = 0;
    r = 0;
    lastB = false;

    if (points.empty()) {
        return false;
    }

    PointData const &first = points.front();
    if (first.time >= time || points.size() == 1) {
        pointL = &first;
        return false;
    }

    PointData const &last = points.back();
    if (last.time <= time) {
        lastB = true;
        pointL = &last;
        return false;
    }

    SearchIndex(time, l, r);
    pointL = &points[l];
    pointR = &points[r];

    float divisor = pointR->time - pointL->time;
    normalTime = divisor != 0 ? (time - pointL->time) / divisor : 0;
    normalTime = Easings::Interpolate(normalTime, pointR->easing);

    return true;
}

bool PointDefinition::isSingle() const {
    return points.size() == 1;
}

void PointDefinitionManager::AddPoint(std::string const &pointDataName, PointDefinition const &pointData) {
    if (this->pointData.contains(pointDataName)) {
        TLogger::GetLogger().error("Duplicate point definition name, %s could not be registered!",
                                   pointDataName.data());
    } else {
        this->pointData.try_emplace(pointDataName, pointData);
    }
}

void PointDefinitionManager::AddPoint(std::string const &pointDataName, PointDefinition &&pointData) {
    if (this->pointData.contains(pointDataName)) {
        TLogger::GetLogger().error("Duplicate point definition name, %s could not be registered!",
                                   pointDataName.data());
    } else {
        this->pointData.try_emplace(pointDataName, std::move(pointData));
    }
}
