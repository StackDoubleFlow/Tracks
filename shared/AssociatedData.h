#pragma once

#include "custom-json-data/shared/CustomEventData.h"
#include "custom-json-data/shared/CustomBeatmapData.h"
#include "Animation/Easings.h"
#include "Animation/Track.h"
#include "Vector.h"
#include "Hash.h"

namespace UnityEngine {
class Renderer;
}

namespace CustomJSONData {
class CustomBeatmapData;
class JSONWrapper;
} // namespace CustomJSONData

namespace TracksAD {

using TracksVector = sbo::small_vector<Track*, 1>;

enum class EventType { animateTrack, assignPathAnimation, unknown };

class BeatmapAssociatedData {
public:
    bool valid = false;
    bool leftHanded = false;
    std::unordered_map<std::string, Track, string_hash, string_equal> tracks;
    std::unordered_map<std::string, PointDefinition, string_hash, string_equal> pointDefinitions;

    // anon point defs are just point defs created later
    // for owning the point definition and freeing them
    std::vector<PointDefinition *> anonPointDefinitions;

    // BeatmapAssociatedData(const BeatmapAssociatedData&) = delete;
    ~BeatmapAssociatedData();
};

struct BeatmapObjectAssociatedData {
    // Should this be an optional? - Fern
    TracksVector tracks;
};

struct AnimateTrackData {
    sbo::small_vector<std::pair<Property *, PointDefinition *>> properties;

    AnimateTrackData(BeatmapAssociatedData &beatmapAD, rapidjson::Value const &customData, Properties& trackProperties);
};

struct AssignPathAnimationData {
    sbo::small_vector<std::pair<PathProperty *, PointDefinition *>> pathProperties;

    AssignPathAnimationData(BeatmapAssociatedData &beatmapAD, rapidjson::Value const &customData, PathProperties &trackPathProperties);
};

struct CustomEventAssociatedData
{
    // This can probably be omitted or a set
    TracksVector tracks;
    float duration;
    Functions easing;
    EventType type;

    // probably not a set, this might be ordered. Oh how much I hate tracks
    sbo::small_vector<AnimateTrackData, 1> animateTrackData;
    sbo::small_vector<AssignPathAnimationData, 1> assignPathAnimation;

    bool parsed = false;
};

void
readBeatmapDataAD(CustomJSONData::CustomBeatmapData *beatmapData);
BeatmapAssociatedData &getBeatmapAD(CustomJSONData::JSONWrapper *customData);
BeatmapObjectAssociatedData &getAD(CustomJSONData::JSONWrapper *customData);
CustomEventAssociatedData &getEventAD(CustomJSONData::CustomEventData const*customEventData);

void clearEventADs();
} // namespace TracksAD