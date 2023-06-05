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
    namespace Constants {
        inline static constexpr const std::string_view V2_DURATION = "_duration";
        inline static constexpr const std::string_view V2_EASING = "_easing";
        inline static constexpr const std::string_view V2_NAME = "_name";
        inline static constexpr const std::string_view V2_POINT_DEFINITIONS = "_pointDefinitions";
        inline static constexpr const std::string_view V2_POINTS = "_points";
        inline static constexpr const std::string_view V2_TRACK = "_track";
        inline static constexpr const std::string_view V2_ANIMATION = "_animation";
        inline static constexpr const std::string_view V2_LOCAL_ROTATION = "_localRotation";
        inline static constexpr const std::string_view V2_POSITION = "_position";
        inline static constexpr const std::string_view V2_LOCAL_POSITION = "_localPosition";
        inline static constexpr const std::string_view V2_ROTATION = "_rotation";
        inline static constexpr const std::string_view V2_SCALE = "_scale";
        inline static constexpr const std::string_view V2_DEFINITE_POSITION = "_definitePosition";
        inline static constexpr const std::string_view V2_DISSOLVE = "_dissolve";
        inline static constexpr const std::string_view V2_DISSOLVE_ARROW = "_dissolveArrow";
        inline static constexpr const std::string_view V2_CUTTABLE = "_interactable";
        inline static constexpr const std::string_view V2_COLOR = "_color";
        inline static constexpr const std::string_view V2_TIME = "_time";
        inline static constexpr const std::string_view V2_ATTENUATION = "_attenuation";
        inline static constexpr const std::string_view V2_OFFSET = "_offset";
        inline static constexpr const std::string_view V2_HEIGHT_FOG_STARTY = "_startY";
        inline static constexpr const std::string_view V2_HEIGHT_FOG_HEIGHT = "_height";

        inline static constexpr const std::string_view INTERACTABLE = "interactable";
        inline static constexpr const std::string_view DURATION = "duration";
        inline static constexpr const std::string_view EASING = "easing";
        inline static constexpr const std::string_view NAME = "name";
        inline static constexpr const std::string_view POINT_DEFINITIONS = "pointDefinitions";
        inline static constexpr const std::string_view POINTS = "points";
        inline static constexpr const std::string_view TRACK = "track";
        inline static constexpr const std::string_view ANIMATION = "animation";
        inline static constexpr const std::string_view POSITION = "position";
        inline static constexpr const std::string_view OFFSET_POSITION = "offsetPosition";
        inline static constexpr const std::string_view OFFSET_ROTATION = "offsetWorldRotation";
        inline static constexpr const std::string_view LOCAL_POSITION = "localPosition";
        inline static constexpr const std::string_view ROTATION = "rotation";
        inline static constexpr const std::string_view LOCAL_ROTATION = "localRotation";
        inline static constexpr const std::string_view SCALE = "scale";
        inline static constexpr const std::string_view DEFINITE_POSITION = "definitePosition";
        inline static constexpr const std::string_view EVENT = "event";
        inline static constexpr const std::string_view REPEAT = "repeat";
        inline static constexpr const std::string_view TYPE = "type";
        inline static constexpr const std::string_view EVENT_DEFINITIONS = "eventDefinitions";
        inline static constexpr const std::string_view ATTENUATION = "attenuation";
        inline static constexpr const std::string_view OFFSET = "offset";
        inline static constexpr const std::string_view HEIGHT_FOG_STARTY = "startY";
        inline static constexpr const std::string_view HEIGHT_FOG_HEIGHT = "height";
        inline static constexpr const std::string_view DISSOLVE = "dissolve";
        inline static constexpr const std::string_view DISSOLVE_ARROW = "dissolveArrow";
        inline static constexpr const std::string_view CUTTABLE = "interactable";
        inline static constexpr const std::string_view COLOR = "color";
        inline static constexpr const std::string_view TIME = "time";

        inline static constexpr const std::string_view LEFT_HANDED_ID = "leftHanded";
    }


using TracksVector = sbo::small_vector<Track*, 1>;

enum class EventType { animateTrack, assignPathAnimation, unknown };

class BeatmapAssociatedData {
public:
    BeatmapAssociatedData() = default;
    ~BeatmapAssociatedData() = default;

    [[deprecated("Don't copy this!")]]
    BeatmapAssociatedData(BeatmapAssociatedData const&) = default;
    bool valid = false;
    bool leftHanded = false;
    bool v2;
    std::unordered_map<std::string, Track, string_hash, string_equal> tracks;
    std::unordered_map<std::string, PointDefinition, string_hash, string_equal> pointDefinitions;

    // anon point defs are just point defs created later
    // for owning the point definition and freeing them
    std::unordered_set<std::shared_ptr<PointDefinition>> anonPointDefinitions;

    inline Track* getTrack(std::string_view name) {
        return &tracks.try_emplace(name.data(), v2, name).first->second;
    }

    // BeatmapAssociatedData(const BeatmapAssociatedData&) = delete;
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
    int repeat;

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