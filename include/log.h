// Abstract City - Jed Fakhfekh - https://github.com/ougi-washi
#pragma once
#include "types.h"

namespace ac
{
    static const char* LOG_LEVEL_INFO =  "INFO"; 
    static const char* LOG_LEVEL_WARN =  "WARN";
    static const char* LOG_LEVEL_ERROR = "ERROR";

    void log_message(const char *level, const char *file, const i32 line, const char *fmt, ...);
} // namespace ac

#define log_info(...) ac::log_message(ac::LOG_LEVEL_INFO, __FILE__, __LINE__, __VA_ARGS__)
#define log_warn(...) ac::log_message(ac::LOG_LEVEL_WARN, __FILE__, __LINE__, __VA_ARGS__)
#define log_error(...) ac::log_message(ac::LOG_LEVEL_ERROR, __FILE__, __LINE__, __VA_ARGS__)
