#pragma once

#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/MonoBehaviour.hpp"
#include "UnityEngine/Transform.hpp"

#include "Track.h"

#include <utility>
#include <vector>
#include <string>
#include <optional>
#include "custom-types/shared/types.hpp"
#include "custom-types/shared/macros.hpp"

namespace Tracks {
struct GameObjectTrackControllerData {
  GameObjectTrackControllerData() = delete;
  GameObjectTrackControllerData(GameObjectTrackControllerData const&) = delete;
  GameObjectTrackControllerData(std::vector<Track*> track, bool v2) : _track(std::move(track)), v2(v2) {}

  std::vector<Track*> const _track;

  bool const v2;

  UnorderedEventCallback<> PositionUpdate;
  UnorderedEventCallback<> ScaleUpdate;
  UnorderedEventCallback<> RotationUpdate;
};
} // namespace Tracks

DECLARE_CLASS_CODEGEN(Tracks, GameObjectTrackController, UnityEngine::MonoBehaviour,
                      private:
                      DECLARE_INSTANCE_FIELD(UnityEngine::Transform*, parent);
                      DECLARE_INSTANCE_FIELD(UnityEngine::Transform*, origin);
                      static bool LeftHanded;

                      private:
                      // This is retrived from the data map since Unity doesn't copy it.
                      GameObjectTrackControllerData* data;
                      uint64_t lastCheckedTime;
                      int attemptedTries;

                      public:
                      void UpdateData(bool force);
                      GameObjectTrackControllerData & getTrackControllerData();

                      static std::optional<GameObjectTrackController*> HandleTrackData(
                          UnityEngine::GameObject * gameObject, std::vector<Track*> const& track,
                          float noteLinesDistance, bool v2, bool overwrite);

                      static void ClearData();

                      DECLARE_INSTANCE_METHOD(void, Awake);
                      DECLARE_INSTANCE_METHOD(void, OnDestroy);
                      DECLARE_INSTANCE_METHOD(void, OnEnable);
                      DECLARE_INSTANCE_METHOD(void, Update);
                      DECLARE_INSTANCE_METHOD(void, OnTransformParentChanged);

                      DECLARE_SIMPLE_DTOR();
                      DECLARE_DEFAULT_CTOR();
                      )
