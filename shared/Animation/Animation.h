#pragma once
#include "PointDefinition.h"
#include "../AssociatedData.h"

namespace GlobalNamespace {
class BeatmapData;
}

namespace Animation {

    PointDefinition *TryGetPointData(TracksAD::BeatmapAssociatedData &beatmapAD, PointDefinition *&anon,
                                     const rapidjson::Value &customData, std::string_view pointName);

#pragma region track_utils

    static constexpr std::optional<NEVector::Vector3>
    MirrorVectorNullable(std::optional<NEVector::Vector3> const &vector) {
        if (!vector) {
            return vector;
        }


        auto modifiedVector = *vector;
        modifiedVector.x *= -1;

        return modifiedVector;
    }

    constexpr static std::optional<NEVector::Quaternion> MirrorQuaternionNullable(std::optional<NEVector::Quaternion> const &quaternion) {
        if (!quaternion) {
            return quaternion;
        }

        auto modifiedVector = *quaternion;

        return NEVector::Quaternion(modifiedVector.x, modifiedVector.y * -1, modifiedVector.z * -1, modifiedVector.w);
    }

// Ok Stack, why the hell does prop param nullptr in the method if track is null, but then if you null-check track in the method it just works:tm:
// C++ compiler tomfoolery that's above my pay grade, that's what this is
// my most educated guess is compiler inlining method magic
    template<typename T>
    static constexpr std::optional<T> getPropertyNullable(Track *track, const std::optional<PropertyValue> &prop) {
        static_assert(std::is_same_v<T, float> ||
                      std::is_same_v<T, NEVector::Vector3> ||
                      std::is_same_v<T, NEVector::Vector4> ||
                      std::is_same_v<T, NEVector::Quaternion>, "Not valid type");

        if (!track) return std::nullopt;
        if (!prop) return std::nullopt;

        if constexpr(std::is_same_v<T, float>) {
            return prop.value().linear;
        } else if constexpr(std::is_same_v<T, NEVector::Vector3>) {
            return prop.value().vector3;
        } else if constexpr(std::is_same_v<T, NEVector::Vector4>) {
            return prop.value().vector4;
        } else if constexpr(std::is_same_v<T, NEVector::Quaternion>) {
            return prop.value().quaternion;
        }

        return std::nullopt;
    }

// why not?
    template<typename T>
    static constexpr std::optional<T> getPropertyNullable(Track *track, const Property &prop) {
        return getPropertyNullable<T>(track, prop.value);
    }

    template<typename T>
    static constexpr std::optional<T>
    getPathPropertyNullable(Track *track, std::optional<PointDefinitionInterpolation> &prop, float time) {
        static_assert(std::is_same_v<T, float> ||
                      std::is_same_v<T, NEVector::Vector3> ||
                      std::is_same_v<T, NEVector::Vector4> ||
                      std::is_same_v<T, NEVector::Quaternion>, "Not valid type");

        if (!track) return std::nullopt;
        if (!prop) return std::nullopt;

        if constexpr(std::is_same_v<T, float>) {
            return prop.value().InterpolateLinear(time);
        } else if constexpr(std::is_same_v<T, NEVector::Vector3>) {
            return prop.value().Interpolate(time);
        } else if constexpr(std::is_same_v<T, NEVector::Vector4>) {
            return prop.value().InterpolateVector4(time);
        } else if constexpr(std::is_same_v<T, NEVector::Quaternion>) {
            return prop.value().InterpolateQuaternion(time);
        }

        return std::nullopt;
    }

    template<typename T, typename VectorExpression = std::function<std::optional<PointDefinitionInterpolation>(
            Track *)>>
    static std::optional<T> MultiTrackPathProps(std::vector<Track *> const &tracks, T const &defaultT, float time,
                                                VectorExpression const &vectorExpression) {
        if (tracks.empty())
            return std::nullopt;

        bool valid = false;
        T total = defaultT;

        for (auto &track: tracks) {
            auto point = vectorExpression(track);
            auto result = getPathPropertyNullable<T>(track, point, time);

            if (result) {
                total = result.value() * total;
                valid = true;
            }
        }

        return valid ? std::make_optional(total) : std::nullopt;
    }

    template<typename T, typename VectorExpression = std::function<std::optional<PointDefinitionInterpolation>(
            Track *)>>
    static std::optional<T> SumTrackPathProps(std::vector<Track *> const &tracks, T const &defaultT, float time,
                                              VectorExpression const &vectorExpression) {
        if (tracks.empty())
            return std::nullopt;

        bool valid = false;
        T total = defaultT;

        for (auto &track: tracks) {
            auto point = vectorExpression(track);
            auto result = getPathPropertyNullable<T>(track, point, time);

            if (result) {
                total = result.value() + total;
                valid = true;
            }
        }

        return valid ? std::make_optional(total) : std::nullopt;
    }

    template<typename T, typename VectorExpression = std::function<std::optional<PropertyValue>(Track *)>>
    static std::optional<T>
    MultiTrackProps(std::vector<Track *> const &tracks, T const &defaultT, VectorExpression const &vectorExpression) {

        if (tracks.empty())
            return std::nullopt;

        bool valid = false;
        T total = defaultT;

        for (auto &track: tracks) {
            auto point = vectorExpression(track);
            auto result = getPropertyNullable<T>(track, point);

            if (result) {
                total = result.value() * total;
                valid = true;
            }
        }

        return valid ? std::make_optional(total) : std::nullopt;
    }

    template<typename T, typename VectorExpression = std::function<std::optional<PropertyValue>(Track *)>>
    static std::optional<T>
    SumTrackProps(std::vector<Track *> const &tracks, T const &defaultT, VectorExpression const &vectorExpression) {
        if (tracks.empty())
            return std::nullopt;

        bool valid = false;
        T total = defaultT;

        for (auto &track: tracks) {
            auto point = vectorExpression(track);
            auto result = getPropertyNullable<T>(track, point);

            if (result) {
                total = result.value() + total;
                valid = true;
            }
        }

        return valid ? std::make_optional(total) : std::nullopt;
    }

#define MSumTrackProps(tracks, defaultT, prop) SumTrackProps(tracks, defaultT, [](Track* track) { return track->properties.prop.value;})
#define MMultTrackProps(tracks, defaultT, prop) MultiTrackProps(tracks, defaultT, [](Track* track) { return track->properties.prop.value; })

#define MSumTrackPathProps(tracks, defaultT, prop, time) SumTrackPathProps(tracks, defaultT, time, [](Track* track) { return track->pathProperties.prop.value; })
#define MMultTrackPathProps(tracks, defaultT, prop, time) MultiTrackPathProps(tracks, defaultT, time, [](Track* track) { return track->pathProperties.prop.value; })

#pragma endregion


}