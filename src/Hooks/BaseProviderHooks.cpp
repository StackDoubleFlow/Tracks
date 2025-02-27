#include "THooks.h"
#include "TLogger.h"
#include "beatsaber-hook/shared/utils/hooking.hpp"

#include "Animation/GameObjectTrackController.hpp"

#include "GlobalNamespace/GameplayCoreInstaller.hpp"
#include "GlobalNamespace/GameplayCoreSceneSetupData.hpp"
#include "GlobalNamespace/PlayerSpecificSettings.hpp"
#include "GlobalNamespace/ColorScheme.hpp"
#include "GlobalNamespace/PlayerTransforms.hpp"
#include "UnityEngine/Transform.hpp"

#include "Animation/PointDefinition.h"
#include "tracks-rs/shared/bindings.h"

using namespace CustomJSONData;
using namespace GlobalNamespace;
using namespace UnityEngine;

MAKE_HOOK_MATCH(GameplayCoreInstaller_InstallBindings, &GlobalNamespace::GameplayCoreInstaller::InstallBindings,
                void, GlobalNamespace::GameplayCoreInstaller* self) {

  GameplayCoreInstaller_InstallBindings(self);
  auto colorScheme = self->_sceneSetupData->colorScheme;
  auto context = internal_tracks_context;

  bool leftHanded = self->_sceneSetupData->playerSpecificSettings->leftHanded;

  auto baseEnvironmentColor0 = colorScheme->environmentColor0;
  auto baseEnvironmentColor0Boost = colorScheme->environmentColor0Boost;
  auto baseEnvironmentColor1 = colorScheme->environmentColor1;
  auto baseEnvironmentColor1Boost = colorScheme->environmentColor1Boost;
  auto baseEnvironmentColorW = colorScheme->environmentColorW;
  auto baseEnvironmentColorWBoost = colorScheme->environmentColorWBoost;
  auto baseNoteColor1 = leftHanded ? colorScheme->saberAColor : colorScheme->saberBColor;
  auto baseNoteColor0 = leftHanded ? colorScheme->saberBColor : colorScheme->saberAColor;
  auto baseObstaclesColor = colorScheme->obstaclesColor;
  auto baseSaberAColor = colorScheme->saberAColor;
  auto baseSaberBColor = colorScheme->saberBColor;
  
  Tracks::tracks_set_base_provider(context, "baseEnvironmentColor0", new float[4] {baseEnvironmentColor0.r, baseEnvironmentColor0.g, baseEnvironmentColor0.b, baseEnvironmentColor0.a}, 4, false);
  Tracks::tracks_set_base_provider(context, "baseEnvironmentColor0Boost", new float[4] {baseEnvironmentColor0Boost.r, baseEnvironmentColor0Boost.g, baseEnvironmentColor0Boost.b, baseEnvironmentColor0Boost.a}, 4, false);
  Tracks::tracks_set_base_provider(context, "baseEnvironmentColor1", new float[4] {baseEnvironmentColor1.r, baseEnvironmentColor1.g, baseEnvironmentColor1.b, baseEnvironmentColor1.a}, 4, false);
  Tracks::tracks_set_base_provider(context, "baseEnvironmentColor1Boost", new float[4] {baseEnvironmentColor1Boost.r, baseEnvironmentColor1Boost.g, baseEnvironmentColor1Boost.b, baseEnvironmentColor1Boost.a}, 4, false);
  Tracks::tracks_set_base_provider(context, "baseEnvironmentColorW", new float[4] {baseEnvironmentColorW.r, baseEnvironmentColorW.g, baseEnvironmentColorW.b, baseEnvironmentColorW.a}, 4, false);
  Tracks::tracks_set_base_provider(context, "baseEnvironmentColorWBoost", new float[4] {baseEnvironmentColorWBoost.r, baseEnvironmentColorWBoost.g, baseEnvironmentColorWBoost.b, baseEnvironmentColorWBoost.a}, 4, false);
  Tracks::tracks_set_base_provider(context, "baseNote0Color", new float[4] {baseNoteColor0.r, baseNoteColor0.g, baseNoteColor0.b, baseNoteColor0.a}, 4, false);
  Tracks::tracks_set_base_provider(context, "baseNote1Color", new float[4] {baseNoteColor1.r, baseNoteColor1.g, baseNoteColor1.b, baseNoteColor1.a}, 4, false);
  Tracks::tracks_set_base_provider(context, "baseObstaclesColor", new float[4] {baseObstaclesColor.r, baseObstaclesColor.g, baseObstaclesColor.b, baseObstaclesColor.a}, 4, false);
  Tracks::tracks_set_base_provider(context, "baseSaberAColor", new float[4] {baseSaberAColor.r, baseSaberAColor.g, baseSaberAColor.b, baseSaberAColor.a}, 4, false);
  Tracks::tracks_set_base_provider(context, "baseSaberBColor", new float[4] {baseSaberBColor.r, baseSaberBColor.g, baseSaberBColor.b, baseSaberBColor.a}, 4, false);
}

MAKE_HOOK_MATCH(PlayerTransforms_Update, &GlobalNamespace::PlayerTransforms::Update,
                void , GlobalNamespace::PlayerTransforms* self) {
  PlayerTransforms_Update(self);
  auto context = internal_tracks_context;

  auto leftHand = self->_leftHandTransform;
  //leftHand = leftHand->parent == nullptr ? leftHand : leftHand->parent;
  auto rightHand = self->_rightHandTransform;
  //rightHand = rightHand->parent == nullptr ? rightHand : rightHand->parent;

  auto baseHeadLocalPosition = self->_headTransform->localPosition;
  auto baseHeadLocalRotation = self->_headTransform->localRotation;
  auto baseHeadLocalScale = self->_headTransform->localScale;
  auto baseHeadPosition = self->_headTransform->position;
  auto baseHeadRotation = self->_headTransform->rotation;
  auto baseLeftHandLocalPosition = leftHand->localPosition;
  auto baseLeftHandLocalRotation = leftHand->localRotation;
  auto baseLeftHandLocalScale = leftHand->localScale;
  auto baseLeftHandPosition = leftHand->position;
  auto baseLeftHandRotation = leftHand->rotation;
  
  auto baseRightHandLocalPosition = rightHand->localPosition;
  auto baseRightHandLocalRotation = rightHand->localRotation;
  auto baseRightHandLocalScale = rightHand->localScale;
  auto baseRightHandPosition = rightHand->position;
  auto baseRightHandRotation = rightHand->rotation;

  Tracks::tracks_set_base_provider(context, "baseHeadLocalPosition", new float[3] {baseHeadLocalPosition.x, baseHeadLocalPosition.y, baseHeadLocalPosition.z}, 3, false);
  Tracks::tracks_set_base_provider(context, "baseHeadLocalRotation", new float[4] {baseHeadLocalRotation.x, baseHeadLocalRotation.y, baseHeadLocalRotation.z, baseHeadLocalRotation.w}, 4, true);
  Tracks::tracks_set_base_provider(context, "baseHeadLocalScale", new float[3] {baseHeadLocalScale.x, baseHeadLocalScale.y, baseHeadLocalScale.z}, 3, false);
  Tracks::tracks_set_base_provider(context, "baseHeadPosition", new float[3] {baseHeadPosition.x, baseHeadPosition.y, baseHeadPosition.z}, 3, false);
  Tracks::tracks_set_base_provider(context, "baseHeadRotation", new float[4] {baseHeadRotation.x, baseHeadRotation.y, baseHeadRotation.z, baseHeadRotation.w}, 4, true);
  Tracks::tracks_set_base_provider(context, "baseLeftHandLocalPosition", new float[3] {baseLeftHandLocalPosition.x, baseLeftHandLocalPosition.y, baseLeftHandLocalPosition.z}, 3, false);
  Tracks::tracks_set_base_provider(context, "baseLeftHandLocalRotation", new float[4] {baseLeftHandLocalRotation.x, baseLeftHandLocalRotation.y, baseLeftHandLocalRotation.z, baseLeftHandLocalRotation.w}, 4, true);
  Tracks::tracks_set_base_provider(context, "baseLeftHandLocalScale", new float[3] {baseLeftHandLocalScale.x, baseLeftHandLocalScale.y, baseLeftHandLocalScale.z}, 3, false);
  Tracks::tracks_set_base_provider(context, "baseLeftHandPosition", new float[3] {baseLeftHandPosition.x, baseLeftHandPosition.y, baseLeftHandPosition.z}, 3, false);
  Tracks::tracks_set_base_provider(context, "baseLeftHandRotation", new float[4] {baseLeftHandRotation.x, baseLeftHandRotation.y, baseLeftHandRotation.z, baseLeftHandRotation.w}, 4, true);
  Tracks::tracks_set_base_provider(context, "baseRightHandLocalPosition", new float[3] {baseRightHandLocalPosition.x, baseRightHandLocalPosition.y, baseRightHandLocalPosition.z}, 3, false);
  Tracks::tracks_set_base_provider(context, "baseRightHandLocalRotation", new float[4] {baseRightHandLocalRotation.x, baseRightHandLocalRotation.y, baseRightHandLocalRotation.z, baseRightHandLocalRotation.w}, 4, true);
  Tracks::tracks_set_base_provider(context, "baseRightHandLocalScale", new float[3] {baseRightHandLocalScale.x, baseRightHandLocalScale.y, baseRightHandLocalScale.z}, 3, false);
  Tracks::tracks_set_base_provider(context, "baseRightHandPosition", new float[3] {baseRightHandPosition.x, baseRightHandPosition.y, baseRightHandPosition.z}, 3, false);
  Tracks::tracks_set_base_provider(context, "baseRightHandRotation", new float[4] {baseRightHandRotation.x, baseRightHandRotation.y, baseRightHandRotation.z, baseRightHandRotation.w}, 4, true);
}

void InstallBaseProviderHooks() {
  auto logger = Paper::ConstLoggerContext("Tracks | InstallBaseProviderHooks");
  INSTALL_HOOK(logger, GameplayCoreInstaller_InstallBindings);
  INSTALL_HOOK(logger, PlayerTransforms_Update);
}

TInstallHooks(InstallBaseProviderHooks)