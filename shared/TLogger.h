#pragma once
#include "beatsaber-hook/shared/utils/logging.hpp"
#include "custom-json-data/shared/CJDLogger.h"

class TLogger {
public:
  static Logger& GetLogger() {
    static auto logger = new Logger(modloader::ModInfo{"Tracks", VERSION, 0}, LoggerOptions(false, true));
    return *logger;
  }
};
