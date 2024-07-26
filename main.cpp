// Abstract City - Jed Fakhfekh - https://github.com/ougi-washi
#include "engine.h"
#include "config.h"
#include "math.h"
#include <thread>
#include <chrono>

i32 main(i32 argc, char const *argv[]){
    ac::engine_start();
    ac::scene scene;
    ac::scene_init(&scene);
    std::thread([&](){
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }).join();
    // ac::config_get_string
    ac::scene_add_model(&scene, ac::engine_get_model("resources/models/monkey.obj"));


    ac::camera* camera = ac::scene_make_new_camera(&scene);
    ac::camera_set_position(camera, {0.0f, 10.0f, 10.0f});
    ac::camera_set_target(camera, {0.0f, 0.0f, 0.0f});
    ac::camera_set_active(camera, true);

    Vector3 camera_pos = { 10.0f, 10.0f, 10.0f };
    while (ac::engine_should_loop()){
        camera_pos.x += sinf(GetTime()) * 0.1f;
        camera_pos.z += cosf(GetTime()) * 0.1f;
        ac::camera_set_position(camera, camera_pos);
        ac::scene_render(&scene);
    }
    ac::engine_end();
    return 0;
}
