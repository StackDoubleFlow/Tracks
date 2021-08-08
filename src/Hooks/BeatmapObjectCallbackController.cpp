#include "THooks.h"
#include "beatsaber-hook/shared/utils/hooking.hpp"
#include "Animation/Events.h"

#include "GlobalNamespace/BeatmapObjectCallbackController.hpp"

using namespace GlobalNamespace;

MAKE_HOOK_MATCH(BeatmapObjectCallbackController_LateUpdate, &BeatmapObjectCallbackController::LateUpdate, void, BeatmapObjectCallbackController *self) {
    Events::UpdateCoroutines(self);
    BeatmapObjectCallbackController_LateUpdate(self);
}

void InstallBeatmapObjectCallbackControllerHooks(Logger& logger) {
    INSTALL_HOOK(logger, BeatmapObjectCallbackController_LateUpdate);
}
TInstallHooks(InstallBeatmapObjectCallbackControllerHooks)