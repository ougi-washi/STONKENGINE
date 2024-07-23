# Abstract City - Jed Fakhfekh - https://github.com/ougi-washi

## General
cmake_minimum_required(VERSION 3.22.1)

project(AbstractCity LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 20)

# Directory setup
set(LIB_DIR "lib")
set(INC_DIR "include")
set(SRC_DIR "src")
set(BIN_DIR "bin")

# setup library macro
macro(setup_library arg_lib_name arg_source_path arg_include_path)
	message(STATUS "Adding ${arg_lib_name}")
	file(GLOB CURRENT_SRC_FILES	"${arg_source_path}/*.c" 
								"${arg_source_path}/*.cpp"
								"${arg_source_path}/**/*.c" 
								"${arg_source_path}/**/*.cpp")
	add_library(${arg_lib_name} STATIC	${CURRENT_SRC_FILES})
	target_include_directories(${arg_lib_name} PUBLIC ${arg_include_path})
endmacro()

find_package(Threads REQUIRED)

# RAYLIB
message(STATUS "Adding Raylib library")
add_subdirectory(${LIB_DIR}/raylib)

# JSON library
set(JSON_DIR "json")
setup_library(json_lib ${PROJECT_SOURCE_DIR}/${LIB_DIR}/${JSON_DIR} ${PROJECT_SOURCE_DIR}/${LIB_DIR}/${JSON_DIR})

# Main module
set(MAIN_MODULE_INCLUDES ${PROJECT_SOURCE_DIR}/${INC_DIR} ${PROJECT_SOURCE_DIR}/${LIB_DIR})
setup_library(main_module ${SRC_DIR} "${MAIN_MODULE_INCLUDES}")
target_link_libraries(main_module PUBLIC raylib json_lib)

# Executable
add_executable(${PROJECT_NAME} main.c)
target_link_libraries(${PROJECT_NAME} PUBLIC main_module)
set_target_properties(${PROJECT_NAME} PROPERTIES
                      RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/${BIN_DIR})

## Definitions
if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()
