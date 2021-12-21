#pragma once
#include "Animation/PointDefinition.h"
#include "Animation/Track.h"

namespace GlobalNamespace {
class BeatmapObjectCallbackController;
}

namespace Events {

void AddEventCallbacks(Logger &logger);
void UpdateCoroutines(GlobalNamespace::BeatmapObjectCallbackController *callbackController);



struct AnimateTrackContext {
    PointDefinition *points;
    Property *property;
    float duration;
    float startTime;
    Functions easing;

    AnimateTrackContext(PointDefinition *points, Property *aProperty, float duration, float startTime, Functions easing)
    : points(points), property(aProperty), duration(duration), startTime(startTime), easing(easing)
                                                         {}

    AnimateTrackContext() = default;
};

struct AssignPathAnimationContext {
    PathProperty *property;
    float duration;
    float startTime;
    Functions easing;

    AssignPathAnimationContext() = default;

    AssignPathAnimationContext(PathProperty *aProperty, float duration, float startTime, Functions easing) : property(
            aProperty), duration(duration), startTime(startTime), easing(easing) {}
};

} // end namespace Events