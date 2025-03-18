#pragma once

#define fM_PI_2 float(M_PI_2)
#define fM_PI float(M_PI)

#include <string>
#include <cmath>

#include "../bindings.h"

using Functions = Tracks::ffi::Functions;
namespace Easings {

constexpr static float EaseLinear(float p) {
  return p;
}

constexpr static float EaseStep(float p) {
  return std::floor(p);
}

constexpr static float EaseOutQuad(float p) {
  return -(p * (p - 2.0f));
}

constexpr static float EaseInQuad(float p) {
  return p * p;
}

constexpr static float EaseInOutQuad(float p) {
  if (p < 0.5f) {
    return 2.0f * p * p;
  } else {
    return (-2.0f * p * p) + (4.0f * p) - 1.0f;
  }
}

constexpr static float EaseInCubic(float p) {
  return p * p * p;
}

constexpr static float EaseOutCubic(float p) {
  float f = p - 1.0f;
  return (f * f * f) + 1.0f;
}

constexpr static float EaseInOutCubic(float p) {
  if (p < 0.5f) {
    return 4.0f * p * p * p;
  } else {
    float f = (2.0f * p) - 2.0f;
    return (0.5f * f * f * f) + 1.0f;
  }
}

constexpr static float EaseInQuart(float p) {
  return p * p * p * p;
}

constexpr static float EaseOutQuart(float p) {
  float f = p - 1.0f;
  return (f * f * f * (1.0f - p)) + 1.0f;
}

constexpr static float EaseInOutQuart(float p) {
  if (p < 0.5f) {
    return 8.0f * p * p * p * p;
  } else {
    float f = p - 1;
    return (-8.0f * f * f * f * f) + 1.0f;
  }
}

constexpr static float EaseInQuint(float p) {
  return p * p * p * p * p;
}

constexpr static float EaseOutQuint(float p) {
  float f = p - 1.0f;
  return (f * f * f * f * f) + 1.0f;
}

constexpr static float EaseInOutQuint(float p) {
  if (p < 0.5f) {
    return 16.0f * p * p * p * p * p;
  } else {
    float f = (2.0f * p) - 2.0f;
    return (0.5f * f * f * f * f * f) + 1.0f;
  }
}

constexpr static float EaseInSine(float p) {
  return std::sin((p - 1.0f) * fM_PI_2) + 1.0f;
}

constexpr static float EaseOutSine(float p) {
  return std::sin(p * fM_PI_2);
}

constexpr static float EaseInOutSine(float p) {
  return 0.5f * (1.0f - std::cos(p * fM_PI));
}

constexpr static float EaseInCirc(float p) {
  return 1.0f - std::sqrt(1.0f - (p * p));
}

constexpr static float EaseOutCirc(float p) {
  return std::sqrt((2.0f - p) * p);
}

constexpr static float EaseInOutCirc(float p) {
  if (p < 0.5f) {
    return 0.5f * (1.0f - std::sqrt(1.0f - (4.0f * (p * p))));
  } else {
    return 0.5f * (std::sqrt(-((2.0f * p) - 3.0f) * ((2.0f * p) - 1.0f)) + 1.0f);
  }
}

constexpr static float EaseInExpo(float p) {
  return (p == 0.0f) ? p : std::pow(2.0f, 10.0f * (p - 1.0f));
}

constexpr static float EaseOutExpo(float p) {
  return (p == 1.0f) ? p : 1.0f - std::pow(2.0f, -10.0f * p);
}

constexpr static float EaseInOutExpo(float p) {
  if (p == 0.0 || p == 1.0) {
    return p;
  }

  if (p < 0.5f) {
    return 0.5f * std::pow(2.0f, (20.0f * p) - 10.0f);
  } else {
    return (-0.5f * std::pow(2.0f, (-20.0f * p) + 10.0f)) + 1.0f;
  }
}

constexpr static float EaseInElastic(float p) {
  return std::sin(13.0f * fM_PI_2 * p) * std::pow(2.0f, 10.0f * (p - 1.0f));
}

constexpr static float EaseOutElastic(float p) {
  return (std::sin(-13.0f * fM_PI_2 * (p + 1.0f)) * std::pow(2.0f, -10.0f * p)) + 1.0f;
}

constexpr static float EaseInOutElastic(float p) {
  if (p < 0.5f) {
    return 0.5f * std::sin(13.0f * fM_PI_2 * (2.0f * p)) * std::pow(2.0f, 10.0f * ((2.0f * p) - 1.0f));
  } else {
    return 0.5f * ((std::sin(-13.0f * fM_PI_2 * (2.0f * p)) * std::pow(2.0f, -10.0f * ((2.0f * p) - 1.0f))) + 2.0f);
  }
}

constexpr static float EaseInBack(float p) {
  return (p * p * p) - (p * std::sin(p * fM_PI));
}

constexpr static float EaseOutBack(float p) {
  float f = 1 - p;
  return 1 - ((f * f * f) - (f * std::sin(f * fM_PI)));
}

constexpr static float EaseInOutBack(float p) {
  if (p < 0.5f) {
    float f = 2 * p;
    return 0.5f * ((f * f * f) - (f * std::sin(f * fM_PI)));
  } else {
    float f = 1 - ((2 * p) - 1);
    return (0.5f * (1 - ((f * f * f) - (f * std::sin(f * fM_PI))))) + 0.5f;
  }
}

constexpr static float EaseOutBounce(float p) {
  if (p < 4 / 11.0f) {
    return 121 * p * p / 16.0f;
  } else if (p < 8 / 11.0f) {
    return (363 / 40.0f * p * p) - (99 / 10.0f * p) + (17 / 5.0f);
  } else if (p < 9 / 10.0f) {
    return (4356 / 361.0f * p * p) - (35442 / 1805.0f * p) + (16061 / 1805.0f);
  } else {
    return (54 / 5.0f * p * p) - (513 / 25.0f * p) + (268 / 25.0f);
  }
}

constexpr static float EaseInBounce(float p) {
  return 1.0f - EaseOutBounce(1.0f - p);
}

constexpr static float EaseInOutBounce(float p) {
  if (p < 0.5f) {
    return 0.5f * EaseInBounce(p * 2.0f);
  } else {
    return (0.5f * EaseOutBounce((p * 2.0f) - 1.0f)) + 0.5f;
  }
}

constexpr static float Interpolate(float p, Functions function) {
  // Short circuit math
  if (p >= 1) return 1;
  if (p <= 0) return 0;

  switch (function) {
  default:
  case Functions::EaseLinear:
    return EaseLinear(p);
  case Functions::EaseStep:
    return EaseStep(p);
  case Functions::EaseOutQuad:
    return EaseOutQuad(p);
  case Functions::EaseInQuad:
    return EaseInQuad(p);
  case Functions::EaseInOutQuad:
    return EaseInOutQuad(p);
  case Functions::EaseInCubic:
    return EaseInCubic(p);
  case Functions::EaseOutCubic:
    return EaseOutCubic(p);
  case Functions::EaseInOutCubic:
    return EaseInOutCubic(p);
  case Functions::EaseInQuart:
    return EaseInQuart(p);
  case Functions::EaseOutQuart:
    return EaseOutQuart(p);
  case Functions::EaseInOutQuart:
    return EaseInOutQuart(p);
  case Functions::EaseInQuint:
    return EaseInQuint(p);
  case Functions::EaseOutQuint:
    return EaseOutQuint(p);
  case Functions::EaseInOutQuint:
    return EaseInOutQuint(p);
  case Functions::EaseInSine:
    return EaseInSine(p);
  case Functions::EaseOutSine:
    return EaseOutSine(p);
  case Functions::EaseInOutSine:
    return EaseInOutSine(p);
  case Functions::EaseInCirc:
    return EaseInCirc(p);
  case Functions::EaseOutCirc:
    return EaseOutCirc(p);
  case Functions::EaseInOutCirc:
    return EaseInOutCirc(p);
  case Functions::EaseInExpo:
    return EaseInExpo(p);
  case Functions::EaseOutExpo:
    return EaseOutExpo(p);
  case Functions::EaseInOutExpo:
    return EaseInOutExpo(p);
  case Functions::EaseInElastic:
    return EaseInElastic(p);
  case Functions::EaseOutElastic:
    return EaseOutElastic(p);
  case Functions::EaseInOutElastic:
    return EaseInOutElastic(p);
  case Functions::EaseInBack:
    return EaseInBack(p);
  case Functions::EaseOutBack:
    return EaseOutBack(p);
  case Functions::EaseInOutBack:
    return EaseInOutBack(p);
  case Functions::EaseInBounce:
    return EaseInBounce(p);
  case Functions::EaseOutBounce:
    return EaseOutBounce(p);
  case Functions::EaseInOutBounce:
    return EaseInOutBounce(p);
  }
}
} // namespace Easings

Functions FunctionFromStr(std::string_view str);
