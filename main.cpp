// Abstract City - Jed Fakhfekh - https://github.com/ougi-washi
#include "engine.h"
#include "config.h"
#include "math.h"
// #include <thread>
// #include <chrono>

void move_camera_right(){
    ac::scene* scene = ac::scene_get_active();
    ac::camera* camera = ac::scene_get_active_camera(scene);
    ac::camera_move(camera, {0.1, 0.f, 0.f}, false);
}

void move_camera_left(){
    ac::scene* scene = ac::scene_get_active();
    ac::camera* camera = ac::scene_get_active_camera(scene);
    ac::camera_move(camera, {-.1f, 0.f, 0.f}, false);
}

void move_camera_forward(){
    ac::scene* scene = ac::scene_get_active();
    ac::camera* camera = ac::scene_get_active_camera(scene);
    ac::camera_move(camera, {0.f, 0.f, -.1f}, false);
}

void move_camera_backward(){
    ac::scene* scene = ac::scene_get_active();
    ac::camera* camera = ac::scene_get_active_camera(scene);
    ac::camera_move(camera, {0.f, 0.f, .1f}, false);
}

void move_camera_up(){
    ac::scene* scene = ac::scene_get_active();
    ac::camera* camera = ac::scene_get_active_camera(scene);
    ac::camera_move(camera, {0.f, 0.1f, 0.f}, false);
}

void move_camera_down(){
    ac::scene* scene = ac::scene_get_active();
    ac::camera* camera = ac::scene_get_active_camera(scene);
    ac::camera_move(camera, {0.f, -0.1f, 0.f}, false);
}

i32 main(i32 argc, const char *argv[]){
    ac::engine_start();

    ac::input_add_map({{KEY_D}, {}, move_camera_right});
    ac::input_add_map({{KEY_A}, {}, move_camera_left});
    ac::input_add_map({{KEY_W}, {}, move_camera_forward});
    ac::input_add_map({{KEY_S}, {}, move_camera_backward});
    ac::input_add_map({{KEY_E}, {}, move_camera_up});
    ac::input_add_map({{KEY_Q}, {}, move_camera_down});

    ac::scene* scene = ac::scene_make_new();
    // std::thread([&](){
    //     std::this_thread::sleep_for(std::chrono::seconds(1));
    // }).join();

    ac::scene_load(scene, "sample_scene_001.json");
    while (ac::engine_should_loop()){
        ac::engine_process_input();
        ac::scene_render(scene);
    }
    ac::engine_end();
    return 0;
}
