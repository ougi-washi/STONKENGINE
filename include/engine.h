// STONKENGINE - Jed Fakhfekh - https://github.com/ougi-washi
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

namespace se{

    struct model{
        Model data = {0};
        std::string path = "";
        std::string name = "";
        std::vector<std::string> materials = {};
        struct instances{
            std::vector<Matrix> transforms = {};
        } instances;
    };

    struct camera {
        Camera camera;
        f32 movement_speed;
        f32 rotation_speed;
        b8 is_active;
    };

    struct light {
        Transform transform;
        Color color;
        f32 intensity;
        f32 radius;
    };

    struct scene {
        std::string path = "";
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

    struct scene_2d_element {
        object_2d* object;
        RenderTexture2D* render_texture;
    };

    struct ui_banner{
        std::string title = "";
        std::string description = "";
        std::string version = "";
    };

    struct ui_layout{
        
    };

    enum input_state {
        PRESSED = 0,
        RELEASED = 1,
        DOWN = 2,
        UP = 3,
    };

    struct input_keyboard{
        KeyboardKey key = KEY_NULL;
        input_state state = input_state::PRESSED;
    };

    struct input_mouse{
        MouseButton button = MOUSE_LEFT_BUTTON;
        input_state state = input_state::PRESSED;
    };

    struct input_map {
        std::vector<input_keyboard> inputs_keyboard = {};
        std::vector<input_mouse> inputs_mouse = {};
        void(*callback)();
    };

    struct input_handler {
        std::vector<input_map> input_maps = {};
        b8 is_active = true;
    };

    struct selection_handler{
        std::vector<se::model*> hovered_models = {};
        std::vector<se::model*> selected_models = {};
        std::vector<se::camera*> selected_cameras = {}; // TODO: Maybe it will be removed?
        std::vector<se::light*> selected_lights = {};
    };

    struct editor{
        b8 is_active = false;
        se::selection_handler selection = {};
        b8 show_grid = true;
        b8 show_wireframe = true;
    };

    typedef std::function<void(void)> command; 
    typedef std::queue<command> command_queue;

    struct engine {
        std::vector<scene> scenes = {};
        std::vector<scene_2d> scenes_2d = {};
        std::vector<model> models_pool = {};
        input_handler input = {};
        command_queue render_queue = {};
        se::editor editor = {};
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
    void engine_enqueue_render_command(const se::command& render_command);
    editor* engine_get_editor();
    // input
    input_handler* input_get_handler();
    void input_add_map(const input_map& input_map);
    void input_process();
    void input_set_active(const b8 new_state);
    // scene
    se::scene* scene_make_new();
    void scene_load(scene* scene, const std::string &path);
    void scene_render(scene* scene);
    se::scene* scene_get_active();
    void scene_clear(scene* scene);
    void scene_save(scene* scene);
    void scene_add_model(scene* scene, se::model* model);
    void scene_add_camera(scene* scene, Camera camera, const f32 movement_speed, const f32 rotation_speed, const b8 is_active);
    se::camera* scene_make_new_camera(scene* scene);
    se::camera* scene_get_active_camera(scene* scene);
    se::camera* scene_get_active_camera();
    // 2D
    se::scene_2d* scene_2d_make_new();
    void scene_2d_render(scene_2d* scene);
    void scene_2d_load(scene_2d* scene, const std::string& path);
    se::scene_2d* scene_2d_get_active();
    scene_2d_element scene_2d_make_new_object(scene_2d* scene);
    void scene_2d_element_bake(scene_2d_element& element);
    void scene_2d_bake_all(scene_2d* scene);
    void scene_2d_bake(object_2d* object, RenderTexture2D* render_texture);
    // model
    se::model* model_load(const json &model_json);
    se::model* model_load(const std::string& path);
    void model_save(se::model* model, json& model_json);
    void model_render(model* model);
    void model_render_wireframe(model* model, const Color& tint);
    void model_render_instances(model* model);
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
    Vector3 camera_move(camera* camera, const Vector3& delta, const b8 apply = true);
    Vector3 camera_rotate(camera* camera, const Vector3& delta, const b8 around_target = false, const b8 apply = true);
    void camera_set_position(camera* camera, const Vector3& position);
    void camera_set_target(camera* camera, const Vector3& target);
    void camera_set_fovy(camera* camera, const f32 fovy);
    void camera_set_active(camera* camera, const b8 new_state);
    void camera_set_position(Camera* camera, const Vector3& position);
    void camera_set_target(Camera* camera, const Vector3& target);
    void camera_set_fovy(Camera* camera, const f32 fovy);
    // transform
    const Vector3 transform_get_translation(const json& transform_json);
    const Vector3 transform_get_rotation(const json& transform_json);
    const Vector3 transform_get_scale(const json& transform_json);
    void transform_set_location(Matrix& transform, const Vector3& location);
    void transform_set_rotation(Matrix& transform, const Vector3& rotation);
    void transform_set_scale(Matrix& transform, const Vector3& scale);
    const Vector3 transform_get_location(const Matrix& transform);
    const Vector3 transform_get_rotation(const Matrix& transform);
    const Quaternion transform_get_rotation_quaternion(const Matrix& transform);
    const Vector3 transform_get_scale(const Matrix& transform);
    const Vector3 matrix_to_euler(const Matrix& mat);
    Matrix get_transform_matrix(const Vector3& translation, const Vector3& rotation, const Vector3& scale, const Vector3& center);
    void get_transform_components(const Matrix& matrix, Vector3* translation, Vector3* rotation, Vector3* scale, const Vector3& center);
    // light
    // ...
    //config
    b8 config_window_get_size(i32* width, i32* height);
    const std::string config_window_get_name();
    const i32 config_render_get_target_fps();
    // editor
    void editor_init();
    void editor_update();
    void editor_render_3d(se::camera* camera);
    void editor_render_2d();
    void editor_toggle_show_grid();
    void editor_toggle_show_wireframe();
    selection_handler* editor_get_selection_handler();  
    b8 editor_is_selecting();
    b8 editor_is_hovering();
    se::model* get_model_from_screen(const Vector2& screen_position);
    b8 editor_hover(const Vector2& screen_position);
    b8 editor_select(const Vector2& screen_position);
}
