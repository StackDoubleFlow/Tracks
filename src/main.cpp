#include "TLogger.h"
#include "THooks.h"
#include "Animation/Events.h"

extern "C" void setup(CModInfo* info) {
  info->id = "Tracks";
  info->version = VERSION;
  info->version_long = 0;
}

extern "C" void late_load() {
  // Force load to ensure order
  auto cjdModInfo = CustomJSONData::modInfo.to_c();
  modloader_require_mod(&cjdModInfo, CMatchType::MatchType_IdOnly);

  Hooks::InstallHooks();
  Events::AddEventCallbacks();
}