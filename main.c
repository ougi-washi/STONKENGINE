// Abstract City - Jed Fakhfekh - https://github.com/ougi-washi
#include "engine.h"

i32 main(i32 argc, c8 const *argv[]){
    ac_engine_start();

    ac_scene scene;
    ac_scene_init(&scene);
    ac_scene_add_model(&scene, ac_engine_get_model("resources/models/monkey.obj"));
    
    Camera main_camera = {0};
    main_camera.position = (Vector3){0.0f, 10.0f, 10.0f};
    main_camera.target = (Vector3){0.0f, 0.0f, 0.0f};
    main_camera.up = (Vector3){0.0f, 1.0f, 0.0f};
    main_camera.fovy = 45.0f;
    main_camera.projection = CAMERA_PERSPECTIVE;
    ac_scene_add_camera(&scene, main_camera);

    while (ac_engine_should_loop()){
        ac_scene_render(&scene);
    }
    ac_engine_end();
    return 0;
}
