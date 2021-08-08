#include "GlobalNamespace/BeatmapObjectCallbackController.hpp"
#include "GlobalNamespace/BeatmapData.hpp"
#include "GlobalNamespace/BeatmapObjectSpawnController.hpp"

#include "beatsaber-hook/shared/utils/hooking.hpp"
#include "custom-json-data/shared/CustomEventData.h"
#include "custom-json-data/shared/CustomBeatmapData.h"
#include "custom-types/shared/register.hpp"

#include "Animation/Easings.h"
#include "Animation/Events.h"
#include "Animation/Easings.h"
#include "Animation/Track.h"
#include "Animation/Animation.h"
#include "TimeSourceHelper.h"
#include "AssociatedData.h"
#include "TLogger.h"
#include "Vector.h"

using namespace Events;
using namespace GlobalNamespace;
using namespace NEVector;
using namespace TracksAD;

BeatmapObjectSpawnController *spawnController;

static std::vector<AnimateTrackContext> coroutines;
static std::vector<AssignPathAnimationContext> pathCoroutines;
static std::vector<PointDefinition*> anonPointDefinitions;

MAKE_HOOK_MATCH(BeatmapObjectSpawnController_Start, &BeatmapObjectSpawnController::Start, void, BeatmapObjectSpawnController *self) {
    spawnController = self;
    coroutines.clear();
    pathCoroutines.clear();
    TLogger::GetLogger().debug("coroutines and pathCoroutines capacity: %lu and %lu", coroutines.capacity(), pathCoroutines.capacity());
    for (auto *pointDefinition : anonPointDefinitions) {
        delete pointDefinition;
    }
    TLogger::GetLogger().debug("Swapping anonPointDefinitions from old capacity: %lu", anonPointDefinitions.capacity());
    std::vector<PointDefinition*>().swap(anonPointDefinitions);
    TLogger::GetLogger().debug("to new capacity: %lu", anonPointDefinitions.capacity());
    BeatmapObjectSpawnController_Start(self);
}

bool UpdateCoroutine(BeatmapObjectCallbackController *callbackController, AnimateTrackContext& context) {
    float elapsedTime = TimeSourceHelper::getSongTime(callbackController->audioTimeSource) - context.startTime;
    float time = Easings::Interpolate(std::min(elapsedTime / context.duration, 1.0f), context.easing);
    if (!context.property->value.has_value()) {
        context.property->value = { 0 };
    }
    switch (context.property->type) {
    case PropertyType::linear:
        context.property->value->linear = context.points->InterpolateLinear(time);
        break;
    case PropertyType::vector3:
        context.property->value->vector3 = context.points->Interpolate(time);
        break;
    case PropertyType::vector4:
        context.property->value->vector4 = context.points->InterpolateVector4(time);
        break;
    case PropertyType::quaternion:
        context.property->value->quaternion = context.points->InterpolateQuaternion(time);
        break;
    }

    return elapsedTime < context.duration;
}

bool UpdatePathCoroutine(BeatmapObjectCallbackController *callbackController, AssignPathAnimationContext& context) {
    float elapsedTime = TimeSourceHelper::getSongTime(callbackController->audioTimeSource) - context.startTime;
    context.property->value->time = Easings::Interpolate(std::min(elapsedTime / context.duration, 1.0f), context.easing);

    return elapsedTime < context.duration;
}

void Events::UpdateCoroutines(BeatmapObjectCallbackController *callbackController) {
    for (auto it = coroutines.begin(); it != coroutines.end();) {
        if (UpdateCoroutine(callbackController, *it)) {
            it++;
        } else {
            delete it->anonPointDef;
            coroutines.erase(it);
        }
    }

    for (auto it = pathCoroutines.begin(); it != pathCoroutines.end();) {
        if (UpdatePathCoroutine(callbackController, *it)) {
            it++;
        } else {
            it->property->value->Finish();
            pathCoroutines.erase(it);
        }
    }
}

void CustomEventCallback(BeatmapObjectCallbackController *callbackController, CustomJSONData::CustomEventData *customEventData) {
    auto *customBeatmapData = reinterpret_cast<CustomJSONData::CustomBeatmapData *>(callbackController->beatmapData);
    BeatmapAssociatedData& ad = getBeatmapAD(customBeatmapData->customData);
    rapidjson::Value& eventData = *customEventData->data;

    EventType type;
    if (customEventData->type == "AnimateTrack") {
        type = EventType::animateTrack;
    } else if (customEventData->type == "AssignPathAnimation") {
        type = EventType::assignPathAnimation;
    } else {
        return;
    }

    Track *track = &ad.tracks[eventData["_track"].GetString()];
    float duration = customEventData->data->HasMember("_duration") ? eventData["_duration"].GetFloat() : 0;
    Functions easing = customEventData->data->HasMember("_easing") ? FunctionFromStr(eventData["_easing"].GetString()) : Functions::easeLinear;

    duration = 60 * duration / spawnController->get_currentBpm();

    auto& properties = track->properties;
    auto& pathProperties = track->pathProperties;

    rapidjson::Value::ConstMemberIterator itr;
    for (itr = eventData.MemberBegin(); itr < eventData.MemberEnd(); itr++) {
        const char *name = (*itr).name.GetString();
        if (strcmp(name, "_track") && strcmp(name, "_duration") && strcmp(name, "_easing")) {
            switch (type) {
            case EventType::animateTrack: {
                Property *property = properties.FindProperty(name);
                if (property) {
                    for (auto it = coroutines.begin(); it != coroutines.end();) {
                        if (it->property == property) {
                            coroutines.erase(it);
                        } else {
                            it++;
                        }
                    }

                    PointDefinition *anonPointDef = nullptr;;
                    auto *pointData = Animation::TryGetPointData(ad, anonPointDef, eventData, name);
                    if (pointData) {
                        coroutines.push_back(AnimateTrackContext { pointData, property, duration, customEventData->time, easing, anonPointDef });
                    }
                } else {
                    TLogger::GetLogger().warning("Could not find track property with name %s", name);
                }
                break;
            }
            case EventType::assignPathAnimation:
                PathProperty *property = pathProperties.FindProperty(name);
                if (property) {
                    for (auto it = pathCoroutines.begin(); it != pathCoroutines.end();) {
                        if (it->property == property) {
                            pathCoroutines.erase(it);
                        } else {
                            it++;
                        }
                    }

                    PointDefinition *anonPointDef = nullptr;
                    auto *pointData = Animation::TryGetPointData(ad, anonPointDef, eventData, name);
                    if (pointData) {
                        if (anonPointDef) {
                            anonPointDefinitions.push_back(anonPointDef);
                        }
                        if (!property->value.has_value()) property->value = PointDefinitionInterpolation();
                        property->value->Init(pointData);
                        pathCoroutines.push_back(AssignPathAnimationContext { property, duration, customEventData->time, easing });
                    }
                } else {
                    TLogger::GetLogger().warning("Could not find track path property with name %s", name);
                }
            }
        }
    }
}

void Events::AddEventCallbacks(Logger& logger) {
    CustomJSONData::CustomEventCallbacks::AddCustomEventCallback(&CustomEventCallback);

    INSTALL_HOOK(logger, BeatmapObjectSpawnController_Start);
}
