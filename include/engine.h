// Abstract City - Jed Fakhfekh - https://github.com/ougi-washi
#pragma once
#include "types.h"
#include "array.h"
#include "raylib.h"
#include <string>
#include <vector>
#include <queue>
#include <functional>

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
        f32 speed;
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

    struct text {
        Font font;
        std::string string;
        Vector2 position;
        float fontSize;
        float spacing;
        Color tint;
    };

    struct object_2d {
        std::string fragment = "";
        std::string vertex = "";
        Shader shader;
        std::vector<Texture2D> textures = {};
        std::vector<text> texts = {};
    };

    struct scene_2d {
        std::vector<object_2d> objects = {}; // order matters, these objects will be rendered in order the moment they are added
        std::vector<RenderTexture2D> render_textures = {};  // Cached render textures as output from the objects, they will be rendered every frame
    };

    struct input_map {
        std::vector<KeyboardKey> keyboard_keys = {}; // for now, default is pressed
        std::vector<MouseButton> mouse_buttons = {}; // for now, default is pressed
        void(*callback)();
    };

    struct input_handler {
        std::vector<input_map> input_maps = {};
    };

    struct selection{
        std::vector<i32> selected_models = {};
        std::vector<i32> selected_cameras = {}; // TODO: Maybe it will be removed?
        std::vector<i32> selected_lights = {};
    };

    struct editor{
        b8 is_active = false;
        selection selection = {};
    };

    typedef std::function<void(void)> command; 
    typedef std::queue<command> command_queue;

    struct engine {
        std::vector<scene> scenes = {};
        std::vector<scene_2d> scenes_2d = {};
        std::vector<model> models_pool = {};
        input_handler input = {};
        command_queue render_queue = {};
    };
    
    // engine
    void engine_start();
    const b8 engine_should_loop();
    void engine_end();
    engine* engine_get_instance();
    void engine_render();
    std::vector<model>* engine_get_models_pool();
    void engine_process_input();
    const f32 engine_get_delta_time();
    command_queue* engine_get_render_queue();
    void engine_enqueue_render_command(const ac::command& render_command);
    // input
    void input_add_map(const input_map& input_map);
    void input_process();
    // scene
    ac::scene* scene_make_new();
    void scene_load(scene* scene, const std::string &path);
    void scene_render(scene* scene);
    ac::scene* scene_get_active();
    void scene_add_model(scene* scene, ac::model* model);
    void scene_add_camera(scene* scene, Camera camera, const f32 speed, const b8 is_active);
    ac::camera* scene_make_new_camera(scene* scene);
    ac::camera* scene_get_active_camera(scene* scene);
    ac::camera* scene_get_active_camera();
    // 2D
    ac::scene_2d* scene_2d_make_new();
    void scene_2d_render(scene_2d* scene);
    void scene_2d_load(scene_2d* scene, const std::string& path);
    ac::scene_2d* scene_2d_get_active();
    void scene_add_object_2d(scene_2d* scene, object_2d* object);
    object_2d* scene_2d_make_new_object(scene_2d* scene);
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
    // shader
    b8 shader_load(Shader& shader, const std::string& vertex, const std::string& fragment);
    i32 shader_set_float(Shader* shader, const f32 value, const std::string& uniform_name);
    i32 shader_set_int(Shader* shader, const i32 value, const std::string& uniform_name);
    i32 shader_set_vector3f(Shader* shader, const Vector3& value, const std::string& uniform_name);
    i32 shader_set_vector3i(Shader* shader, const Vector3& value, const std::string& uniform_name);
    i32 shader_set_matrix(Shader* shader, const Matrix& value, const std::string& uniform_name);
    i32 shader_set_texture(Shader* shader, Texture2D& texture, const std::string& uniform_name);
    // material
    b8 material_load(Material& material, const std::string& vertex, const std::string& fragment);
    i32 material_set_float(Material* material, const f32 value, const std::string& uniform_name);
    i32 material_set_int(Material* material, const i32 value, const std::string& uniform_name);
    i32 material_set_vector3f(Material* material, const Vector3& value, const std::string& uniform_name);
    i32 material_set_vector3i(Material* material, const Vector3& value, const std::string& uniform_name);
    i32 material_set_matrix(Material* material, const Matrix& value, const std::string& uniform_name);
    i32 material_set_texture(Material* material, const Texture2D& texture, const std::string& uniform_name);
    // camera
    void camera_move(camera* camera, const Vector3& delta);
    void camera_rotate_around_target(camera* camera, const Vector3& delta);
    void camera_rotate(camera* camera, const Vector3& delta);
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
    const Vector3 transform_get_location(const Matrix& transform);
    const Vector3 transform_get_rotation(const Matrix& transform);
    const Vector3 transform_get_scale(const Matrix& transform);
    // light
    // ...
    //config
    b8 config_window_get_size(i32* width, i32* height);
    const std::string config_window_get_name();
    const i32 config_render_get_target_fps();
    // editor
    void editor_init();
    void editor_render_3d(ac::camera* camera);
    void editor_render_2d();
}
