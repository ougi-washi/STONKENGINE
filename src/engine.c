// Abstract City - Jed Fakhfekh - https://github.com/ougi-washi
#include "engine.h"
#include "raylib.h"
#include "config.h"
#include "log.h"
// #include "rlgl.h"

void ac_engine_start(){
    log_info("Starting engine");
    ac_config_load();
    const i32 window_width = ac_config_get_int("window_width");
    const i32 window_height = ac_config_get_int("window_height");
    const c8* window_name = ac_config_get_string("window_name");
    const i32 target_fps = ac_config_get_int("target_fps");

    InitWindow(window_width, window_height, window_name);
    SetTargetFPS(target_fps);
}

b8 ac_engine_should_loop(){
    return !WindowShouldClose();
}

void ac_engine_end(){
    CloseWindow();
}
