// Abstract City - Jed Fakhfekh - https://github.com/ougi-washi
#pragma once
#include "types.h"
#include "array.h"
#include "raylib.h"
#include <string>
#include <vector>

namespace ac{
    typedef struct model
    {
        Model* model;
        Transform transform; // world transform
    } model;

    typedef struct model_loaded
    {
        char path[256]; // TODO: define max path length in defines.h
        Model model;
    }model_loaded;

    typedef struct camera
    {
        Camera camera;
        b8 is_active;
    }camera;

    typedef struct light
    {
        Transform transform;
        Color color;
        f32 intensity;
        f32 radius;
    }light;

    typedef struct scene
    {
        std::vector<model> models = {};
        std::vector<light> lights = {};
        std::vector<camera> cameras = {};
    } scene;

    typedef struct engine
    {
        scene scene;
        std::vector<model_loaded> models_pool = {};
    } engine;

    // engine
    void engine_start();
    b8 engine_should_loop();
    void engine_end();
    engine* engine_get_instance();
    Model* engine_get_model(const char* path);
    // scene
    void scene_init(scene* scene);
    void scene_render(scene* scene);
    ac::scene* scene_get_active();
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
    b8 config_window_get_size(i32* width, i32* height);
    const std::string config_window_get_name();
    const i32 config_render_get_target_fps();


}
