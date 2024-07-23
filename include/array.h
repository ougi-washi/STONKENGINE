// Abstract City - Jed Fakhfekh - https://github.com/ougi-washi
#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "types.h"

#define AC_DECLARE_ARRAY(type, name) \
    typedef struct { \
        type* data; \
        size_t size; \
        size_t capacity; \
        size_t type_size; \
    } name;

#define AC_INIT(arr, type) \
    do { \
        (arr)->data = NULL; \
        (arr)->size = 0; \
        (arr)->capacity = 0; \
        (arr)->type_size = sizeof(type); \
    } while(0)

#define AC_GET(arr, index) (arr).data[(index)]
#define AC_SIZE(arr) (arr).size
#define AC_BACK(arr) (arr).data[AC_SIZE(arr) - 1]

#define AC_PUSH_BACK(arr, value) \
    do { \
        if ((arr)->size == (arr)->capacity) { \
            (arr)->capacity = (arr)->capacity == 0 ? 1 : (arr)->capacity * 2; \
            (arr)->data = realloc((arr)->data, (arr)->capacity * (arr)->type_size); \
        } \
        memcpy((char*)(arr)->data + (arr)->size * (arr)->type_size, &(value), (arr)->type_size); \
        (arr)->size++; \
    } while(0)

#define AC_INSERT_AT(arr, index, value) \
    do { \
        if ((index) <= (arr)->size) { \
            if ((arr)->size == (arr)->capacity) { \
                (arr)->capacity = (arr)->capacity == 0 ? 1 : (arr)->capacity * 2; \
                (arr)->data = realloc((arr)->data, (arr)->capacity * (arr)->type_size); \
            } \
            memmove((char*)(arr)->data + (index + 1) * (arr)->type_size, (char*)(arr)->data + (index) * (arr)->type_size, ((arr)->size - (index)) * (arr)->type_size); \
            memcpy((char*)(arr)->data + (index) * (arr)->type_size, &(value), (arr)->type_size); \
            (arr)->size++; \
        } \
    } while(0)

#define AC_REMOVE_AT(arr, index) \
    do { \
        if ((index) < (arr)->size) { \
            memmove((char*)(arr)->data + (index) * (arr)->type_size, \
                    (char*)(arr)->data + (index + 1) * (arr)->type_size, \
                    ((arr)->size - (index) - 1) * (arr)->type_size); \
            (arr)->size--; \
        } \
    } while(0)

#define AC_REMOVE_VAL(arr, val) \
    do { \
        for (size_t i = 0; i < (arr)->size; ) { \
            if ((arr)->data[i] == (val)) { \
                memmove(&(arr)->data[i], &(arr)->data[i + 1], ((arr)->size - i - 1) * sizeof(type)); \
                (arr)->size--; \
            } else { \
                i++; \
            } \
        } \
    } while(0)

#define AC_REMOVE_IF(arr, condition) \
    do { \
        for (size_t i = 0; i < (arr)->size; ) { \
            if (condition((char*)(arr)->data + i * (arr)->type_size)) { \
                memmove((char*)(arr)->data + i * (arr)->type_size, (char*)(arr)->data + (i + 1) * (arr)->type_size, ((arr)->size - i - 1) * (arr)->type_size); \
                (arr)->size--; \
            } else { \
                i++; \
            } \
        } \
    } while(0)

#define AC_FREE(arr) \
    do { \
        free((arr)->data); \
        (arr)->data = NULL; \
        (arr)->size = 0; \
        (arr)->capacity = 0; \
    } while(0)

AC_DECLARE_ARRAY(u8, u8_array)
AC_DECLARE_ARRAY(u16, u16_array)
AC_DECLARE_ARRAY(u32, u32_array)
AC_DECLARE_ARRAY(u64, u64_array)
AC_DECLARE_ARRAY(i8, i8_array)
AC_DECLARE_ARRAY(i16, i16_array)
AC_DECLARE_ARRAY(i32, i32_array)
AC_DECLARE_ARRAY(i64, i64_array)
AC_DECLARE_ARRAY(f32, f32_array)
AC_DECLARE_ARRAY(f64, f64_array)
AC_DECLARE_ARRAY(sz, sz_array)
AC_DECLARE_ARRAY(b8, b8_array)
AC_DECLARE_ARRAY(c8, c8_array)
AC_DECLARE_ARRAY(uc8, uc8_array)
AC_DECLARE_ARRAY(void*, void_array)
