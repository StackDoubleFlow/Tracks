#pragma once

#include "Animation/Easings.h"
#include "Animation/Track.h"
#include "Animation/PointDefinition.h"
#include "Animation/Animation.h"
#include "Hash.h"
#include "Vector.h"
#include "custom-json-data/shared/CustomBeatmapData.h"
#include "custom-json-data/shared/CustomEventData.h"
#include <string_view>
#include <memory>

namespace UnityEngine {
class Renderer;
}

namespace CustomJSONData {
class CustomBeatmapData;
class JSONWrapper;
} // namespace CustomJSONData

namespace TracksAD {
namespace Constants {
inline static constexpr std::string_view const V2_DURATION = "_duration";
inline static constexpr std::string_view const V2_EASING = "_easing";
inline static constexpr std::string_view const V2_NAME = "_name";
inline static constexpr std::string_view const V2_POINT_DEFINITIONS = "_pointDefinitions";
inline static constexpr std::string_view const V2_POINTS = "_points";
inline static constexpr std::string_view const V2_TRACK = "_track";
inline static constexpr std::string_view const V2_ANIMATION = "_animation";
inline static constexpr std::string_view const V2_LOCAL_ROTATION = "_localRotation";
inline static constexpr std::string_view const V2_POSITION = "_position";
inline static constexpr std::string_view const V2_LOCAL_POSITION = "_localPosition";
inline static constexpr std::string_view const V2_ROTATION = "_rotation";
inline static constexpr std::string_view const V2_SCALE = "_scale";
inline static constexpr std::string_view const V2_DEFINITE_POSITION = "_definitePosition";
inline static constexpr std::string_view const V2_DISSOLVE = "_dissolve";
inline static constexpr std::string_view const V2_DISSOLVE_ARROW = "_dissolveArrow";
inline static constexpr std::string_view const V2_CUTTABLE = "_interactable";
inline static constexpr std::string_view const V2_COLOR = "_color";
inline static constexpr std::string_view const V2_TIME = "_time";
inline static constexpr std::string_view const V2_ATTENUATION = "_attenuation";
inline static constexpr std::string_view const V2_OFFSET = "_offset";
inline static constexpr std::string_view const V2_HEIGHT_FOG_STARTY = "_startY";
inline static constexpr std::string_view const V2_HEIGHT_FOG_HEIGHT = "_height";

inline static constexpr std::string_view const INTERACTABLE = "interactable";
inline static constexpr std::string_view const DURATION = "duration";
inline static constexpr std::string_view const EASING = "easing";
inline static constexpr std::string_view const NAME = "name";
inline static constexpr std::string_view const POINT_DEFINITIONS = "pointDefinitions";
inline static constexpr std::string_view const POINTS = "points";
inline static constexpr std::string_view const TRACK = "track";
inline static constexpr std::string_view const ANIMATION = "animation";
inline static constexpr std::string_view const POSITION = "position";
inline static constexpr std::string_view const OFFSET_POSITION = "offsetPosition";
inline static constexpr std::string_view const OFFSET_ROTATION = "offsetWorldRotation";
inline static constexpr std::string_view const LOCAL_POSITION = "localPosition";
inline static constexpr std::string_view const ROTATION = "rotation";
inline static constexpr std::string_view const LOCAL_ROTATION = "localRotation";
inline static constexpr std::string_view const SCALE = "scale";
inline static constexpr std::string_view const DEFINITE_POSITION = "definitePosition";
inline static constexpr std::string_view const EVENT = "event";
inline static constexpr std::string_view const REPEAT = "repeat";
inline static constexpr std::string_view const TYPE = "type";
inline static constexpr std::string_view const EVENT_DEFINITIONS = "eventDefinitions";
inline static constexpr std::string_view const ATTENUATION = "attenuation";
inline static constexpr std::string_view const OFFSET = "offset";
inline static constexpr std::string_view const HEIGHT_FOG_STARTY = "startY";
inline static constexpr std::string_view const HEIGHT_FOG_HEIGHT = "height";
inline static constexpr std::string_view const DISSOLVE = "dissolve";
inline static constexpr std::string_view const DISSOLVE_ARROW = "dissolveArrow";
inline static constexpr std::string_view const CUTTABLE = "interactable";
inline static constexpr std::string_view const COLOR = "color";
inline static constexpr std::string_view const TIME = "time";

inline static constexpr std::string_view const LEFT_HANDED_ID = "leftHanded";
} // namespace Constants

using TracksVector = sbo::small_vector<TrackW, 1>;

enum class EventType { animateTrack, assignPathAnimation, unknown };

class TracksContext {
public:
  TracksContext() {
    internal_tracks_context = Tracks::ffi::tracks_context_create();
  }
  TracksContext(TracksContext const&) = delete;

  TracksContext(TracksContext&& o) noexcept : internal_tracks_context(o.internal_tracks_context) {
    o.internal_tracks_context = nullptr;
  }
  ~TracksContext() {
    if (!internal_tracks_context) return;
    Tracks::ffi::tracks_context_destroy(internal_tracks_context);
  }

  Tracks::ffi::TracksContext* internal_tracks_context = nullptr;
  operator Tracks::ffi::TracksContext const*() const {
    return internal_tracks_context;
  }
  operator Tracks::ffi::TracksContext*() {
    return internal_tracks_context;
  }

  Tracks::ffi::CoroutineManager* GetCoroutineManager() {
    return Tracks::ffi::tracks_context_get_coroutine_manager(internal_tracks_context);
  }

  Tracks::ffi::BaseProviderContext* GetBaseProviderContext() {
    return Tracks::ffi::tracks_context_get_base_provider_context(internal_tracks_context);
  }
};

class BeatmapAssociatedData {
public:
  BeatmapAssociatedData() = default;
  ~BeatmapAssociatedData() = default;


  [[deprecated("Don't copy this!")]]
  BeatmapAssociatedData(BeatmapAssociatedData const&) = default;

  bool valid = false;
  bool leftHanded = false;
  bool v2;
  std::unordered_map<std::string, TrackW, string_hash, string_equal> tracks;
  std::unordered_map<std::string, PointDefinitionW, string_hash, string_equal> pointDefinitions;
  std::shared_ptr < TracksContext> internal_tracks_context = nullptr;

  inline PointDefinitionW getPointDefinition(rapidjson::Value const& val, std::string_view key,
                                                              Tracks::ffi::WrapBaseValueType type) {
    PointDefinitionW pointData = Animation::TryGetPointData(*this, val, key, type);

    return pointData;
  }

  TrackW getTrack(std::string_view name);
};

struct BeatmapObjectAssociatedData {
  // Should this be an optional? - Fern
  TracksVector tracks;
};

struct AnimateTrackData {
  sbo::small_vector<std::pair<PropertyW, PointDefinitionW>> properties;

  AnimateTrackData(BeatmapAssociatedData& beatmapAD, rapidjson::Value const& customData, TrackW trackProperties);
};

struct AssignPathAnimationData {
  sbo::small_vector<std::pair<PathPropertyW, PointDefinitionW>> pathProperties;

  AssignPathAnimationData(BeatmapAssociatedData& beatmapAD, rapidjson::Value const& customData,
                          TrackW trackPathProperties);
};

struct CustomEventAssociatedData {
  // This can probably be omitted or a set
  TracksVector tracks;
  float duration;
  Functions easing;
  EventType type;
  uint32_t repeat;

  // probably not a set, this might be ordered. Oh how much I hate tracks
  sbo::small_vector<AnimateTrackData, 1> animateTrackData;
  sbo::small_vector<AssignPathAnimationData, 1> assignPathAnimation;

  bool parsed = false;
};

void LoadTrackEvent(CustomJSONData::CustomEventData const* customEventData, TracksAD::BeatmapAssociatedData& beatmapAD,
                    bool v2);
void readBeatmapDataAD(CustomJSONData::CustomBeatmapData* beatmapData);
BeatmapAssociatedData& getBeatmapAD(CustomJSONData::JSONWrapper* customData);
BeatmapObjectAssociatedData& getAD(CustomJSONData::JSONWrapper* customData);
CustomEventAssociatedData& getEventAD(CustomJSONData::CustomEventData const* customEventData);

void clearEventADs();
} // namespace TracksAD

namespace NEJSON {
static std::optional<TracksAD::TracksVector> ReadOptionalTracks(rapidjson::Value const& object,
                                                                std::string_view const key,
                                                                TracksAD::BeatmapAssociatedData& beatmapAD) {
  auto tracksIt = object.FindMember(key.data());
  if (tracksIt != object.MemberEnd()) {
    TracksAD::TracksVector tracks;

    auto size = tracksIt->value.IsString() ? 1 : tracksIt->value.Size();
    tracks.reserve(size);

    if (tracksIt->value.IsString()) {
      tracks.emplace_back(beatmapAD.getTrack(tracksIt->value.GetString()));
    } else if (tracksIt->value.IsArray()) {
      for (auto const& it : tracksIt->value.GetArray()) {
        tracks.emplace_back(beatmapAD.getTrack(it.GetString()));
      }
    }

    return tracks;
  }
  return std::nullopt;
}
} // namespace NEJSON