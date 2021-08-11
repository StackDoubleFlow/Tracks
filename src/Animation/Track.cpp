#include "Animation/Track.h"

enum class PropertyName {
    position,
    rotation,
    scale,
    localRotation,
    definitePosition,
    dissolve,
    dissolveArrow,
    time,
    cuttable,
    color
};

Property *Properties::FindProperty(std::string name) {
    static std::unordered_map<std::string, PropertyName> const functions = {
        { "_position", PropertyName::position },
        { "_rotation", PropertyName::rotation },
        { "_scale", PropertyName::scale },
        { "_localRotation", PropertyName::localRotation },
        { "_dissolve", PropertyName::dissolve },
        { "_dissolveArrow", PropertyName::dissolveArrow },
        { "_time", PropertyName::time },
        { "_cuttable", PropertyName::cuttable },
        { "_color", PropertyName::color }
    };

    auto itr = functions.find(name);
    if (itr != functions.end()) {
        switch (itr->second) {
            case PropertyName::position: return &position;
            case PropertyName::rotation: return &rotation;
            case PropertyName::scale: return &scale;
            case PropertyName::localRotation: return &localRotation;
            case PropertyName::dissolve: return &dissolve;
            case PropertyName::dissolveArrow: return &dissolveArrow;
            case PropertyName::time: return &time;
            case PropertyName::cuttable: return &cuttable;
            case PropertyName::color: return &color;
            default: return nullptr;
        }
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
        { "_definitePosition", PropertyName::definitePosition },
        { "_dissolve", PropertyName::dissolve },
        { "_dissolveArrow", PropertyName::dissolveArrow },
        { "_cuttable", PropertyName::cuttable },
        { "_color", PropertyName::color }
    };

    auto itr = functions.find(name);
    if (itr != functions.end()) {
        switch (itr->second) {
            case PropertyName::position: return &position;
            case PropertyName::rotation: return &rotation;
            case PropertyName::scale: return &scale;
            case PropertyName::localRotation: return &localRotation;
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
}