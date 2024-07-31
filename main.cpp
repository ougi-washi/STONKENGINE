// STONKENGINE - Jed Fakhfekh - https://github.com/ougi-washi
#include "engine.h"
#include "config.h"
#include "math.h"
#include "raylib.h"


i32 main(i32 argc, const char *argv[]){
    se::engine_start();
    se::scene* scene = se::scene_make_new();
    se::scene_2d* scene_2d = se::scene_2d_make_new();
    se::scene_load(scene, "sample_scene_001.json");
    
    // load scene_2d (this is a test for the 2D rendering, it should be parsed from a json)
    se::scene_2d_element ui_element = se::scene_2d_make_new_object(scene_2d);
    ui_element.object->fragment = "ui_frag.glsl";
    ui_element.object->vertex = "ui_vert.glsl";
    const std::string texture_path = se::config_get_root_path() + se::config_get_textures_path() + "test.png";
    ui_element.object->textures.push_back(LoadTexture(texture_path.c_str()));
    se::text* text = se::push_back(&ui_element.object->texts);
    text->font = GetFontDefault();
    text->string = "STONKENGINE";
    text->position = {600, 20};
    text->fontSize = 30;
    text->spacing = 10;
    text->tint = {255, 255, 255, 255};
    se::scene_2d_element_bake(ui_element);

    while (se::engine_should_loop()){
        se::engine_process_input();
        se::engine_render();
    }
    se::engine_end();
    return 0;
}
