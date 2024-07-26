// Abstract City - Jed Fakhfekh - https://github.com/ougi-washi
#pragma once
#include "types.h"

namespace ac
{
    const c8* LOG_LEVEL_INFO =  "INFO"; 
    const c8* LOG_LEVEL_WARN =  "WARN";
    const c8* LOG_LEVEL_ERROR = "ERROR";

    void log_message(const c8 *level, const c8 *file, const i32 line, const c8 *fmt, ...);
} // namespace ac

#define log_info(...) ac::log_message(ac::LOG_LEVEL_INFO, __FILE__, __LINE__, __VA_ARGS__)
#define log_warn(...) ac::log_message(ac::LOG_LEVEL_WARN, __FILE__, __LINE__, __VA_ARGS__)
#define log_error(...) ac::log_message(ac::LOG_LEVEL_ERROR, __FILE__, __LINE__, __VA_ARGS__)