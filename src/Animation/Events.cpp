#include "beatsaber-hook/shared/utils/hooking.hpp"

#include "GlobalNamespace/BeatmapCallbacksController.hpp"
#include "GlobalNamespace/BeatmapData.hpp"
#include "GlobalNamespace/BeatmapObjectSpawnController.hpp"
#include "GlobalNamespace/BpmController.hpp"

#include "UnityEngine/Resources.hpp"

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
#include "StaticHolders.hpp"

using namespace Events;
using namespace GlobalNamespace;
using namespace NEVector;
using namespace TracksAD;

BeatmapObjectSpawnController *spawnController;
// BeatmapObjectSpawnController.cpp

static std::vector<AnimateTrackContext> coroutines;
static std::vector<AssignPathAnimationContext> pathCoroutines;

MAKE_HOOK_MATCH(BeatmapObjectSpawnController_Start, &BeatmapObjectSpawnController::Start, void, BeatmapObjectSpawnController *self) {
    spawnController = self;
    coroutines.clear();
    pathCoroutines.clear();
    TLogger::GetLogger().debug("coroutines and pathCoroutines capacity: %lu and %lu", coroutines.capacity(), pathCoroutines.capacity());
    BeatmapObjectSpawnController_Start(self);
}

constexpr bool UpdateCoroutine(AnimateTrackContext const& context, float songTime) {
    float elapsedTime = songTime - context.startTime;
    float normalizedTime = context.duration > 0 ? std::min(elapsedTime / context.duration, 1.0f) : 1;
    float time = Easings::Interpolate(normalizedTime, context.easing);
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

    return context.duration <= 0 || elapsedTime < context.duration;
}

constexpr bool UpdatePathCoroutine(AssignPathAnimationContext const& context, float songTime) {
    float elapsedTime = songTime - context.startTime;
    context.property->value->time = Easings::Interpolate(std::min(elapsedTime / context.duration, 1.0f), context.easing);

    return context.duration <= 0 || elapsedTime < context.duration;
}

void Events::UpdateCoroutines(BeatmapCallbacksController *callbackController) {
    auto songTime = callbackController->songTime;
    for (auto it = coroutines.begin(); it != coroutines.end();) {
        if (UpdateCoroutine(*it, songTime)) {
            it++;
        } else {

            if (it->repeat <= 0) {
                it = coroutines.erase(it);
            } else {
                it->repeat--;
                it->startTime += it->duration;
            }
        }
    }

    for (auto it = pathCoroutines.begin(); it != pathCoroutines.end();) {
        if (UpdatePathCoroutine(*it, songTime)) {
            it++;
        } else {
            it->property->value->Finish();
            if (it->repeat <= 0) {
                it = pathCoroutines.erase(it);
            } else {
                it->repeat--;
                it->startTime += it->duration;
                it->property->value->Restart();
            }
        }
    }
}

// BeatmapDataTransformHelper.cpp
void LoadTrackEvent(CustomJSONData::CustomEventData const *customEventData, TracksAD::BeatmapAssociatedData &beatmapAD,
                    bool v2);

void CustomEventCallback(BeatmapCallbacksController *callbackController, CustomJSONData::CustomEventData *customEventData) {
    PAPER_IL2CPP_CATCH_HANDLER(
    bool isType = false;

    auto typeHash = customEventData->typeHash;

#define TYPE_GET(jsonName, varName)                                \
    static auto jsonNameHash_##varName = std::hash<std::string_view>()(jsonName); \
    if (!isType && typeHash == (jsonNameHash_##varName))                      \
        isType = true;

    TYPE_GET("AnimateTrack", AnimateTrack)
    TYPE_GET("AssignPathAnimation", AssignPathAnimation)

    if (!isType) {
        return;
    }

    CustomEventAssociatedData const &eventAD = getEventAD(customEventData);

    // fail safe, idek why this needs to be done smh
    // CJD you bugger
    if (!eventAD.parsed) {
        auto *customBeatmapData = (CustomJSONData::CustomBeatmapData *)callbackController->beatmapData;
        TracksAD::BeatmapAssociatedData &beatmapAD = TracksAD::getBeatmapAD(customBeatmapData->customData);

        if (!beatmapAD.valid) {
            TLogger::GetLogger().debug("Beatmap wasn't parsed when event is invoked, what?");
            TracksAD::readBeatmapDataAD(customBeatmapData);
        }

        LoadTrackEvent(customEventData, beatmapAD, customBeatmapData->v2orEarlier);
    }

    auto duration = eventAD.duration;

    if (!TracksStatic::bpmController) {
        CJDLogger::Logger.fmtLog<Paper::LogLevel::ERR>("BPM CONTROLLER NOT INITIALIZED");
    }

    auto bpm = TracksStatic::bpmController->currentBpm; // spawnController->get_currentBpm()

    duration = 60.0f * duration / bpm;

    auto easing = eventAD.easing;
    auto repeat = eventAD.repeat;

    switch (eventAD.type)
    {
        case EventType::animateTrack: {
            auto const& animateTrackDataList = eventAD.animateTrackData;

            for (auto const& animateTrackData : animateTrackDataList) {

                for (auto const &[property, pointData]: animateTrackData.properties) {
                    for (auto it = coroutines.begin(); it != coroutines.end();) {
                        if (it->property == property) {
                            it = coroutines.erase(it);
                            break;
                        } else {
                            it++;
                        }
                    }

                    if (pointData) {
                        float pointDuration = pointData->isSingle() ? 0 : duration;
                        coroutines.emplace_back(pointData, property, pointDuration, customEventData->time, easing, repeat);
                    } else {
                        property->value = std::nullopt;
                    }
                }
            }
            break;
        }
        case EventType::assignPathAnimation: {
            auto const& assignPathAnimationDataList = eventAD.assignPathAnimation;

            for (auto const& assignPathAnimationData : assignPathAnimationDataList) {
                for (auto const& [property, pointData] : assignPathAnimationData.pathProperties) {
                    for (auto it = pathCoroutines.begin(); it != pathCoroutines.end();) {
                        if (it->property == property) {
                            it = pathCoroutines.erase(it);
                            break;
                        } else {
                            it++;
                        }
                    }

                    if (pointData) {
                        if (!property->value.has_value())
                            property->value = PointDefinitionInterpolation();
                        property->value->Init(pointData);
//                        float pointDuration = pointData->isSingle() ? 0 : duration;
                        pathCoroutines.emplace_back(property, duration, customEventData->time, easing, repeat);
                    } else {
                        property->value = std::nullopt;
                    }
                }
            }
            break;
        }
        default:
            break;
    }
    )
}

void Events::AddEventCallbacks(Logger& logger) {
    CustomJSONData::CustomEventCallbacks::AddCustomEventCallback(&CustomEventCallback);

    INSTALL_HOOK(logger, BeatmapObjectSpawnController_Start);
}
