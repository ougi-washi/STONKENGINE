// Abstract City - Jed Fakhfekh - https://github.com/ougi-washi
#pragma once
#include "types.h"
#include "defines.h"
#include "log.h"
#include <fstream>
#include "json/json.h"

using json = nlohmann::json;

namespace ac{

    template <typename T>
    T config_get_value(const std::string& key) {
        std::ifstream file(AC_CONFIG_FILE);
        if (!file.is_open()) {
            log_error("Could not get config value, unable to open file: %s", AC_CONFIG_FILE);
            return T();
        }

        std::ifstream f("example.json");
        json data = json::parse(f);
        file.close();

        if (!data.contains(key)) {
            log_error("Could not find key in config: %s", key.c_str());
            return T();
        }

        json found_value = data[key];
        
        if constexpr (std::is_same<T, f32>::value) {
            if (found_value.is_number_float()) {
                return found_value.get<f32>();
            }
        } else if constexpr (std::is_same<T, i32>::value) {
            if (found_value.is_number_integer()) {
                return found_value.get<int>();
            }
        } else if constexpr (std::is_same<T, std::string>::value) {
            if (found_value.is_string()) {
                return found_value.get<std::string>();
            }
        } else if constexpr (std::is_same<T, json>::value) {
            if (found_value.is_array() || found_value.is_object()) {
                return found_value;
            }
        }
        log_error("Could not get config value for key: %s, invalid json type %s", key.c_str());
        return T();
    }
}
// void config_load();
// void config_unload();

// i32 config_get_int(const char* key);
// f32 config_get_float(const char* key);
// char* config_get_string(const char* key);
// b8 config_get_bool(const char* key);
// const json_t* config_get_json(const char* key);

// b8 config_get_int_array(const char* key, i32* out, i32* count);
// b8 config_get_float_array(const char* key, f32* out, i32* count);
// b8 config_get_string_array(const char* key, char** out, i32* count);
// b8 config_get_bool_array(const char* key, b8* out, i32* count);
// b8 config_get_json_array(const char* key, const json_t** out, i32* count);

// b8 config_get_game_models_path(const char* key, char* out, i32* out_size);
// b8 config_get_game_texturs_path(const char* key, char* out, i32* out_size);
// b8 config_get_game_shaders_path(const char* key, char* out, i32* out_size);
// b8 config_get_game_sounds_path(const char* key, char* out, i32* out_size);
// b8 config_get_game_fonts_path(const char* key, char* out, i32* out_size);
// b8 config_get_engine_models_path(const char* key, char* out, i32* out_size);
// b8 config_get_engine_texturs_path(const char* key, char* out, i32* out_size);
// b8 config_get_engine_shaders_path(const char* key, char* out, i32* out_size);
// b8 config_get_engine_sounds_path(const char* key, char* out, i32* out_size);
// b8 config_get_engine_fonts_path(const char* key, char* out, i32* out_size);
