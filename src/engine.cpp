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
    SetConfigFlags(FLAG_VSYNC_HINT);
	SetConfigFlags(FLAG_MSAA_4X_HINT);
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    SetTargetFPS(target_fps);
    rlClearColor(0, 0, 0, 255);
    rlEnableDepthTest();
	rlEnableColorBlend();
	rlSetBlendMode(RL_BLEND_ALPHA);
	rlEnableBackfaceCulling();
}

b8 ac::engine_should_loop(){
    // Check if the escape key is pressed to exit the program (TODO: fix this)
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
    delete engine_instance;
}

ac::engine *ac::engine_get_instance(){
    if (!engine_instance) { log_error("Engine instance is NULL"); return NULL; }
    return engine_instance;
}

std::vector<ac::model>* ac::engine_get_models_pool(){
    return &ac::engine_get_instance()->models_pool;
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

void ac::scene_load(ac::scene* scene, const std::string &path){
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
            f32 fovy = object.contains("fovy") ? (f32)object["fovy"] : 45.0f;
            b8 is_active = object.contains("is_active") ? (b8)object["is_active"] : false;

            ac::scene_add_camera(scene, {
                position,
                target,
                up,
                fovy,
                CAMERA_PERSPECTIVE
            }, is_active);
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

void ac::scene_add_camera(ac::scene *scene, Camera camera, const b8 is_active){
    // edit json instead of adding camera directly
    if(!scene) { log_error("Scene is NULL"); return; }
    ac::camera* scene_camera = ac::push_back(&scene->cameras);
    if(scene_camera) {
        scene_camera->camera = camera;
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
    Camera* camera = ac::scene_get_active_camera(scene);
    if (!camera) { log_error("Cannot render, camera is NULL"); return; }

    BeginDrawing();
    {
        ClearBackground(BLACK);
        BeginMode3D(*camera);
        for (ac::model& model : scene->models){
            ac::model_render(&model);
        }
        DrawGrid(10, 1.0f);
        EndMode3D();
    }
    EndDrawing();
}

ac::scene *ac::scene_get_active(){
    // currently only one scene is supported
    ac::scene* scene = &ac::engine_get_instance()->scenes[0];
    return scene;
}

Camera *ac::scene_get_active_camera(ac::scene *scene){
    if(!scene) { log_error("Cannot get active camera, scene is NULL"); return NULL; }
    for (ac::camera& camera : scene->cameras){
        if(camera.is_active) { return &camera.camera; }
    }
    return NULL;
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

// Material *ac::material_load(const std::string &vertex, const std::string &fragment) {
//     const std::string& shaders_path = ac::config_get_root_path() + ac::config_get_shaders_path();
//     const std::string& vertex_path = shaders_path + vertex;
//     const std::string& fragment_path = shaders_path + fragment;
//     // ac::material_loaded* material = ac::push_back(ac::engine_get_materials_pool());
//     if(!material) { log_error("Failed to add material to the pool"); return nullptr; }

//     material->material = LoadMaterialDefault();
//     if (IsFileExtension(vertex_path.c_str(), ".glsl") && IsFileExtension(fragment_path.c_str(), ".glsl")){
//         Shader shader = LoadShader(vertex_path.c_str(), fragment_path.c_str());
//         material->material.shader = shader;
//         material->vertex = vertex_path;
//         material->fragment = fragment_path;
//     }
//     return &material->material;
// }

void ac::material_load(Material &material, const std::string &vertex, const std::string &fragment){
    const std::string& shaders_path = ac::config_get_root_path() + ac::config_get_shaders_path();
    const std::string& vertex_path = shaders_path + vertex;
    const std::string& fragment_path = shaders_path + fragment;
    if (IsFileExtension(vertex_path.c_str(), ".glsl") && IsFileExtension(fragment_path.c_str(), ".glsl")){
        Shader shader = LoadShader(vertex_path.c_str(), fragment_path.c_str());
        material.shader = shader;
    }
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

void ac::camera_set_position(ac::camera *camera, const Vector3& position){
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

b8 ac::config_window_get_size(i32 *width, i32 *height){
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
