#pragma once
#include "Animation/PointDefinition.h"
#include "Animation/Track.h"

namespace GlobalNamespace {
class BeatmapCallbacksController;
}

namespace Events {

void AddEventCallbacks(Logger &logger);
void UpdateCoroutines(GlobalNamespace::BeatmapCallbacksController *callbackController);



struct AnimateTrackContext {
    PointDefinition *points;
    Property *property;
    float duration;
    float startTime;
    Functions easing;
    int repeat;

    constexpr AnimateTrackContext(PointDefinition *points, Property *aProperty, float duration, float startTime, Functions easing, int repeat)
    : points(points), property(aProperty), duration(duration), startTime(startTime), easing(easing), repeat(repeat)
                                                         {}

    constexpr AnimateTrackContext() = default;
};

struct AssignPathAnimationContext {
    PathProperty *property;
    float duration;
    float startTime;
    Functions easing;
    int repeat;

    constexpr AssignPathAnimationContext() = default;

    constexpr AssignPathAnimationContext(PathProperty *aProperty, float duration, float startTime, Functions easing, int repeat) : property(
            aProperty), duration(duration), startTime(startTime), easing(easing), repeat(repeat) {}
};

} // end namespace Events