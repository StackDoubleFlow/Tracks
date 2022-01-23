#include "THooks.h"
#include "beatsaber-hook/shared/utils/hooking.hpp"
#include "Animation/Events.h"

#include "GlobalNamespace/BeatmapObjectCallbackController.hpp"
#include "custom-types/shared/coroutine.hpp"

using namespace GlobalNamespace;

//MAKE_HOOK_MATCH(BeatmapObjectCallbackController_LateUpdate, &BeatmapObjectCallbackController::LateUpdate, void, BeatmapObjectCallbackController *self) {
//    Events::UpdateCoroutines(self);
//    BeatmapObjectCallbackController_LateUpdate(self);
//}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
custom_types::Helpers::Coroutine updateCoroutines(BeatmapObjectCallbackController* self) {
    while (true) {
        Events::UpdateCoroutines(self);
        co_yield nullptr;
    }
}
#pragma clang diagnostic pop


MAKE_HOOK_MATCH(BeatmapObjectCallbackController_Start, &BeatmapObjectCallbackController::Start, void, BeatmapObjectCallbackController *self) {
    BeatmapObjectCallbackController_Start(self);
    self->StartCoroutine(custom_types::Helpers::CoroutineHelper::New(updateCoroutines(self)));
}


void InstallBeatmapObjectCallbackControllerHooks(Logger& logger) {
    INSTALL_HOOK(logger, BeatmapObjectCallbackController_Start);
}
TInstallHooks(InstallBeatmapObjectCallbackControllerHooks)