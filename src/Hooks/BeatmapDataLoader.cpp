#include "TLogger.h"
#include "THooks.h"
#include "AssociatedData.h"
#include "Animation/PointDefinition.h"
#include "beatsaber-hook/shared/utils/hooking.hpp"
#include "custom-json-data/shared/CustomBeatmapData.h"
#include "GlobalNamespace/BeatmapLineData.hpp"

#include "GlobalNamespace/BeatmapData.hpp"
#include "GlobalNamespace/BeatmapDataLoader.hpp"

#include "System/Linq/Enumerable.hpp"

using namespace GlobalNamespace;
using namespace TracksAD;


void InstallBeatmapDataLoaderHooks(Logger &logger) {
//    INSTALL_HOOK(logger, GetBeatmapDataFromBeatmapSaveData);
}

TInstallHooks(InstallBeatmapDataLoaderHooks);