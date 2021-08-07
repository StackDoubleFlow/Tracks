#include "AssociatedData.h"
#include "Animation/Animation.h"
#include "Animation/PointDefinition.h"
#include "custom-json-data/shared/CustomBeatmapData.h"

using namespace TracksAD;

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