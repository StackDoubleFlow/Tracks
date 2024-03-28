#include "TLogger.h"
#include "THooks.h"
#include "Animation/Events.h"

extern "C" void setup(CModInfo* info) {
  info->id = "Tracks";
  info->version = VERSION;
  info->version_long = 0;
}

extern "C" void late_load() {
  Hooks::InstallHooks();
  Events::AddEventCallbacks();
}