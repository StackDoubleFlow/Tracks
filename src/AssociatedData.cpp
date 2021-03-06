#include "AssociatedData.h"
#include "Animation/Animation.h"
#include "Animation/PointDefinition.h"
#include "custom-json-data/shared/CustomBeatmapData.h"
#include "TLogger.h"

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

static std::unordered_map<CustomJSONData::CustomEventData const *, CustomEventAssociatedData> eventDataMap;

::CustomEventAssociatedData &getEventAD(CustomJSONData::CustomEventData const *customData)
{
    return eventDataMap[customData];
}

void clearEventADs()
{
    eventDataMap.clear();
}

AnimateTrackData::AnimateTrackData(BeatmapAssociatedData &beatmapAD, rapidjson::Value const &customData, Properties& trackProperties)
{
    for (auto const& member : customData.GetObject()) {
        const char *name = member.name.GetString();
        if (strcmp(name, "_track") && strcmp(name, "_duration") && strcmp(name, "_easing"))
        {
            Property *property = trackProperties.FindProperty(name);
            if (property)
            {
                PointDefinition* anonPointDef = nullptr;
                auto pointData = Animation::TryGetPointData(beatmapAD, anonPointDef, customData, name);

                if (anonPointDef)
                    beatmapAD.anonPointDefinitions.emplace_back(anonPointDef);

                this->properties.emplace_back(property, pointData);
            }
            else
            {
                TLogger::GetLogger().warning("Could not find track property with name %s", name);
            }
        }
    }
}

AssignPathAnimationData::AssignPathAnimationData(BeatmapAssociatedData &beatmapAD, rapidjson::Value const &customData, PathProperties &trackPathProperties)
{
    for (auto const &member : customData.GetObject())
    {
        const char *name = member.name.GetString();
        if (strcmp(name, "_track") && strcmp(name, "_duration") && strcmp(name, "_easing"))
        {
            PathProperty *property = trackPathProperties.FindProperty(name);
            if (property)
            {
                PointDefinition* anonPointDef = nullptr;
                auto pointData = Animation::TryGetPointData(beatmapAD, anonPointDef, customData, name);
                if (anonPointDef)
                    beatmapAD.anonPointDefinitions.emplace_back(anonPointDef);

                pathProperties.emplace_back(property, pointData);
            }
            else
            {
                TLogger::GetLogger().warning("Could not find track path property with name %s", name);
            }
        }
    }
}

}