// Abstract City - Jed Fakhfekh - https://github.com/ougi-washi
#include "log.h"
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <string.h>

void ac::log_message(const c8 *level, const c8 *file, const i32 line, const c8 *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    time_t now = time(NULL);
    c8 *date_str = ctime(&now);
    date_str[strlen(date_str) - 1] = '\0'; // Remove newline

    if (level == AC_LOG_LEVEL_INFO) {
        fprintf(stderr, "[%s | %s] ", level, date_str);
    }
    else{
        fprintf(stderr, "[%s | %s (%s:%d)] ", level, date_str, file, line);
    }
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n");

    va_end(args);
}
