// Abstract City - Jed Fakhfekh - https://github.com/ougi-washi
#include "engine.h"
#include "config.h"
#include "log.h"
#include "raymath.h"
// #include "rlgl.h"

ac_engine engine = {0};

void ac_engine_start(){
    log_info("Starting engine");
    ac_config_load();
    const i32 window_width = ac_config_get_int("window_width");
    const i32 window_height = ac_config_get_int("window_height");
    const c8* window_name = ac_config_get_string("window_name");
    const i32 target_fps = ac_config_get_int("target_fps");
    InitWindow(window_width, window_height, window_name);
    SetTargetFPS(target_fps);

    log_info("Initializing models pool");
    ac_model_pool_init(&engine.model_pool);
}

b8 ac_engine_should_loop(){
    return !WindowShouldClose();
}

void ac_engine_end(){
    log_info("Clearing engine");
    CloseWindow();
}

Model* ac_engine_get_model(const c8 *path){
    for (i32 i = 0; i < ac_model_pool_size(&engine.model_pool); i++){
        ac_model_loaded* model_loaded = ac_model_pool_get(&engine.model_pool, i);
        if (model_loaded){
            if (strcmp(model_loaded->path, path) == 0){
                return &model_loaded->model;
            }            
        }
        else{
            log_info("Model not found, loading model");
            ac_model_loaded m = {0};
            strcpy(m.path, path);
            m.model = LoadModel(path);
            ac_model_pool_push_back(&engine.model_pool, m);
            // return &AC_BACK(engine.model_pool.models).model;
        }
    }
    return NULL;
}

void ac_scene_init(ac_scene *scene)
{
    if(!scene) { log_error("Scene is NULL"); return; }
    ac_model_array_init(&scene->models);
    ac_camera_array_init(&scene->cameras);
}

void ac_scene_add_model(ac_scene *scene, Model* model)
{
    if(!scene) { log_error("Scene is NULL"); return; }
    if(!model) { log_error("Model is NULL"); return; }
    ac_model* scene_model = ac_model_array_increment(&scene->models);
    if(scene_model) {
        scene_model->model = model; 
        scene_model->transform = (Transform){0};
    }
    else { log_error("Failed to add model to scene"); }
}

void ac_scene_add_camera(ac_scene *scene, Camera camera)
{
    if(!scene) { log_error("Scene is NULL"); return; }
    // if(!camera) { log_error("Camera is NULL"); return; }
    ac_camera* scene_camera = ac_camera_array_increment(&scene->cameras);
    if(scene_camera) {
        scene_camera->camera = camera;
        scene_camera->is_active = false;
    }
    else { log_error("Failed to add camera to scene"); }
}

void ac_scene_render(ac_scene *scene)
{
    if(!scene) { log_error("Cannot render, scene is NULL"); return; }
    
    ac_camera* camera = ac_camera_array_get(&scene->cameras, 0);
    if (!camera) { log_error("Cannot render, camera is NULL"); return; }

    BeginMode3D(camera->camera);
    ClearBackground(BLACK);
    for (i32 i = 0; i < ac_model_array_size(&scene->models); i++){
        ac_model* model = ac_model_array_get(&scene->models, i);
        if (model) { 
            ac_model_render(model); 
        }
    }
    EndDrawing();
}

void ac_model_render(ac_model *model)
{
    if(!model) { log_error("Cannot render, model is NULL"); return; }
    if(!model->model) { log_error("Cannot render, rlmodel is NULL"); return; }
    for (i32 i = 0; i < model->model->meshCount; i++){
        const Matrix transform_matrix = TransformToMatrix(model->transform);
        // DrawMesh(model->model->meshes[i], model->model->materials[model->model->meshMaterial[i]], );

        // Matrix matRotationX = MatrixRotate((Vector){0, 0, 0}, rotationAngle*DEG2RAD);
        // Matrix matRotationY = MatrixRotate(rotationAxis, rotationAngle*DEG2RAD);
        // Matrix matRotationZ = MatrixRotate(rotationAxis, rotationAngle*DEG2RAD);

        // Matrix matTranslation = MatrixTranslate(position.x, position.y, position.z);
        // Matrix matTransform = MatrixMultiply(MatrixMultiply(matScale, matRotation), matTranslation);
    }
}

void ac_model_render_wireframe(ac_model *model)
{
    if(!model) { log_error("Cannot render wireframe, model is NULL"); return; }
    if(!model->model) { log_error("Cannot render wireframe, rlmodel is NULL"); return; }
    for (i32 i = 0; i < model->model->meshCount; i++){
        DrawMeshWires(model->model->meshes[i], model->model->materials[model->model->meshMaterial[i]], model->transform);
    }
}

void ac_model_render_instances(ac_model *model, Matrix *transforms, const i32 count)
{
    if(!model) { log_error("Cannot render instances, model is NULL"); return; }
    if(!model->model) { log_error("Cannot render instances, rlmodel is NULL"); return; }
    for (i32 i = 0; i < model->model->meshCount; i++){
        DrawMeshInstanced(model->model->meshes[i], model->model->materials[model->model->meshMaterial[i]], transforms, count);
    }
}

void ac_camera_set_position(ac_camera *camera, Vector3 position)
{
    if (!camera) { log_error("Cannot set camera position, camera is NULL"); return; }
    camera->camera.position = position;
}

void ac_camera_set_target(ac_camera *camera, Vector3 target)
{
    if (!camera) { log_error("Cannot set camera target, camera is NULL"); return; }
    camera->camera.target = target;
}

void ac_camera_set_fovy(ac_camera *camera, f32 fovy)
{
    if (!camera) { log_error("Cannot set camera fovy, camera is NULL"); return; }
    camera->camera.fovy = fovy;
}

void ac_camera_set_active(ac_camera *camera, b8 new_state)
{
    if (!camera) { log_error("Cannot set camera active state, camera is NULL"); return; }
    camera->is_active = new_state;
}
