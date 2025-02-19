#include "Animation/PointDefinition.h"

#include <utility>
#include <numeric>
#include "Animation/Track.h"
#include "Animation/Easings.h"
#include "TLogger.h"
#include "custom-json-data/shared/CJDLogger.h"
#include "sombrero/shared/HSBColor.hpp"
#include "tracks-rs/shared/bindings.h"

using namespace NEVector;

Tracks::BaseProviderContext* internal_tracks_context = Tracks::tracks_make_base_provider_context();

void PointDefinitionManager::AddPoint(std::string const& pointDataName, const rapidjson::Value& pointData) {
  if (this->pointData.contains(pointDataName)) {
    TLogger::Logger.error("Duplicate point definition name, {} could not be registered!", pointDataName.data());
  } else {
    auto x = &pointData;
    this->pointData.try_emplace(pointDataName, &pointData);
  }
}