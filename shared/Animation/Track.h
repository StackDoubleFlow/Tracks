#pragma once
#include <map>
#include <string>
#include "PointDefinitionInterpolation.h"
#include "../Vector.h"
#include "../sv/small_vector.h"
#include "UnityEngine/GameObject.hpp"

#include "beatsaber-hook/shared/utils/typedefs-wrappers.hpp"

#include <chrono>

inline auto getCurrentTime() {
  return std::chrono::high_resolution_clock::now().time_since_epoch().count();
}

namespace Events {
struct AnimateTrackContext;
}

enum struct PropertyType { vector3, vector4, quaternion, linear };

union PropertyValue {
  float linear;
  NEVector::Vector3 vector3;
  NEVector::Vector4 vector4;
  NEVector::Quaternion quaternion;
};

struct Property {
  Property(PropertyType t) : type{ t }, value{ std::nullopt } {};
  PropertyType type;
  std::optional<PropertyValue> value;
  uint64_t lastUpdated; // set to 0 to force update
};

struct PathProperty {
  PathProperty(PropertyType t) : type{ t }, value{ std::nullopt } {};
  PropertyType type;
  std::optional<PointDefinitionInterpolation> value;
};

using PropertiesMap = std::unordered_map<std::string_view, Property>;
using PathPropertiesMap = std::unordered_map<std::string_view, PathProperty>;

class Properties {
public:
  Properties(bool v2)
      : v2(v2), position{ Property(PropertyType::vector3) }, rotation{ Property(PropertyType::quaternion) },
        scale{ Property(PropertyType::vector3) }, localRotation{ Property(PropertyType::quaternion) },
        localPosition{ Property(PropertyType::vector3) }, dissolve{ Property(PropertyType::linear) },
        dissolveArrow{ Property(PropertyType::linear) }, time{ Property(PropertyType::linear) },
        cuttable{ Property(PropertyType::linear) }, color{ Property(PropertyType::vector4) } {};
  Property* FindProperty(std::string_view name);

  bool v2;

  // Noodle
  Property position;
  Property rotation;
  Property scale;
  Property localRotation;
  Property localPosition;
  Property dissolve;
  Property dissolveArrow;
  Property time;
  Property cuttable;

  // Chroma
  Property color;
  Property attentuation{ PropertyType::linear };
  Property fogOffset{ PropertyType::linear };
  Property heightFogStartY{ PropertyType::linear };
  Property heightFogHeight{ PropertyType::linear };
  PropertiesMap extraProperties;
};

class PathProperties {
public:
  PathProperties(bool v2)
      : v2(v2), position{ PropertyType::vector3 }, rotation{ PropertyType::quaternion }, scale{ PropertyType::vector3 },
        localRotation{ PropertyType::quaternion }, localPosition{ PropertyType::vector3 },
        definitePosition{ PropertyType::vector3 }, dissolve{ PropertyType::linear },
        dissolveArrow{ PropertyType::linear }, cuttable{ PropertyType::linear }, color{ PropertyType::vector4 } {};
  bool v2;
  PathProperty* FindProperty(std::string_view name);

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
  PathPropertiesMap extraProperties;
};
namespace TrackRegister {
void BuildPropertyCallback(std::optional<std::function<PropertiesMap(bool v2)>> const& propertyBuilder,
                           std::optional<std::function<PathPropertiesMap(bool v2)>> const& pathPropertyBuilder);

void BuildProperties(Properties& properties, bool v2);

void BuildPathProperties(PathProperties& properties, bool v2);
} // namespace TrackRegister

struct Track {
  bool const v2;
  const std::string_view name;

  Track(bool v2, std::string_view name) : v2(v2), properties(v2), pathProperties(v2), name(name) {
    TrackRegister::BuildProperties(properties, v2);
    TrackRegister::BuildPathProperties(pathProperties, v2);

    gameObjects = {};
    gameObjectModificationEvent = {};
  }

  Properties properties;
  PathProperties pathProperties;
  sbo::small_vector<UnityEngine::GameObject*> gameObjects;
  // bool is true if removed
  UnorderedEventCallback<Track*, UnityEngine::GameObject*, bool> gameObjectModificationEvent;

  void AddGameObject(UnityEngine::GameObject* go) {
    gameObjects.push_back(go);

    gameObjectModificationEvent.invoke(this, go, false);
  }

  void RemoveGameObject(UnityEngine::GameObject* go) {
    auto it = std::find(gameObjects.begin(), gameObjects.end(), go);

    if (it != gameObjects.end()) {
      gameObjects.erase(it);
      gameObjectModificationEvent.invoke(this, go, true);
    }
  }

  void ResetVariables();
};
