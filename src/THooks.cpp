#include "THooks.h"

std::vector<void (*)(Logger& logger)> Hooks::installFuncs;