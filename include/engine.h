// Abstract City - Jed Fakhfekh - https://github.com/ougi-washi
#pragma once
#include "types.h"
#include "array.h"
#include "raylib.h"
#include "json.h"

typedef struct ac_model
{
    Model* model;
    Transform transform; // world transform
} ac_model;
AC_DECLARE_ARRAY(ac_model, ac_model_array)
AC_DECLARE_ARRAY(Model, Model_array)

typedef struct ac_model_loaded
{
    c8 path[256]; // TODO: define max path length in defines.h
    Model model;
}ac_model_loaded;
AC_DECLARE_ARRAY(ac_model_loaded, ac_model_pool)

typedef struct ac_camera
{
    Camera camera;
    b8 is_active;
}ac_camera;
AC_DECLARE_ARRAY(ac_camera, ac_camera_array)

typedef struct ac_light
{
    Transform transform;
    Color color;
    f32 intensity;
    f32 radius;
}ac_light;
AC_DECLARE_ARRAY(ac_light, ac_light_array)

typedef struct ac_scene
{
    ac_model_array models;
    ac_camera_array cameras;
} ac_scene;

typedef struct ac_engine
{
    ac_scene scene;
    ac_model_pool model_pool;
    ac_json_config config;
} ac_engine;

extern ac_engine* engine;

// engine
void ac_engine_start();
b8 ac_engine_should_loop();
void ac_engine_end();
Model* ac_engine_get_model(const c8* path);
// scene
void ac_scene_init(ac_scene* scene);
void ac_scene_render(ac_scene* scene);
void ac_scene_add_model(ac_scene* scene, Model* model);
void ac_scene_add_camera(ac_scene* scene, Camera camera, const b8 is_active);
ac_camera* ac_scene_make_new_camera(ac_scene* scene);
Camera* ac_scene_get_active_camera(ac_scene* scene);
// model
void ac_model_render(ac_model* model);
void ac_model_render_wireframe(ac_model* model);
void ac_model_render_instances(ac_model* model, Matrix* transforms, const i32 count);
// camera
void ac_camera_set_position(ac_camera* camera, const Vector3 position);
void ac_camera_set_target(ac_camera* camera, const Vector3 target);
void ac_camera_set_fovy(ac_camera* camera, const f32 fovy);
void ac_camera_set_active(ac_camera* camera, const b8 new_state);
//config
void ac_config_load();
void ac_config_unload();
b8 ac_config_window_get_size(i32* width, i32* height);
c8* ac_config_window_get_name();
