// Abstract City - Jed Fakhfekh - https://github.com/ougi-washi
#include "engine.h"

i32 main(i32 argc, c8 const *argv[]){
    ac_engine_start();

    ac_scene scene;
    AC_INIT(&scene.models, ac_model);
    AC_INIT(&scene.cameras, ac_camera);

    while (ac_engine_should_loop()){

    }
    ac_engine_end();
    return 0;
}
