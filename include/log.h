// Abstract City - Jed Fakhfekh - https://github.com/ougi-washi
#pragma once
#include "types.h"

#define AC_LOG_LEVEL_INFO "INFO"
#define AC_LOG_LEVEL_WARN "WARN"
#define AC_LOG_LEVEL_ERROR "ERROR"

void ac_log_message(const c8 *level, const c8 *file, const i32 line, const c8 *fmt, ...);
#define log_info(...) ac_log_message(AC_LOG_LEVEL_INFO, __FILE__, __LINE__, __VA_ARGS__)
#define log_warn(...) ac_log_message(AC_LOG_LEVEL_WARN, __FILE__, __LINE__, __VA_ARGS__)
#define log_error(...) ac_log_message(AC_LOG_LEVEL_ERROR, __FILE__, __LINE__, __VA_ARGS__)