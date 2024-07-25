// Abstract City - Jed Fakhfekh - https://github.com/ougi-washi
#include "json.h"
#include "log.h"
#include "defines.h"

void ac_json_load(ac_json_config* config, const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        log_error("Failed to open config file: %s", filename);
        return;
    }
    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);

    config->json_content = (c8*)malloc(length + 1);
    fread(config->json_content, 1, length, file);
    config->json_content[length] = '\0';
    fclose(file);

    config->json_config = json_create(config->json_content, config->json_pool, sizeof(config->json_pool) / sizeof(config->json_pool[0]));
}

void ac_json_unload(ac_json_config* config) {
    free(config->json_content);
    config->json_config = NULL;
}

i32 ac_json_get_int(const ac_json_config* config, const c8* key) {
    const json_t* field = json_getProperty(config->json_config, key);
    return field ? (i32)json_getInteger(field) : 0;
}

f32 ac_json_get_float(const ac_json_config* config, const c8* key) {
    const json_t* field = json_getProperty(config->json_config, key);
    return field ? (f32)json_getReal(field) : 0.0f;
}

c8* ac_json_get_string(const ac_json_config* config, const c8* key) {
    const json_t* field = json_getProperty(config->json_config, key);
    return field ? (c8*)json_getValue(field) : NULL;
}

b8 ac_json_get_bool(const ac_json_config* config, const c8* key) {
    const json_t* field = json_getProperty(config->json_config, key);
    return field ? json_getBoolean(field) : 0;
}

const json_t* ac_json_get_json(const ac_json_config* config, const c8* key) {
    return json_getProperty(config->json_config, key);
}

b8 ac_json_get_int_array(const ac_json_config* config, const c8* key, i32* out, i32* count) {
    const json_t* arr = json_getProperty(config->json_config, key);
    if (!arr || json_getType(arr) != JSON_ARRAY) return 0;
    i32 i = 0;
    const json_t* elem;
    while ((elem = json_getChild(arr))) {
        if (i < *count) {
            out[i++] = (i32)json_getInteger(elem);
            arr = json_getSibling(elem);
        } else break;
    }
    *count = i;
    return 1;
}

b8 ac_json_get_float_array(const ac_json_config* config, const c8* key, f32* out, i32* count) {
    const json_t* arr = json_getProperty(config->json_config, key);
    if (!arr || json_getType(arr) != JSON_ARRAY) return 0;
    i32 i = 0;
    const json_t* elem;
    while ((elem = json_getChild(arr))) {
        if (i < *count) {
            out[i++] = (f32)json_getReal(elem);
            arr = json_getSibling(elem);
        } else break;
    }
    *count = i;
    return 1;
}

b8 ac_json_get_string_array(const ac_json_config* config, const c8* key, c8** out, i32* count) {
    const json_t* arr = json_getProperty(config->json_config, key);
    if (!arr || json_getType(arr) != JSON_ARRAY) return 0;
    i32 i = 0;
    const json_t* elem;
    while ((elem = json_getChild(arr))) {
        if (i < *count) {
            out[i++] = (c8*)json_getValue(elem);
            arr = json_getSibling(elem);
        } else break;
    }
    *count = i;
    return 1;
}

b8 ac_json_get_bool_array(const ac_json_config* config, const c8* key, b8* out, i32* count) {
    const json_t* arr = json_getProperty(config->json_config, key);
    if (!arr || json_getType(arr) != JSON_ARRAY) return 0;
    i32 i = 0;
    const json_t* elem;
    while ((elem = json_getChild(arr))) {
        if (i < *count) {
            out[i++] = json_getBoolean(elem);
            arr = json_getSibling(elem);
        } else break;
    }
    *count = i;
    return 1;
}

b8 ac_json_get_json_array(const ac_json_config* config, const c8* key, const json_t** out, i32* count) {
    const json_t* arr = json_getProperty(config->json_config, key);
    if (!arr || json_getType(arr) != JSON_ARRAY) return 0;
    i32 i = 0;
    const json_t* elem;
    while ((elem = json_getChild(arr))) {
        if (i < *count) {
            out[i++] = elem;
            arr = json_getSibling(elem);
        } else break;
    }
    *count = i;
    return 1;
}

i32 ac_json_get_int(const json_t *json, const c8 *key){
    const json_t* field = json_getProperty(json, key);
    return field ? (i32)json_getInteger(field) : 0;
}

f32 ac_json_get_float(const json_t *json, const c8 *key){
    const json_t* field = json_getProperty(json, key);
    return field ? (f32)json_getReal(field) : 0.0f;
}

c8 *ac_json_get_string(const json_t *json, const c8 *key){
    const json_t* field = json_getProperty(json, key);
    return field ? (c8*)json_getValue(field) : NULL;
}

b8 ac_json_get_bool(const json_t *json, const c8 *key){
    const json_t* field = json_getProperty(json, key);
    return field ? json_getBoolean(field) : 0;
}

const json_t *ac_json_get_json(const json_t *json, const c8 *key){
    return json_getProperty(json, key);
}

b8 ac_json_get_int_array(const json_t *json, const c8 *key, i32 *out, i32 *count){
    const json_t* arr = json_getProperty(json, key);
    if (!arr || json_getType(arr) != JSON_ARRAY) return 0;
    i32 i = 0;
    const json_t* elem;
    while ((elem = json_getChild(arr))) {
        if (i < *count) {
            out[i++] = (i32)json_getInteger(elem);
            arr = json_getSibling(elem);
        } else break;
    }
    *count = i;
    return 1;
}

b8 ac_json_get_float_array(const json_t *json, const c8 *key, f32 *out, i32 *count){
    const json_t* arr = json_getProperty(json, key);
    if (!arr || json_getType(arr) != JSON_ARRAY) return 0;
    i32 i = 0;
    const json_t* elem;
    while ((elem = json_getChild(arr))) {
        if (i < *count) {
            out[i++] = (f32)json_getReal(elem);
            arr = json_getSibling(elem);
        } else break;
    }
    *count = i;
    return 1;
}

b8 ac_json_get_string_array(const json_t *json, const c8 *key, c8 **out, i32 *count){
    const json_t* arr = json_getProperty(json, key);
    if (!arr || json_getType(arr) != JSON_ARRAY) return 0;
    i32 i = 0;
    const json_t* elem;
    while ((elem = json_getChild(arr))) {
        if (i < *count) {
            out[i++] = (c8*)json_getValue(elem);
            arr = json_getSibling(elem);
        } else break;
    }
    *count = i;
    return 1;
}

b8 ac_json_get_bool_array(const json_t *json, const c8 *key, b8 *out, i32 *count){
    const json_t* arr = json_getProperty(json, key);
    if (!arr || json_getType(arr) != JSON_ARRAY) return 0;
    i32 i = 0;
    const json_t* elem;
    while ((elem = json_getChild(arr))) {
        if (i < *count) {
            out[i++] = json_getBoolean(elem);
            arr = json_getSibling(elem);
        } else break;
    }
    *count = i;
    return 1;
}

b8 ac_json_get_json_array(const json_t *json, const c8 *key, const json_t **out, i32 *count){
    const json_t* arr = json_getProperty(json, key);
    if (!arr || json_getType(arr) != JSON_ARRAY) return 0;
    i32 i = 0;
    const json_t* elem;
    while ((elem = json_getChild(arr))) {
        if (i < *count) {
            out[i++] = elem;
            arr = json_getSibling(elem);
        } else break;
    }
    *count = i;
    return 1;
}
