#pragma once
#include <map>
#include <string>
#include "PointDefinitionInterpolation.h"
#include "../Vector.h"
#include "UnityEngine/GameObject.hpp"

#include "beatsaber-hook/shared/utils/typedefs-wrappers.hpp"

namespace Events {
    struct AnimateTrackContext;
}

enum struct PropertyType {
    vector3,
    vector4,
    quaternion,
    linear
};

union PropertyValue {
    float linear;
    NEVector::Vector3 vector3;
    NEVector::Vector4 vector4;
    NEVector::Quaternion quaternion;
};

struct Property {
    Property(PropertyType t) : type{t}, value{std::nullopt} {};
    PropertyType type;
    std::optional<PropertyValue> value;
};

struct PathProperty {
    PathProperty(PropertyType t) : type{t}, value{std::nullopt} {};
    PropertyType type;
    std::optional<PointDefinitionInterpolation> value;
};

class Properties {
public:
    Properties() : position{Property(PropertyType::vector3)},
                   rotation{Property(PropertyType::quaternion)},
                   scale{Property(PropertyType::vector3)},
                   localRotation{Property(PropertyType::quaternion)},
                   localPosition{Property(PropertyType::vector3)},
                   dissolve{Property(PropertyType::linear)},
                   dissolveArrow{Property(PropertyType::linear)},
                   time{Property(PropertyType::linear)},
                   cuttable{Property(PropertyType::linear)},
                   color{Property(PropertyType::vector4)} {};
    Property *FindProperty(std::string_view name);
    
    Property position;
    Property rotation;
    Property scale;
    Property localRotation;
    Property localPosition;
    Property dissolve;
    Property dissolveArrow;
    Property time;
    Property cuttable;
    Property color;
};

class PathProperties {
public:
    PathProperties() : position{PropertyType::vector3},
                       rotation{PropertyType::quaternion},
                       scale{PropertyType::vector3},
                       localRotation{PropertyType::quaternion},
                       localPosition{PropertyType::vector3},
                       definitePosition{PropertyType::linear},
                       dissolve{PropertyType::linear},
                       dissolveArrow{PropertyType::linear},
                       cuttable{PropertyType::linear},
                       color{PropertyType::vector4} {};
    PathProperty *FindProperty(std::string_view name);

    PathProperty position;
    PathProperty rotation;
    PathProperty scale;
    PathProperty localRotation;
    PathProperty localPosition;
    PathProperty definitePosition;
    PathProperty dissolve;
    PathProperty dissolveArrow;
    PathProperty cuttable;
    PathProperty color;
};

struct Track {
    Properties properties;
    PathProperties pathProperties;
    std::vector<UnityEngine::GameObject*> gameObjects;
    // bool is true if removed
    EventCallback<Track *, UnityEngine::GameObject *, bool> gameObjectModificationEvent;

    void
    AddGameObject(UnityEngine::GameObject *go)
    {
        gameObjects.push_back(go);

        gameObjectModificationEvent.invoke(this, go, false);
    }

    void RemoveGameObject(UnityEngine::GameObject *go)
    {
        auto it = std::find(gameObjects.begin(), gameObjects.end(), go);

        if (it != gameObjects.end())
        {
            gameObjects.erase(it);
            gameObjectModificationEvent.invoke(this, go, true);
        }
    }

    void ResetVariables();
};
