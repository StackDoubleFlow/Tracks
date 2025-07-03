#pragma once

#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
namespace Tracks {
namespace ffi {
#endif  // __cplusplus

enum CEventTypeEnum
#ifdef __cplusplus
  : uint32_t
#endif // __cplusplus
 {
  AnimateTrack = 0,
  AssignPathAnimation = 1,
};
#ifndef __cplusplus
typedef uint32_t CEventTypeEnum;
#endif // __cplusplus

typedef enum Functions {
  EaseLinear,
  EaseStep,
  EaseInQuad,
  EaseOutQuad,
  EaseInOutQuad,
  EaseInCubic,
  EaseOutCubic,
  EaseInOutCubic,
  EaseInQuart,
  EaseOutQuart,
  EaseInOutQuart,
  EaseInQuint,
  EaseOutQuint,
  EaseInOutQuint,
  EaseInSine,
  EaseOutSine,
  EaseInOutSine,
  EaseInCirc,
  EaseOutCirc,
  EaseInOutCirc,
  EaseInExpo,
  EaseOutExpo,
  EaseInOutExpo,
  EaseInElastic,
  EaseOutElastic,
  EaseInOutElastic,
  EaseInBack,
  EaseOutBack,
  EaseInOutBack,
  EaseInBounce,
  EaseOutBounce,
  EaseInOutBounce,
} Functions;

/**
 * JSON FFI
 */
typedef enum JsonValueType {
  Number,
  Null,
  String,
  Array,
} JsonValueType;

enum PropertyNames
#ifdef __cplusplus
  : uint32_t
#endif // __cplusplus
 {
  Position,
  Rotation,
  Scale,
  LocalRotation,
  LocalPosition,
  DefinitePosition,
  Dissolve,
  DissolveArrow,
  Time,
  Cuttable,
  Color,
  Attentuation,
  FogOffset,
  HeightFogStartY,
  HeightFogHeight,
};
#ifndef __cplusplus
typedef uint32_t PropertyNames;
#endif // __cplusplus

typedef enum WrapBaseValueType {
  Vec3 = 0,
  Quat = 1,
  Vec4 = 2,
  Float = 3,
} WrapBaseValueType;

typedef struct BaseFFIProviderValues BaseFFIProviderValues;

typedef struct BasePointDefinition BasePointDefinition;

typedef struct BaseProviderContext BaseProviderContext;

typedef struct CoroutineManager CoroutineManager;

typedef struct EventData EventData;

typedef struct FloatPointDefinition FloatPointDefinition;

typedef struct PointDefinitionInterpolation PointDefinitionInterpolation;

typedef struct QuaternionPointDefinition QuaternionPointDefinition;

typedef struct Track Track;

typedef struct TracksContext TracksContext;

typedef struct ValueProperty ValueProperty;

typedef struct Vector3PointDefinition Vector3PointDefinition;

typedef struct Vector4PointDefinition Vector4PointDefinition;

typedef struct PointDefinitionInterpolation PathProperty;

typedef union CEventTypeData {
  /**
   * AnimateTrack(ValueProperty)
   */
  const struct ValueProperty *property;
  /**
   * AssignPathAnimation(PathProperty)
   */
  const PathProperty *path_property;
} CEventTypeData;

typedef struct CEventType {
  CEventTypeEnum ty;
  union CEventTypeData data;
} CEventType;

typedef struct CEventData {
  float raw_duration;
  enum Functions easing;
  uint32_t repeat;
  float start_time;
  struct CEventType event_type;
  struct Track *track_ptr;
  const struct BasePointDefinition *point_data_ptr;
} CEventData;

typedef struct JsonArray {
  const struct FFIJsonValue *elements;
  uintptr_t length;
} JsonArray;

typedef union JsonValueData {
  double number_value;
  const char *string_value;
  const struct JsonArray *array;
} JsonValueData;

typedef struct FFIJsonValue {
  enum JsonValueType value_type;
  union JsonValueData data;
} FFIJsonValue;

typedef struct WrappedValues {
  const float *values;
  uintptr_t length;
} WrappedValues;

typedef struct WrappedValues (*BaseFFIProvider)(const struct BaseProviderContext*, void*);

typedef struct FloatInterpolationResult {
  float value;
  bool is_last;
} FloatInterpolationResult;

typedef struct WrapVec3 {
  float x;
  float y;
  float z;
} WrapVec3;

typedef struct WrapQuat {
  float x;
  float y;
  float z;
  float w;
} WrapQuat;

typedef struct WrapVec4 {
  float x;
  float y;
  float z;
  float w;
} WrapVec4;

typedef union WrapBaseValueUnion {
  struct WrapVec3 vec3;
  struct WrapQuat quat;
  struct WrapVec4 vec4;
  float float_v;
} WrapBaseValueUnion;

typedef struct WrapBaseValue {
  enum WrapBaseValueType ty;
  union WrapBaseValueUnion value;
} WrapBaseValue;

typedef struct Vector3InterpolationResult {
  struct WrapVec3 value;
  bool is_last;
} Vector3InterpolationResult;

typedef struct Vector4InterpolationResult {
  struct WrapVec4 value;
  bool is_last;
} Vector4InterpolationResult;

typedef struct QuaternionInterpolationResult {
  struct WrapQuat value;
  bool is_last;
} QuaternionInterpolationResult;

typedef struct CValueNullable {
  bool has_value;
  struct WrapBaseValue value;
} CValueNullable;

typedef struct CTimeUnit {
  uint64_t _0;
  uint32_t _1;
} CTimeUnit;

typedef struct CValueProperty {
  struct CValueNullable value;
  struct CTimeUnit last_updated;
} CValueProperty;

typedef struct GameObject {
  const void *ptr;
} GameObject;

typedef struct CPropertiesMap {
  const struct ValueProperty *position;
  const struct ValueProperty *rotation;
  const struct ValueProperty *scale;
  const struct ValueProperty *local_rotation;
  const struct ValueProperty *local_position;
  const struct ValueProperty *dissolve;
  const struct ValueProperty *dissolve_arrow;
  const struct ValueProperty *time;
  const struct ValueProperty *cuttable;
  const struct ValueProperty *color;
  const struct ValueProperty *attentuation;
  const struct ValueProperty *fog_offset;
  const struct ValueProperty *height_fog_start_y;
  const struct ValueProperty *height_fog_height;
} CPropertiesMap;

typedef struct CPathPropertiesMap {
  PathProperty *position;
  PathProperty *rotation;
  PathProperty *scale;
  PathProperty *local_rotation;
  PathProperty *local_position;
  PathProperty *definite_position;
  PathProperty *dissolve;
  PathProperty *dissolve_arrow;
  PathProperty *cuttable;
  PathProperty *color;
} CPathPropertiesMap;

typedef void (*CGameObjectCallback)(struct GameObject, bool, void*);



#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

struct TracksContext *tracks_context_create(void);

/**
 * Consumes the context and frees its memory.
 */
void tracks_context_destroy(struct TracksContext *context);

/**
 * Consumes the track and moves
 * it into the context. Returns a const pointer to the track.
 */
const struct Track *tracks_context_add_track(struct TracksContext *context, struct Track *track);

/**
 * Consumes the point definition and moves it into the context.
 * Returns a const pointer to the point definition.
 *
 * If id is null/empty, generates a uuid for the point definition.
 */
const struct BasePointDefinition *tracks_context_add_point_definition(struct TracksContext *context,
                                                                      const char *id,
                                                                      struct BasePointDefinition *point_def);

const struct BasePointDefinition *tracks_context_get_point_definition(struct TracksContext *context,
                                                                      const char *name,
                                                                      enum WrapBaseValueType ty);

struct Track *tracks_context_get_track_by_name(struct TracksContext *context, const char *name);

struct Track *tracks_context_get_track(struct TracksContext *context, uintptr_t index);

struct CoroutineManager *tracks_context_get_coroutine_manager(struct TracksContext *context);

struct BaseProviderContext *tracks_context_get_base_provider_context(struct TracksContext *context);

/**
 * Creates a new CoroutineManager instance and returns a raw pointer to it.
 * The caller is responsible for freeing the memory using destroy_coroutine_manager.
 */
struct CoroutineManager *create_coroutine_manager(void);

/**
 * Destroys a CoroutineManager instance, freeing its memory.
 */
void destroy_coroutine_manager(struct CoroutineManager *manager);

/**
 * Starts an event coroutine in the manager. Consumes event_data
 */
void start_event_coroutine(struct CoroutineManager *manager,
                           float bpm,
                           float song_time,
                           const struct BaseProviderContext *context,
                           struct EventData *event_data);

/**
 * Polls all events in the manager, updating their state based on the current song time.
 */
void poll_events(struct CoroutineManager *manager,
                 float song_time,
                 const struct BaseProviderContext *context);

/**
 * C-compatible wrapper for easing functions
 */
float interpolate_easing(enum Functions easing_function, float t);

/**
 * Gets an easing function by index (useful for FFI where enums might be troublesome)
 * Returns Functions::EaseLinear if the index is out of bounds
 */
enum Functions get_easing_function_by_index(int32_t index);

/**
 * Gets the total number of available easing functions
 */
int32_t get_easing_function_count(void);

/**
 * Converts a CEventData into a Rust EventData
 * Does not consume the CEventData
 * Returns a raw pointer to the Rust EventData
 */
struct EventData *event_data_to_rust(const struct CEventData *c_event_data);

void event_data_dispose(struct EventData *event_data);

struct FFIJsonValue tracks_create_json_number(double value);

struct FFIJsonValue tracks_create_json_string(const char *value);

struct FFIJsonValue tracks_create_json_array(const struct FFIJsonValue *elements, uintptr_t length);

void tracks_free_json_value(struct FFIJsonValue *json_value);

struct BaseFFIProviderValues *tracks_make_base_ffi_provider(const BaseFFIProvider *func,
                                                            void *user_value);

/**
 * Dispose the base provider. Consumes
 */
void tracks_dipose_base_ffi_provider(struct BaseFFIProviderValues *func);

/**
 * CONTEXT
 */
struct BaseProviderContext *tracks_make_base_provider_context(void);

void tracks_set_base_provider(struct BaseProviderContext *context,
                              const char *base,
                              float *values,
                              uintptr_t count,
                              bool quat);

/**
 *FLOAT POINT DEFINITION
 */
const struct FloatPointDefinition *tracks_make_float_point_definition(const struct FFIJsonValue *json,
                                                                      struct BaseProviderContext *context);

struct FloatInterpolationResult tracks_interpolate_float(const struct FloatPointDefinition *point_definition,
                                                         float time,
                                                         struct BaseProviderContext *context);

uintptr_t tracks_float_count(const struct FloatPointDefinition *point_definition);

bool tracks_float_has_base_provider(const struct FloatPointDefinition *point_definition);

/**
 *BASE POINT DEFINITION
 */
struct BasePointDefinition *tracks_make_base_point_definition(const struct FFIJsonValue *json,
                                                              enum WrapBaseValueType ty,
                                                              struct BaseProviderContext *context);

struct WrapBaseValue tracks_interpolate_base_point_definition(const struct BasePointDefinition *point_definition,
                                                              float time,
                                                              bool *is_last_out,
                                                              struct BaseProviderContext *context);

uintptr_t tracks_base_point_definition_count(const struct BasePointDefinition *point_definition);

bool tracks_base_point_definition_has_base_provider(const struct BasePointDefinition *point_definition);

/**
 *VECTOR3 POINT DEFINITION
 */
const struct Vector3PointDefinition *tracks_make_vector3_point_definition(const struct FFIJsonValue *json,
                                                                          struct BaseProviderContext *context);

struct Vector3InterpolationResult tracks_interpolate_vector3(const struct Vector3PointDefinition *point_definition,
                                                             float time,
                                                             struct BaseProviderContext *context);

uintptr_t tracks_vector3_count(const struct Vector3PointDefinition *point_definition);

bool tracks_vector3_has_base_provider(const struct Vector3PointDefinition *point_definition);

/**
 *VECTOR4 POINT DEFINITION
 */
const struct Vector4PointDefinition *tracks_make_vector4_point_definition(const struct FFIJsonValue *json,
                                                                          struct BaseProviderContext *context);

struct Vector4InterpolationResult tracks_interpolate_vector4(const struct Vector4PointDefinition *point_definition,
                                                             float time,
                                                             struct BaseProviderContext *context);

uintptr_t tracks_vector4_count(const struct Vector4PointDefinition *point_definition);

bool tracks_vector4_has_base_provider(const struct Vector4PointDefinition *point_definition);

/**
 *QUATERNION POINT DEFINITION
 */
const struct QuaternionPointDefinition *tracks_make_quat_point_definition(const struct FFIJsonValue *json,
                                                                          struct BaseProviderContext *context);

struct QuaternionInterpolationResult tracks_interpolate_quat(const struct QuaternionPointDefinition *point_definition,
                                                             float time,
                                                             struct BaseProviderContext *context);

uintptr_t tracks_quat_count(const struct QuaternionPointDefinition *point_definition);

bool tracks_quat_has_base_provider(const struct QuaternionPointDefinition *point_definition);

PathProperty *path_property_create(void);

void path_property_finish(PathProperty *ptr);

void path_property_init(PathProperty *ptr, const struct BasePointDefinition *new_point_data);

/**
 * Consumes the path property and frees its memory.
 */
void path_property_free(PathProperty *ptr);

float path_property_get_time(const PathProperty *ptr);

void path_property_set_time(PathProperty *ptr, float time);

struct CValueNullable path_property_interpolate(PathProperty *ptr,
                                                float time,
                                                struct BaseProviderContext *context);

enum WrapBaseValueType path_property_get_type(const PathProperty *ptr);

enum WrapBaseValueType property_get_type(const struct ValueProperty *ptr);

struct CValueProperty property_get_value(const struct ValueProperty *ptr);

struct CTimeUnit property_get_last_updated(const struct ValueProperty *ptr);

struct Track *track_create(void);

/**
 * Consumes the track and frees its memory.
 */
void track_destroy(struct Track *track);

void track_set_name(struct Track *track, const char *name);

const char *track_get_name(const struct Track *track);

void track_register_game_object(struct Track *track, struct GameObject game_object);

void track_unregister_game_object(struct Track *track, struct GameObject game_object);

const struct GameObject *track_get_game_objects(const struct Track *track, uintptr_t *size);

void track_register_property(struct Track *track, const char *id, struct ValueProperty *property);

const struct ValueProperty *track_get_property(const struct Track *track, const char *id);

const struct ValueProperty *track_get_property_by_name(const struct Track *track, PropertyNames id);

PathProperty *track_get_path_property_by_name(struct Track *track, PropertyNames id);

void track_register_path_property(struct Track *track, const char *id, PathProperty *property);

PathProperty *track_get_path_property(struct Track *track, const char *id);

struct CPropertiesMap track_get_properties_map(const struct Track *track);

struct CPathPropertiesMap track_get_path_properties_map(struct Track *track);

void (**track_register_game_object_callback(struct Track *track,
                                            CGameObjectCallback callback,
                                            void *user_data))(struct GameObject, bool);

void track_remove_game_object_callback(struct Track *track, void (**callback)(struct GameObject,
                                                                              bool));

struct CTimeUnit get_time(void);

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus

#ifdef __cplusplus
}  // namespace ffi
}  // namespace Tracks
#endif  // __cplusplus
