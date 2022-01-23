#include "Animation/PointDefinitionInterpolation.h"
#include "Animation/PointDefinition.h"

using namespace UnityEngine;

UnityEngine::Vector3 PointDefinitionInterpolation::Interpolate(float time) const {
    if (!previousPointData) {
        return basePointData->Interpolate(time);
    }
    return NEVector::Vector3::LerpUnclamped(previousPointData->Interpolate(time), basePointData->Interpolate(time), this->time);
}

UnityEngine::Quaternion PointDefinitionInterpolation::InterpolateQuaternion(float time) const {
    if (!previousPointData) {
        return basePointData->InterpolateQuaternion(time);
    }
    return NEVector::Quaternion::SlerpUnclamped(previousPointData->InterpolateQuaternion(time), basePointData->InterpolateQuaternion(time), this->time);
}

float PointDefinitionInterpolation::InterpolateLinear(float time) const {
    if (!previousPointData) {
        return basePointData->InterpolateLinear(time);
    }
    return std::lerp(previousPointData->InterpolateLinear(time), basePointData->InterpolateLinear(time), this->time);
}

UnityEngine::Vector4 PointDefinitionInterpolation::InterpolateVector4(float time) const {
    if (!previousPointData) {
        return basePointData->InterpolateVector4(time);
    }
    Vector4 a = previousPointData->InterpolateVector4(time);
    Vector4 b = basePointData->InterpolateVector4(time);
    float t = this->time;

    return NEVector::Vector4::LerpUnclamped(a, b, t);
}

void PointDefinitionInterpolation::Init(PointDefinition const* newPointData) {
    time = 0;
    if (basePointData) {
        previousPointData = basePointData;
    } else {
        previousPointData = &PointDefinition::EMPTY_POINT;
    }
    basePointData = newPointData;
}

void PointDefinitionInterpolation::Finish() {
    previousPointData = nullptr;
}