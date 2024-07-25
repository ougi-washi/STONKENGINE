// Abstract City - Jed Fakhfekh - https://github.com/ougi-washi
#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "types.h"


#define AC_DECLARE_ARRAY(type, name) \
    typedef struct { \
        type* data; \
        sz size; \
        sz capacity; \
    } name; \
    \
    static inline void name##_init(name* arr) { \
        arr->data = NULL; \
        arr->size = 0; \
        arr->capacity = 0; \
    } \
    \
    static inline type* name##_get(name* arr, const sz index) { \
        return (index >= 0 && index < arr->size) ? &arr->data[index] : NULL; \
    } \
    static inline sz name##_size(const name* arr) { \
        return arr->size; \
    } \
    static inline sz name##_push_back(name* arr, type value) { \
        if (arr->size == arr->capacity) { \
            arr->capacity = arr->capacity == 0 ? 1 : arr->capacity * 2; \
            arr->data = (type*)realloc(arr->data, arr->capacity * sizeof(type)); \
        } \
        arr->data[arr->size++] = value; \
        return arr->size - 1; \
    } \
    static inline type* name##_increment(name* arr) { \
        if (arr->size == arr->capacity) { \
            arr->capacity = arr->capacity == 0 ? 1 : arr->capacity * 2; \
            arr->data = (type*)realloc(arr->data, arr->capacity * sizeof(type)); \
            return &arr->data[arr->size++]; \
        } \
        return NULL; \
    } \
    static inline void name##_insert_at(name* arr, sz index, type value) { \
        if (index > arr->size) return; \
        if (arr->size == arr->capacity) { \
            arr->capacity = arr->capacity == 0 ? 1 : arr->capacity * 2; \
            arr->data = (type*)realloc(arr->data, arr->capacity * sizeof(type)); \
        } \
        memmove(&arr->data[index + 1], &arr->data[index], (arr->size - index) * sizeof(type)); \
        arr->data[index] = value; \
        arr->size++; \
    } \
    \
    static inline void name##_remove_at(name* arr, sz index) { \
        if (index >= arr->size) return; \
        memmove(&arr->data[index], &arr->data[index + 1], (arr->size - index - 1) * sizeof(type)); \
        arr->size--; \
    } \
    \
    static inline void name##_remove(name* arr, type* value) { \
        for (sz i = 0; i < arr->size; i++) { \
            if (&arr->data[i] == value) { \
                name##_remove_at(arr, i); \
                return; \
            } \
        } \
    } \
    \
    static inline void name##_clear(name* arr) { \
        free(arr->data); \
        arr->data = NULL; \
        arr->size = 0; \
        arr->capacity = 0; \
    }

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
