#include "Animation/Animation.h"
#include "TLogger.h"

using namespace TracksAD;

namespace Animation {

PointDefinition *TryGetPointData(BeatmapAssociatedData &beatmapAD, PointDefinition *&anon,
                                 const rapidjson::Value &customData, std::string_view pointName) {
    PointDefinition *pointData = nullptr;

    auto customDataItr = customData.FindMember(pointName.data());
    if (customDataItr == customData.MemberEnd())
        return pointData;
    const rapidjson::Value &pointString = customDataItr->value;

    switch (pointString.GetType()) {
    case rapidjson::kNullType:
        return pointData;
    case rapidjson::kStringType: {
        auto &ad = beatmapAD;
        auto itr = ad.pointDefinitions.find(pointString.GetString());
        if (itr != ad.pointDefinitions.end()) {
            pointData = &itr->second;
        } else {
            TLogger::GetLogger().warning("Could not find point definition %s",
                                         pointString.GetString());
        }
        break;
    }
    default:
        pointData = new PointDefinition(pointString);
        anon = pointData;
    }

    return pointData;
}

} // namespace Animation