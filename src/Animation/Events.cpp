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

BeatmapObjectSpawnController* spawnController;
// BeatmapObjectSpawnController.cpp

MAKE_HOOK_MATCH(BeatmapObjectSpawnController_Start, &BeatmapObjectSpawnController::Start, void,
                BeatmapObjectSpawnController* self) {
  spawnController = self;
  BeatmapObjectSpawnController_Start(self);
}

void Events::UpdateCoroutines(BeatmapCallbacksController* callbackController) {
  auto songTime = callbackController->songTime;
  auto* customBeatmapData = (CustomJSONData::CustomBeatmapData*)callbackController->_beatmapData;
  auto& beatmapAD = getBeatmapAD(customBeatmapData->customData);

  auto tracksContext = beatmapAD.internal_tracks_context.get()->internal_tracks_context;

  auto coroutine = Tracks::ffi::tracks_context_get_coroutine_manager(tracksContext);
  auto baseManager = Tracks::ffi::tracks_context_get_base_provider_context(tracksContext);
  Tracks::ffi::poll_events(coroutine, songTime, baseManager);
}

void CustomEventCallback(BeatmapCallbacksController* callbackController,
                         CustomJSONData::CustomEventData* customEventData) {
  PAPER_IL2CPP_CATCH_HANDLER(
      bool isType = false;

      auto typeHash = customEventData->typeHash;

#define TYPE_GET(jsonName, varName)                                                                                    \
  static auto jsonNameHash_##varName = std::hash<std::string_view>()(jsonName);                                        \
  if (!isType && typeHash == (jsonNameHash_##varName)) isType = true;

      TYPE_GET("AnimateTrack", AnimateTrack) TYPE_GET("AssignPathAnimation", AssignPathAnimation)

          if (!isType) { return; }

      CustomEventAssociatedData const& eventAD = getEventAD(customEventData);

      // fail safe, idek why this needs to be done smh
      // CJD you bugger
      if (!eventAD.parsed) {
        TLogger::Logger.debug("callbackController {}", fmt::ptr(callbackController));
        TLogger::Logger.debug("_beatmapData {}", fmt::ptr(callbackController->_beatmapData));
        auto* customBeatmapData = (CustomJSONData::CustomBeatmapData*)callbackController->_beatmapData;
        TLogger::Logger.debug("customBeatmapData {}", fmt::ptr(customBeatmapData));

        TracksAD::BeatmapAssociatedData& beatmapAD = TracksAD::getBeatmapAD(customBeatmapData->customData);

        if (!beatmapAD.valid) {
          TLogger::Logger.debug("Beatmap wasn't parsed when event is invoked, what?");
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

      auto easing = eventAD.easing; auto repeat = eventAD.repeat;

      bool noDuration = duration == 0 || customEventData->time + (duration * (repeat + 1)) <
                                             TracksStatic::bpmController->_beatmapCallbacksController->songTime;

      // TODO: Enqueue event
  )
}

void Events::AddEventCallbacks() {
  auto logger = Paper::ConstLoggerContext("Tracks | AddEventCallbacks");
  CustomJSONData::CustomEventCallbacks::AddCustomEventCallback(&CustomEventCallback);

  INSTALL_HOOK(logger, BeatmapObjectSpawnController_Start);
}
