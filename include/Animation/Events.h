#pragma once
#include "Animation/PointDefinition.h"
#include "Animation/Track.h"

namespace GlobalNamespace {
    class BeatmapObjectCallbackController;
}

// BeatmapObjectCallbackController.cpp
extern GlobalNamespace::BeatmapObjectCallbackController *callbackController;

namespace Events {

void AddEventCallbacks(Logger& logger);
void UpdateCoroutines();

enum class EventType {
    animateTrack,
    assignPathAnimation
};

struct AnimateTrackContext {
    PointDefinition *points;
    Property *property;
    float duration;
    float startTime;
    Functions easing;
    PointDefinition *anonPointDef;
};

struct AssignPathAnimationContext {
    PathProperty *property;
    float duration;
    float startTime;
    Functions easing;
};

} // end namespace Events