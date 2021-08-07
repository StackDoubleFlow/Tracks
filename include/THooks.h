#pragma once
#include "TLogger.h"

class Hooks {
private:
    static std::vector<void (*)(Logger& logger)> installFuncs;
public:
    static void AddInstallFunc(void (*installFunc)(Logger& logger)) {
        installFuncs.push_back(installFunc);
    }

    static void InstallHooks(Logger& logger) {
        for (auto installFunc : installFuncs) {
            installFunc(logger);
        }
    }
};

#define TInstallHooks(func) \
struct __TRegister##func { \
    __TRegister##func() { \
        Hooks::AddInstallFunc(func); \
        __android_log_print(ANDROID_LOG_DEBUG, "TInstallHooks", "Registered install func: " #func); \
    } \
}; \
static __TRegister##func __TRegisterInstance##func;

void InstallAndRegisterAll();