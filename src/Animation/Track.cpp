#include "Animation/Track.h"

// TODO: Deprecate
enum class PropertyName {
    position,
    rotation,
    scale,
    localRotation,
    localPosition,
    definitePosition,
    dissolve,
    dissolveArrow,
    time,
    cuttable,
    color
};

Property *Properties::FindProperty(std::string_view name) {
    static std::unordered_map<std::string_view, Property*> const functions = {
        {"_position", &position},
        {"_rotation", &rotation},
        {"_scale", &scale},
        {"_localRotation", &localRotation},
        {"_localPosition", &localPosition},
        {"_dissolve", &dissolve},
        {"_dissolveArrow", &dissolveArrow},
        {"_time", &time},
        {"_interactable", &cuttable},
        {"_color", &color},
        {"_attenuation", &attentuation},
        {"_offset", &fogOffset},
        {"_startY", &heightFogStartY},
        {"_height", &heightFogHeight}
    };

    auto itr = functions.find(name);
    if (itr != functions.end()) {
        return itr->second;
    } else {
        return nullptr;
    }
}

PathProperty *PathProperties::FindProperty(std::string_view name) {
    static std::unordered_map<std::string_view, PropertyName> const functions = {
        { "_position", PropertyName::position },
        { "_rotation", PropertyName::rotation },
        { "_scale", PropertyName::scale },
        { "_localRotation", PropertyName::localRotation },
        { "_localPosition", PropertyName::localPosition },
        { "_definitePosition", PropertyName::definitePosition },
        { "_dissolve", PropertyName::dissolve },
        { "_dissolveArrow", PropertyName::dissolveArrow },
        { "_interactable", PropertyName::cuttable },
        { "_color", PropertyName::color }
    };

    auto itr = functions.find(name);
    if (itr != functions.end()) {
        switch (itr->second) {
            case PropertyName::position: return &position;
            case PropertyName::rotation: return &rotation;
            case PropertyName::scale: return &scale;
            case PropertyName::localRotation: return &localRotation;
            case PropertyName::localPosition: return &localPosition;
            case PropertyName::definitePosition: return &definitePosition;
            case PropertyName::dissolve: return &dissolve;
            case PropertyName::dissolveArrow: return &dissolveArrow;
            case PropertyName::cuttable: return &cuttable;
            case PropertyName::color: return &color;
            default: return nullptr;
        }
    } else {
        return nullptr;
    }
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