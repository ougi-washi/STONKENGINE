// Abstract City - Jed Fakhfekh - https://github.com/ougi-washi
#include "engine.h"

i32 main(i32 argc, c8 const *argv[])
{
    
    // enum { MAX_FIELDS = 4 };
    // json_t pool[ MAX_FIELDS ];
    // char str[] = "{ \"name\": \"peter\", \"age\": 32 }";	

    // json_t const* parent = json_create( str, pool, MAX_FIELDS );
    // if ( parent == NULL ) return EXIT_FAILURE;
    
    ac_engine_start();
    while (ac_engine_should_loop())
    {
        /* code */
    }
    ac_engine_end();
    return 0;
}
