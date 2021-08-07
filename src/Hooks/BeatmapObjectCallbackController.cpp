#include "THooks.h"
#include "beatsaber-hook/shared/utils/hooking.hpp"
#include "Animation/Events.h"

#include "GlobalNamespace/BeatmapObjectCallbackController.hpp"

using namespace GlobalNamespace;

BeatmapObjectCallbackController *callbackController;

MAKE_HOOK_MATCH(BeatmapObjectCallbackController_LateUpdate, &BeatmapObjectCallbackController::LateUpdate, void, BeatmapObjectCallbackController *self) {
    callbackController = self;
    Events::UpdateCoroutines();
}

void InstallBeatmapObjectCallbackControllerHooks(Logger& logger) {
    INSTALL_HOOK(logger, BeatmapObjectCallbackController_LateUpdate);
}
TInstallHooks(InstallBeatmapObjectCallbackControllerHooks)