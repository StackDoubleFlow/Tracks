#include "Animation/Easings.h"
#include "TLogger.h"

Functions FunctionFromStr(std::string_view str) {
    static std::unordered_map<std::string_view, Functions> const functions = {
        { "easeLinear", Functions::easeLinear },
        { "easeStep", Functions::easeStep },
        { "easeInQuad", Functions::easeInQuad },
        { "easeOutQuad", Functions::easeOutQuad },
        { "easeInOutQuad", Functions::easeInOutQuad },
        { "easeInCubic", Functions::easeInCubic },
        { "easeOutCubic", Functions::easeOutCubic },
        { "easeInOutCubic", Functions::easeInOutCubic },
        { "easeInQuart", Functions::easeInQuart },
        { "easeOutQuart", Functions::easeOutQuart },
        { "easeInOutQuart", Functions::easeInOutQuart },
        { "easeInQuint", Functions::easeInQuint },
        { "easeOutQuint", Functions::easeOutQuint },
        { "easeInOutQuint", Functions::easeInOutQuint },
        { "easeInSine", Functions::easeInSine },
        { "easeOutSine", Functions::easeOutSine },
        { "easeInOutSine", Functions::easeInOutSine },
        { "easeInCirc", Functions::easeInCirc },
        { "easeOutCirc", Functions::easeOutCirc },
        { "easeInOutCirc", Functions::easeInOutCirc },
        { "easeInExpo", Functions::easeInExpo },
        { "easeOutExpo", Functions::easeOutExpo },
        { "easeInOutExpo", Functions::easeInOutExpo },
        { "easeInElastic", Functions::easeInElastic },
        { "easeOutElastic", Functions::easeOutElastic },
        { "easeInOutElastic", Functions::easeInOutElastic },
        { "easeInBack", Functions::easeInBack },
        { "easeOutBack", Functions::easeOutBack },
        { "easeInOutBack", Functions::easeInOutBack },
        { "easeInBounce", Functions::easeInBounce },
        { "easeOutBounce", Functions::easeOutBounce },
        { "easeInOutBounce", Functions::easeInOutBounce }
    };

    auto itr = functions.find(str);
    if (itr != functions.end()) {
        return itr->second;
    } else {
        TLogger::GetLogger().error("Invalid function with name %s", str.data());
        // Use linear by default
        return Functions::easeLinear;
    }
}