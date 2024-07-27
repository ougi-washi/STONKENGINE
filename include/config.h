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

        json data = json::parse(file);
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

    json config_get_paths();
    json config_get_assets_paths();
    std::string config_get_root_path();
    std::string config_get_scenes_path();
    std::string config_get_models_path();
    std::string config_get_shaders_path();
    std::string config_get_textures_path();
    std::string config_get_sounds_path();
    std::string config_get_fonts_path();
}
