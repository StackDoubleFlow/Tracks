#pragma once

#include "GlobalNamespace/BpmController.hpp"

struct __attribute__((visibility("default"))) TracksStatic {
  static SafePtr<GlobalNamespace::BpmController> bpmController;
};