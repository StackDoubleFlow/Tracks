#include "Animation/PointDefinitionInterpolation.h"
#include "Animation/PointDefinition.h"

using namespace UnityEngine;

UnityEngine::Vector3 PointDefinitionInterpolation::Interpolate(float time) const {
  [[maybe_unused]] bool last;

  if (!previousPointData) {
    return basePointData->Interpolate(time, last);
  }
  return NEVector::Vector3::LerpUnclamped(previousPointData->Interpolate(time, last),
                                          basePointData->Interpolate(time, last), this->time);
}

UnityEngine::Quaternion PointDefinitionInterpolation::InterpolateQuaternion(float time) const {
  [[maybe_unused]] bool last;
  if (!previousPointData) {
    return basePointData->InterpolateQuaternion(time, last);
  }
  static auto Quaternion_SlerpUnclamped =
      il2cpp_utils::il2cpp_type_check::FPtrWrapper<&NEVector::Quaternion::SlerpUnclamped>::get();

  return Quaternion_SlerpUnclamped(previousPointData->InterpolateQuaternion(time, last),
                                   basePointData->InterpolateQuaternion(time, last), this->time);
}

float PointDefinitionInterpolation::InterpolateLinear(float time) const {
  [[maybe_unused]] bool last;
  if (!previousPointData) {
    return basePointData->InterpolateLinear(time, last);
  }
  return std::lerp(previousPointData->InterpolateLinear(time, last), basePointData->InterpolateLinear(time, last),
                   this->time);
}

UnityEngine::Vector4 PointDefinitionInterpolation::InterpolateVector4(float time) const {
  [[maybe_unused]] bool last;
  if (!previousPointData) {
    return basePointData->InterpolateVector4(time, last);
  }
  Vector4 a = previousPointData->InterpolateVector4(time, last);
  Vector4 b = basePointData->InterpolateVector4(time, last);
  float t = this->time;

  return NEVector::Vector4::LerpUnclamped(a, b, t);
}

void PointDefinitionInterpolation::Init(const PointDefinition* newPointData) {
  time = 0;
  previousPointData = basePointData;
  if (newPointData == nullptr) {
    basePointData = nullptr;
    return;
  }

  basePointData = newPointData;
}

void PointDefinitionInterpolation::Finish() {
  previousPointData = nullptr;
}

void PointDefinitionInterpolation::Restart() {
  Init(basePointData);
}
