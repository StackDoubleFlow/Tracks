#include "AssociatedData.h"
#include "Animation/Track.h"

using namespace TracksAD::Constants;

#include <functional>

#define PROP_GET(jsonName, varName)                                \
    static auto jsonNameHash_##varName = stringViewHash(jsonName); \
    if (nameHash == (jsonNameHash_##varName))                      \
        return &varName;

Property *Properties::FindProperty(std::string_view name) {
    static std::hash<std::string_view> stringViewHash;
    auto nameHash = stringViewHash(name);

    if (v2) {
        PROP_GET(V2_POSITION, position)
        PROP_GET(V2_ROTATION, rotation)
        PROP_GET(V2_SCALE, scale)
        PROP_GET(V2_LOCAL_ROTATION, localRotation)
        PROP_GET(V2_LOCAL_POSITION, localPosition)
        PROP_GET(V2_DISSOLVE, dissolve)
        PROP_GET(V2_DISSOLVE_ARROW, dissolveArrow)
        PROP_GET(V2_TIME, time)
        PROP_GET(V2_CUTTABLE, cuttable)
        PROP_GET(V2_COLOR, color)
        PROP_GET(V2_ATTENUATION, attentuation)
        PROP_GET(V2_OFFSET, fogOffset)
        PROP_GET(V2_HEIGHT_FOG_STARTY, heightFogStartY)
        PROP_GET(V2_HEIGHT_FOG_HEIGHT, heightFogHeight)
    } else {
        PROP_GET(OFFSET_POSITION, position)
        PROP_GET(OFFSET_ROTATION, rotation)
        PROP_GET(SCALE, scale)
        PROP_GET(LOCAL_ROTATION, localRotation)
        PROP_GET(LOCAL_POSITION, localPosition)
        PROP_GET(DISSOLVE, dissolve)
        PROP_GET(DISSOLVE_ARROW, dissolveArrow)
        PROP_GET(TIME, time)
        PROP_GET(INTERACTABLE, cuttable)
        PROP_GET(COLOR, color)
        PROP_GET(ATTENUATION, attentuation)
        PROP_GET(OFFSET, fogOffset)
        PROP_GET(HEIGHT_FOG_STARTY, heightFogStartY)
        PROP_GET(HEIGHT_FOG_HEIGHT, heightFogHeight)
    }

    return nullptr;
}

PathProperty *PathProperties::FindProperty(std::string_view name) {
    static std::hash<std::string_view> stringViewHash;
    auto nameHash = stringViewHash(name);

    if (v2) {
        PROP_GET(V2_POSITION, position)
        PROP_GET(V2_ROTATION, rotation)
        PROP_GET(V2_SCALE, scale)
        PROP_GET(V2_LOCAL_ROTATION, localRotation)
        PROP_GET(V2_LOCAL_POSITION, localPosition)
        PROP_GET(V2_DEFINITE_POSITION, definitePosition)
        PROP_GET(V2_DISSOLVE, dissolve)
        PROP_GET(V2_DISSOLVE_ARROW, dissolveArrow)
        PROP_GET(V2_CUTTABLE, cuttable)
        PROP_GET(V2_COLOR, color)
    } else {
        PROP_GET(OFFSET_POSITION, position)
        PROP_GET(OFFSET_ROTATION, rotation)
        PROP_GET(SCALE, scale)
        PROP_GET(LOCAL_ROTATION, localRotation)
        PROP_GET(LOCAL_POSITION, localPosition)
        PROP_GET(DEFINITE_POSITION, definitePosition)
        PROP_GET(DISSOLVE, dissolve)
        PROP_GET(DISSOLVE_ARROW, dissolveArrow)
        PROP_GET(INTERACTABLE, cuttable)
        PROP_GET(COLOR, color)
    }

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
    properties = Properties(v2);
    pathProperties = PathProperties(v2);
    gameObjects.clear();
    gameObjectModificationEvent.clear();
}