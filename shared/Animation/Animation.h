#pragma once
#include "PointDefinition.h"
#include "../AssociatedData.h"

namespace GlobalNamespace {
class BeatmapData;
}

namespace Animation {

PointDefinition *TryGetPointData(TracksAD::BeatmapAssociatedData &beatmapAD, std::unique_ptr<PointDefinition> &anon,
                                 const rapidjson::Value &customData, std::string_view pointName);

}