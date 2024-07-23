// Abstract City - Jed Fakhfekh - https://github.com/ougi-washi
#include "engine.h"
#include "raylib.h"
// #include "rlgl.h"

void ac_test_window(){
    const int screen_width = 800;
    const int screen_height = 600;
    InitWindow(screen_width, screen_height, "Test window");
    while (!WindowShouldClose()){
        BeginDrawing();
        ClearBackground(RAYWHITE);
        EndDrawing();
    }
    CloseWindow();
}

// void ac_create_window(i32 width, i32 height, const c8* title){
//     InitWindow(width, height, title);
// }

// void ac_close_window(){
//     CloseWindow();
// }
