// Abstract City - Jed Fakhfekh - https://github.com/ougi-washi
#include "engine.h"
#include "config.h"
#include "math.h"

i32 main(i32 argc, const char *argv[]){
    ac::engine_start();
    ac::scene* scene = ac::scene_make_new();
    ac::scene_2d* scene_2d = ac::scene_2d_make_new();
    ac::scene_load(scene, "sample_scene_001.json");

    // load scene_2d
    ac::object_2d* object = ac::push_back(&scene_2d->objects);
    object->fragment = ac::config_get_shaders_path() + "ui_frag.glsl";
    object->vertex = ac::config_get_shaders_path() + "ui_vert.glsl";
    const std::string texture_path = ac::config_get_textures_path() + "test.png";
    object->textures.push_back(LoadTexture(texture_path.c_str()));
    ac::text* text = ac::push_back(&object->texts);
    text->font = GetFontDefault(); //LoadFontEx(ac::config_get_fonts_path().c_str(), 32, 0, 250);
    text->string = "Hello, World!";
    text->position = {100, 100};
    text->fontSize = 32;
    text->spacing = 0;
    text->tint = WHITE;
    
    while (ac::engine_should_loop()){
        ac::engine_process_input();
        ac::scene_render(scene);
        ac::scene_2d_render(scene_2d);
    }
    ac::engine_end();
    return 0;
}
