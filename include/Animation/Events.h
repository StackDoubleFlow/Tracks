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
    PointDefinition *anonPointDef;
};

struct AssignPathAnimationContext {
    PathProperty *property;
    float duration;
    float startTime;
    Functions easing;
};

} // end namespace Events