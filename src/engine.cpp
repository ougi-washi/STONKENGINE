// Abstract City - Jed Fakhfekh - https://github.com/ougi-washi
#include "engine.h"
#include "array.h"
#include "config.h"
#include "log.h"
#include "utility.h"
#include "raymath.h"
#include "defines.h"
#include "rlgl.h"

ac::engine* engine_instance = NULL;

void ac::engine_start(){
    log_info("Starting engine");
    engine_instance = new engine();
    // load config
    i32 window_width = 0;
    i32 window_height = 0;
    ac::config_window_get_size(&window_width, &window_height);
    const std::string& window_name = ac::config_window_get_name();
    const i32 target_fps = ac::config_render_get_target_fps();
    // init raylib
    InitWindow(window_width, window_height, window_name.c_str());
    // SetExitKey(KEY_NULL);
    // SetConfigFlags(FLAG_VSYNC_HINT);
	SetConfigFlags(FLAG_MSAA_4X_HINT);
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    SetTargetFPS(target_fps);
    rlClearColor(0, 0, 0, 255);
    rlEnableDepthTest();
	rlEnableColorBlend();
	rlSetBlendMode(RL_BLEND_ALPHA);
	rlEnableBackfaceCulling();
#ifdef AC_EDITOR_MODE
    editor_init();
#endif // AC_EDITOR_MODE
}

const b8 ac::engine_should_loop(){

    return !WindowShouldClose();
}

void ac::engine_end(){
    log_info("Clearing engine");
    CloseWindow();
    engine* engine = ac::engine_get_instance();
    std::vector<ac::model>* models_pool = ac::engine_get_models_pool();
    if (models_pool){
        for (ac::model& model : *models_pool){
            for (i32 i = 0; i < model.data.materialCount; i++){
                UnloadShader(model.data.materials[i].shader);
            }
            UnloadModel(model.data); 
        }
        models_pool->clear();
    }
    for (ac::scene& scene : engine->scenes){
        scene.models.clear();
        scene.cameras.clear();
    }
    for (ac::scene_2d& scene_2d : engine->scenes_2d){
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

ac::engine *ac::engine_get_instance(){
    if (!engine_instance) { log_error("Engine instance is NULL"); return NULL; }
    return engine_instance;
}

void ac::engine_render(){
    ac::scene* scene = ac::scene_get_active();
    ac::scene_render(scene);
    ac::scene_2d* scene_2d = ac::scene_2d_get_active();
    ac::scene_2d_render(scene_2d);

    command_queue* render_queue = ac::engine_get_render_queue();
    BeginDrawing();
    {
        while(!render_queue->empty()){
            command render_command = ac::engine_get_instance()->render_queue.front();
            if (render_command) { 
                render_command(); 
            }
            ac::engine_get_instance()->render_queue.pop();
        }
    }
    EndDrawing();
}

std::vector<ac::model>* ac::engine_get_models_pool(){
    return &ac::engine_get_instance()->models_pool;
}

void ac::engine_process_input(){
    ac::engine* engine = ac::engine_get_instance();
    ac::input_process();
}

const f32 ac::engine_get_delta_time(){
    return GetFrameTime();
}

ac::command_queue* ac::engine_get_render_queue(){
    ac::engine* engine = ac::engine_get_instance();
    return &engine->render_queue;
}

void ac::engine_enqueue_render_command(const ac::command &render_command){
    ac::engine_get_render_queue()->emplace(render_command);
}

void ac::input_add_map(const ac::input_map &input_map){
    ac::engine* engine = ac::engine_get_instance();
    ac::input_map* input = ac::push_back(&engine->input.input_maps);
    if(input) {
        *input = input_map;
    }
    else { log_error("Failed to add input map"); }
}

void ac::input_process(){
    ac::engine* engine = ac::engine_get_instance();
    for (ac::input_map& input_map : engine->input.input_maps){
        if (input_map.keyboard_keys.empty() && input_map.mouse_buttons.empty()) { 
            log_error("Cannot process input, no keys or buttons found"); continue; 
        }  
        b8 valid_keyboard_keys = true;
        for (const i32& key : input_map.keyboard_keys){
            if (!IsKeyDown(key)){
                valid_keyboard_keys = false;
            }
        }
        b8 valid_mouse_buttons = true;
        for (const i32& button : input_map.mouse_buttons){
            if (!IsMouseButtonDown(button)){
                valid_mouse_buttons = false;
            }
        }
        if (valid_keyboard_keys && valid_mouse_buttons){
            input_map.callback();
        }
    }
}

b8 ac::shader_load(Shader &shader, const std::string &vertex, const std::string &fragment){
    const std::string& shaders_path = ac::config_get_root_path() + ac::config_get_shaders_path();
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

i32 ac::shader_set_float(Shader* shader, const f32 value, const std::string& uniform_name) {
    if (!shader) { log_error("Shader is NULL"); return -1; }
    int location = GetShaderLocation(*shader, uniform_name.c_str());
    if (location == -1) { log_error("Uniform location not found"); return -1; }
    SetShaderValue(*shader, location, &value, SHADER_UNIFORM_FLOAT);
    return location;
}

i32 ac::shader_set_int(Shader* shader, const i32 value, const std::string& uniform_name) {
    if (!shader) { log_error("Shader is NULL"); return -1; }
    int location = GetShaderLocation(*shader, uniform_name.c_str());
    if (location == -1) { log_error("Uniform location not found"); return -1; }
    SetShaderValue(*shader, location, &value, SHADER_UNIFORM_INT);
    return location;
}

i32 ac::shader_set_vector3f(Shader* shader, const Vector3& value, const std::string& uniform_name) {
    if (!shader) { log_error("Shader is NULL"); return -1; }
    int location = GetShaderLocation(*shader, uniform_name.c_str());
    if (location == -1) { log_error("Uniform location not found"); return -1; }
    SetShaderValue(*shader, location, &value, SHADER_UNIFORM_VEC3);
    return location;
}

i32 ac::shader_set_vector3i(Shader* shader, const Vector3& value, const std::string& uniform_name) {
    if (!shader) { log_error("Shader is NULL"); return -1; }
    int location = GetShaderLocation(*shader, uniform_name.c_str());
    if (location == -1) { log_error("Uniform location not found"); return -1; }
    SetShaderValue(*shader, location, &value, SHADER_UNIFORM_IVEC3);
    return location;
}

i32 ac::shader_set_matrix(Shader* shader, const Matrix& value, const std::string& uniform_name) {
    if (!shader) { log_error("Shader is NULL"); return -1; }
    int location = GetShaderLocation(*shader, uniform_name.c_str());
    if (location == -1) { log_error("Uniform location not found"); return -1; }
    SetShaderValueMatrix(*shader, location, value);
    return location;
}

i32 ac::shader_set_texture(Shader* shader, Texture2D& texture, const std::string& uniform_name) {
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
    ac::material_load(material, vertex_path, fragment_path);
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
                ac::material_set_float(&material, value, uniform_name);
            }
            else if (uniform_type == "int"){
                if (!uniform.contains("value")) { log_error("Could not load material, uniform value not found"); continue; }
                const i32 value = uniform["value"];
                ac::material_set_int(&material, value, uniform_name);
            }
            else if (uniform_type == "vec3f"){
                if (!uniform.contains("value")) { log_error("Could not load material, uniform value not found"); continue; }
                const Vector3& value = {uniform["value"][0], uniform["value"][1], uniform["value"][2]};
                ac::material_set_vector3f(&material, value, uniform_name);
            }
            else if (uniform_type == "vec3i"){
                if (!uniform.contains("value")) { log_error("Could not load material, uniform value not found"); continue; }
                const Vector3& value = {uniform["value"][0], uniform["value"][1], uniform["value"][2]};
                ac::material_set_vector3i(&material, value, uniform_name);
            }
            else if (uniform_type == "matrix"){
                if (!uniform.contains("value")) { log_error("Could not load material, uniform value not found"); continue; }
                const Matrix& value = {
                    uniform["value"][0], uniform["value"][1], uniform["value"][2], uniform["value"][3],
                    uniform["value"][4], uniform["value"][5], uniform["value"][6], uniform["value"][7],
                    uniform["value"][8], uniform["value"][9], uniform["value"][10], uniform["value"][11],
                    uniform["value"][12], uniform["value"][13], uniform["value"][14], uniform["value"][15]
                };
                ac::material_set_matrix(&material, value, uniform_name);
            }
            else if (uniform_type == "texture"){
                if (!uniform.contains("value")) { log_error("Could not load material, uniform value not found"); continue; }
                const std::string& texture_path = uniform["value"];
                const Texture2D& texture = LoadTexture(texture_path.c_str());
                ac::material_set_texture(&material, texture, uniform_name);
            }
        }
    }
}

ac::scene *ac::scene_make_new(){
    ac::engine* engine = ac::engine_get_instance();
    return ac::push_back(&engine->scenes);
}

void ac::scene_load(ac::scene *scene, const std::string &path)
{
    if(!scene) { log_error("Cannot load scene, scene is NULL"); return; }
    const std::string& file_content = ac::read_file(ac::config_get_root_path() + ac::config_get_scenes_path() + path);
    if (file_content.empty()) { log_error("Could not load scene, file is empty"); return; }
    const json& scene_json = json::parse(file_content);
    if (scene_json.empty()) { log_error("Could not load scene, json is empty"); return; }

    if (!scene_json.contains("objects")) { log_error("Could not load scene, objects not found"); return; }
    const std::vector<json>& objects = scene_json["objects"];

    for (const json& object : objects){
        if (!object.contains("type")) { log_error("Could not load scene, object type not found"); continue; }
        const std::string& type = object["type"];
        if (type == "model"){
            ac::model* model = ac::model_load(object);
            if (!model) { log_error("Could not load scene, model is NULL"); continue; }
            ac::scene_add_model(scene, model);
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
            const f32 speed = object.contains("speed") ? (f32)object["speed"] : 1.0f;            

            ac::scene_add_camera(scene, {
                position,
                target,
                up,
                fovy,
                CAMERA_PERSPECTIVE
            }, speed, is_active);
        }
    }
}

void ac::scene_add_model(ac::scene *scene, ac::model* model){
    // edit json instead of adding model directly
    if(!scene) { log_error("Scene is NULL"); return; }
    if(!model) { log_error("Model is NULL"); return; }
    ac::model* scene_model = ac::push_back(&scene->models);
    if(scene_model) {
        *scene_model = *model;
    }
    else { log_error("Failed to add model to scene"); }
}

void ac::scene_add_camera(ac::scene *scene, Camera camera, const f32 speed, const b8 is_active){
    // edit json instead of adding camera directly
    if(!scene) { log_error("Scene is NULL"); return; }
    ac::camera* scene_camera = ac::push_back(&scene->cameras);
    if(scene_camera) {
        scene_camera->camera = camera;
        scene_camera->speed = speed;
        ac::camera_set_active(scene_camera, is_active);
    }
    else { log_error("Failed to add camera to scene"); }
}

ac::camera *ac::scene_make_new_camera(ac::scene *scene){
    // edit json instead of adding camera directly
    if(!scene) { log_error("Scene is NULL"); return NULL; }
    ac::camera* scene_camera = ac::push_back(&scene->cameras);
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

void ac::scene_render(ac::scene *scene){
    if(!scene) { log_error("Cannot render, scene is NULL"); return; }
    ac::camera* camera = ac::scene_get_active_camera(scene);
    if (!camera) { log_error("Cannot render, camera is NULL"); return; }

    engine_enqueue_render_command([scene = scene, camera = camera](){
        ClearBackground(BLACK);
        BeginMode3D(camera->camera);
        for (ac::model& model : scene->models){
            ac::model_render(&model);
        }
#ifdef AC_EDITOR_MODE
        editor_render_3d(camera);
#endif //AC_EDITOR_MODE
        EndMode3D();
    });
}

ac::scene *ac::scene_get_active(){
    // currently only one scene is supported
    ac::scene* scene = &ac::engine_get_instance()->scenes[0];
    return scene;
}

ac::camera *ac::scene_get_active_camera(ac::scene *scene){
    if(!scene) { log_error("Cannot get active camera, scene is NULL"); return NULL; }
    for (ac::camera& camera : scene->cameras){
        if(camera.is_active) { return &camera; }
    }
    return NULL;
}

ac::camera *ac::scene_get_active_camera(){
    ac::scene* scene = ac::scene_get_active();
    return ac::scene_get_active_camera(scene);
}

ac::scene_2d *ac::scene_2d_make_new(){
    ac::engine* engine = ac::engine_get_instance();
    return ac::push_back(&engine->scenes_2d);
}

void ac::scene_2d_render(scene_2d *scene){
    if(!scene) { log_error("Cannot render, scene is NULL"); return; }

    for (sz i = 0 ; i < scene->objects.size(); ++i){
        object_2d& object = scene->objects[i];
        RenderTexture2D& render_texture = scene->render_textures[i];
        BeginTextureMode(render_texture);
        {
            ClearBackground({0, 0, 0, 255});
            shader_load(object.shader, object.vertex, object.fragment);
            for (sz i = 0; i < object.textures.size(); ++i) {
                shader_set_texture(&object.shader, object.textures[i], "texture" + std::to_string(i));
            }
            BeginShaderMode(object.shader);
            {
                // rlBegin(RL_QUADS);
                // {
                //     // Bottom-left
                //     rlTexCoord2f(0.0f, 0.0f);
                //     rlVertex2f(-1.0f, -1.0f);
                    
                //     // Bottom-right
                //     rlTexCoord2f(1.0f, 0.0f);
                //     rlVertex2f(1.0f, -1.0f);
                    
                //     // Top-right
                //     rlTexCoord2f(1.0f, 1.0f);
                //     rlVertex2f(1.0f, 1.0f);
                    
                //     // Top-left
                //     rlTexCoord2f(0.0f, 1.0f);
                //     rlVertex2f(-1.0f, 1.0f);
                // }
                // rlEnd();
                DrawCircle(100, 100, 50, RED);
                // DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), WHITE);
                // DrawRectangleGradientEx(
                //     {0, 0, (f32)GetScreenWidth(), (f32)GetScreenHeight()}, 
                //     {0, 0, 0, 255}, {255, 0, 0, 255}, {0, 255, 0, 255}, {255, 255, 0, 255});
            }
            EndShaderMode();
            for (const text& text : object.texts){
                DrawTextEx(text.font, text.string.c_str(), { text.position.x, text.position.y }, text.fontSize, text.spacing, text.tint);
            }
        }
        EndTextureMode();
    }
    
    engine_enqueue_render_command([scene = scene](){
        for (sz i = 0 ; i < scene->render_textures.size(); ++i){
            RenderTexture2D& render_texture = scene->render_textures[i];
            DrawTextureRec(render_texture.texture, { 0, 0, (f32)render_texture.texture.width, -(f32)render_texture.texture.height }, { 0, 0 }, { 255, 255, 255, 100 });
        }
#ifdef AC_EDITOR_MODE
            editor_render_2d();
#endif //AC_EDITOR_MODE
    });
}

void ac::scene_2d_load(scene_2d *scene, const std::string &path){
    if(!scene) { log_error("Cannot load scene, scene is NULL"); return; }
    const std::string& file_content = ac::read_file(ac::config_get_root_path() + ac::config_get_scenes_path() + path);
    if (file_content.empty()) { log_error("Could not load scene, file is empty"); return; }
    const json& scene_json = json::parse(file_content);
    if (scene_json.empty()) { log_error("Could not load scene, json is empty"); return; }

    if (!scene_json.contains("objects")) { log_error("Could not load scene, objects not found"); return; }
    const std::vector<json>& objects = scene_json["objects"];

    for (const json& object : objects){
        ac::object_2d* object_2d = ac::push_back(&scene->objects);
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
                ac::text* scene_text = ac::push_back(&object_2d->texts);
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

ac::scene_2d *ac::scene_2d_get_active(){
    // currently only one scene is supported
    ac::scene_2d* scene = &ac::engine_get_instance()->scenes_2d[0];
    return scene;
}

void ac::scene_add_object_2d(ac::scene_2d *scene, ac::object_2d *object){
    if(!scene) { log_error("Cannot add object, scene is NULL"); return; }
    if(!object) { log_error("Cannot add object, object is NULL"); return; }
    ac::object_2d* scene_object = ac::push_back(&scene->objects);
    if(scene_object) {
        *scene_object = *object;
    }
    else { log_error("Failed to add object to scene"); }
}

ac::object_2d *ac::scene_2d_make_new_object(ac::scene_2d *scene){
    if(!scene) { log_error("Cannot make new object, scene is NULL"); return nullptr; }
    ac::object_2d* object = ac::push_back(&scene->objects);
    scene->render_textures.push_back(LoadRenderTexture(GetScreenWidth(), GetScreenHeight()));
    if(object) {
        return object;
    }
    log_error("Failed to add object to scene"); 
    return nullptr;
}

ac::model* ac::model_load(const json &model_json){
    ac::model* model = nullptr;
    if (!model_json.contains("path")) { log_error("Could not load scene, model path not found"); return nullptr; }
    const std::string& object_path = model_json["path"];
    model = ac::model_load(object_path);
    if (!model) { log_error("Could not load scene, model is NULL"); return nullptr; }
    if (model_json.contains("transform")){
        const json& transform = model_json["transform"];
        if (transform.contains("translation")){
            const Vector3& translation = {transform["translation"][0], transform["translation"][1], transform["translation"][2]};
            ac::model_set_location(model, translation);
        }
        if (transform.contains("rotation")){
            const Vector3& rotation = {transform["rotation"][0], transform["rotation"][1], transform["rotation"][2]};
            ac::model_set_rotation(model, rotation);
        }
        if (transform.contains("scale")){
            const Vector3& scale = {transform["scale"][0], transform["scale"][1], transform["scale"][2]};
            ac::model_set_scale(model, scale);
        }
    }
    if (model_json.contains("materials")){
        const std::vector<json>& materials = model_json["materials"];
        // model->data.materialCount = materials.size();
        // model->data.materials = (Material*)RL_MALLOC(model->data.materialCount*sizeof(Material));
        for (i32 i = 0; i < materials.size(); i++){
            const json& material_json = materials[i];
            Material& material = model->data.materials[i];
            if (!material_json.contains("vertex") || !material_json.contains("fragment")) { 
                log_error("Could not load scene, material vertex or fragment not found"); continue; 
            }
            model->data.materialCount++;
            model->data.materials = (Material*)RL_REALLOC(model->data.materials, model->data.materialCount*sizeof(Material));
            material_load(model->data.materials[i], material_json);
            ac::model_set_material(model, &material, i);
        }
    }
    return model;
}

ac::model *ac::model_load(const std::string &path){
    const std::string& model_path = ac::config_get_root_path() + ac::config_get_models_path() + path;
    ac::engine* engine = ac::engine_get_instance();
    ac::model* model = nullptr;
    for (ac::model& current_model : *ac::engine_get_models_pool()){
        if (current_model.path == model_path && current_model.data.meshCount > 0){
            model = &current_model;
        }
    }
    if (!model){
        log_info("Model %s not in pool, loading model", model_path.c_str());
        model = ac::push_back(ac::engine_get_models_pool());
        if (!model) { log_error("Failed to add model to the pool"); return NULL; }
        model->data = LoadModel(model_path.c_str());
        model->path = model_path;
        model->transform = {{0.0f, 0.0f, 0.0f}, QuaternionIdentity(), {1.0f, 1.0f, 1.0f}};
    }
    return model;
}

void ac::model_render(ac::model *model) {
    if(!model) { log_error("Cannot render, model is NULL"); return; }
    for (i32 i = 0; i < model->data.meshCount; i++){
        const Matrix translation_matrix = MatrixTranslate(model->transform.translation.x, model->transform.translation.y, model->transform.translation.z);
        const Matrix rotation_matrix = MatrixRotateXYZ(QuaternionToEuler(model->transform.rotation));
        const Matrix scale_matrix = MatrixScale(model->transform.scale.x, model->transform.scale.y, model->transform.scale.z);
        const Matrix model_matrix = MatrixMultiply(MatrixMultiply(translation_matrix, rotation_matrix), scale_matrix);
        if (model->data.meshCount != model->data.materialCount) { log_error("Model mesh count does not match material count"); continue; }
        DrawMesh(model->data.meshes[i], model->data.materials[i], model_matrix);
    }
}

void ac::model_render_wireframe(ac::model *model){
    if(!model) { log_error("Cannot render wireframe, model is NULL"); return; }
    for (i32 i = 0; i < model->data.meshCount; i++){
        rlEnableWireMode();
        ac::model_render(model);
        rlDisableWireMode();
    }
}

void ac::model_render_instances(ac::model *model, Matrix *transforms, const i32 count){
    if(!model) { log_error("Cannot render instances, model is NULL"); return; }
    for (i32 i = 0; i < model->data.meshCount; i++){
        DrawMeshInstanced(model->data.meshes[i], model->data.materials[model->data.meshMaterial[i]], transforms, count);
    }
}

void ac::model_set_location(model *model, const Vector3& location) {
    if(!model) { log_error("Cannot set location, model is NULL"); return; }
    model->transform.translation = location;
}

void ac::model_set_rotation(model *model, const Vector3& rotation) {
    if(!model) { log_error("Cannot set rotation, model is NULL"); return; }
    model->transform.rotation = QuaternionFromEuler(rotation.x, rotation.y, rotation.z);
}

void ac::model_set_scale(model *model, const Vector3& scale) {
    if(!model) { log_error("Cannot set scale, model is NULL"); return; }
    model->transform.scale = scale;
}

void ac::model_set_material(model *model, Material *material, const i32 mesh_index){
    if(!model) { log_error("Cannot set material, model is NULL"); return; }
    if(!material) { log_error("Cannot set material, material is NULL"); return; }
    if (mesh_index < 0 || mesh_index >= model->data.meshCount) { log_error("Cannot set material, mesh index out of bounds"); return; }
    if (mesh_index >= model->data.materialCount) { log_error("Cannot set material, mesh index out of bounds"); return; }
    model->data.materials[mesh_index] = *material;
}

b8 ac::material_load(Material &material, const std::string &vertex, const std::string &fragment){
    return shader_load(material.shader, vertex, fragment);
}

i32 ac::material_set_float(Material *material, const f32 value, const std::string &uniform_name)
{
    const i32 slot = GetShaderLocation(material->shader, uniform_name.c_str());
    SetShaderValue(material->shader, slot, &value, SHADER_UNIFORM_FLOAT);
    return slot;
}

i32 ac::material_set_int(Material *material, const i32 value, const std::string &uniform_name) {
    const i32 slot = GetShaderLocation(material->shader, uniform_name.c_str());
    SetShaderValue(material->shader, slot, &value, SHADER_UNIFORM_INT);
    return slot;
}

i32 ac::material_set_vector3f(Material *material, const Vector3 &value, const std::string &uniform_name){
    const i32 slot = GetShaderLocation(material->shader, uniform_name.c_str());
    SetShaderValueV(material->shader, slot, &value, SHADER_UNIFORM_VEC3, 3);
    return slot;
}

i32 ac::material_set_vector3i(Material *material, const Vector3 &value, const std::string &uniform_name){
    const i32 slot = GetShaderLocation(material->shader, uniform_name.c_str());
    SetShaderValueV(material->shader, slot, &value, SHADER_UNIFORM_IVEC3, 3);
    return slot;
}

i32 ac::material_set_matrix(Material *material, const Matrix &value, const std::string &uniform_name){
    const i32 slot = GetShaderLocation(material->shader, uniform_name.c_str());
    SetShaderValueMatrix(material->shader, slot, value);
    return slot;
}

i32 ac::material_set_texture(Material *material, const Texture2D &texture, const std::string &uniform_name) {
    const i32 slot = GetShaderLocation(material->shader, uniform_name.c_str());
    SetShaderValueTexture(material->shader, slot, texture);
    return slot;
}

void ac::camera_move(ac::camera *camera, const Vector3 &delta){
    if (!camera) { log_error("Cannot move camera, camera is NULL"); return; }
    const Vector3 front = Vector3Normalize(camera->camera.target - camera->camera.position);
    const Vector3 right = Vector3Normalize(Vector3CrossProduct(front, camera->camera.up));
    const Vector3 up = Vector3Normalize(Vector3CrossProduct(right, front));
    const Vector3 front_offset = front * delta.z;
    Vector3 offset = front_offset + Vector3Scale(right, delta.x);
    offset.y = Vector3DotProduct(up, delta);
    offset *= camera->speed;
    camera->camera.position = camera->camera.position + offset;
    camera->camera.target = camera->camera.target + offset;
}

void ac::camera_rotate_around_target(ac::camera *camera, const Vector3 &delta){
    if (!camera) { log_error("Cannot rotate camera, camera is NULL"); return; }
    const Vector3 offset = delta * camera->speed;
    camera->camera.position = Vector3RotateByQuaternion(camera->camera.position, QuaternionFromEuler(offset.x, offset.y, offset.z));
    camera->camera.target = Vector3RotateByQuaternion(camera->camera.target, QuaternionFromEuler(offset.x, offset.y, offset.z));
}

void ac::camera_rotate(ac::camera *camera, const Vector3 &delta){
    if (!camera) { log_error("Cannot rotate camera, camera is NULL"); return; }
    const Vector3 offset = delta * camera->speed;
    camera->camera.position = Vector3RotateByQuaternion(camera->camera.position, QuaternionFromEuler(offset.x, offset.y, offset.z));
    camera->camera.target = Vector3RotateByQuaternion(camera->camera.target, QuaternionFromEuler(offset.x, offset.y, offset.z));
}

void ac::camera_set_position(ac::camera *camera, const Vector3 &position)
{
    if (!camera) { log_error("Cannot set camera position, camera is NULL"); return; }
    camera->camera.position = position;
}

void ac::camera_set_target(ac::camera *camera, const Vector3& target){
    if (!camera) { log_error("Cannot set camera target, camera is NULL"); return; }
    camera->camera.target = target;
}

void ac::camera_set_fovy(ac::camera *camera, const f32 fovy){
    if (!camera) { log_error("Cannot set camera fovy, camera is NULL"); return; }
    camera->camera.fovy = fovy;
}

void ac::camera_set_active(ac::camera *camera, const b8 new_state){
    if (!camera) { log_error("Cannot set camera active state, camera is NULL"); return; }
    ac::scene* scene = ac::scene_get_active();
    for (ac::camera& camera : scene->cameras){
        camera.is_active = false;
    }
    camera->is_active = new_state;
}

void ac::camera_set_position(Camera* camera, const Vector3& position){
    if (!camera) { log_error("Cannot set camera position, camera is NULL"); return; }
    camera->position = position;
}
void ac::camera_set_target(Camera* camera, const Vector3& target){
    if (!camera) { log_error("Cannot set camera target, camera is NULL"); return; }
    camera->target = target;
}
void ac::camera_set_fovy(Camera* camera, const f32 fovy){
    if (!camera) { log_error("Cannot set camera fovy, camera is NULL"); return; }
    camera->fovy = fovy;
}

void ac::transform_set_location(Matrix &transform, const Vector3 &location){
    transform.m12 = location.x;
    transform.m13 = location.y;
    transform.m14 = location.z;
}

void ac::transform_set_rotation(Matrix &transform, const Vector3 &rotation){
    Quaternion quaternion = QuaternionFromEuler(rotation.x, rotation.y, rotation.z);
    transform.m0 = quaternion.x;
    transform.m1 = quaternion.y;
    transform.m2 = quaternion.z;
    transform.m3 = quaternion.w;
}

void ac::transform_set_scale(Matrix &transform, const Vector3 &scale){
    transform.m0 *= scale.x;
    transform.m5 *= scale.y;
    transform.m10 *= scale.z;
}

const Vector3 ac::transform_get_location(const Matrix &transform){
    return {transform.m12, transform.m13, transform.m14};
}

const Vector3 ac::transform_get_rotation(const Matrix &transform){
    Quaternion quaternion = {transform.m0, transform.m1, transform.m2, transform.m3};
    return QuaternionToEuler(quaternion);
}

const Vector3 ac::transform_get_scale(const Matrix &transform){
    return {transform.m0, transform.m5, transform.m10};
}

b8 ac::config_window_get_size(i32 *width, i32 *height)
{
    const json& window_json = ac::config_get_value<json>("window");
    if (!window_json.contains("width")) { log_error("Could not find window width in config"); return false; }
    if (!window_json.contains("height")) { log_error("Could not find window height in config"); return false; }
    if (!window_json["width"].is_number_integer()) { log_error("Window width is not an integer"); return false; }
    if (!window_json["height"].is_number_integer()) { log_error("Window height is not an integer"); return false; }
    *width = window_json["width"].get<i32>();
    *height = window_json["height"].get<i32>();
    return true;
}

const std::string ac::config_window_get_name(){
    const json& window_json = ac::config_get_value<json>("window");
    if (!window_json.contains("name")) { log_error("Could not find window name in config"); return AC_STRING_NONE; }
    if (!window_json["name"].is_string()) { log_error("Window name is not a string"); return AC_STRING_NONE; }
    return window_json["name"].get<std::string>();
}

const i32 ac::config_render_get_target_fps(){
    const json& render_json = ac::config_get_value<json>("render");
    if (!render_json.contains("target_fps")) { log_error("Could not find target_fps in config"); return 60; }
    if (!render_json["target_fps"].is_number_integer()) { log_error("target_fps is not an integer"); return 60; }
    return render_json["target_fps"].get<i32>();
}

void ac::editor_init()
{
    struct input_functions_t {
        static void move_camera_right(){
            ac::camera* camera = ac::scene_get_active_camera();
            ac::camera_move(camera, {1., 0.f, 0.f});
        }

        static void move_camera_left(){
            ac::camera* camera = ac::scene_get_active_camera();
            ac::camera_move(camera, {-1.f, 0.f, 0.f});
        }

        static void move_camera_forward(){
            ac::camera* camera = ac::scene_get_active_camera();
            ac::camera_move(camera, {0.f, 0.f, 1.f});
        }

        static void move_camera_backward(){
            ac::camera* camera = ac::scene_get_active_camera();
            ac::camera_move(camera, {0.f, 0.f, -1.f});
        }

        static void move_camera_up(){
            ac::camera* camera = ac::scene_get_active_camera();
            ac::camera_move(camera, {0.f, 1.f, 0.f});
        }

        static void move_camera_down(){
            ac::camera* camera = ac::scene_get_active_camera();
            ac::camera_move(camera, {0.f, -1.f, 0.f});
        }
        static void select_object(){
            log_info("Selecting object");
            ac::camera* camera = ac::scene_get_active_camera();
        }
    } input_functions;
    // input
    ac::input_add_map({{KEY_D}, {}, input_functions.move_camera_right});
    ac::input_add_map({{KEY_A}, {}, input_functions.move_camera_left});
    ac::input_add_map({{KEY_W}, {}, input_functions.move_camera_forward});
    ac::input_add_map({{KEY_S}, {}, input_functions.move_camera_backward});
    ac::input_add_map({{KEY_E}, {}, input_functions.move_camera_up});
    ac::input_add_map({{KEY_Q}, {}, input_functions.move_camera_down});
}

void ac::editor_render_3d(ac::camera* camera){
    if (!camera) { log_error("Cannot render editor, camera is NULL"); return; }
    const Color& red_color = {255, 0, 0, 255};
    DrawSphereWires(camera->camera.target, 0.02f, 6, 6, red_color);
    DrawGrid(50, 1.f);
    Ray res_ray = GetScreenToWorldRay({(f32)GetScreenWidth() / 2.f, (f32)GetScreenHeight() / 2.f}, camera->camera);
    for (ac::model& ac_model : ac::scene_get_active()->models){
        const Model& model = ac_model.data;
        for (i32 i = 0; i < model.meshCount; i++){
            const RayCollision& ray_collision =  GetRayCollisionMesh(res_ray, model.meshes[i], model.transform);
            if (ray_collision.hit){
                DrawModelWires(model, transform_get_location(model.transform), 1.01f, red_color);
            }
        }
    }
}

void ac::editor_render_2d(){
    DrawText(" - Editor Mode -", 10, 10, 20, {255, 0, 0, 255});
}
