// Abstract City - Jed Fakhfekh - https://github.com/ougi-washi
#include "utility.h"
#include "log.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <fstream>
#include <sstream>

std::string ac::read_file_text(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        log_error("Could not read file: %s", path.c_str());
        return ""; // Failed to open file
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();

    return buffer.str();
}
