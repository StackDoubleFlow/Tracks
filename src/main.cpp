#include "TLogger.h"
#include "THooks.h"
#include "Animation/Events.h"

extern "C" void setup(ModInfo &info) {
    info.id = "Tracks";
    info.version = VERSION;
    TLogger::modInfo = info;
}

extern "C" void load() {
    Logger &logger = TLogger::GetLogger();
    Hooks::InstallHooks(logger);
    Events::AddEventCallbacks(logger);
}