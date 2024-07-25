// Abstract City - Jed Fakhfekh - https://github.com/ougi-washi
#include "engine.h"
#include "config.h"
#include "log.h"
#include "raymath.h"
#include "defines.h"
// #include "rlgl.h"

ac_engine* engine = NULL;

void ac_engine_start(){
    log_info("Starting engine");
    engine = malloc(sizeof(ac_engine));
    ac_config_load();
    i32 window_width = 0;
    i32 window_height = 0;
    ac_config_window_get_size(&window_width, &window_height);
    const c8* window_name = ac_config_window_get_name();
    const i32 target_fps = ac_json_get_int(&engine->config, "target_fps");
    InitWindow(window_width, window_height, window_name);
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    SetTargetFPS(target_fps);

    log_info("Initializing models pool");
    ac_model_pool_init(&engine->model_pool);
}

b8 ac_engine_should_loop(){
    return !WindowShouldClose();
}

void ac_engine_end(){
    log_info("Clearing engine");
    CloseWindow();
    ac_model_pool_clear(&engine->model_pool);
    ac_config_unload();
    free(engine);
}

Model* ac_engine_get_model(const c8 *path){
    for (i32 i = 0; i < ac_model_pool_size(&engine->model_pool); i++){
        ac_model_loaded* model_loaded = ac_model_pool_get(&engine->model_pool, i);
        if (model_loaded){
            if (strcmp(model_loaded->path, path) == 0){
                return &model_loaded->model;
            }
        }
        else{
            log_info("Model not found, loading model");
            ac_model_loaded* model_loaded = ac_model_pool_increment(&engine->model_pool);
            if (!model_loaded) { log_error("Failed to add model to the loop"); return NULL; }
            model_loaded->model = LoadModel(path);
            strcpy(model_loaded->path, path);
            return &model_loaded->model; 
        }
    }
    return NULL;
}

void ac_scene_init(ac_scene *scene){
    if(!scene) { log_error("Scene is NULL"); return; }
    ac_model_array_init(&scene->models);
    ac_camera_array_init(&scene->cameras);
}

void ac_scene_add_model(ac_scene *scene, Model* model){
    if(!scene) { log_error("Scene is NULL"); return; }
    if(!model) { log_error("Model is NULL"); return; }
    ac_model* scene_model = ac_model_array_increment(&scene->models);
    if(scene_model) {
        scene_model->model = model; 
        scene_model->transform = (Transform){0};
    }
    else { log_error("Failed to add model to scene"); }
}

void ac_scene_add_camera(ac_scene *scene, Camera camera, const b8 is_active){
    if(!scene) { log_error("Scene is NULL"); return; }
    ac_camera* scene_camera = ac_camera_array_increment(&scene->cameras);
    if(scene_camera) {
        scene_camera->camera = camera;
        ac_camera_set_active(scene_camera, is_active);
    }
    else { log_error("Failed to add camera to scene"); }
}

ac_camera *ac_scene_make_new_camera(ac_scene *scene)
{
    if(!scene) { log_error("Scene is NULL"); return NULL; }
    ac_camera* scene_camera = ac_camera_array_increment(&scene->cameras);
    if(scene_camera) {
        scene_camera->camera = (Camera){
            .position = (Vector3){10.0f, 10.0f, 10.0f},
            .target = (Vector3){0.0f, 0.0f, 0.0f},
            .up = (Vector3){0.0f, 1.0f, 0.0f},
            .fovy = 45.0f,
            .projection = CAMERA_PERSPECTIVE
        };
        scene_camera->is_active = false;
        return scene_camera;
    }
    else { log_error("Failed to add camera to scene"); return NULL; }
}

void ac_scene_render(ac_scene *scene){
    if(!scene) { log_error("Cannot render, scene is NULL"); return; }
    Camera* camera = ac_scene_get_active_camera(scene);
    if (!camera) { log_error("Cannot render, camera is NULL"); return; }

    BeginDrawing();
    {
        ClearBackground(BLACK);
        BeginMode3D(*camera);
        DrawCube((Vector3){ 0.0f, 0.0f, 0.0f }, 2.0f, 2.0f, 2.0f, RED);
        DrawGrid(10, 1.0f);
        EndMode3D();
    }
    EndDrawing();
}

Camera *ac_scene_get_active_camera(ac_scene *scene){
    if(!scene) { log_error("Cannot get active camera, scene is NULL"); return NULL; }
    for (i32 i = 0; i < ac_camera_array_size(&scene->cameras); i++){
        ac_camera* camera = ac_camera_array_get(&scene->cameras, i);
        if(camera && camera->is_active) { return &camera->camera; }
    }
    return NULL;
}

void ac_model_render(ac_model *model){
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

void ac_model_render_wireframe(ac_model *model){
    if(!model) { log_error("Cannot render wireframe, model is NULL"); return; }
    if(!model->model) { log_error("Cannot render wireframe, rlmodel is NULL"); return; }
    for (i32 i = 0; i < model->model->meshCount; i++){
        rlEnableWireMode();
        ac_model_render(model);
        rlDisableWireMode();
    }
}

void ac_model_render_instances(ac_model *model, Matrix *transforms, const i32 count){
    if(!model) { log_error("Cannot render instances, model is NULL"); return; }
    if(!model->model) { log_error("Cannot render instances, rlmodel is NULL"); return; }
    for (i32 i = 0; i < model->model->meshCount; i++){
        DrawMeshInstanced(model->model->meshes[i], model->model->materials[model->model->meshMaterial[i]], transforms, count);
    }
}

void ac_camera_set_position(ac_camera *camera, const Vector3 position){
    if (!camera) { log_error("Cannot set camera position, camera is NULL"); return; }
    camera->camera.position = position;
}

void ac_camera_set_target(ac_camera *camera, const Vector3 target){
    if (!camera) { log_error("Cannot set camera target, camera is NULL"); return; }
    camera->camera.target = target;
}

void ac_camera_set_fovy(ac_camera *camera, const f32 fovy){
    if (!camera) { log_error("Cannot set camera fovy, camera is NULL"); return; }
    camera->camera.fovy = fovy;
}

void ac_camera_set_active(ac_camera *camera, const b8 new_state){
    if (!camera) { log_error("Cannot set camera active state, camera is NULL"); return; }
    for(sz i = 0; i < ac_camera_array_size(&engine->scene.cameras); i++){
        ac_camera* scene_camera = ac_camera_array_get(&engine->scene.cameras, i);
        if(scene_camera) { scene_camera->is_active = false; }
    }
    camera->is_active = new_state;
}

void ac_config_load()
{
    ac_json_load(&engine->config, AC_CONFIG_FILE);
}

void ac_config_unload()
{
    ac_json_unload(&engine->config);
}

b8 ac_config_window_get_size(i32 *width, i32 *height){
    if (!width || !height) { log_error("Cannot get window size, width or height is NULL"); return false; }
    const json_t* window_json = ac_json_get_json(&engine->config, "window");
    *width = ac_json_get_int(window_json, "width");
    *height = ac_json_get_int(window_json, "height");
    return true;
}

c8 *ac_config_window_get_name(){
    const json_t* window_json = ac_json_get_json(&engine->config, "window");
    return ac_json_get_string(window_json, "name");
}
