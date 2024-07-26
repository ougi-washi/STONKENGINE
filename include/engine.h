// Abstract City - Jed Fakhfekh - https://github.com/ougi-washi
#pragma once
#include "types.h"
#include "array.h"
#include "raylib.h"
#include "json.h"

namespace ac{
    typedef struct model
    {
        Model* model;
        Transform transform; // world transform
    } model;
    AC_DECLARE_ARRAY(model, model_array)
    AC_DECLARE_ARRAY(Model, Model_array)

    typedef struct model_loaded
    {
        c8 path[256]; // TODO: define max path length in defines.h
        Model model;
    }model_loaded;
    AC_DECLARE_ARRAY(model_loaded, model_pool)

    typedef struct camera
    {
        Camera camera;
        b8 is_active;
    }camera;
    AC_DECLARE_ARRAY(camera, camera_array)

    typedef struct light
    {
        Transform transform;
        Color color;
        f32 intensity;
        f32 radius;
    }light;
    AC_DECLARE_ARRAY(light, light_array)

    typedef struct scene
    {
        model_array models;
        camera_array cameras;
    } scene;

    typedef struct engine
    {
        scene scene;
        model_pool model_pool;
        json_config config;
    } engine;

    // engine
    void engine_start();
    b8 engine_should_loop();
    void engine_end();
    engine* engine_get_instance();
    Model* engine_get_model(const c8* path);
    // scene
    void scene_init(scene* scene);
    void scene_render(scene* scene);
    void scene_add_model(scene* scene, Model* model);
    void scene_add_camera(scene* scene, Camera camera, const b8 is_active);
    camera* scene_make_new_camera(scene* scene);
    Camera* scene_get_active_camera(scene* scene);
    // model
    void model_render(model* model);
    void model_render_wireframe(model* model);
    void model_render_instances(model* model, Matrix* transforms, const i32 count);
    // camera
    void camera_set_position(camera* camera, const Vector3 position);
    void camera_set_target(camera* camera, const Vector3 target);
    void camera_set_fovy(camera* camera, const f32 fovy);
    void camera_set_active(camera* camera, const b8 new_state);
    //config
    void config_load();
    void config_unload();
    b8 config_window_get_size(i32* width, i32* height);
    c8* config_window_get_name();
}
