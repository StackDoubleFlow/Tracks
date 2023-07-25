#pragma once
#include "UnityEngine/Vector3.hpp"
#include "UnityEngine/Vector4.hpp"
#include "UnityEngine/Quaternion.hpp"

class PointDefinition;

class PointDefinitionInterpolation {
public:
  [[nodiscard]] UnityEngine::Vector3 Interpolate(float time) const;
  [[nodiscard]] UnityEngine::Quaternion InterpolateQuaternion(float time) const;
  [[nodiscard]] float InterpolateLinear(float time) const;
  [[nodiscard]] UnityEngine::Vector4 InterpolateVector4(float time) const;
  void Init(PointDefinition const* newPointData);
  void Restart();
  void Finish();

  float time;

private:
  PointDefinition const* basePointData;
  PointDefinition const* previousPointData;
};