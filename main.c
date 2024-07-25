// Abstract City - Jed Fakhfekh - https://github.com/ougi-washi
#include "engine.h"
#include "config.h"
#include "math.h"

i32 main(i32 argc, c8 const *argv[]){
    ac_engine_start();

    ac_scene scene;
    ac_scene_init(&scene);

    // ac_config_get_string
    // ac_scene_add_model(&scene, ac_engine_get_model("resources/models/monkey.obj"));


    ac_camera* camera = ac_scene_make_new_camera(&scene);
    ac_camera_set_position(camera, (Vector3){0.0f, 10.0f, 10.0f});
    ac_camera_set_target(camera, (Vector3){0.0f, 0.0f, 0.0f});
    ac_camera_set_active(camera, true);

    Vector3 camera_pos = { 10.0f, 10.0f, 10.0f };
    while (ac_engine_should_loop()){
        camera_pos.x += sinf(GetTime()) * 0.1f;
        camera_pos.z += cosf(GetTime()) * 0.1f;
        ac_camera_set_position(camera, camera_pos);
        ac_scene_render(&scene);
    }
    ac_engine_end();
    return 0;
}
