#pragma once

#include "Animation/Track.h"
#include "Vector.h"

namespace UnityEngine {
class Renderer;
}

namespace CustomJSONData {
class CustomBeatmapData;
class JSONWrapper;
} // namespace CustomJSONData

namespace TracksAD {

class BeatmapAssociatedData {
public:
    bool valid = false;
    std::unordered_map<std::string, Track> tracks;
    std::unordered_map<std::string, PointDefinition> pointDefinitions;
    std::vector<PointDefinition *> anonPointDefinitions;

    // BeatmapAssociatedData(const BeatmapAssociatedData&) = delete;
    ~BeatmapAssociatedData();
};

struct BeatmapObjectAssociatedData {
    Track *track;
};

void readBeatmapDataAD(CustomJSONData::CustomBeatmapData *beatmapData);
BeatmapAssociatedData &getBeatmapAD(CustomJSONData::JSONWrapper *customData);
BeatmapObjectAssociatedData &getAD(CustomJSONData::JSONWrapper *customData);

} // namespace TracksAD