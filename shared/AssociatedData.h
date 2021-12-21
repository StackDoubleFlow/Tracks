#pragma once

#include "custom-json-data/shared/CustomEventData.h"
#include "custom-json-data/shared/CustomBeatmapData.h"
#include "Animation/Easings.h"
#include "Animation/Track.h"
#include "Vector.h"

namespace UnityEngine {
class Renderer;
}

namespace CustomJSONData {
class CustomBeatmapData;
class JSONWrapper;
} // namespace CustomJSONData

namespace TracksAD {

enum class EventType { animateTrack, assignPathAnimation, unknown };

class BeatmapAssociatedData {
public:
    bool valid = false;
    std::unordered_map<std::string, Track> tracks;
    std::unordered_map<std::string, PointDefinition> pointDefinitions;

    // anon point defs are just point defs created later
    // for owning the point definition and freeing them
    std::vector<PointDefinition *> anonPointDefinitions;

    // BeatmapAssociatedData(const BeatmapAssociatedData&) = delete;
    ~BeatmapAssociatedData();
};

struct BeatmapObjectAssociatedData {
    // Should this be an optional? - Fern
    std::vector<Track*> tracks;
};

struct AnimateTrackData {
    std::vector<Property *> properties;

    std::unordered_map<Property const*, PointDefinition *> pointData;

    AnimateTrackData(BeatmapAssociatedData &beatmapAD, rapidjson::Value const &customData, Properties& trackProperties);
};

struct AssignPathAnimationData {
    std::vector<PathProperty *> pathProperties;

    std::unordered_map<PathProperty const*, PointDefinition *> pointData;

    AssignPathAnimationData(BeatmapAssociatedData &beatmapAD, rapidjson::Value const &customData, PathProperties &trackPathProperties);
};

struct CustomEventAssociatedData
{
    // This can probably be omitted or a set
    std::vector<Track *> tracks;
    float duration;
    Functions easing;
    EventType type;

    // probably not a set, this might be ordered. Oh how much I hate tracks
    std::vector<AnimateTrackData> animateTrackData;
    std::vector<AssignPathAnimationData> assignPathAnimation;

    bool parsed = false;
};

void
readBeatmapDataAD(CustomJSONData::CustomBeatmapData *beatmapData);
BeatmapAssociatedData &getBeatmapAD(CustomJSONData::JSONWrapper *customData);
BeatmapObjectAssociatedData &getAD(CustomJSONData::JSONWrapper *customData);
CustomEventAssociatedData &getEventAD(CustomJSONData::CustomEventData const*customEventData);

void clearEventADs();
} // namespace TracksAD