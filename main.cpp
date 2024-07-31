// STONKENGINE - Jed Fakhfekh - https://github.com/ougi-washi
#include "engine.h"
#include "config.h"
#include "math.h"
#include "raylib.h"


i32 main(i32 argc, const char *argv[]){
    ac::engine_start();
    ac::scene* scene = ac::scene_make_new();
    ac::scene_2d* scene_2d = ac::scene_2d_make_new();
    ac::scene_load(scene, "sample_scene_001.json");
    
    // load scene_2d (this is a test for the 2D rendering, it should be parsed from a json)
    ac::scene_2d_element ui_element = ac::scene_2d_make_new_object(scene_2d);
    ui_element.object->fragment = "ui_frag.glsl";
    ui_element.object->vertex = "ui_vert.glsl";
    const std::string texture_path = ac::config_get_root_path() + ac::config_get_textures_path() + "test.png";
    ui_element.object->textures.push_back(LoadTexture(texture_path.c_str()));
    ac::text* text = ac::push_back(&ui_element.object->texts);
    text->font = GetFontDefault();
    text->string = "STONKENGINE";
    text->position = {600, 20};
    text->fontSize = 30;
    text->spacing = 10;
    text->tint = {255, 255, 255, 255};
    ac::scene_2d_element_bake(ui_element);

    while (ac::engine_should_loop()){
        ac::engine_process_input();
        ac::engine_render();
    }
    ac::engine_end();
    return 0;
}
