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



// TODO: Make a recursive cleanup method
const Tracks::ffi::FFIJsonValue* convert_rapidjson(rapidjson::Value const& value) {
    // Handle different types of rapidjson values
    if (value.IsNumber()) {
        return new Tracks::ffi::FFIJsonValue{Tracks::ffi::JsonValueType::Number, {.number_value = value.GetDouble()}};
    } else if (value.IsNull()) {
        return new Tracks::ffi::FFIJsonValue{Tracks::ffi::JsonValueType::Null, {}};
    } else if (value.IsString()) {
        return new Tracks::ffi::FFIJsonValue{Tracks::ffi::JsonValueType::String, {.string_value = value.GetString()}};
    } else if (value.IsArray()) {
        // Create array of FFIJsonValue for each element in the array
        auto size = value.Size();
        auto *elements = new Tracks::ffi::FFIJsonValue[size];
        
        for (size_t i = 0; i < size; i++) {
            elements[i] = *convert_rapidjson(value[i]);
        }
        
        auto jsonArray = new Tracks::ffi::JsonArray{elements, size};
        return new Tracks::ffi::FFIJsonValue{Tracks::ffi::JsonValueType::Array, {.array = jsonArray}};
    } else {
        TLogger::Logger.error("Unsupported JSON value type in conversion");
        // Return null as fallback
        return new Tracks::ffi::FFIJsonValue{Tracks::ffi::JsonValueType::Null, {}};
    }
}

void PointDefinitionManager::AddPoint(std::string const& pointDataName, const rapidjson::Value& pointData) {
  if (this->pointData.contains(pointDataName)) {
    TLogger::Logger.error("Duplicate point definition name, {} could not be registered!", pointDataName.data());
  } else {
    auto x = &pointData;

    PointDefinitionW pointDef = PointDefinitionW(pointData, "idk"/* TODO: */, internal_tracks_context);
    this->pointData.try_emplace(pointDataName, &pointDef);
  }
}