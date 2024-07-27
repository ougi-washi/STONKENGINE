// Abstract City - Jed Fakhfekh - https://github.com/ougi-washi
#pragma once
#include "types.h"
#include "array.h"
#include "raylib.h"
#include <string>
#include <vector>

namespace ac{
    struct model{
        Model* model;
        Transform transform; // world transform
    };

    
    struct camera {
        Camera camera;
        b8 is_active;
    };

    struct light {
        Transform transform;
        Color color;
        f32 intensity;
        f32 radius;
    };

    struct scene {
        std::vector<model> models = {};
        std::vector<light> lights = {};
        std::vector<camera> cameras = {};
    };

    struct model_loaded {
        std::string path = "";
        Model model;
    };

    struct material_loaded {
        std::string vertex = "";
        std::string fragment = "";
        Material material;
    };

    struct engine {
        scene scenes[1] = {};
        std::vector<model_loaded> models_pool = {};
        std::vector<material_loaded> materials_pool = {};
    };

    // engine
    void engine_start();
    b8 engine_should_loop();
    void engine_end();
    engine* engine_get_instance();
    // scene
    void scene_init(scene* scene);
    void scene_render(scene* scene);
    ac::scene* scene_get_active();
    void scene_add_model(scene* scene, Model* model);
    void scene_add_camera(scene* scene, Camera camera, const b8 is_active);
    camera* scene_make_new_camera(scene* scene);
    Camera* scene_get_active_camera(scene* scene);
    // model
    Model* model_load(const std::string& path, const b8 root_path = false);
    void model_render(model* model);
    void model_render_wireframe(model* model);
    void model_render_instances(model* model, Matrix* transforms, const i32 count);
    // material
    Material* material_load(const std::string& vertex, const std::string& fragment);
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
