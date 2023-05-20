#pragma once

#define fM_PI_2 float(M_PI_2)
#define fM_PI float(M_PI)

#include <string>
#include <cmath>
#include <unordered_map>

enum struct Functions {
    easeLinear,
    easeStep,
    easeInQuad,
    easeOutQuad,
    easeInOutQuad,
    easeInCubic,
    easeOutCubic,
    easeInOutCubic,
    easeInQuart,
    easeOutQuart,
    easeInOutQuart,
    easeInQuint,
    easeOutQuint,
    easeInOutQuint,
    easeInSine,
    easeOutSine,
    easeInOutSine,
    easeInCirc,
    easeOutCirc,
    easeInOutCirc,
    easeInExpo,
    easeOutExpo,
    easeInOutExpo,
    easeInElastic,
    easeOutElastic,
    easeInOutElastic,
    easeInBack,
    easeOutBack,
    easeInOutBack,
    easeInBounce,
    easeOutBounce,
    easeInOutBounce,
};

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
    if (p < 0.5f)
    {
        float f = 2 * p;
        return 0.5f * ((f * f * f) - (f * std::sin(f * fM_PI)));
    }
    else
    {
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
        case Functions::easeLinear: return EaseLinear(p);
        case Functions::easeStep: return EaseStep(p);
        case Functions::easeOutQuad: return EaseOutQuad(p);
        case Functions::easeInQuad: return EaseInQuad(p);
        case Functions::easeInOutQuad: return EaseInOutQuad(p);
        case Functions::easeInCubic: return EaseInCubic(p);
        case Functions::easeOutCubic: return EaseOutCubic(p);
        case Functions::easeInOutCubic: return EaseInOutCubic(p);
        case Functions::easeInQuart: return EaseInQuart(p);
        case Functions::easeOutQuart: return EaseOutQuart(p);
        case Functions::easeInOutQuart: return EaseInOutQuart(p);
        case Functions::easeInQuint: return EaseInQuint(p);
        case Functions::easeOutQuint: return EaseOutQuint(p);
        case Functions::easeInOutQuint: return EaseInOutQuint(p);
        case Functions::easeInSine: return EaseInSine(p);
        case Functions::easeOutSine: return EaseOutSine(p);
        case Functions::easeInOutSine: return EaseInOutSine(p);
        case Functions::easeInCirc: return EaseInCirc(p);
        case Functions::easeOutCirc: return EaseOutCirc(p);
        case Functions::easeInOutCirc: return EaseInOutCirc(p);
        case Functions::easeInExpo: return EaseInExpo(p);
        case Functions::easeOutExpo: return EaseOutExpo(p);
        case Functions::easeInOutExpo: return EaseInOutExpo(p);
        case Functions::easeInElastic: return EaseInElastic(p);
        case Functions::easeOutElastic: return EaseOutElastic(p);
        case Functions::easeInOutElastic: return EaseInOutElastic(p);
        case Functions::easeInBack: return EaseInBack(p);
        case Functions::easeOutBack: return EaseOutBack(p);
        case Functions::easeInOutBack: return EaseInOutBack(p);
        case Functions::easeInBounce: return EaseInBounce(p);
        case Functions::easeOutBounce: return EaseOutBounce(p);
        case Functions::easeInOutBounce: return EaseInOutBounce(p);
    }
}
}

Functions FunctionFromStr(std::string_view str);
