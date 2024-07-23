// Abstract City - Jed Fakhfekh - https://github.com/ougi-washi
#include "engine.h"
#include "config.h"
#include "log.h"
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
    AC_INIT(&engine.model_pool.models, ac_model_loaded);
}

b8 ac_engine_should_loop(){
    return !WindowShouldClose();
}

void ac_engine_end(){
    log_info("Clearing engine");
    CloseWindow();
}

Model* ac_engine_get_model(const c8 *path){
    for (int i = 0; i < AC_SIZE(engine.model_pool.models); i++){
        if (strcmp(engine.model_pool.models.data[i].path, path) == 0){
            return &engine.model_pool.models.data[i].model;
        }
        else{
            log_info("Model not found, loading model");
            ac_model_loaded m = {0};
            strcpy(m.path, path);
            m.model = LoadModel(path);
            AC_PUSH_BACK(&engine.model_pool.models, m);
            return &AC_BACK(engine.model_pool.models).model;
        }
    }
    return NULL;
}

void ac_scene_init(ac_scene *scene)
{
    AC_INIT(&scene->models, ac_model);
    AC_INIT(&scene->cameras, ac_camera);
}

void ac_scene_add_model(ac_scene *scene, Model* model)
{
    if(!scene) { log_error("Scene is NULL"); return; }
    if(!model) { log_error("Model is NULL"); return; }
    ac_model m = {model, {0}};
    AC_PUSH_BACK(&scene->models, m);
}

void ac_scene_add_camera(ac_scene *scene, Camera* camera)
{
    if(!scene) { log_error("Scene is NULL"); return; }
    if(!camera) { log_error("Camera is NULL"); return; }
    ac_camera c = {camera, true};
    AC_PUSH_BACK(&scene->cameras, c);
}
