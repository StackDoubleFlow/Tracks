#include "AssociatedData.h"
#include "Animation/Animation.h"
#include "Animation/PointDefinition.h"
#include "custom-json-data/shared/CustomBeatmapData.h"

using namespace TracksAD;

namespace {

PointDefinition *TryGetPointData(BeatmapAssociatedData &beatmapAD,
                                 const rapidjson::Value &animation, const char *name) {
    PointDefinition *anonPointDef;
    PointDefinition *pointDef =
        Animation::TryGetPointData(beatmapAD, anonPointDef, animation, name);
    if (anonPointDef) {
        beatmapAD.anonPointDefinitions.push_back(anonPointDef);
    }
    return pointDef;
}

} // namespace

namespace TracksAD {

BeatmapAssociatedData::~BeatmapAssociatedData() {
    for (auto *pointDefinition : anonPointDefinitions) {
        delete pointDefinition;
    }
}

BeatmapObjectAssociatedData &getAD(CustomJSONData::JSONWrapper *customData) {
    std::any &ad = customData->associatedData['T'];
    if (!ad.has_value())
        ad = std::make_any<BeatmapObjectAssociatedData>();
    return std::any_cast<BeatmapObjectAssociatedData &>(ad);
}

BeatmapAssociatedData &getBeatmapAD(CustomJSONData::JSONWrapper *customData) {
    std::any &ad = customData->associatedData['T'];
    if (!ad.has_value())
        ad = std::make_any<BeatmapAssociatedData>();
    return std::any_cast<BeatmapAssociatedData &>(ad);
}

}