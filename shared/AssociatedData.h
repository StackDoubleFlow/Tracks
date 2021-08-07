#pragma once

#include "custom-json-data/shared/JSONWrapper.h"

#include "Animation/Track.h"
#include "Vector.h"

namespace UnityEngine {
class Renderer;
}

namespace TracksAD {

class BeatmapAssociatedData {
public:
    std::unordered_map<std::string, Track> tracks;
    std::unordered_map<std::string, PointDefinition> pointDefinitions;
    std::vector<PointDefinition*> anonPointDefinitions;

    ~BeatmapAssociatedData();
};

struct BeatmapObjectAssociatedData {
    Track *track;
};

BeatmapAssociatedData& getBeatmapAD(CustomJSONData::JSONWrapper *customData);
BeatmapObjectAssociatedData& getAD(CustomJSONData::JSONWrapper *customData);

}