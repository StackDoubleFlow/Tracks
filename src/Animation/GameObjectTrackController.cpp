#include "Animation/GameObjectTrackController.hpp"

#include "Animation/Animation.h"
#include "TLogger.h"

#include "sombrero/shared/Vector3Utils.hpp"
#include "sombrero/shared/QuaternionUtils.hpp"

using namespace Tracks;

bool GameObjectTrackController::LeftHanded = false;

DEFINE_TYPE(Tracks, GameObjectTrackController)

using namespace Tracks;

// static NEVector::Quaternion QuatInverse(const NEVector::Quaternion &a) {
//         NEVector::Quaternion conj = {-a.x, -a.y, -a.z, a.w};
//          float norm2 = NEVector::Quaternion::Dot(a, a);
//          return {conj.x / norm2, conj.y / norm2, conj.z / norm2,
//                            conj.w / norm2};
// }
GameObjectTrackControllerData& GameObjectTrackController::getTrackControllerData() {
  return *data;
}

void GameObjectTrackController::ClearData() {
  CJDLogger::Logger.fmtLog<Paper::LogLevel::INF>("Clearing track game objects");
}

void GameObjectTrackController::Awake() {
  attemptedTries = 0;

  // OnTransformParentChanged();
}

void GameObjectTrackController::Start() {
  CJDLogger::Logger.fmtLog<Paper::LogLevel::INF>("Checking data {}", fmt::ptr(data));
  // CRASH_UNLESS(data);
}

void GameObjectTrackController::OnDestroy() {
  if (data == nullptr) {
    return;
  }

  delete data;
  data = nullptr;
}

void GameObjectTrackController::OnEnable() {
  OnTransformParentChanged();
}

void GameObjectTrackController::OnTransformParentChanged() {
  origin = get_transform();
  parent = origin->get_parent();
  CJDLogger::Logger.fmtLog<Paper::LogLevel::ERR>("Parent changed {}", static_cast<std::string>(this->get_name()));
  UpdateData(true);
}

void GameObjectTrackController::Update() {
  UpdateData(false);
}

void GameObjectTrackController::UpdateData(bool force) {
  if (!data) {

    // Wait once just in case
    if (attemptedTries > 1 && attemptedTries < 10) {
      CJDLogger::Logger.fmtLog<Paper::LogLevel::ERR>(
          "Data is null! Should remove component or just early return? {} {}", fmt::ptr(this),
          static_cast<std::string>(get_gameObject()->get_name()));
    }

    // Destroy the object if the data is never found
    if (attemptedTries > 100) {
      CJDLogger::Logger.fmtLog<Paper::LogLevel::ERR>("Destroying object", fmt::ptr(this),
                                                     static_cast<std::string>(get_gameObject()->get_name()));
      Destroy(this);
      CJDLogger::Logger.Backtrace(10);
    } else {
      attemptedTries++;
    }
    return;
  }

  auto const _noteLinesDistance = 0.6f; // StaticBeatmapObjectSpawnMovementData.kNoteLinesDistance
  auto const& tracks = data->_track;

  if (tracks.empty()) {
    CJDLogger::Logger.fmtLog<Paper::LogLevel::ERR>("Track is null! Should remove component or just early return? {} {}",
                                                   fmt::ptr(this),
                                                   static_cast<std::string>(get_gameObject()->get_name()));
    Destroy(this);
    return;
  }
  if (force) {
    lastCheckedTime = TimeUnit();
  }

  std::optional<NEVector::Quaternion> rotation;
  std::optional<NEVector::Quaternion> localRotation;
  std::optional<NEVector::Vector3> position;
  std::optional<NEVector::Vector3> localPosition;
  std::optional<NEVector::Vector3> scale;

  if (tracks.size() == 1) {
    auto track = tracks.front();

    // after
    rotation = track.GetPropertyNamed(PropertyNames::Rotation).GetQuat();
    rotation = track.GetPropertyNamed(PropertyNames::LocalRotation).GetQuat();
    position = track.GetPropertyNamed(PropertyNames::Position).GetVec3();
    localPosition = track.GetPropertyNamed(PropertyNames::LocalPosition).GetVec3();
    scale = track.GetPropertyNamed(PropertyNames::Scale).GetVec3();

  } else {

    // now
    auto localRotations = Animation::getPropertiesQuat(tracks, PropertyNames::LocalRotation, lastCheckedTime);
    auto rotations = Animation::getPropertiesQuat(tracks, PropertyNames::Rotation, lastCheckedTime);
    auto positions = Animation::getPropertiesVec3(tracks, PropertyNames::Position, lastCheckedTime);
    auto localPositions = Animation::getPropertiesVec3(tracks, PropertyNames::LocalPosition, lastCheckedTime);
    auto scales = Animation::getPropertiesVec3(tracks, PropertyNames::Scale, lastCheckedTime);

    TRACKS_LIST_OPERATE_MULTIPLE(localRotation, localRotations, *);
    TRACKS_LIST_OPERATE_MULTIPLE(rotation, rotations, *);
    TRACKS_LIST_OPERATE_MULTIPLE(scale, scales, *);
    TRACKS_LIST_OPERATE_MULTIPLE(position, positions, +);
    TRACKS_LIST_OPERATE_MULTIPLE(localPosition, localPositions, +);
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

  lastCheckedTime = Animation::getCurrentTime();
}

std::optional<GameObjectTrackController*>
GameObjectTrackController::HandleTrackData(UnityEngine::GameObject* gameObject, std::span<TrackW const> track,
                                           float noteLinesDistance, bool v2, bool overwrite) {
  auto* existingTrackController = gameObject->GetComponent<GameObjectTrackController*>();

  if (existingTrackController != nullptr) {
    if (overwrite) {
      CJDLogger::Logger.fmtLog<Paper::LogLevel::INF>("Overwriting existing TransformController on {}...",
                                                     std::string(gameObject->get_name()));
      UnityEngine::Object::Destroy(existingTrackController);
    } else {
      CJDLogger::Logger.fmtLog<Paper::LogLevel::INF>("Could not create TransformController, {} already has one.",
                                                     std::string(gameObject->get_name()));
      return existingTrackController;
    }
  }

  if (track.empty()) {
    return std::nullopt;
  }

  auto* trackController = gameObject->AddComponent<GameObjectTrackController*>();
  CRASH_UNLESS(!track.empty());
  CJDLogger::Logger.fmtLog<Paper::LogLevel::INF>("Created track game object with ID {}",
                                                 static_cast<std::string>(gameObject->get_name()));
  // cleaned up on OnDestroy
  trackController->data = new GameObjectTrackControllerData(track, v2);

  for (auto t : track) {
    t.RegisterGameObject(gameObject);
  }

  return trackController;
}
