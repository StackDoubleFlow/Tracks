#pragma once
#include "TLogger.h"
#include <android/log.h>

class Hooks {
private:
  static inline std::vector<void (*)()> installFuncs;

public:
  static inline void AddInstallFunc(void (*installFunc)()) {
    installFuncs.push_back(installFunc);
  }

  static inline void InstallHooks() {
    for (auto installFunc : installFuncs) {
      installFunc();
    }
  }
};

#define TInstallHooks(func)                                                                                            \
  struct __TRegister##func {                                                                                           \
    __TRegister##func() {                                                                                              \
      Hooks::AddInstallFunc(func);                                                                                     \
      __android_log_print(ANDROID_LOG_DEBUG, "TInstallHooks", "Registered install func: " #func);                      \
    }                                                                                                                  \
  };                                                                                                                   \
  static __TRegister##func __TRegisterInstance##func;

void InstallAndRegisterAll();