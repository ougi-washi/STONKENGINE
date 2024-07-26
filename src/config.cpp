// Abstract City - Jed Fakhfekh - https://github.com/ougi-washi
#include "config.h"
#include "utility.h"
#include "defines.h"
#include "log.h"

json ac::config_get_paths(){
    return config_get_value<json>("paths");
}

json ac::config_get_assets_paths()
{
    json paths = ac::config_get_paths();
    if (paths.empty()) {
        log_error("Could not get assets paths, paths json is empty");
        return json();
    }
    return paths["assets"];
}

std::string ac::config_get_root_path()
{
    json paths = ac::config_get_paths();
    if (paths.empty()) {
        log_error("Could not get root path, paths json is empty");
        return std::string();
    }
    return paths["root"];
}

std::string ac::config_get_models_path(){
    json paths = ac::config_get_assets_paths();
    if (paths.empty()) {
        log_error("Could not get models path, paths json is empty");
        return std::string();
    }
    return paths["models"];
}

std::string ac::config_get_shaders_path(){
    json paths = ac::config_get_assets_paths();
    if (paths.empty()) {
        log_error("Could not get shaders path,  paths json is empty");
        return std::string();
    }
    return paths["shaders"];
}

std::string ac::config_get_textures_path(){
    json paths = ac::config_get_assets_paths();
    if (paths.empty()) {
        log_error("Could not get  textures path,  paths json is empty");
        return std::string();
    }
    return paths["textures"];
}
std::string ac::config_get_sounds_path(){
    json paths = ac::config_get_assets_paths();
    if (paths.empty()) {
        log_error("Could not get  sounds path,  paths json is empty");
        return std::string();
    }
    return paths["sounds"];
}

std::string ac::config_get_fonts_path(){
    json paths = ac::config_get_assets_paths();
    if (paths.empty()) {
        log_error("Could not get  fonts path,  paths json is empty");
        return std::string();
    }
    return paths["fonts"];
}
