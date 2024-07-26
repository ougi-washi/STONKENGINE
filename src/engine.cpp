// Abstract City - Jed Fakhfekh - https://github.com/ougi-washi
#include "engine.h"
#include "config.h"
#include "log.h"
#include "raymath.h"
#include "defines.h"
#include "rlgl.h"

ac::engine* engine_instance = NULL;

void ac::engine_start(){
    log_info("Starting engine");
    engine_instance = new engine();
    ac::config_load();
    i32 window_width = 0;
    i32 window_height = 0;
    ac::config_window_get_size(&window_width, &window_height);
    const c8* window_name = ac::config_window_get_name();
    const i32 target_fps = ac::json_get_int(&engine->config, "target_fps");
    InitWindow(window_width, window_height, window_name);
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    SetTargetFPS(target_fps);

    log_info("Initializing models pool");
    ac::model_pool_init(&engine->model_pool);
}

b8 ac::engine_should_loop(){
    return !WindowShouldClose();
}

void ac::engine_end(){
    log_info("Clearing engine");
    CloseWindow();
    ac::model_pool_clear(&engine->model_pool);
    ac::config_unload();
    delete engine_instance;
}

ac::engine *ac::engine_get_instance()
{
    if (!engine_instance) { log_error("Engine instance is NULL"); return NULL; }
    return engine_instance;
}

Model* ac::engine_get_model(const c8 *path){
    for (i32 i = 0; i < ac::model_pool_size(&engine->model_pool); i++){
        ac::model_loaded* model_loaded = ac::model_pool_get(&engine->model_pool, i);
        if (model_loaded){
            if (strcmp(model_loaded->path, path) == 0){
                return &model_loaded->model;
            }
        }
        else{
            log_info("Model not found, loading model");
            ac::model_loaded* model_loaded = ac::model_pool_increment(&engine->model_pool);
            if (!model_loaded) { log_error("Failed to add model to the loop"); return NULL; }
            model_loaded->model = LoadModel(path);
            strcpy(model_loaded->path, path);
            return &model_loaded->model; 
        }
    }
    return NULL;
}

void ac::scene_init(ac::scene *scene){
    if(!scene) { log_error("Scene is NULL"); return; }
    ac::model_array_init(&scene->models);
    ac::camera_array_init(&scene->cameras);
}

void ac::scene_add_model(ac::scene *scene, Model* model){
    if(!scene) { log_error("Scene is NULL"); return; }
    if(!model) { log_error("Model is NULL"); return; }
    ac::model* scene_model = ac::model_array_increment(&scene->models);
    if(scene_model) {
        scene_model->model = model; 
        scene_model->transform = {0};
    }
    else { log_error("Failed to add model to scene"); }
}

void ac::scene_add_camera(ac::scene *scene, Camera camera, const b8 is_active){
    if(!scene) { log_error("Scene is NULL"); return; }
    ac::camera* scene_camera = ac::camera_array_increment(&scene->cameras);
    if(scene_camera) {
        scene_camera->camera = camera;
        ac::camera_set_active(scene_camera, is_active);
    }
    else { log_error("Failed to add camera to scene"); }
}

ac::camera *ac::scene_make_new_camera(ac::scene *scene)
{
    if(!scene) { log_error("Scene is NULL"); return NULL; }
    ac::camera* scene_camera = ac::camera_array_increment(&scene->cameras);
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
        DrawCube({ 0.0f, 0.0f, 0.0f }, 2.0f, 2.0f, 2.0f, RED);
        DrawGrid(10, 1.0f);
        EndMode3D();
    }
    EndDrawing();
}

Camera *ac::scene_get_active_camera(ac::scene *scene){
    if(!scene) { log_error("Cannot get active camera, scene is NULL"); return NULL; }
    for (i32 i = 0; i < ac::camera_array_size(&scene->cameras); i++){
        ac::camera* camera = ac::camera_array_get(&scene->cameras, i);
        if(camera && camera->is_active) { return &camera->camera; }
    }
    return NULL;
}

void ac::model_render(ac::model *model){
    if(!model) { log_error("Cannot render, model is NULL"); return; }
    if(!model->model) { log_error("Cannot render, rlmodel is NULL"); return; }
    for (i32 i = 0; i < model->model->meshCount; i++){
        const Matrix translation_matrix = MatrixTranslate(model->transform.translation.x, model->transform.translation.y, model->transform.translation.z);
        const Matrix rotation_matrix = MatrixRotateXYZ(QuaternionToEuler(model->transform.rotation));
        const Matrix scale_matrix = MatrixScale(model->transform.scale.x, model->transform.scale.y, model->transform.scale.z);
        const Matrix model_matrix = MatrixMultiply(MatrixMultiply(translation_matrix, rotation_matrix), scale_matrix);
        DrawMesh(model->model->meshes[i], model->model->materials[model->model->meshMaterial[i]], model_matrix);
    }
}

void ac::model_render_wireframe(ac::model *model){
    if(!model) { log_error("Cannot render wireframe, model is NULL"); return; }
    if(!model->model) { log_error("Cannot render wireframe, rlmodel is NULL"); return; }
    for (i32 i = 0; i < model->model->meshCount; i++){
        rlEnableWireMode();
        ac::model_render(model);
        rlDisableWireMode();
    }
}

void ac::model_render_instances(ac::model *model, Matrix *transforms, const i32 count){
    if(!model) { log_error("Cannot render instances, model is NULL"); return; }
    if(!model->model) { log_error("Cannot render instances, rlmodel is NULL"); return; }
    for (i32 i = 0; i < model->model->meshCount; i++){
        DrawMeshInstanced(model->model->meshes[i], model->model->materials[model->model->meshMaterial[i]], transforms, count);
    }
}

void ac::camera_set_position(ac::camera *camera, const Vector3 position){
    if (!camera) { log_error("Cannot set camera position, camera is NULL"); return; }
    camera->camera.position = position;
}

void ac::camera_set_target(ac::camera *camera, const Vector3 target){
    if (!camera) { log_error("Cannot set camera target, camera is NULL"); return; }
    camera->camera.target = target;
}

void ac::camera_set_fovy(ac::camera *camera, const f32 fovy){
    if (!camera) { log_error("Cannot set camera fovy, camera is NULL"); return; }
    camera->camera.fovy = fovy;
}

void ac::camera_set_active(ac::camera *camera, const b8 new_state){
    if (!camera) { log_error("Cannot set camera active state, camera is NULL"); return; }
    for(sz i = 0; i < ac::camera_array_size(&engine->scene.cameras); i++){
        ac::camera* scene_camera = ac::camera_array_get(&engine->scene.cameras, i);
        if(scene_camera) { scene_camera->is_active = false; }
    }
    camera->is_active = new_state;
}

void ac::config_load()
{
    ac::json_load(&engine->config, ac::CONFIG_FILE);
}

void ac::config_unload()
{
    ac::json_unload(&engine->config);
}

b8 ac::config_window_get_size(i32 *width, i32 *height){
    if (!width || !height) { log_error("Cannot get window size, width or height is NULL"); return false; }
    const json_t* window_json = ac::json_get_json(&engine->config, "window");
    // *width = ac::json_get_int(window_json, "width");
    // *height = ac::json_get_int(window_json, "height");
    return true;
}

c8 *ac::config_window_get_name(){
    const json_t* window_json = ac::json_get_json(&engine->config, "window");
    // return ac::json_get_string(window_json, "name");
    return NULL;
}
