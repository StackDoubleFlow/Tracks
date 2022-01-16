#include "GlobalNamespace/BeatmapObjectCallbackController.hpp"
#include "GlobalNamespace/BeatmapData.hpp"
#include "GlobalNamespace/BeatmapObjectSpawnController.hpp"
#include "GlobalNamespace/VariableBpmProcessor.hpp"

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

MAKE_HOOK_MATCH(BeatmapObjectSpawnController_Start, &BeatmapObjectSpawnController::Start, void, BeatmapObjectSpawnController *self) {
    spawnController = self;
    coroutines.clear();
    pathCoroutines.clear();
    TLogger::GetLogger().debug("coroutines and pathCoroutines capacity: %lu and %lu", coroutines.capacity(), pathCoroutines.capacity());
    BeatmapObjectSpawnController_Start(self);
}

constexpr bool UpdateCoroutine(AnimateTrackContext const& context, float songTime) {
    float elapsedTime = songTime - context.startTime;
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

constexpr bool UpdatePathCoroutine(AssignPathAnimationContext const& context, float songTime) {
    float elapsedTime = songTime - context.startTime;
    context.property->value->time = Easings::Interpolate(std::min(elapsedTime / context.duration, 1.0f), context.easing);

    return elapsedTime < context.duration;
}

void Events::UpdateCoroutines(BeatmapObjectCallbackController *callbackController) {
    auto songTime = TimeSourceHelper::getSongTime(callbackController->audioTimeSource);
    for (auto it = coroutines.begin(); it != coroutines.end();) {
        if (UpdateCoroutine(*it, songTime)) {
            it++;
        } else {
            it = coroutines.erase(it);
        }
    }

    for (auto it = pathCoroutines.begin(); it != pathCoroutines.end();) {
        if (UpdatePathCoroutine(*it, songTime)) {
            it++;
        } else {
            it->property->value->Finish();
            it = pathCoroutines.erase(it);
        }
    }
}

// BeatmapDataTransformHelper.cpp
void LoadTrackEvent(CustomJSONData::CustomEventData const* customEventData, TracksAD::BeatmapAssociatedData& beatmapAD);

void CustomEventCallback(BeatmapObjectCallbackController *callbackController, CustomJSONData::CustomEventData *customEventData) {
    bool isType = false;

    static std::hash<std::string_view> stringViewHash;
    auto typeHash = stringViewHash(customEventData->type);

#define TYPE_GET(jsonName, varName)                                \
    static auto jsonNameHash_##varName = stringViewHash(jsonName); \
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

        LoadTrackEvent(customEventData, beatmapAD);
    }

    auto duration = eventAD.duration;

    auto bpm = spawnController->variableBpmProcessor->currentBpm; // spawnController->get_currentBpm()

    duration = 60 * duration / bpm;

    auto easing = eventAD.easing;

    switch (eventAD.type)
    {
        case EventType::animateTrack: {
            auto const& animateTrackDataList = eventAD.animateTrackData;

            for (auto const& animateTrackData : animateTrackDataList) {

                for (auto const &[property, pointData]: animateTrackData.properties) {
                    for (auto it = coroutines.begin(); it != coroutines.end();) {
                        if (it->property == property) {
                            it = coroutines.erase(it);
                        } else {
                            it++;
                        }
                    }

                    if (pointData) {
                        coroutines.emplace_back(pointData, property, duration, customEventData->time, easing);
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
                        } else {
                            it++;
                        }
                    }

                    if (pointData) {
                        if (!property->value.has_value())
                            property->value = PointDefinitionInterpolation();
                        property->value->Init(pointData);
                        pathCoroutines.emplace_back(property, duration, customEventData->time, easing);
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
}

void Events::AddEventCallbacks(Logger& logger) {
    CustomJSONData::CustomEventCallbacks::AddCustomEventCallback(&CustomEventCallback);

    INSTALL_HOOK(logger, BeatmapObjectSpawnController_Start);
}
