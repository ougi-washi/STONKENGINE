// Abstract City - Jed Fakhfekh - https://github.com/ougi-washi
#include "engine.h"

i32 main(i32 argc, c8 const *argv[])
{
    ac_engine_start();
    while (ac_engine_should_loop())
    {
        /* code */
    }
    ac_engine_end();
    return 0;
}
