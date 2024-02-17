#pragma once

#include "UnityEngine/Quaternion.hpp"
#include "UnityEngine/Vector2.hpp"
#include "UnityEngine/Vector3.hpp"
#include "UnityEngine/Vector4.hpp"

#include "sombrero/shared/Vector2Utils.hpp"
#include "sombrero/shared/Vector3Utils.hpp"
#include "sombrero/shared/QuaternionUtils.hpp"

namespace NEVector {

using Vector2 = Sombrero::FastVector2;
using Vector3 = Sombrero::FastVector3;
using Quaternion = Sombrero::FastQuaternion;

struct Vector4 : public UnityEngine::Vector4 {
  constexpr Vector4(float x = 0, float y = 0, float z = 0, float w = 0) : UnityEngine::Vector4(x, y, z, w) {}
  constexpr Vector4(UnityEngine::Vector4 const& other) : UnityEngine::Vector4(other) {}

  static Vector4 LerpUnclamped(Vector4 a, Vector4 b, float t) {
    return { a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t, a.z + (b.z - a.z) * t, a.w + (b.w - a.w) * t };
  }

  constexpr operator Vector3() const {
    return Vector3(x, y, z);
  }
  bool operator==(const Vector4& other) const {
    return x == other.x && y == other.y && z == other.z && w == other.w;
  }
};

struct Vector5 {
  float x, y, z, w, v;

  constexpr Vector5(float x = 0, float y = 0, float z = 0, float w = 0, float v = 0)
      : x{ x }, y{ y }, z{ z }, w{ w }, v{ v } {};

  constexpr operator Vector4() const {
    return Vector4(x, y, z, w);
  }
};

} // end namespace NEVector