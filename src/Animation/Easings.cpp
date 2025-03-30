#include "Animation/Easings.h"
#include "TLogger.h"

Functions FunctionFromStr(std::string_view str) {
  static std::unordered_map<std::string_view, Functions> const functions = {
    { "easeLinear", Functions::EaseLinear },         { "easeStep", Functions::EaseStep },
    { "easeInQuad", Functions::EaseInQuad },         { "easeOutQuad", Functions::EaseOutQuad },
    { "easeInOutQuad", Functions::EaseInOutQuad },   { "easeInCubic", Functions::EaseInCubic },
    { "easeOutCubic", Functions::EaseOutCubic },     { "easeInOutCubic", Functions::EaseInOutCubic },
    { "easeInQuart", Functions::EaseInQuart },       { "easeOutQuart", Functions::EaseOutQuart },
    { "easeInOutQuart", Functions::EaseInOutQuart }, { "easeInQuint", Functions::EaseInQuint },
    { "easeOutQuint", Functions::EaseOutQuint },     { "easeInOutQuint", Functions::EaseInOutQuint },
    { "easeInSine", Functions::EaseInSine },         { "easeOutSine", Functions::EaseOutSine },
    { "easeInOutSine", Functions::EaseInOutSine },   { "easeInCirc", Functions::EaseInCirc },
    { "easeOutCirc", Functions::EaseOutCirc },       { "easeInOutCirc", Functions::EaseInOutCirc },
    { "easeInExpo", Functions::EaseInExpo },         { "easeOutExpo", Functions::EaseOutExpo },
    { "easeInOutExpo", Functions::EaseInOutExpo },   { "easeInElastic", Functions::EaseInElastic },
    { "easeOutElastic", Functions::EaseOutElastic }, { "easeInOutElastic", Functions::EaseInOutElastic },
    { "easeInBack", Functions::EaseInBack },         { "easeOutBack", Functions::EaseOutBack },
    { "easeInOutBack", Functions::EaseInOutBack },   { "easeInBounce", Functions::EaseInBounce },
    { "easeOutBounce", Functions::EaseOutBounce },   { "easeInOutBounce", Functions::EaseInOutBounce }
  };

  auto itr = functions.find(str);
  if (itr != functions.end()) {
    return itr->second;
  } else {
    TLogger::Logger.error("Invalid function with name {}", str.data());
    // Use linear by default
    return Functions::EaseLinear;
  }
}