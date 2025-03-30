#pragma once
#include "Animation/PointDefinition.h"
#include "Animation/Track.h"

namespace GlobalNamespace {
class BeatmapCallbacksController;
}

namespace Events {

void AddEventCallbacks();
void UpdateCoroutines(GlobalNamespace::BeatmapCallbacksController* callbackController);

struct AnimateTrackContext {
  PointDefinitionW points;
  PropertyW property;
  float duration;
  Functions easing;
  float startTime;
  int repeat;

  constexpr AnimateTrackContext(PointDefinitionW points, PropertyW aProperty, float duration, float startTime,
                                Functions easing, int repeat)
      : points(points), property(aProperty), duration(duration), startTime(startTime), easing(easing), repeat(repeat) {}

};

struct AssignPathAnimationContext {
  PathPropertyW property;
  float duration;
  Functions easing;
  float startTime;
  int repeat;

  constexpr AssignPathAnimationContext(PathPropertyW aProperty, float duration, float startTime, Functions easing,
                                       int repeat)
      : property(aProperty), duration(duration), startTime(startTime), easing(easing), repeat(repeat) {}
};

} // end namespace Events