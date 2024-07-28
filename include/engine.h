// Abstract City - Jed Fakhfekh - https://github.com/ougi-washi
#pragma once
#include "types.h"
#include "array.h"
#include "raylib.h"
#include <string>
#include <vector>

#include "json/json.h"
using json = nlohmann::json;

namespace ac{
    struct model{
        Model data = {};
        Transform transform = {}; // world transform
        std::string path = "";
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

    struct mapped_keyboard_input {
        KeyboardKey key;
        void(*callback)();
    };

    struct mapped_mouse_input {
        MouseButton key;
        void(*callback)();
    };

    struct input_handler {
        std::vector<mapped_keyboard_input> keyboard_inputs = {};
        std::vector<mapped_mouse_input> mouse_inputs = {};
    };

    struct engine {
        scene scenes[1] = {};
        std::vector<model> models_pool = {};
        input_handler input = {};
        // std::vector<material_loaded> materials_pool = {};
    };

    // engine
    void engine_start();
    b8 engine_should_loop();
    void engine_end();
    engine* engine_get_instance();
    std::vector<model>* engine_get_models_pool();
    void* engine_add_keyboard_input(const KeyboardKey key, void(*callback)());
    void* engine_add_mouse_input(const MouseButton key, void(*callback)());
    // scene
    void scene_load(scene* scene, const std::string &path);
    void scene_render(scene* scene);
    ac::scene* scene_get_active();
    void scene_add_model(scene* scene, ac::model* model);
    void scene_add_camera(scene* scene, Camera camera, const b8 is_active);
    camera* scene_make_new_camera(scene* scene);
    Camera* scene_get_active_camera(scene* scene);
    // model
    ac::model* model_load(const json &model_json);
    ac::model* model_load(const std::string& path);
    void model_render(model* model);
    void model_render_wireframe(model* model);
    void model_render_instances(model* model, Matrix* transforms, const i32 count);
    void model_set_location(model* model, const Vector3& location);
    void model_set_rotation(model* model, const Vector3& rotation);
    void model_set_scale(model* model, const Vector3& scale);
    void model_set_material(model* model, Material* material, const i32 mesh_index);
    // material
    void material_load(Material& material, const std::string& vertex, const std::string& fragment);
    i32 material_set_float(Material* material, const f32 value, const std::string& uniform_name);
    i32 material_set_int(Material* material, const i32 value, const std::string& uniform_name);
    i32 material_set_vector3f(Material* material, const Vector3& value, const std::string& uniform_name);
    i32 material_set_vector3i(Material* material, const Vector3& value, const std::string& uniform_name);
    i32 material_set_matrix(Material* material, const Matrix& value, const std::string& uniform_name);
    i32 material_set_texture(Material* material, const Texture2D& texture, const std::string& uniform_name);
    // camera
    void camera_set_position(camera* camera, const Vector3& position);
    void camera_set_target(camera* camera, const Vector3& target);
    void camera_set_fovy(camera* camera, const f32 fovy);
    void camera_set_active(camera* camera, const b8 new_state);
    void camera_set_position(Camera* camera, const Vector3& position);
    void camera_set_target(Camera* camera, const Vector3& target);
    void camera_set_fovy(Camera* camera, const f32 fovy);
    
    // transform
    void transform_set_location(Matrix& transform, const Vector3& location);
    void transform_set_rotation(Matrix& transform, const Vector3& rotation);
    void transform_set_scale(Matrix& transform, const Vector3& scale);
    //config
    b8 config_window_get_size(i32* width, i32* height);
    const std::string config_window_get_name();
    const i32 config_render_get_target_fps();
}
