// Abstract City - Jed Fakhfekh - https://github.com/ougi-washi
#include "engine.h"
#include "raylib.h"
// #include "rlgl.h"

void ac_engine_start(){
    InitWindow(800, 600, "Abstract City");
    SetTargetFPS(260);
}

b8 ac_engine_should_loop()
{
    return !WindowShouldClose();
}

void ac_engine_end()
{
    CloseWindow();
}
