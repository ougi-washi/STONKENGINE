// Abstract City - Jed Fakhfekh - https://github.com/ougi-washi
#include "engine.h"
#include "config.h"
#include "math.h"
// #include <thread>
// #include <chrono>

void ProcessInput(){
    if (IsKeyPressed(KEY_ESCAPE)) {
        CloseWindow();
    }
    
}

i32 main(i32 argc, const char *argv[]){
    ac::engine_start();
    ac::scene scene;
    // std::thread([&](){
    //     std::this_thread::sleep_for(std::chrono::seconds(1));
    // }).join();

    ac::scene_load(&scene, "sample_scene_001.json");
    Camera* camera = ac::scene_get_active_camera(&scene);
    while (ac::engine_should_loop()){
        if (IsKeyPressed(KEY_ESCAPE)) {
            CloseWindow();
        }
        camera->position.x += sinf(GetTime()) * 0.1f;
        camera->position.z += cosf(GetTime()) * 0.1f;
        ac::scene_render(&scene);
    }
    // ac::scene_load("scene.json");
    ac::engine_end();
    return 0;
}
