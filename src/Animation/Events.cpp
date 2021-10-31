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
    if (customEventData->type != "AnimateTrack" && customEventData->type != "AssignPathAnimation") {
        return;
    }

    CustomEventAssociatedData const &eventAD = getEventAD(customEventData);

    auto duration = eventAD.duration;

    duration = 60 * duration / spawnController->get_currentBpm();

    auto easing = eventAD.easing;
    auto track = eventAD.track;

    switch (eventAD.type)
    {
        case EventType::animateTrack: {
            AnimateTrackData animateTrackData = *eventAD.animateTrackData;

            for (auto const& property : animateTrackData.properties) {
                for (auto it = coroutines.begin(); it != coroutines.end();) {
                    if (it->property == property) {
                        it = coroutines.erase(it);
                    } else {
                        it++;
                    }
                }

                auto anonPointDef = animateTrackData.anonPointDef[property];
                auto pointData = animateTrackData.pointData[property];

                if (pointData)
                {
                    coroutines.emplace_back(pointData, property, duration, customEventData->time, easing, anonPointDef);
                }
                else
                {
                    property->value = std::nullopt;
                }
            }
            break;
        }
        case EventType::assignPathAnimation: {
            AssignPathAnimationData assignPathAnimationData = *eventAD.assignPathAnimation;

            for (auto const& property : assignPathAnimationData.pathProperties) {
                for (auto it = pathCoroutines.begin(); it != pathCoroutines.end();)
                {
                    if (it->property == property)
                    {
                        it = pathCoroutines.erase(it);
                    }
                    else
                    {
                        it++;
                    }
                }

                PointDefinition *anonPointDef = assignPathAnimationData.anonPointDef[property];
                auto *pointData = assignPathAnimationData.pointData[property];
                if (pointData)
                {
                    if (anonPointDef)
                    {
                        anonPointDefinitions.push_back(anonPointDef);
                    }
                    if (!property->value.has_value())
                        property->value = PointDefinitionInterpolation();
                    property->value->Init(pointData);
                    pathCoroutines.emplace_back(property, duration, customEventData->time, easing);
                }
                else
                {
                    property->value = std::nullopt;
                }
            }
            break;
        }
        default:
            break;
    }
}

void Events::AddEventCallbacks(Logger& logger) {
    CustomJSONData::CustomEventCallbacks::AddCustomEventCallback(&CustomEventCallback);

    INSTALL_HOOK(logger, BeatmapObjectSpawnController_Start);
}
