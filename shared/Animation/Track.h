#pragma once
#include <map>
#include <string>
#include "PointDefinitionInterpolation.h"
#include "../Vector.h"
#include "../sv/small_vector.h"
#include "UnityEngine/GameObject.hpp"

#include "beatsaber-hook/shared/utils/typedefs-wrappers.hpp"

#include <chrono>

#include "tracks-rs/shared/bindings.h"

namespace Events {
struct AnimateTrackContext;
}

struct TrackWrapped {
  Tracks::ffi::Track* track;
  bool v2;
};