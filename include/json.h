#pragma once
#include "types.h"

// #include "json.h"
// #include <cstring>
// #include <cstdlib>

// enum ac_json_value_type {
//     AC_TYPE_INT,
//     AC_TYPE_FLOAT,
//     AC_TYPE_STRING,
//     AC_TYPE_BOOL,
//     AC_TYPE_ARRAY,
//     AC_TYPE_UNKNOWN
// };

// ac_json_value_type ac_get_json_type(const c8* json, const c8* key) {
//     c8 keyPattern[256];
//     snprintf(keyPattern, sizeof(keyPattern), "\"%s\":", key);

//     const c8* keyPos = strstr(json, keyPattern);
//     if (!keyPos) return AC_TYPE_UNKNOWN;

//     const c8* valueStart = keyPos + strlen(keyPattern);

//     if (*valueStart == '\"') {
//         return AC_TYPE_STRING;
//     } else if (isdigit(*valueStart) || *valueStart == '-') {
//         if (strchr(valueStart, '.') != NULL) {
//             return AC_TYPE_FLOAT;
//         } else {
//             return AC_TYPE_INT;
//         }
//     } else if (*valueStart == '[') {
//         return AC_TYPE_ARRAY;
//     } else if (*valueStart == '{') {
//         return AC_TYPE_UNKNOWN; // JSON object, not handled as a specific type here
//     } else if (strncmp(valueStart, "true", 4) == 0 || strncmp(valueStart, "false", 5) == 0) {
//         return AC_TYPE_BOOL;
//     } else {
//         return AC_TYPE_UNKNOWN;
//     }
// }

// void* ac_parse_json(const c8* json, const c8* key) {
//     c8 keyPattern[256];
//     snprintf(keyPattern, sizeof(keyPattern), "\"%s\":", key);

//     const c8* keyPos = strstr(json, keyPattern);
//     if (!keyPos) return NULL;

//     const c8* valueStart = keyPos + strlen(keyPattern);
//     const c8* end = NULL;
//     size_t length;

//     if (*valueStart == '\"') {
//         valueStart++;
//         end = strchr(valueStart, '\"');
//         if (!end) return NULL;
//         length = end - valueStart;
//     } else if (isdigit(*valueStart) || *valueStart == '-') {
//         end = strpbrk(valueStart, ",}]");
//         if (!end) end = valueStart + strlen(valueStart);
//         length = end - valueStart;
//     } else if (*valueStart == '[') {
//         end = strchr(valueStart, ']');
//         if (!end) return NULL;
//         length = end - valueStart + 1;
//     } else if (*valueStart == '{') {
//         end = strchr(valueStart, '}');
//         if (!end) return NULL;
//         length = end - valueStart + 1;
//     } else if (strncmp(valueStart, "true", 4) == 0 || strncmp(valueStart, "false", 5) == 0 || strncmp(valueStart, "null", 4) == 0) {
//         end = valueStart + (strncmp(valueStart, "false", 5) == 0 ? 5 : 4);
//         length = end - valueStart;
//     } else {
//         return NULL;
//     }

//     void* value = malloc(length + 1);
//     if (!value) return NULL;

//     strncpy((c8*)value, valueStart, length);
//     ((c8*)value)[length] = '\0';

//     return value;
// }
