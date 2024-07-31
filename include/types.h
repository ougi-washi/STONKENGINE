// Binance Eye Bot - Jed Fakhfekh - https://github.com/ougi-washi

#pragma once

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

// Base types
typedef uint8_t		    u8;
typedef uint16_t	    u16;
typedef uint32_t    	u32;
typedef uint64_t	    u64;
typedef int8_t	    	i8;
typedef int16_t 		i16;
typedef int32_t		    i32;
typedef int64_t	    	i64;
typedef float   		f32;
typedef double		    f64;
typedef size_t		    sz;
typedef bool		    b8;

// Platform detection
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#define PLATFORM_WINDOWS 1
#elif defined(__linux__) || defined(__gnu_linux__)
// Linux OS
#define PLATFORM_LINUX 1
#if defined(__ANDROID__)
#define PLATFORM_ANDROID 1
#endif
#elif defined(__unix__)
// Catch anything not caught by the above.
#define PLATFORM_UNIX 1
#elif defined(_POSIX_VERSION)
// POSIX
#define PLATFORM_POSIX 1
#elif __APPLE__
// Apple platforms
#define PLATFORM_APPLE 1
#include <TargetConditionals.h>
#if TARGET_IPHONE_SIMULATOR
// iOS Simulator
#define PLATFORM_IOS 1
#define PLATFORM_IOS_SIMULATOR 1
#elif TARGET_OS_IPHONE
#define PLATFORM_IOS 1
// iOS device
#elif TARGET_OS_MAC
// Other kinds of Mac OS
#else
#error "Unknown Apple platform"
#endif
#else
#error "Unknown platform"
#endif

// LOG & ERROR handling
#define CRASH_ON_ERROR 0

// #if PLATFORM_WINDOWS
// #define _WINSOCKAPI_    // stops windows.h including winsock.h
// #include <windows.h>
// #elif PLATFORM_LINUX || PLATFORM_UNIX
// #include <unistd.h>
// #include <pthread.h>
// #include <sys/syscall.h>
// #endif

typedef enum flag
{
	SE_INVALID			= 0x00,
	SE_VALID			= 0x01,
	SE_LOCKED			= 0x02,
	SE_DIRTY			= 0x03,
	SE_MAX				= 0x04,
} flag;

#define SE_DECLARE_TYPE flag obj_flag

#define SE_IS_VALID(_obj)      	((_obj) && (u8)(_obj)->obj_flag >= SE_VALID && (u8)(_obj)->obj_flag < SE_MAX)
#define SE_IS_INVALID(_obj)    	(!(&((_obj)->obj_flag)) || !SE_IS_VALID(_obj))
#define SE_IS_LOCKED(_obj)     	((u8)(_obj)->obj_flag == SE_LOCKED)
#define SE_IS_UNLOCKED(_obj)   	((u8)(_obj)->obj_flag == SE_VALID)
#define SE_IS_DIRTY(_obj)   	((u8)(_obj)->obj_flag == SE_DIRTY)

#define SE_SET_VALID(_obj)     	if(SE_IS_INVALID(_obj))	(_obj)->obj_flag = SE_VALID
#define SE_SET_INVALID(_obj)	(_obj)->obj_flag = SE_INVALID
#define SE_LOCK(_obj)    	    if(SE_IS_UNLOCKED(_obj))	(_obj)->obj_flag = SE_LOCKED
#define SE_UNLOCK(_obj)  	    if(SE_IS_LOCKED(_obj))		(_obj)->obj_flag = SE_VALID
#define SE_SET_DIRTY(_obj)  	(_obj)->obj_flag = SE_DIRTY

#define SE_WAIT_AND_LOCK(_obj) 																\
do{ u64 loop_counter = 0; 																	\
while (SE_IS_LOCKED(_obj) && loop_counter < 1000000000) { loop_counter++; };				\
SE_LOCK(_obj); } while(0)

// This is a dummy type to avoid compilation errors when using void pointers
typedef struct flagged_struct { SE_DECLARE_TYPE; } flagged_struct;
