#include "Animation/GameObjectTrackController.hpp"

#include "Animation/Animation.h"

#include "sombrero/shared/Vector3Utils.hpp"
#include "sombrero/shared/QuaternionUtils.hpp"

DEFINE_TYPE(Tracks, GameObjectTrackController)

using namespace Tracks;

template<typename T, typename F>
static std::optional<std::vector<T>> getPropertiesNullable(std::span<Track const*> tracks, F&& propFn, uint32_t lastCheckedTime) {
    if (tracks.empty()) return std::nullopt;

    std::vector<T> props;

    for (auto t : tracks) {
        if (!t) continue;
        auto const& prop = propFn(t->properties);

        if (lastCheckedTime != 0 && prop.lastUpdated != 0 && prop.lastUpdated < lastCheckedTime) continue;

        auto val = Animation::getPropertyNullable<T>(t, prop.value);
        if (val)
            props.template emplace_back(*val);
    }

    if (props.empty()) return std::nullopt;

    return props;
}

template<typename T>
static constexpr std::optional<T> getPropertyNullable(Track const* track, const Property& prop, uint32_t lastCheckedTime) {
    if (lastCheckedTime != 0 && prop.lastUpdated != 0 && prop.lastUpdated < lastCheckedTime) return std::nullopt;

    auto ret = Animation::getPropertyNullable<T>(track, prop.value);

    if (GameObjectTrackController::LeftHanded) {
        if constexpr(std::is_same_v<T, NEVector::Vector3>) {
            return Animation::MirrorVectorNullable(ret);
        }

        if constexpr(std::is_same_v<T, NEVector::Quaternion>) {
            return Animation::MirrorQuaternionNullable(ret);
        }
    }

    return ret;
}

//static NEVector::Quaternion QuatInverse(const NEVector::Quaternion &a) {
//        NEVector::Quaternion conj = {-a.x, -a.y, -a.z, a.w};
//         float norm2 = NEVector::Quaternion::Dot(a, a);
//         return {conj.x / norm2, conj.y / norm2, conj.z / norm2,
//                           conj.w / norm2};
//}
GameObjectTrackControllerData &GameObjectTrackController::getTrackControllerData() {
    if (!data) {
        auto it = _dataMap.find(id);

        if (it != _dataMap.end()) {
            data = &it->second;
        }
    }

    return *data;
}

void GameObjectTrackController::ClearData() {
    _dataMap.clear();
    nextId = 0;
}

void GameObjectTrackController::Awake() {
//    OnTransformParentChanged();
}

void GameObjectTrackController::OnEnable() {
    OnTransformParentChanged();
}

void GameObjectTrackController::OnTransformParentChanged() {
    origin = get_transform();
    parent = origin->get_parent();
    UpdateData(true);
}

void GameObjectTrackController::Update() {
    UpdateData(false);
}

void GameObjectTrackController::UpdateData(bool force) {
    getTrackControllerData();

    if (!data){
        CJDLogger::Logger.fmtLog<Paper::LogLevel::ERR>("Data is null! Should remove component or just early return? {} {}", fmt::ptr(this), static_cast<std::string>(get_gameObject()->get_name()).c_str());
        Destroy(this);
        return;
    }
    const auto _noteLinesDistance = 0.6f; // StaticBeatmapObjectSpawnMovementData.kNoteLinesDistance
    const auto _track = data->_track;


    if (_track.empty()){
        CJDLogger::Logger.fmtLog<Paper::LogLevel::ERR>("Track is null! Should remove component or just early return? {} {}", fmt::ptr(this), static_cast<std::string>(get_gameObject()->get_name()).c_str());
        Destroy(this);
        return;
    }
    if (force) {
        lastCheckedTime = 0;
    }

    std::optional<NEVector::Quaternion> rotation;
    std::optional<NEVector::Quaternion> localRotation;
    std::optional<NEVector::Vector3> position;
    std::optional<NEVector::Vector3> localPosition;
    std::optional<NEVector::Vector3> scale;

    // I hate this
    auto tracks = std::span<Track const*>(const_cast<Track const **>(&*_track.begin()), _track.size());

    if (tracks.size() == 1) {
        auto track = tracks.front();
        auto properties = track->properties;

        rotation = getPropertyNullable<NEVector::Quaternion>(track, properties.rotation, lastCheckedTime);
        localRotation = getPropertyNullable<NEVector::Quaternion>(track, properties.localRotation, lastCheckedTime);
        position = getPropertyNullable<NEVector::Vector3>(track, properties.position, lastCheckedTime);
        localPosition = getPropertyNullable<NEVector::Vector3>(track, properties.localPosition, lastCheckedTime);
        scale = getPropertyNullable<NEVector::Vector3>(track, properties.scale, lastCheckedTime);

    } else {

#define combine(target, list, op) \
        if (list)                          \
        for (auto const& i : *list) {      \
            if (!target) target = i;       \
            else target = *target op i;\
        }

        auto localRotations = getPropertiesNullable<NEVector::Quaternion>(tracks, [](Properties const& p) {return p.localRotation;}, lastCheckedTime);
        auto rotations = getPropertiesNullable<NEVector::Quaternion>(tracks, [](Properties const& p) {return p.rotation;}, lastCheckedTime);
        auto positions = getPropertiesNullable<NEVector::Vector3>(tracks, [](Properties const& p) {return p.position;}, lastCheckedTime);
        auto localPositions = getPropertiesNullable<NEVector::Vector3>(tracks, [](Properties const& p) {return p.localPosition;}, lastCheckedTime);
        auto scales = getPropertiesNullable<NEVector::Vector3>(tracks, [](Properties const& p) {return p.scale;}, lastCheckedTime);

        combine(localRotation, localRotations, *);
        combine(rotation, rotations, *);
        combine(scale, scales, +);
        combine(position, positions, +);
        combine(localPosition, localPositions, +);
    }


    auto transform = origin;

    static auto Transform_Position = il2cpp_utils::il2cpp_type_check::FPtrWrapper<&UnityEngine::Transform::set_position>::get();
    static auto Transform_LocalPosition = il2cpp_utils::il2cpp_type_check::FPtrWrapper<&UnityEngine::Transform::set_localPosition>::get();
    static auto Transform_Rotation = il2cpp_utils::il2cpp_type_check::FPtrWrapper<&UnityEngine::Transform::set_rotation>::get();
    static auto Transform_LocalRotation = il2cpp_utils::il2cpp_type_check::FPtrWrapper<&UnityEngine::Transform::set_localRotation>::get();
    static auto Transform_Scale = il2cpp_utils::il2cpp_type_check::FPtrWrapper<&UnityEngine::Transform::set_localScale>::get();
    if (localRotation) {

        Transform_LocalRotation(transform, localRotation.value());
        data->RotationUpdate.invoke();

    }else if (rotation) {
        Transform_Rotation(transform, rotation.value());
        data->RotationUpdate.invoke();
    }

    if (localPosition)
    {
        if (data->v2) {
            *localPosition *= _noteLinesDistance;
        }
        Transform_LocalPosition(transform, *localPosition);
        data->PositionUpdate.invoke();
    } else if (position)
    {
        if (data->v2) {
            *position *= _noteLinesDistance;
        }

        Transform_Position(transform, *position);
        data->PositionUpdate.invoke();
    }

    if (scale)
    {
        Transform_Scale(transform, scale.value());
        data->ScaleUpdate.invoke();
    }

    lastCheckedTime = getCurrentTime();
}

void GameObjectTrackController::Init(std::vector<Track*> const& track, float noteLinesDistance, bool v2) {
    CRASH_UNLESS(!track.empty());
    this->data = &_dataMap.try_emplace(nextId, track, noteLinesDistance, v2).first->second;
    nextId++;
}

std::optional<GameObjectTrackController*> GameObjectTrackController::HandleTrackData(UnityEngine::GameObject *gameObject,
                                                        const std::vector<Track*>& track,
                                                        float noteLinesDistance, bool v2) {
    auto* existingTrackController = gameObject->GetComponent<GameObjectTrackController*>();
    if (existingTrackController)
    {
        Destroy(existingTrackController);
    }

    if (!track.empty())
    {
        auto* trackController = gameObject->AddComponent<GameObjectTrackController*>();
        trackController->Init(track, noteLinesDistance, v2);

        for (auto t : track)
            t->AddGameObject(gameObject);

        return trackController;
    }

    return std::nullopt;
}
