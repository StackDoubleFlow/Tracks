#include "Animation/GameObjectTrackController.hpp"

#include "Animation/Animation.h"
#include "TLogger.h"

#include "sombrero/shared/Vector3Utils.hpp"
#include "sombrero/shared/QuaternionUtils.hpp"

using namespace Tracks;

std::unordered_map<int, GameObjectTrackControllerData> GameObjectTrackController::_dataMap = {};
int GameObjectTrackController::nextId = 0;
bool GameObjectTrackController::LeftHanded = false;

DEFINE_TYPE(Tracks, GameObjectTrackController)

using namespace Tracks;

template <typename T>
static constexpr std::optional<T> getPropertyNullable(Track const* track, Property const& prop,
                                                      uint32_t lastCheckedTime) {
  if (lastCheckedTime != 0 && prop.lastUpdated != 0 && prop.lastUpdated < lastCheckedTime) return std::nullopt;

  auto ret = Animation::getPropertyNullable<T>(track, prop.value);

  return ret;
}

// static NEVector::Quaternion QuatInverse(const NEVector::Quaternion &a) {
//         NEVector::Quaternion conj = {-a.x, -a.y, -a.z, a.w};
//          float norm2 = NEVector::Quaternion::Dot(a, a);
//          return {conj.x / norm2, conj.y / norm2, conj.z / norm2,
//                            conj.w / norm2};
// }
GameObjectTrackControllerData& GameObjectTrackController::getTrackControllerData() {
  if (id == -1) {
    return *data;
  }

  if (!data) {
    auto it = _dataMap.find(id);

    if (it != _dataMap.end()) {
      data = &it->second;
    }
  }

  return *data;
}

void GameObjectTrackController::ClearData() {
  CJDLogger::Logger.fmtLog<Paper::LogLevel::INF>("Clearing track game objects");

  _dataMap.clear();
  nextId = 0;
}

void GameObjectTrackController::Awake() {
  OnTransformParentChanged();
}

void GameObjectTrackController::OnEnable() {
  CJDLogger::Logger.fmtLog<Paper::LogLevel::INF>("OnEnable runs");
  OnTransformParentChanged();
}

void GameObjectTrackController::OnTransformParentChanged() {
  origin = get_transform();
  parent = origin->get_parent();
  CJDLogger::Logger.fmtLog<Paper::LogLevel::ERR>("Parent changed {}", id);
  UpdateData(true);
}

void GameObjectTrackController::Update() {
  UpdateData(false);
}

void GameObjectTrackController::UpdateData(bool force) {
  getTrackControllerData();

  if (id == -1) {
    // wait for assignment of an id
    return;
  }
  if (!data) {
    CJDLogger::Logger.fmtLog<Paper::LogLevel::ERR>(
        "Data {} is null! Should remove component or just early return? {} {}", id, fmt::ptr(this),
        static_cast<std::string>(get_gameObject()->get_name()));
    CJDLogger::Logger.Backtrace(10);
    Destroy(this);
    return;
  }
  auto const _noteLinesDistance = 0.6f; // StaticBeatmapObjectSpawnMovementData.kNoteLinesDistance
  auto const _track = data->_track;

  if (_track.empty()) {
    CJDLogger::Logger.fmtLog<Paper::LogLevel::ERR>("Track is null! Should remove component or just early return? {} {}",
                                                   fmt::ptr(this),
                                                   static_cast<std::string>(get_gameObject()->get_name()));
    Destroy(this);
    return;
  }
  if (force) {
    lastCheckedTime = 0;
  }

  std::optional<NEVector::Quaternion> rotation;
  std::optional<NEVector::Quaternion> localRotation;
  std::optional<NEVector::Vector3> position;
  std::optional<NEVector::Vector3> localPosition;
  std::optional<NEVector::Vector3> scale;

  // I hate this
  auto tracks = std::span<Track const*>(const_cast<Track const**>(&*_track.begin()), _track.size());

  if (tracks.size() == 1) {
    auto track = tracks.front();
    auto const& properties = track->properties;

    rotation = getPropertyNullable<NEVector::Quaternion>(track, properties.rotation, lastCheckedTime);
    localRotation = getPropertyNullable<NEVector::Quaternion>(track, properties.localRotation, lastCheckedTime);
    position = getPropertyNullable<NEVector::Vector3>(track, properties.position, lastCheckedTime);
    localPosition = getPropertyNullable<NEVector::Vector3>(track, properties.localPosition, lastCheckedTime);
    scale = getPropertyNullable<NEVector::Vector3>(track, properties.scale, lastCheckedTime);

  } else {

#define combine(target, list, op)                                                                                      \
  if (list)                                                                                                            \
    for (auto const& i : *list) {                                                                                      \
      if (!target)                                                                                                     \
        target = i;                                                                                                    \
      else                                                                                                             \
        target = *target op i;                                                                                         \
    }

    auto localRotations = Animation::getPropertiesNullable<NEVector::Quaternion>(
        tracks, [](Properties const& p) { return p.localRotation; }, lastCheckedTime);
    auto rotations = Animation::getPropertiesNullable<NEVector::Quaternion>(
        tracks, [](Properties const& p) { return p.rotation; }, lastCheckedTime);
    auto positions = Animation::getPropertiesNullable<NEVector::Vector3>(
        tracks, [](Properties const& p) { return p.position; }, lastCheckedTime);
    auto localPositions = Animation::getPropertiesNullable<NEVector::Vector3>(
        tracks, [](Properties const& p) { return p.localPosition; }, lastCheckedTime);
    auto scales = Animation::getPropertiesNullable<NEVector::Vector3>(
        tracks, [](Properties const& p) { return p.scale; }, lastCheckedTime);

    combine(localRotation, localRotations, *);
    combine(rotation, rotations, *);
    combine(scale, scales, +);
    combine(position, positions, +);
    combine(localPosition, localPositions, +);
  }

  if (GameObjectTrackController::LeftHanded) {
    localPosition = Animation::MirrorVectorNullable(localPosition);
    position = Animation::MirrorVectorNullable(position);

    rotation = Animation::MirrorQuaternionNullable(rotation);
    localRotation = Animation::MirrorQuaternionNullable(localRotation);
  }

  auto transform = origin;

  static auto Transform_Position =
      il2cpp_utils::il2cpp_type_check::FPtrWrapper<&UnityEngine::Transform::set_position>::get();
  static auto Transform_LocalPosition =
      il2cpp_utils::il2cpp_type_check::FPtrWrapper<&UnityEngine::Transform::set_localPosition>::get();
  static auto Transform_Rotation =
      il2cpp_utils::il2cpp_type_check::FPtrWrapper<&UnityEngine::Transform::set_rotation>::get();
  static auto Transform_LocalRotation =
      il2cpp_utils::il2cpp_type_check::FPtrWrapper<&UnityEngine::Transform::set_localRotation>::get();
  static auto Transform_Scale =
      il2cpp_utils::il2cpp_type_check::FPtrWrapper<&UnityEngine::Transform::set_localScale>::get();
  if (localRotation) {

    Transform_LocalRotation(transform, localRotation.value());
    data->RotationUpdate.invoke();

  } else if (rotation) {
    Transform_Rotation(transform, rotation.value());
    data->RotationUpdate.invoke();
  }

  if (localPosition) {
    if (data->v2) {
      *localPosition *= _noteLinesDistance;
    }
    Transform_LocalPosition(transform, *localPosition);
    data->PositionUpdate.invoke();
  } else if (position) {
    if (data->v2) {
      *position *= _noteLinesDistance;
    }

    Transform_Position(transform, *position);
    data->PositionUpdate.invoke();
  }

  if (scale) {
    Transform_Scale(transform, scale.value());
    data->ScaleUpdate.invoke();
  }

  lastCheckedTime = getCurrentTime();
}

std::optional<GameObjectTrackController*>
GameObjectTrackController::HandleTrackData(UnityEngine::GameObject* gameObject, std::vector<Track*> const& track,
                                           float noteLinesDistance, bool v2, bool overwrite) {
  // This should never be called with an empty track list
  if (track.empty()) {
    CJDLogger::Logger.fmtLog<Paper::LogLevel::WRN>("Track list is empty {}", nextId);
    return std::nullopt;
  }

  auto* existingTrackController = gameObject->GetComponent<GameObjectTrackController*>();

  if (existingTrackController) {
    if (overwrite) {
      CJDLogger::Logger.fmtLog<Paper::LogLevel::ERR>("Overwriting existing track controller on {}",
                                                     (std::string)gameObject->get_name());
      Destroy(existingTrackController);
    } else {
      CJDLogger::Logger.fmtLog<Paper::LogLevel::ERR>("Could not create track controller on {}, it already has one",
                                                     (std::string)gameObject->get_name());
      return existingTrackController;
    }
  }

  auto* trackController = gameObject->AddComponent<GameObjectTrackController*>();
  CRASH_UNLESS(!track.empty());
  CJDLogger::Logger.fmtLog<Paper::LogLevel::INF>("Created track game object with ID {}", nextId);
  trackController->id = nextId;
  trackController->data = &_dataMap.try_emplace(nextId, track, v2).first->second;
  nextId++;

  // Heck does it outside because it is used in other places that add game objects to tracks themselves
  for (auto* t : track) {
    t->AddGameObject(gameObject);
  }

  return trackController;
}
