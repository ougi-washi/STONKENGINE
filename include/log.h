// STONKENGINE - Jed Fakhfekh - https://github.com/ougi-washi
#pragma once
#include "types.h"

namespace se
{
    static const char* LOG_LEVEL_INFO =  "INFO"; 
    static const char* LOG_LEVEL_WARN =  "WARN";
    static const char* LOG_LEVEL_ERROR = "ERROR";

    void log_message(const char *level, const char *file, const i32 line, const char *fmt, ...);
} // namespace se

#define log_info(...) se::log_message(se::LOG_LEVEL_INFO, __FILE__, __LINE__, __VA_ARGS__)
#define log_warn(...) se::log_message(se::LOG_LEVEL_WARN, __FILE__, __LINE__, __VA_ARGS__)
#define log_error(...) se::log_message(se::LOG_LEVEL_ERROR, __FILE__, __LINE__, __VA_ARGS__)
