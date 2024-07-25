// Abstract City - Jed Fakhfekh - https://github.com/ougi-washi
#pragma once
#include "types.h"
#include "tiny-json.h"

#define AC_JSON_CONFIG_MAX_JSON 256

typedef struct ac_json_config{
    json_t json_pool[AC_JSON_CONFIG_MAX_JSON];
    const json_t* json_config;
    c8* json_content;
} ac_json_config;

// config
void ac_json_load(ac_json_config* config, const char* filename);
void ac_json_unload(ac_json_config* config);
i32 ac_json_get_int(const ac_json_config* config, const c8* key);
f32 ac_json_get_float(const ac_json_config* config, const c8* key);
c8* ac_json_get_string(const ac_json_config* config, const c8* key);
b8 ac_json_get_bool(const ac_json_config* config, const c8* key);
const json_t* ac_json_get_json(const ac_json_config* config, const c8* key);
b8 ac_json_get_int_array(const ac_json_config* config, const c8* key, i32* out, i32* count);
b8 ac_json_get_float_array(const ac_json_config* config, const c8* key, f32* out, i32* count);
b8 ac_json_get_string_array(const ac_json_config* config, const c8* key, c8** out, i32* count);
b8 ac_json_get_bool_array(const ac_json_config* config, const c8* key, b8* out, i32* count);
b8 ac_json_get_json_array(const ac_json_config* config, const c8* key, const json_t** out, i32* count);
// raw
i32 ac_json_get_int(const json_t* json, const c8* key);
f32 ac_json_get_float(const json_t* json, const c8* key);
c8* ac_json_get_string(const json_t* json, const c8* key);
b8 ac_json_get_bool(const json_t* json, const c8* key);
const json_t* ac_json_get_json(const json_t* json, const c8* key);
b8 ac_json_get_int_array(const json_t* json, const c8* key, i32* out, i32* count);
b8 ac_json_get_float_array(const json_t* json, const c8* key, f32* out, i32* count);
b8 ac_json_get_string_array(const json_t* json, const c8* key, c8** out, i32* count);
b8 ac_json_get_bool_array(const json_t* json, const c8* key, b8* out, i32* count);
b8 ac_json_get_json_array(const json_t* json, const c8* key, const json_t** out, i32* count);