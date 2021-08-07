#include "Animation/Animation.h"

using namespace TracksAD;

namespace Animation {

PointDefinition *TryGetPointData(BeatmapAssociatedData &beatmapAD, PointDefinition *&anon,
                                 const rapidjson::Value &customData, std::string pointName) {
    PointDefinition *pointData = nullptr;

    if (!customData.HasMember(pointName.c_str()))
        return pointData;
    const rapidjson::Value &pointString = customData[pointName.c_str()];

    switch (pointString.GetType()) {
    case rapidjson::kNullType:
        return pointData;
    case rapidjson::kStringType: {
        auto &ad = beatmapAD;
        auto itr = ad.pointDefinitions.find(pointString.GetString());
        if (itr != ad.pointDefinitions.end()) {
            pointData = &itr->second;
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