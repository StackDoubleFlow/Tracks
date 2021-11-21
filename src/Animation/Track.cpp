#include "Animation/Track.h"

#define PROP_GET(jsonName, varName) if (name == (jsonName)) return &varName;

Property *Properties::FindProperty(std::string_view name) {
    PROP_GET("_position", position)
    PROP_GET("_rotation", rotation)
    PROP_GET("_scale", scale)
    PROP_GET("_localRotation", localRotation)
    PROP_GET("_localPosition", localPosition)
    PROP_GET("_dissolve", dissolve)
    PROP_GET("_dissolveArrow", dissolveArrow)
    PROP_GET("_time", time)
    PROP_GET("_interactable", cuttable)
    PROP_GET("_color", color)
    PROP_GET("_attenuation", attentuation)
    PROP_GET("_offset", fogOffset)
    PROP_GET("_startY", heightFogStartY)
    PROP_GET("_height", heightFogHeight)

    return nullptr;
}

PathProperty *PathProperties::FindProperty(std::string_view name) {
    PROP_GET("_position", position)
    PROP_GET("_rotation", rotation)
    PROP_GET("_scale", scale)
    PROP_GET("_localRotation", localRotation)
    PROP_GET("_localPosition", localPosition)
    PROP_GET("_definitePosition", definitePosition)
    PROP_GET("_dissolve", dissolve)
    PROP_GET("_dissolveArrow", dissolveArrow)
    PROP_GET("_interactable", cuttable)
    PROP_GET("_color", color)

    return nullptr;
}

void Track::ResetVariables() {
    // properties.position.value = std::nullopt;
    // properties.rotation.value = std::nullopt;
    // properties.scale.value = std::nullopt;
    // properties.localRotation.value = std::nullopt;
    // properties.dissolve.value = std::nullopt;
    // properties.dissolveArrow.value = std::nullopt;
    // properties.time.value = std::nullopt;
    // properties.cuttable.value = std::nullopt;
    // properties.color.value = std::nullopt;

    // pathProperties.position.value = std::nullopt;
    // pathProperties.rotation.value = std::nullopt;
    // pathProperties.scale.value = std::nullopt;
    // pathProperties.localRotation.value = std::nullopt;
    // pathProperties.definitePosition.value = std::nullopt;
    // pathProperties.dissolve.value = std::nullopt;
    // pathProperties.dissolveArrow.value = std::nullopt;
    // pathProperties.cuttable.value = std::nullopt;
    // pathProperties.color.value = std::nullopt;
    properties = Properties();
    pathProperties = PathProperties();
    gameObjects.clear();
    gameObjectModificationEvent.clear();
}