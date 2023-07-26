#pragma once
#include "beatsaber-hook/shared/utils/logging.hpp"
#include "custom-json-data/shared/CJDLogger.h"

class TLogger {
public:
  static inline ModInfo modInfo = ModInfo();
  static Logger& GetLogger() {
    static auto logger = new Logger(modInfo, LoggerOptions(false, true));
    return *logger;
  }
};
