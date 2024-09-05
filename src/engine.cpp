// STONKENGINE - Jed Fakhfekh - https://github.com/ougi-washi
#include "engine.h"
#include "array.h"
#include "config.h"
#include "log.h"
#include "utility.h"
#include "raymath.h"
#include "defines.h"
#include "rlgl.h"
#include "rcamera.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

se::engine* engine_instance = NULL;

void se::engine_start(){
    log_info("Starting engine");
    engine_instance = new engine();
    // load config
    i32 window_width = 0;
    i32 window_height = 0;
    se::config_window_get_size(&window_width, &window_height);
    const std::string& window_name = se::config_window_get_name();
    const i32 target_fps = se::config_render_get_target_fps();
    // init raylib
    InitWindow(window_width, window_height, window_name.c_str());
    SetExitKey(KEY_NULL);
    // SetConfigFlags(FLAG_VSYNC_HINT);
	SetConfigFlags(FLAG_MSAA_4X_HINT);
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    SetTargetFPS(target_fps);
    rlClearColor(0, 0, 0, 255);
    rlEnableDepthTest();
	rlEnableColorBlend();
	rlSetBlendMode(RL_BLEND_ALPHA);
	rlEnableBackfaceCulling();
#if SE_EDITOR_MODE
    editor_init();
#endif // SE_EDITOR_MODE
}

const b8 se::engine_should_loop(){

    return !WindowShouldClose();
}

void se::engine_end(){
    log_info("Clearing engine");
    CloseWindow();
    engine* engine = se::engine_get_instance();
    std::vector<se::model>* models_pool = se::engine_get_models_pool();
    if (models_pool){
        for (se::model& model : *models_pool){
            for (i32 i = 0; i < model.data.materialCount; i++){
                UnloadShader(model.data.materials[i].shader);
            }
            UnloadModel(model.data); 
        }
        models_pool->clear();
    }
    for (se::scene& scene : engine->scenes){
        scene.models.clear();
        scene.cameras.clear();
    }
    for (se::scene_2d& scene_2d : engine->scenes_2d){
        for (object_2d& object : scene_2d.objects){
            UnloadShader(object.shader);
            for (Texture2D& texture : object.textures){
                UnloadTexture(texture);
            }
        }
        scene_2d.objects.clear();
        for (RenderTexture2D& render_texture : scene_2d.render_textures){
            UnloadRenderTexture(render_texture);
        }
        scene_2d.render_textures.clear();
    }
    engine->scenes_2d.clear();
    delete engine_instance;
}

se::engine *se::engine_get_instance(){
    if (!engine_instance) { log_error("Engine instance is NULL"); return NULL; }
    return engine_instance;
}

void se::engine_render(){
#if SE_EDITOR_MODE
    se::editor_update();
#endif //SE_EDITOR_MODE
    se::scene* scene = se::scene_get_active();
    se::scene_render(scene);
    se::scene_2d* scene_2d = se::scene_2d_get_active();
    se::scene_2d_render(scene_2d);

    command_queue* render_queue = se::engine_get_render_queue();
    BeginDrawing();
    {
        while(!render_queue->empty()){
            command render_command = se::engine_get_instance()->render_queue.front();
            if (render_command) { 
                render_command(); 
            }
            se::engine_get_instance()->render_queue.pop();
        }
    }
    EndDrawing();
}

std::vector<se::model>* se::engine_get_models_pool(){
    return &se::engine_get_instance()->models_pool;
}

void se::engine_process_input(){
    se::engine* engine = se::engine_get_instance();
    se::input_process();
}

const f32 se::engine_get_delta_time(){
    return GetFrameTime();
}

se::command_queue* se::engine_get_render_queue(){
    se::engine* engine = se::engine_get_instance();
    return &engine->render_queue;
}

void se::engine_enqueue_render_command(const se::command &render_command){
    se::engine_get_render_queue()->emplace(render_command);
}

se::editor *se::engine_get_editor(){
    return &se::engine_get_instance()->editor;
}

se::input_handler *se::input_get_handler(){
    return &se::engine_get_instance()->input;
}

void se::input_add_map(const se::input_map &input_map)
{
    se::engine* engine = se::engine_get_instance();
    se::input_map* input = se::push_back(&engine->input.input_maps);
    if(input) {
        *input = input_map;
    }
    else { log_error("Failed to add input map"); }
}

void se::input_process(){
    if (!se::engine_get_instance()->input.is_active) { return; }
    se::engine* engine = se::engine_get_instance();
    for (se::input_map& input_map : engine->input.input_maps){
        if (input_map.inputs_keyboard.empty() && input_map.inputs_mouse.empty()) { 
            log_error("Cannot process input, no keys or buttons found"); continue; 
        }  
        b8 valid_keyboard_keys = true;
        for (const input_keyboard& input : input_map.inputs_keyboard){
            const b8 required_state =  
                        (input.state == PRESSED) ? IsKeyPressed(input.key) : 
                        (input.state == RELEASED) ? IsKeyReleased(input.key) : 
                        (input.state == DOWN) ? IsKeyDown(input.key) :
                        IsKeyUp(input.key);
            if (!required_state){
                valid_keyboard_keys = false;
                break;
            }
        }
        b8 valid_mouse_buttons = true;
        for (const input_mouse& input : input_map.inputs_mouse){
            b8 required_state = 
                        (input.state == PRESSED) ? IsMouseButtonPressed(input.button) : 
                        (input.state == RELEASED) ? IsMouseButtonReleased(input.button) : 
                        (input.state == DOWN) ? IsMouseButtonDown(input.button) :
                        IsMouseButtonUp(input.button);
            if (!required_state){
                valid_mouse_buttons = false;
                break;
            }
        }
        if (valid_keyboard_keys && valid_mouse_buttons){
            input_map.callback();
        }
    }
}

void se::input_set_active(const b8 new_state){
    se::engine* engine = se::engine_get_instance();
    engine->input.is_active = new_state;
}

b8 se::shader_load(Shader &shader, const std::string &vertex, const std::string &fragment){
    const std::string& shaders_path = se::config_get_root_path() + se::config_get_shaders_path();
    const std::string& vertex_path = shaders_path + vertex;
    const std::string& fragment_path = shaders_path + fragment;
    if (IsShaderReady(shader) &&
        (vertex == "") ? true : IsFileExtension(vertex_path.c_str(), ".glsl") && 
        (fragment == "") ? true : IsFileExtension(fragment_path.c_str(), ".glsl")){
        shader = LoadShader((vertex == "") ? 0 : vertex_path.c_str(), (fragment == "") ? 0 : fragment_path.c_str());
        return shader.id != 0;
    }
    return false;
}

i32 se::shader_set_float(Shader* shader, const f32 value, const std::string& uniform_name) {
    if (!shader) { log_error("Shader is NULL"); return -1; }
    int location = GetShaderLocation(*shader, uniform_name.c_str());
    if (location == -1) { log_error("Uniform location not found"); return -1; }
    SetShaderValue(*shader, location, &value, SHADER_UNIFORM_FLOAT);
    return location;
}

i32 se::shader_set_int(Shader* shader, const i32 value, const std::string& uniform_name) {
    if (!shader) { log_error("Shader is NULL"); return -1; }
    int location = GetShaderLocation(*shader, uniform_name.c_str());
    if (location == -1) { log_error("Uniform location not found"); return -1; }
    SetShaderValue(*shader, location, &value, SHADER_UNIFORM_INT);
    return location;
}

i32 se::shader_set_vector3f(Shader* shader, const Vector3& value, const std::string& uniform_name) {
    if (!shader) { log_error("Shader is NULL"); return -1; }
    int location = GetShaderLocation(*shader, uniform_name.c_str());
    if (location == -1) { log_error("Uniform location not found"); return -1; }
    SetShaderValue(*shader, location, &value, SHADER_UNIFORM_VEC3);
    return location;
}

i32 se::shader_set_vector3i(Shader* shader, const Vector3& value, const std::string& uniform_name) {
    if (!shader) { log_error("Shader is NULL"); return -1; }
    int location = GetShaderLocation(*shader, uniform_name.c_str());
    if (location == -1) { log_error("Uniform location not found"); return -1; }
    SetShaderValue(*shader, location, &value, SHADER_UNIFORM_IVEC3);
    return location;
}

i32 se::shader_set_matrix(Shader* shader, const Matrix& value, const std::string& uniform_name) {
    if (!shader) { log_error("Shader is NULL"); return -1; }
    int location = GetShaderLocation(*shader, uniform_name.c_str());
    if (location == -1) { log_error("Uniform location not found"); return -1; }
    SetShaderValueMatrix(*shader, location, value);
    return location;
}

i32 se::shader_set_texture(Shader* shader, Texture2D& texture, const std::string& uniform_name) {
    if (!shader) { log_error("Shader is NULL"); return -1; }
    const i32 location = GetShaderLocation(*shader, uniform_name.c_str());
    if (location == -1) { log_error("Uniform location not found"); return -1; }
    SetShaderValueTexture(*shader, 6, texture);
    return location;
}

void material_load(Material& material, const json &material_json){
    if (!material_json.contains("vertex")) { log_error("Could not load material, vertex path not found"); return; }
    if (!material_json.contains("fragment")) { log_error("Could not load material, fragment path not found"); return; }
    const std::string& vertex_path = material_json["vertex"];
    const std::string& fragment_path = material_json["fragment"];
    se::material_load(material, vertex_path, fragment_path);
    if (material_json.contains("uniforms")){
        const json& uniforms = material_json["uniforms"];
        for (const json& uniform : uniforms){
            if (!uniform.contains("name")) { log_error("Could not load material, uniform name not found"); continue; }
            if (!uniform.contains("type")) { log_error("Could not load material, uniform type not found"); continue; }
            const std::string& uniform_name = uniform["name"];
            const std::string& uniform_type = uniform["type"];
            if (uniform_type == "float"){
                if (!uniform.contains("value")) { log_error("Could not load material, uniform value not found"); continue; }
                const f32 value = uniform["value"];
                se::material_set_float(&material, value, uniform_name);
            }
            else if (uniform_type == "int"){
                if (!uniform.contains("value")) { log_error("Could not load material, uniform value not found"); continue; }
                const i32 value = uniform["value"];
                se::material_set_int(&material, value, uniform_name);
            }
            else if (uniform_type == "vec3f"){
                if (!uniform.contains("value")) { log_error("Could not load material, uniform value not found"); continue; }
                const Vector3& value = {uniform["value"][0], uniform["value"][1], uniform["value"][2]};
                se::material_set_vector3f(&material, value, uniform_name);
            }
            else if (uniform_type == "vec3i"){
                if (!uniform.contains("value")) { log_error("Could not load material, uniform value not found"); continue; }
                const Vector3& value = {uniform["value"][0], uniform["value"][1], uniform["value"][2]};
                se::material_set_vector3i(&material, value, uniform_name);
            }
            else if (uniform_type == "matrix"){
                if (!uniform.contains("value")) { log_error("Could not load material, uniform value not found"); continue; }
                const Matrix& value = {
                    uniform["value"][0], uniform["value"][1], uniform["value"][2], uniform["value"][3],
                    uniform["value"][4], uniform["value"][5], uniform["value"][6], uniform["value"][7],
                    uniform["value"][8], uniform["value"][9], uniform["value"][10], uniform["value"][11],
                    uniform["value"][12], uniform["value"][13], uniform["value"][14], uniform["value"][15]
                };
                se::material_set_matrix(&material, value, uniform_name);
            }
            else if (uniform_type == "texture"){
                if (!uniform.contains("value")) { log_error("Could not load material, uniform value not found"); continue; }
                const std::string& texture_path = uniform["value"];
                const Texture2D& texture = LoadTexture(texture_path.c_str());
                se::material_set_texture(&material, texture, uniform_name);
            }
        }
    }

}

se::scene *se::scene_make_new(){
    se::engine* engine = se::engine_get_instance();
    return se::push_back(&engine->scenes);
}

void se::scene_load(se::scene *scene, const std::string &path)
{
    if(!scene) { log_error("Cannot load scene, scene is NULL"); return; }
    const std::string& file_content = se::read_file(se::config_get_root_path() + se::config_get_scenes_path() + path);
    if (file_content.empty()) { log_error("Could not load scene, file is empty"); return; }
    const json& scene_json = json::parse(file_content);
    if (scene_json.empty()) { log_error("Could not load scene, json is empty"); return; }
    scene_clear(scene);
    scene->path = path;
    if (!scene_json.contains("objects")) { log_error("Could not load scene, objects not found"); return; }
    const std::vector<json>& objects = scene_json["objects"];

    for (const json& object : objects){
        if (!object.contains("type")) { log_error("Could not load scene, object type not found"); continue; }
        const std::string& type = object["type"];
        if (type == "model"){
            se::model* model = se::model_load(object);
            if (!model) { log_error("Could not load scene, model is NULL"); continue; }
            se::scene_add_model(scene, model);
        }
        else if (type == "camera"){
            // const json& camera = object["camera"];
            if (!object.contains("position")) { log_error("Could not load scene, camera position not found"); continue; }
            if (!object.contains("target")) { log_error("Could not load scene, camera target not found"); continue; }
            if (!object.contains("up")) { log_warn("Could not load scene, camera up not found"); }
            if (!object.contains("fovy")) { log_warn("Could not load scene, camera fovy not found"); }
            if (!object.contains("is_active")) { log_warn("Could not load scene, camera is_active not found"); }
            
            const Vector3& position = {object["position"][0], object["position"][1], object["position"][2]};
            const Vector3& target = {object["target"][0], object["target"][1], object["target"][2]};
            const Vector3& up = (object.contains("up") ? Vector3{object["up"][0], object["up"][1], object["up"][2]} : Vector3{0.0f, 1.0f, 0.0f});
            const f32 fovy = object.contains("fovy") ? (f32)object["fovy"] : 45.0f;
            const b8 is_active = object.contains("is_active") ? (b8)object["is_active"] : false;
            const f32 movement_speed = object.contains("movement_speed") ? (f32)object["movement_speed"] : 1.0f;
            const f32 rotation_speed = object.contains("rotation_speed") ? (f32)object["rotation_speed"] : 1.0f;            

            se::scene_add_camera(scene, {
                position,
                target,
                up,
                fovy,
                CAMERA_PERSPECTIVE
            }, movement_speed, rotation_speed, is_active);
        }
    }
}

void se::scene_add_model(se::scene *scene, se::model* model){
    // edit json instead of adding model directly
    if(!scene) { log_error("Scene is NULL"); return; }
    if(!model) { log_error("Model is NULL"); return; }
    se::model* scene_model = se::push_back(&scene->models);
    if(scene_model) {
        *scene_model = *model;
    }
    else { log_error("Failed to add model to scene"); }
}

void se::scene_add_camera(se::scene *scene, Camera camera, const f32 movement_speed, const f32 rotation_speed, const b8 is_active){
    // edit json instead of adding camera directly
    if(!scene) { log_error("Scene is NULL"); return; }
    se::camera* scene_camera = se::push_back(&scene->cameras);
    if(scene_camera) {
        scene_camera->camera = camera;
        scene_camera->movement_speed = movement_speed;
        scene_camera->rotation_speed = rotation_speed;
        se::camera_set_active(scene_camera, is_active);
    }
    else { log_error("Failed to add camera to scene"); }
}

se::camera *se::scene_make_new_camera(se::scene *scene){
    // edit json instead of adding camera directly
    if(!scene) { log_error("Scene is NULL"); return NULL; }
    se::camera* scene_camera = se::push_back(&scene->cameras);
    if(scene_camera) {
        scene_camera->camera = {
            {10.0f, 10.0f, 10.0f}, // position
            {0.0f, 0.0f, 0.0f}, // target
            {0.0f, 1.0f, 0.0f}, // up
            45.0f, // fovy
            CAMERA_PERSPECTIVE // projection
        };
        scene_camera->is_active = false;
        return scene_camera;
    }
    else { log_error("Failed to add camera to scene"); return NULL; }
}

void se::scene_render(se::scene *scene){
    if(!scene) { log_error("Cannot render, scene is NULL"); return; }
    se::camera* camera = se::scene_get_active_camera(scene);
    if (!camera) { log_error("Cannot render, camera is NULL"); return; }

    engine_enqueue_render_command([scene = scene, camera = camera](){
        ClearBackground(BLACK);
        BeginMode3D(camera->camera);
        for (se::model& model : scene->models){
            se::model_render(&model);
        }
#if SE_EDITOR_MODE
        editor_render_3d(camera);
#endif //SE_EDITOR_MODE
        EndMode3D();
    });
}

se::scene *se::scene_get_active(){
    // currently only one scene is supported
    se::scene* scene = &se::engine_get_instance()->scenes[0];
    return scene;
}

void se::scene_clear(scene *scene){
    if(!scene) { log_error("Cannot clear scene, scene is NULL"); return; }
    scene->models.clear();
    scene->cameras.clear();
}

void se::scene_save(scene *scene){
    if(!scene) { log_error("Cannot save scene, scene is NULL"); return; }
    const std::string& file_path = se::config_get_root_path() + se::config_get_scenes_path() + scene->path;
    json scene_json;
    scene_json["objects"] = json::array();
    for (se::model& model : scene->models){
        json model_json;
        se::model_save(&model, model_json);
        scene_json["objects"].push_back(model_json);
    }
    for (se::camera& camera : scene->cameras){
        json camera_json;
        camera_json["type"] = "camera";
        camera_json["position"] = {camera.camera.position.x, camera.camera.position.y, camera.camera.position.z};
        camera_json["target"] = {camera.camera.target.x, camera.camera.target.y, camera.camera.target.z};
        camera_json["up"] = {camera.camera.up.x, camera.camera.up.y, camera.camera.up.z};
        camera_json["fovy"] = camera.camera.fovy;
        camera_json["is_active"] = camera.is_active;
        camera_json["movement_speed"] = camera.movement_speed;
        camera_json["rotation_speed"] = camera.rotation_speed;
        scene_json["objects"].push_back(camera_json);
    }
    std::ofstream file(file_path);
    file << scene_json.dump(4);
    file.close();
}

se::camera *se::scene_get_active_camera(se::scene *scene){
    if(!scene) { log_error("Cannot get active camera, scene is NULL"); return NULL; }
    for (se::camera& camera : scene->cameras){
        if(camera.is_active) { return &camera; }
    }
    return NULL;
}

se::camera *se::scene_get_active_camera(){
    se::scene* scene = se::scene_get_active();
    return se::scene_get_active_camera(scene);
}

se::scene_2d *se::scene_2d_make_new(){
    se::engine* engine = se::engine_get_instance();
    return se::push_back(&engine->scenes_2d);
}

void se::scene_2d_render(scene_2d *scene){
    if(!scene) { log_error("Cannot render, scene is NULL"); return; }

    engine_enqueue_render_command([scene = scene](){
        for (sz i = 0 ; i < scene->render_textures.size(); ++i){
            RenderTexture2D& render_texture = scene->render_textures[i];
            DrawTextureRec(render_texture.texture, { 0, 0, (f32)render_texture.texture.width, -(f32)render_texture.texture.height }, { 0, 0 }, { 255, 255, 255, 100 });
        }
#if SE_EDITOR_MODE
            editor_render_2d();
#endif //SE_EDITOR_MODE
    });
}

void se::scene_2d_load(scene_2d *scene, const std::string &path){
    if(!scene) { log_error("Cannot load scene, scene is NULL"); return; }
    const std::string& file_content = se::read_file(se::config_get_root_path() + se::config_get_scenes_path() + path);
    if (file_content.empty()) { log_error("Could not load scene, file is empty"); return; }
    const json& scene_json = json::parse(file_content);
    if (scene_json.empty()) { log_error("Could not load scene, json is empty"); return; }

    if (!scene_json.contains("objects")) { log_error("Could not load scene, objects not found"); return; }
    const std::vector<json>& objects = scene_json["objects"];

    for (const json& object : objects){
        se::object_2d* object_2d = se::push_back(&scene->objects);
        if (!object_2d) { log_error("Failed to add object_2d to scene"); continue; }
        if (!object.contains("fragment")) { log_error("Could not load scene, object_2d fragment not found"); continue; }
        if (!object.contains("vertex")) { log_error("Could not load scene, object_2d vertex not found"); continue; }
        object_2d->fragment = object["fragment"];
        object_2d->vertex = object["vertex"];
        if (object.contains("textures")){
            const std::vector<json>& textures = object["textures"];
            for (const json& texture : textures){
                if (!texture.contains("path")) { log_error("Could not load scene, texture path not found"); continue; }
                const std::string& texture_path = texture["path"];
                object_2d->textures.push_back(LoadTexture(texture_path.c_str()));
            }
        }
        if (object.contains("texts")){
            const std::vector<json>& texts = object["texts"];
            for (const json& text : texts){
                if (!text.contains("text")) { log_error("Could not load scene, text not found"); continue; }
                if (!text.contains("font")) { log_error("Could not load scene, font not found"); continue; }
                if (!text.contains("position")) { log_error("Could not load scene, text position not found"); continue; }
                if (!text.contains("fontSize")) { log_error("Could not load scene, text fontSize not found"); continue; }
                if (!text.contains("spacing")) { log_error("Could not load scene, text spacing not found"); continue; }
                if (!text.contains("tint")) { log_error("Could not load scene, text tint not found"); continue; }
                const std::string& text_text = text["text"];
                const std::string& text_font = text["font"];
                const Vector2& text_position = {text["position"][0], text["position"][1]};
                const f32 text_fontSize = text["fontSize"];
                const f32 text_spacing = text["spacing"];
                const Color text_tint = {text["tint"][0], text["tint"][1], text["tint"][2], text["tint"][3]};
                se::text* scene_text = se::push_back(&object_2d->texts);
                if (scene_text){
                    scene_text->string = text_text;
                    scene_text->font = LoadFont(text_font.c_str());
                    scene_text->position = text_position;
                    scene_text->fontSize = text_fontSize;
                    scene_text->spacing = text_spacing;
                    scene_text->tint = text_tint;
                }
                else { log_error("Failed to add text to object_2d"); }
            }
        }
    }
}

se::scene_2d *se::scene_2d_get_active(){
    // currently only one scene is supported
    se::scene_2d* scene = &se::engine_get_instance()->scenes_2d[0];
    return scene;
}

se::scene_2d_element se::scene_2d_make_new_object(se::scene_2d *scene){
    if(!scene) { log_error("Cannot make new object, scene is NULL"); return {NULL, NULL}; }
    se::object_2d* object = se::push_back(&scene->objects);
    if(object) {
        scene->render_textures.push_back(LoadRenderTexture(GetScreenWidth(), GetScreenHeight()));
        return {object , &scene->render_textures.back()};
    }
    log_error("Failed to add object to scene"); 
    return {NULL, NULL};
}

void se::scene_2d_element_bake(scene_2d_element &element){
    if (element.object == nullptr) { log_error("Cannot bake object, object is NULL"); return; }
    if (element.render_texture == nullptr) { log_error("Cannot bake object, render_texture is NULL"); return; }
    se::scene_2d_bake(element.object, element.render_texture);
}

void se::scene_2d_bake_all(scene_2d *scene){
    for (sz i = 0 ; i < scene->objects.size(); ++i){
        object_2d& object = scene->objects[i];
        RenderTexture2D& render_texture = scene->render_textures[i];
        scene_2d_bake(&object, &render_texture);
    }
}

void se::scene_2d_bake(object_2d *object, RenderTexture2D *render_texture){
    if (object == nullptr) { log_error("Cannot bake object, object is NULL"); return; }
    if (render_texture == nullptr) { log_error("Cannot bake object, render_texture is NULL"); return; }
    BeginTextureMode(*render_texture);
    {
        ClearBackground({0, 0, 0, 0});
        // TODO: Implement shader system (not working atm)
        // shader_load(object->shader, object->vertex, object->fragment);
        // for (sz i = 0; i < object->textures.size(); ++i) {
        //     shader_set_texture(&object->shader, object->textures[i], "texture" + std::to_string(i));
        // }
        // BeginShaderMode(object->shader);
        // {
        //     rlBegin(RL_QUADS);
        //     {
        //         rlTexCoord2f(0.0f, 0.0f);
        //         rlVertex2f(-1.0f, -1.0f);
        //         rlTexCoord2f(1.0f, 0.0f);
        //         rlVertex2f(1.0f, -1.0f);
        //         rlTexCoord2f(1.0f, 1.0f);
        //         rlVertex2f(1.0f, 1.0f);
        //         rlTexCoord2f(0.0f, 1.0f);
        //         rlVertex2f(-1.0f, 1.0f);
        //     }
        //     rlEnd();
        //     
        // }
        // EndShaderMode();
        DrawTextureRec(render_texture->texture, { 0, 0, (f32)render_texture->texture.width, -(f32)render_texture->texture.height }, { 0, 0 }, { 255, 255, 255, 255 });
        for (const text& text : object->texts){
            DrawTextEx(text.font, text.string.c_str(), { text.position.x, text.position.y }, text.fontSize, text.spacing, text.tint);
        }
    }
    EndTextureMode();
}

se::model* se::model_load(const json &model_json){
    se::model* model = nullptr;
    if (!model_json.contains("path")) { log_error("Could not load scene, model path not found"); return nullptr; }
    const std::string& object_path = model_json["path"];
    model = se::model_load(object_path);
    if (!model) { log_error("Could not load scene, model is NULL"); return nullptr; }
    if (model_json.contains("transform")){
        const json& transform = model_json["transform"];
        const Vector3& translation = transform_get_translation(transform);
        const Vector3& rotation = transform_get_rotation(transform);
        const Vector3& scale = transform_get_scale(transform);
        model->data.transform = get_transform_matrix(translation, rotation, scale, translation);
    }
    if (model_json.contains("materials")){
        const std::vector<json>& materials = model_json["materials"];
        // model->data.materialCount = materials.size();
        // model->data.materials = (Material*)RL_MALLOC(model->data.materialCount*sizeof(Material));
        model->data.materialCount = 0;
        for (i32 i = 0; i < materials.size(); i++){
            const std::string& material_path = materials[i];
            model->materials.push_back(material_path);
            Material& material = model->data.materials[i];
            const std::string& material_file_content = se::read_file(se::config_get_root_path() + se::config_get_materials_path() + material_path);
            if (material_file_content.empty()) { log_error("Could not load material, file is empty"); continue; } 
            model->data.materialCount++;
            model->data.materials = (Material*)RL_REALLOC(model->data.materials, model->data.materialCount*sizeof(Material));
            material_load(model->data.materials[i], json::parse(material_file_content));
        }
    }
    if (model_json.contains("instances")){
        const std::vector<json>& instances = model_json["instances"];
        for (const json& instance : instances){
            if (!instance.contains("transform")) { log_error("Could not load instance, transform not found"); continue; }
            const Vector3& translation = transform_get_translation(instance["transform"]);
            const Vector3& rotation = transform_get_rotation(instance["transform"]);
            const Vector3& scale = transform_get_scale(instance["transform"]);
            model->instances.transforms.push_back(get_transform_matrix({0}, rotation, scale, {0}));
        }
    }
    return model;
}

se::model *se::model_load(const std::string &path){
    const std::string& model_path = se::config_get_root_path() + se::config_get_models_path() + path;
    se::engine* engine = se::engine_get_instance();
    se::model* model = nullptr;
    for (se::model& current_model : *se::engine_get_models_pool()){
        if (current_model.path == model_path && current_model.data.meshCount > 0){
            model = &current_model;
        }
    }
    if (!model){
        log_info("Model %s not in pool, loading model", model_path.c_str());
        model = se::push_back(se::engine_get_models_pool());
        if (!model) { log_error("Failed to add model to the pool"); return NULL; }
        model->data = LoadModel(model_path.c_str());
        model->path = path;
        model->data.transform = MatrixIdentity();
    }
    return model;
}

void se::model_save(se::model *model, json &model_json){
    if(!model) { log_error("Cannot save model, model is NULL"); return; }
    model_json["type"] = "model";
    model_json["name"] = model->name;
    model_json["path"] = model->path;
    model_json["transform"] = json::object();
    json& transform = model_json["transform"];
    // Vector3 translation = transform_get_location(model->data.transform);
    // Vector3 rotation = transform_get_rotation(model->data.transform);
    // Vector3 scale = transform_get_scale(model->data.transform);
    Vector3 translation = {};
    Vector3 rotation = {};
    Vector3 scale = {};
    get_transform_components(model->data.transform, &translation, &rotation, &scale, translation);
    transform["translation"] = {translation.x, translation.y, translation.z};
    transform["rotation"] = {rotation.x, rotation.y, rotation.z};
    transform["scale"] = {scale.x, scale.y, scale.z};
    model_json["materials"] = json::array();
    for (i32 i = 0; i < model->materials.size(); i++){
        model_json["materials"].push_back(model->materials[i]);
    }
}

void se::model_render(se::model *model) {
    if(!model) { log_error("Cannot render, model is NULL"); return; }
    
    if (model->instances.transforms.size() > 0) {
        se::model_render_instances(model);
    }
    else {
        for (i32 i = 0; i < model->data.meshCount; i++){
            for (i32 j = 0; j < model->data.materialCount; j++){
                if (model->data.meshMaterial[i] == j){
                    DrawMesh(model->data.meshes[i], model->data.materials[j], model->data.transform);
                }
            }
        }
    }
}

void se::model_render_wireframe(se::model *model, const Color& tint){
    if(!model) { log_error("Cannot render wireframe, model is NULL"); return; }
    Model model_copy = model->data;
    model_copy.materials = (Material*)RL_MALLOC(model_copy.materialCount*sizeof(Material));
    // model_copy.mater = 
    // Shader default_shader = LoadShader(0, 0);
    for (i32 i = 0; i < model_copy.materialCount; i++){
        model_copy.materials[i] = LoadMaterialDefault();;
        model_copy.materials[i].maps[MATERIAL_MAP_DIFFUSE].color = tint;
    }
    DrawModelWires(model_copy, {0}, {1}, tint);
    RL_FREE(model_copy.materials);
    // UnloadShader(default_shader);
}

void se::model_render_instances(se::model *model){
    if(!model) { log_error("Cannot render instances, model is NULL"); return; }
    for (i32 i = 0; i < model->data.meshCount; i++){
        for (i32 j = 0; j < model->data.materialCount; j++){
            if (model->data.meshMaterial[i] == j){
                DrawMeshInstanced(model->data.meshes[i], model->data.materials[j], model->instances.transforms.data(), model->instances.transforms.size());
            }
        }
        // DrawMeshInstanced(model->data.meshes[i], model->data.materials[model->data.meshMaterial[i]], transforms, count);
    }
}

void se::model_set_location(model *model, const Vector3& location) {
    if(!model) { log_error("Cannot set location, model is NULL"); return; }
    model->data.transform = MatrixTranslate(location.x, location.y, location.z);
}

void se::model_set_rotation(model *model, const Vector3& rotation) {
    if(!model) { log_error("Cannot set rotation, model is NULL"); return; }
    const Matrix& rotation_matrix = MatrixRotateXYZ({rotation.x, rotation.y, rotation.z});
    model->data.transform = MatrixMultiply(model->data.transform, rotation_matrix);
}

void se::model_set_scale(model *model, const Vector3& scale) {
    if(!model) { log_error("Cannot set scale, model is NULL"); return; }
    const Matrix& scale_matrix = MatrixScale(scale.x, scale.y, scale.z);
    model->data.transform = MatrixMultiply(model->data.transform, scale_matrix);
}

void se::model_set_material(model *model, Material *material, const i32 mesh_index){
    if(!model) { log_error("Cannot set material, model is NULL"); return; }
    if(!material) { log_error("Cannot set material, material is NULL"); return; }
    if (mesh_index < 0 || mesh_index >= model->data.meshCount) { log_error("Cannot set material, mesh index out of bounds"); return; }
    if (mesh_index >= model->data.materialCount) { log_error("Cannot set material, mesh index out of bounds"); return; }
    model->data.materials[mesh_index] = *material;
}

b8 se::material_load(Material &material, const std::string &vertex, const std::string &fragment){
    return shader_load(material.shader, vertex, fragment);
}

i32 se::material_set_float(Material *material, const f32 value, const std::string &uniform_name)
{
    const i32 slot = GetShaderLocation(material->shader, uniform_name.c_str());
    SetShaderValue(material->shader, slot, &value, SHADER_UNIFORM_FLOAT);
    return slot;
}

i32 se::material_set_int(Material *material, const i32 value, const std::string &uniform_name) {
    const i32 slot = GetShaderLocation(material->shader, uniform_name.c_str());
    SetShaderValue(material->shader, slot, &value, SHADER_UNIFORM_INT);
    return slot;
}

i32 se::material_set_vector3f(Material *material, const Vector3 &value, const std::string &uniform_name){
    const i32 slot = GetShaderLocation(material->shader, uniform_name.c_str());
    SetShaderValueV(material->shader, slot, &value, SHADER_UNIFORM_VEC3, 3);
    return slot;
}

i32 se::material_set_vector3i(Material *material, const Vector3 &value, const std::string &uniform_name){
    const i32 slot = GetShaderLocation(material->shader, uniform_name.c_str());
    SetShaderValueV(material->shader, slot, &value, SHADER_UNIFORM_IVEC3, 3);
    return slot;
}

i32 se::material_set_matrix(Material *material, const Matrix &value, const std::string &uniform_name){
    const i32 slot = GetShaderLocation(material->shader, uniform_name.c_str());
    SetShaderValueMatrix(material->shader, slot, value);
    return slot;
}

i32 se::material_set_texture(Material *material, const Texture2D &texture, const std::string &uniform_name) {
    const i32 slot = GetShaderLocation(material->shader, uniform_name.c_str());
    SetShaderValueTexture(material->shader, slot, texture);
    return slot;
}

Vector3 se::camera_move(se::camera *camera, const Vector3 &delta, const b8 apply){
    if (!camera) { log_error("Cannot move camera, camera is NULL"); return {0}; }
    Vector3 offset = delta * camera->movement_speed * engine_get_delta_time();
    if (apply){
        CameraMoveRight(&camera->camera, offset.x, false);
        CameraMoveForward(&camera->camera, offset.z, false);
        CameraMoveUp(&camera->camera, offset.y);
    }
    return offset;
}

Vector3 se::camera_rotate(se::camera *camera, const Vector3 &delta, const b8 around_target, const b8 apply){
    if (!camera) { log_error("Cannot rotate camera, camera is NULL"); return {0}; }
    const Vector3 offset = -delta * camera->rotation_speed * engine_get_delta_time();
    if (apply) {
        CameraPitch(&camera->camera, offset.y, true, around_target, false);
        CameraYaw(&camera->camera, offset.x, around_target);
    }
    return offset;
}

void se::camera_set_position(se::camera *camera, const Vector3 &position)
{
    if (!camera) { log_error("Cannot set camera position, camera is NULL"); return; }
    camera->camera.position = position;
}

void se::camera_set_target(se::camera *camera, const Vector3& target){
    if (!camera) { log_error("Cannot set camera target, camera is NULL"); return; }
    camera->camera.target = target;
}

void se::camera_set_fovy(se::camera *camera, const f32 fovy){
    if (!camera) { log_error("Cannot set camera fovy, camera is NULL"); return; }
    camera->camera.fovy = fovy;
}

void se::camera_set_active(se::camera *camera, const b8 new_state){
    if (!camera) { log_error("Cannot set camera active state, camera is NULL"); return; }
    se::scene* scene = se::scene_get_active();
    for (se::camera& camera : scene->cameras){
        camera.is_active = false;
    }
    camera->is_active = new_state;
}

void se::camera_set_position(Camera* camera, const Vector3& position){
    if (!camera) { log_error("Cannot set camera position, camera is NULL"); return; }
    camera->position = position;
}
void se::camera_set_target(Camera* camera, const Vector3& target){
    if (!camera) { log_error("Cannot set camera target, camera is NULL"); return; }
    camera->target = target;
}
void se::camera_set_fovy(Camera* camera, const f32 fovy){
    if (!camera) { log_error("Cannot set camera fovy, camera is NULL"); return; }
    camera->fovy = fovy;
}

const Vector3 se::transform_get_translation(const json &transform_json){
    if (transform_json.contains("translation")){
        return {transform_json["translation"][0], transform_json["translation"][1], transform_json["translation"][2]};
    }
    return {0};
}

const Vector3 se::transform_get_rotation(const json &transform_json){
    if (transform_json.contains("rotation")){
        return {transform_json["rotation"][0], transform_json["rotation"][1], transform_json["rotation"][2]};
    }
    return {0};
}

const Vector3 se::transform_get_scale(const json &transform_json){
    if (transform_json.contains("scale")){
        return {transform_json["scale"][0], transform_json["scale"][1], transform_json["scale"][2]};
    }
    return {1};
}

void se::transform_set_location(Matrix &transform, const Vector3 &location){
    transform.m12 = location.x;
    transform.m13 = location.y;
    transform.m14 = location.z;
}

void se::transform_set_rotation(Matrix &transform, const Vector3 &rotation){
    Quaternion quaternion = QuaternionFromEuler(rotation.x, rotation.y, rotation.z);
    transform.m0 = quaternion.x;
    transform.m1 = quaternion.y;
    transform.m2 = quaternion.z;
    transform.m3 = quaternion.w;
}

void se::transform_set_scale(Matrix &transform, const Vector3 &scale){
    transform.m0 *= scale.x;
    transform.m5 *= scale.y;
    transform.m10 *= scale.z;
}

const Vector3 se::transform_get_location(const Matrix &transform){
    return {transform.m12, transform.m13, transform.m14};
}

const Vector3 se::transform_get_rotation(const Matrix &transform){
    return matrix_to_euler(transform);
}

const Quaternion se::transform_get_rotation_quaternion(const Matrix &transform){
    return {transform.m0, transform.m1, transform.m2, transform.m3};
}

const Vector3 se::transform_get_scale(const Matrix &transform){
    Vector3 scale;
    scale.x = Vector3Length({transform.m0, transform.m1, transform.m2});
    scale.y = Vector3Length({transform.m4, transform.m5, transform.m6});
    scale.z = Vector3Length({transform.m8, transform.m9, transform.m10});
    return scale;
}

const Vector3 se::matrix_to_euler(const Matrix &mat) {
    Vector3 euler;

    float sy = sqrt(mat.m0 * mat.m0 + mat.m1 * mat.m1);
    bool singular = sy < 1e-6; // If

    if (!singular) {
        euler.x = atan2(mat.m6, mat.m10); // Roll
        euler.y = atan2(-mat.m2, sy);     // Pitch
        euler.z = atan2(mat.m1, mat.m0);  // Yaw
    } else {
        euler.x = atan2(-mat.m9, mat.m5); // Roll
        euler.y = atan2(-mat.m2, sy);     // Pitch
        euler.z = 0; // Yaw
    }

    return euler;
}

Matrix se::get_transform_matrix(const Vector3 &translation, const Vector3 &rotation, const Vector3 &scale, const Vector3 &center) {
    Matrix translationToOrigin = MatrixTranslate(-center.x, -center.y, -center.z);
    Matrix rotationMatrix = MatrixRotateXYZ(rotation);
    Matrix scaleMatrix = MatrixScale(scale.x, scale.y, scale.z);
    Matrix translationBack = MatrixTranslate(center.x, center.y, center.z);
    Matrix translationMatrix = MatrixTranslate(translation.x, translation.y, translation.z);

    Matrix transformMatrix = MatrixMultiply(scaleMatrix, rotationMatrix);
    transformMatrix = MatrixMultiply(transformMatrix, translationToOrigin);
    transformMatrix = MatrixMultiply(transformMatrix, translationBack);
    transformMatrix = MatrixMultiply(transformMatrix, translationMatrix);

    return transformMatrix;
}

void se::get_transform_components(const Matrix &matrix, Vector3 *translation, Vector3 *rotation, Vector3 *scale, const Vector3 &center) {
    *translation = {matrix.m12, matrix.m13, matrix.m14};
    Vector3 xAxis = {matrix.m0, matrix.m1, matrix.m2};
    Vector3 yAxis = {matrix.m4, matrix.m5, matrix.m6};
    Vector3 zAxis = {matrix.m8, matrix.m9, matrix.m10};

    scale->x = Vector3Length(xAxis);
    scale->y = Vector3Length(yAxis);
    scale->z = Vector3Length(zAxis);

    xAxis = Vector3Normalize(xAxis);
    yAxis = Vector3Normalize(yAxis);
    zAxis = Vector3Normalize(zAxis);

    Matrix rotationMatrix = { xAxis.x, xAxis.y, xAxis.z, 0.0f,
                              yAxis.x, yAxis.y, yAxis.z, 0.0f,
                              zAxis.x, zAxis.y, zAxis.z, 0.0f,
                              0.0f,    0.0f,    0.0f,    1.0f };

    *rotation = matrix_to_euler(rotationMatrix);
}

b8 se::config_window_get_size(i32 *width, i32 *height)
{
    const json& window_json = se::config_get_value<json>("window");
    if (!window_json.contains("width")) { log_error("Could not find window width in config"); return false; }
    if (!window_json.contains("height")) { log_error("Could not find window height in config"); return false; }
    if (!window_json["width"].is_number_integer()) { log_error("Window width is not an integer"); return false; }
    if (!window_json["height"].is_number_integer()) { log_error("Window height is not an integer"); return false; }
    *width = window_json["width"].get<i32>();
    *height = window_json["height"].get<i32>();
    return true;
}

const std::string se::config_window_get_name(){
    const json& window_json = se::config_get_value<json>("window");
    if (!window_json.contains("name")) { log_error("Could not find window name in config"); return SE_STRING_NONE; }
    if (!window_json["name"].is_string()) { log_error("Window name is not a string"); return SE_STRING_NONE; }
    return window_json["name"].get<std::string>();
}

const i32 se::config_render_get_target_fps(){
    const json& render_json = se::config_get_value<json>("render");
    if (!render_json.contains("target_fps")) { log_error("Could not find target_fps in config"); return 60; }
    if (!render_json["target_fps"].is_number_integer()) { log_error("target_fps is not an integer"); return 60; }
    return render_json["target_fps"].get<i32>();
}

void se::editor_init(){

    struct input_functions_t {
        
        static void move_selected_object(){
            se::camera* camera = se::scene_get_active_camera();
            se::selection_handler* selection_handler = se::editor_get_selection_handler();
            for (se::model* model : selection_handler->selected_models){
                transform_set_location(model->data.transform, camera->camera.target);
            }
        }

        static void rotate_selected_object(const Vector3& delta){
            se::camera* camera = se::scene_get_active_camera();
            se::selection_handler* selection_handler = se::editor_get_selection_handler();
            for (se::model* model : selection_handler->selected_models){
                Vector3 modelCenter = transform_get_location(model->data.transform);
                Matrix translationToOrigin = MatrixTranslate(-modelCenter.x, -modelCenter.y, -modelCenter.z);
                Matrix translationBack = MatrixTranslate(modelCenter.x, modelCenter.y, modelCenter.z);
                Matrix rotation = MatrixRotateXYZ({ delta.z, delta.x, delta.y });
                Matrix transform = MatrixMultiply(translationToOrigin, rotation);
                transform = MatrixMultiply(transform, translationBack);
                model->data.transform = MatrixMultiply(model->data.transform, transform);
            }
        }   

        static void move_camera_right(){
            se::camera* camera = se::scene_get_active_camera();
            Vector3 delta = se::camera_move(camera, {1., 0.f, 0.f});
            move_selected_object();
        }

        static void move_camera_left(){
            se::camera* camera = se::scene_get_active_camera();
            Vector3 delta = se::camera_move(camera, {-1.f, 0.f, 0.f});
            move_selected_object();
        }

        static void move_camera_forward(){
            se::camera* camera = se::scene_get_active_camera();
            Vector3 delta = se::camera_move(camera, {0.f, 0.f, 1.f});
            move_selected_object();
        }

        static void move_camera_backward(){
            se::camera* camera = se::scene_get_active_camera();
            Vector3 delta = se::camera_move(camera, {0.f, 0.f, -1.f});
            move_selected_object();
        }

        static void move_camera_up(){
            se::camera* camera = se::scene_get_active_camera();
            Vector3 delta = se::camera_move(camera, {0.f, 1.f, 0.f});
            move_selected_object();
        }

        static void move_camera_down(){
            se::camera* camera = se::scene_get_active_camera();
            Vector3 delta = se::camera_move(camera, {0.f, -1.f, 0.f});
            move_selected_object();
        }

        static void move_camera_rotate_right(){
            se::camera* camera = se::scene_get_active_camera();
            Vector3 delta = se::camera_rotate(camera, {1.f, 0.f, 0.f}, se::editor_is_selecting(), !se::editor_is_selecting());
            rotate_selected_object(delta);
        }

        static void move_camera_rotate_left(){
            se::camera* camera = se::scene_get_active_camera();
            Vector3 delta = se::camera_rotate(camera, {-1.f, 0.f, 0.f}, se::editor_is_selecting(), !se::editor_is_selecting());
            rotate_selected_object(delta);
        }

        static void move_camera_rotate_up(){
            se::camera* camera = se::scene_get_active_camera();
            Vector3 delta = se::camera_rotate(camera, {0.f, -1.f, 0.f}, se::editor_is_selecting(), !se::editor_is_selecting());
            rotate_selected_object(delta);
        }

        static void move_camera_rotate_down(){
            se::camera* camera = se::scene_get_active_camera();
            Vector3 delta = se::camera_rotate(camera, {0.f, 1.f, 0.f}, se::editor_is_selecting(), !se::editor_is_selecting());
            rotate_selected_object(delta);
        }

        static void zoom_camera_in(){
            se::camera* camera = se::scene_get_active_camera();
            Vector3 cached_target = camera->camera.target;
            Vector3 delta = se::camera_move(camera, {0.f, 0.f, 1.f});
            camera->camera.target = cached_target;
        }

        static void zoom_camera_out(){
            se::camera* camera = se::scene_get_active_camera();
            Vector3 cached_target = camera->camera.target;
            Vector3 delta = se::camera_move(camera, {0.f, 0.f, -1.f});
            camera->camera.target = cached_target;
        }

        static void select_object_mouse(){
            log_info("Selecting object with mouse / touch");
            se::editor_select({(f32)GetMouseX(), (f32)GetMouseY()});            
        }

        static void select_object_center(){
            if (se::editor_is_selecting()) {
                log_info("Deselecting objects");
                deselect_objects();
            }
            else{
                log_info("Selecting object using the center of the screen");
                se::editor_select({(f32)GetScreenWidth() / 2.f, (f32)GetScreenHeight() / 2.f});
            }
        }

        static void deselect_objects(){
            log_info("Deselecting objects");
            se::selection_handler* selection_handler = se::editor_get_selection_handler();
            selection_handler->selected_models.clear();
        }

        static void save_active_scene(){
            log_info("Saving scene");
            se::scene* scene = se::scene_get_active();
            se::scene_save(scene);
        }

        static void reload_scene(){
            log_info("Reloading scene");
            se::scene* scene = se::scene_get_active();
            se::scene_load(scene, scene->path);
        }
    } input_functions;
    // input
    
    se::input_add_map({{{KEY_D, DOWN}}, {},                                     input_functions.move_camera_right});
    se::input_add_map({{{KEY_Z, DOWN}}, {},                                     input_functions.move_camera_left});
    se::input_add_map({{{KEY_A, DOWN}}, {},                                     input_functions.move_camera_left});
    se::input_add_map({{{KEY_W, DOWN}}, {},                                     input_functions.move_camera_forward});
    se::input_add_map({{{KEY_S, DOWN}, {KEY_LEFT_CONTROL, UP}}, {},             input_functions.move_camera_backward});
    se::input_add_map({{{KEY_E, DOWN}}, {},                                     input_functions.move_camera_up});
    se::input_add_map({{{KEY_Q, DOWN}}, {},                                     input_functions.move_camera_down});
    se::input_add_map({{{KEY_RIGHT, DOWN}}, {},                                 input_functions.move_camera_rotate_right});
    se::input_add_map({{{KEY_LEFT, DOWN}}, {},                                  input_functions.move_camera_rotate_left});
    se::input_add_map({{{KEY_UP, DOWN}}, {},                                    input_functions.move_camera_rotate_up});
    se::input_add_map({{{KEY_DOWN, DOWN}}, {},                                  input_functions.move_camera_rotate_down});
    se::input_add_map({{{KEY_PAGE_UP, DOWN}}, {},                               input_functions.zoom_camera_in});
    se::input_add_map({{{KEY_PAGE_DOWN, DOWN}}, {},                             input_functions.zoom_camera_out});
    se::input_add_map({{{KEY_LEFT_CONTROL, UP}, {KEY_G, PRESSED}}, {},          editor_toggle_show_grid});
    se::input_add_map({{{KEY_LEFT_CONTROL, DOWN}, {KEY_G, PRESSED}}, {},        editor_toggle_show_wireframe});
    se::input_add_map({{{KEY_SPACE, PRESSED}}, {},                              input_functions.select_object_center});
    se::input_add_map({{}, {{MOUSE_BUTTON_LEFT, PRESSED}},                      input_functions.select_object_mouse});
    se::input_add_map({{{KEY_BACKSPACE, PRESSED}}, {},                          input_functions.deselect_objects});
    se::input_add_map({{{KEY_LEFT_CONTROL, DOWN}, {KEY_S, PRESSED}}, {},        input_functions.save_active_scene});
    se::input_add_map({{{KEY_LEFT_CONTROL, DOWN}, {KEY_R, PRESSED}}, {},        input_functions.reload_scene});
}

void se::editor_update(){
    se::editor_hover({(f32)GetScreenWidth() / 2.f, (f32)GetScreenHeight() / 2.f});
}

void se::editor_render_3d(se::camera* camera){
    if (!camera) { log_error("Cannot render editor, camera is NULL"); return; }
    const Color& hover_color = {128, 128, 0, 255};
    const Color& select_color = {255, 0, 0, 255};
    DrawSphereWires(camera->camera.target, 0.02f, 6, 6, hover_color);
    se::editor* editor = se::engine_get_editor();
    if (editor->show_grid){
        DrawGrid(50, 1.f);
    }

    se::selection_handler* selection = se::editor_get_selection_handler();
    for (se::model* model : selection->selected_models){
        model_render_wireframe(model, select_color);
    }
    for (se::model* model : selection->hovered_models){
        model_render_wireframe(model, hover_color);
    }
}

void se::editor_render_2d(){
    DrawText("- Editor Mode - (F1: help)", 12, 10, 30, {200, 0, 0, 255});
    std::vector<std::string> lines = {
        "WASDAE: movement",
        "ARROWS: rotation",
        "PAGE UP/DOWN: zoom",
        "SPACE/LEFT_MOUSE: selection",
        "G: toggle grid | CTRL+G: toggle hover wireframe",
        "CTRL+S: save",
        "CTRL+R: reload",
    };
    for (sz i = 0; i < lines.size(); i++){
        DrawText(lines[i].c_str(), 12, 50 + i*30, 20, {180, 0, 0, 255});
    }
}

void se::editor_toggle_show_grid(){
    se::engine_get_editor()->show_grid = !se::engine_get_editor()->show_grid;
}

void se::editor_toggle_show_wireframe(){
    se::engine_get_editor()->show_wireframe = !se::engine_get_editor()->show_wireframe;
}

se::selection_handler *se::editor_get_selection_handler(){
    return &se::engine_get_editor()->selection;
}

b8 se::editor_is_selecting(){
    return se::editor_get_selection_handler()->selected_models.size() > 0;
}

b8 se::editor_is_hovering(){
    return se::editor_get_selection_handler()->hovered_models.size() > 0;
}

se::model *se::get_model_from_screen(const Vector2 &screen_position){
    se::camera* camera = se::scene_get_active_camera();
    Ray mouse_ray = GetScreenToWorldRay(screen_position, camera->camera);
    se::selection_handler* selection_handler = se::editor_get_selection_handler();
    for (se::model& se_model : se::scene_get_active()->models){
        const Model& model = se_model.data;
        for (i32 i = 0; i < model.meshCount; i++){
            const RayCollision& mouse_ray_collision =  GetRayCollisionMesh(mouse_ray, model.meshes[i], model.transform);
            if (mouse_ray_collision.hit){
                return &se_model;
            }
        }
    }
    return nullptr;
}

b8 se::editor_hover(const Vector2 &screen_position){
   se::model* hovered_model = get_model_from_screen(screen_position);
    se::editor_get_selection_handler()->hovered_models.clear();
   if (hovered_model){
       se::editor_get_selection_handler()->hovered_models.push_back(hovered_model);
       return true;
   }
    return false;
}

b8 se::editor_select(const Vector2 &screen_position){
    se::model* selected_model = get_model_from_screen(screen_position);
    se::selection_handler* selection_handler = se::editor_get_selection_handler();
    selection_handler->selected_models.clear();
    if (selected_model){
        if (selection_handler->selected_models.size() > 0){
            selection_handler->selected_models.clear();
        }
        selection_handler->selected_models.push_back(selected_model);
        scene_get_active_camera()->camera.target = se::transform_get_location(selected_model->data.transform);
        return true;
    }
    return false;
}
