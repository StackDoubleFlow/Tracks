#include "AssociatedData.h"
#include "THooks.h"
#include "beatsaber-hook/shared/utils/hooking.hpp"
#include "Animation/Events.h"

#include "GlobalNamespace/BeatmapCallbacksController.hpp"
#include "GlobalNamespace/BeatmapCallbacksUpdater.hpp"
#include "GlobalNamespace/BpmController.hpp"
#include "custom-types/shared/coroutine.hpp"
#include "UnityEngine/Resources.hpp"
#include "StaticHolders.hpp"

#include "Animation/GameObjectTrackController.hpp"

using namespace GlobalNamespace;

// MAKE_HOOK_MATCH(BeatmapObjectCallbackController_LateUpdate, &BeatmapObjectCallbackController::LateUpdate, void,
// BeatmapObjectCallbackController *self) {
//     Events::UpdateCoroutines(self);
//     BeatmapObjectCallbackController_LateUpdate(self);
// }

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
custom_types::Helpers::Coroutine updateCoroutines(BeatmapCallbacksController* self) {
  IL2CPP_CATCH_HANDLER(while (true) {
    Events::UpdateCoroutines(self);
    co_yield nullptr;
  })
}
#pragma clang diagnostic pop

BeatmapCallbacksController* controller;
SafePtr<BpmController> TracksStatic::bpmController;

MAKE_HOOK_MATCH(BeatmapObjectCallbackController_Start, &BeatmapCallbacksController::ManualUpdate, void,
                BeatmapCallbacksController* self, float songTime) {
  BeatmapObjectCallbackController_Start(self, songTime);
  if (controller != self) {
    controller = self;

    if (auto customBeatmap = il2cpp_utils::try_cast<CustomJSONData::CustomBeatmapData>(self->_beatmapData)) {
      if (customBeatmap.value()->customData) {
        auto& tracksBeatmapAD = TracksAD::getBeatmapAD(customBeatmap.value()->customData);
        Tracks::GameObjectTrackController::LeftHanded = tracksBeatmapAD.leftHanded;
      }

      UnityEngine::Resources::FindObjectsOfTypeAll<BeatmapCallbacksUpdater*>().get(0)->StartCoroutine(
          custom_types::Helpers::CoroutineHelper::New(updateCoroutines(self)));
    }


  }
}

MAKE_HOOK_FIND_INSTANCE(BpmController_ctor, classof(BpmController*), ".ctor", void, BpmController* self,
                        BpmController::InitData* initData, BeatmapCallbacksController* beatmapCallbacksController) {
  BpmController_ctor(self, initData, beatmapCallbacksController);
  TracksStatic::bpmController = self;
}

void InstallBeatmapObjectCallbackControllerHooks() {
  auto logger = Paper::ConstLoggerContext("Tracks | InstallBeatmapObjectCallbackControllerHooks");
  INSTALL_HOOK(logger, BeatmapObjectCallbackController_Start);
  INSTALL_HOOK(logger, BpmController_ctor);
}
TInstallHooks(InstallBeatmapObjectCallbackControllerHooks)