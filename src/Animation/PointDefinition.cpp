#include "Animation/PointDefinition.h"

#include <utility>
#include <numeric>
#include "Animation/Track.h"
#include "Animation/Easings.h"
#include "TLogger.h"

using namespace NEVector;

const PointDefinition PointDefinition::EMPTY_POINT = PointDefinition();

inline constexpr Vector4 v4lerp(Vector4 const& a, Vector4 const& b, float t) {
    return Vector4(a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t, a.z + (b.z - a.z) * t, a.w + (b.w - a.w) * t);
}

constexpr Vector3 SmoothVectorLerp(std::span<PointData> points, int a, int b, float time) {
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

constexpr void PointDefinition::SearchIndex(float time, int& l, int& r) const {
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

    TempPointData(sbo::small_vector<float, 5> copiedList, float time, Functions easing, bool spline)
            : copiedList(std::move(copiedList)),
              time(time),
              easing(easing),
              spline(spline) {}

    explicit TempPointData(sbo::small_vector<float, 5> copiedList, float time) : copiedList(std::move(copiedList)), time(time) {}
};

PointDefinition::PointDefinition(const rapidjson::Value& value) {
    std::vector<TempPointData> tempPointDatas;
    sbo::small_vector<float, 5> alternateList;

    for (int i = 0; i < value.Size(); i++) {
        const rapidjson::Value& rawPoint = value[i];
        if (rawPoint.IsNull()) continue;

        // if [[...]]
        if (rawPoint.IsArray()) {
            sbo::small_vector<float, 5> copiedList;
            bool spline = false;
            Functions easing = Functions::easeLinear;

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

            tempPointDatas.emplace_back(copiedList, time, easing, spline);
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

        points.emplace_back(copiedList, time, easing, spline);
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

Vector3 PointDefinition::Interpolate(float time) const {
    PointData const* pointL;
    PointData const* pointR;
    float normalTime;
    int l;
    int r;

    if (InterpolateRaw(time, pointL, pointR, normalTime, l, r))
    {
        if (pointR->smooth) {
            return SmoothVectorLerp(points, l, r, normalTime);
        } else {
            return Vector3::LerpUnclamped(points[l].toVector3(), points[r].toVector3(), normalTime);
        }
    }

    return pointL ? pointL->toVector3() : NEVector::Vector3::zero();
}

Quaternion PointDefinition::InterpolateQuaternion(float time) const {
    PointData const* pointL;
    PointData const* pointR;
    float normalTime;
    int l;
    int r;

    static auto Quaternion_Euler = il2cpp_utils::il2cpp_type_check::FPtrWrapper<static_cast<UnityEngine::Quaternion (*)(UnityEngine::Vector3)>(&UnityEngine::Quaternion::Euler)>::get();
    static auto Quaternion_SlerpUnclamped = il2cpp_utils::il2cpp_type_check::FPtrWrapper<&NEVector::Quaternion::SlerpUnclamped>::get();


    if (InterpolateRaw(time, pointL, pointR, normalTime, l, r))
    {
        auto quat1 = Quaternion_Euler(pointL->toVector3());
        auto quat2 = Quaternion_Euler(pointR->toVector3());

        return Quaternion_SlerpUnclamped(quat1, quat2, normalTime);
    }

    return pointL ? Quaternion_Euler(pointL->toVector3()) : Quaternion::identity();
}

float PointDefinition::InterpolateLinear(float time) const {
    PointData const* pointL;
    PointData const* pointR;
    float normalTime;
    int l;
    int r;

    if (InterpolateRaw(time, pointL, pointR, normalTime, l, r))
    {
        return std::lerp(pointL->toFloat(), pointR->toFloat(), normalTime);
    }

    return pointL ? pointL->toFloat() : 0;
}

Vector4 PointDefinition::InterpolateVector4(float time) const {
    PointData const* pointL;
    PointData const* pointR;
    float normalTime;
    int l;
    int r;

    if (InterpolateRaw(time, pointL, pointR, normalTime, l, r))
    {
        return Vector4::LerpUnclamped(pointL->toVector4(), pointR->toVector4(), normalTime);
    }

    return pointL ? pointL->toVector4() : NEVector::Vector4(0,0,0,0);
}

bool PointDefinition::InterpolateRaw(float time, PointData const*&pointL, PointData const*&pointR,
                                     float &normalTime, int &l, int &r) const {

    pointL = nullptr;
    pointR = nullptr;
    normalTime = 0;
    l = 0;
    r = 0;

    if (points.empty()) {
        return false;
    }

    PointData const &first = points.front();
    if (first.time >= time) {
        pointL = &first;
        return false;
    }

    PointData const &last = points.back();
    if (last.time <= time) {
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

void PointDefinitionManager::AddPoint(std::string const& pointDataName, PointDefinition const& pointData) {
    if (this->pointData.contains(pointDataName)) {
        TLogger::GetLogger().error("Duplicate point definition name, %s could not be registered!", pointDataName.data());
    } else {
        this->pointData.try_emplace(pointDataName, pointData);
    }
} 