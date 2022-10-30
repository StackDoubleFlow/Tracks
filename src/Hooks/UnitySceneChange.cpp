#include "THooks.h"
#include "beatsaber-hook/shared/utils/hooking.hpp"

#include "Animation/GameObjectTrackController.hpp"

#include "GlobalNamespace/GameScenesManager.hpp"
#include "UnityEngine/SceneManagement/SceneManager.hpp"
#include "UnityEngine/SceneManagement/Scene.hpp"
#include "UnityEngine/SceneManagement/LoadSceneMode.hpp"
#include "System/Action.hpp"
#include "custom-json-data/shared/CustomBeatmapData.h"


using namespace CustomJSONData;
using namespace GlobalNamespace;
using namespace UnityEngine;

MAKE_HOOK_MATCH(SceneManager_Internal_SceneLoaded,
                 &UnityEngine::SceneManagement::SceneManager::Internal_SceneLoaded,
                 void, UnityEngine::SceneManagement::Scene scene, UnityEngine::SceneManagement::LoadSceneMode mode) {


    if (scene && scene.IsValid() && scene.get_name() == "GameCore") {
        Tracks::GameObjectTrackController::ClearData();
    }

    SceneManager_Internal_SceneLoaded(scene, mode);
}

void SceneManager_Internal(Logger& logger) {
    INSTALL_HOOK(logger, SceneManager_Internal_SceneLoaded);
}

TInstallHooks(SceneManager_Internal)