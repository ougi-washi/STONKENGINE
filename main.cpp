// Abstract City - Jed Fakhfekh - https://github.com/ougi-washi
#include "engine.h"
#include "config.h"
#include "math.h"
// #include <thread>
// #include <chrono>

void move_camera_right(){
    ac::scene* scene = ac::scene_get_active();
    Camera* camera = ac::scene_get_active_camera(scene);
    camera->position.x += 0.1f;
}

void move_camera_left(){
    ac::scene* scene = ac::scene_get_active();
    Camera* camera = ac::scene_get_active_camera(scene);
    camera->position.x -= 0.1f;
}

void move_camera_forward(){
    ac::scene* scene = ac::scene_get_active();
    Camera* camera = ac::scene_get_active_camera(scene);
    camera->position.z -= 0.1f;
}

void move_camera_backward(){
    ac::scene* scene = ac::scene_get_active();
    Camera* camera = ac::scene_get_active_camera(scene);
    camera->position.z += 0.1f;
}

void move_camera_up(){
    ac::scene* scene = ac::scene_get_active();
    Camera* camera = ac::scene_get_active_camera(scene);
    camera->position.y += 0.1f;
}

void move_camera_down(){
    ac::scene* scene = ac::scene_get_active();
    Camera* camera = ac::scene_get_active_camera(scene);
    camera->position.y -= 0.1f;
}

i32 main(i32 argc, const char *argv[]){
    ac::engine_start();
    ac::scene* scene = ac::scene_make_new();
    // std::thread([&](){
    //     std::this_thread::sleep_for(std::chrono::seconds(1));
    // }).join();

    ac::input_add_map({{KEY_D}, {}, move_camera_right});
    ac::input_add_map({{KEY_A}, {}, move_camera_left});
    ac::input_add_map({{KEY_W}, {}, move_camera_forward});
    ac::input_add_map({{KEY_S}, {}, move_camera_backward});
    ac::input_add_map({{KEY_E}, {}, move_camera_up});
    ac::input_add_map({{KEY_Q}, {}, move_camera_down});

    ac::scene_load(scene, "sample_scene_001.json");
    Camera* camera = ac::scene_get_active_camera(scene);
    while (ac::engine_should_loop()){
        ac::engine_process_input();
        // camera->position.x += sinf(GetTime()) * 0.1f;
        // camera->position.z += cosf(GetTime()) * 0.1f;
        ac::scene_render(scene);
    }
    // ac::scene_load("scene.json");
    ac::engine_end();
    return 0;
}
