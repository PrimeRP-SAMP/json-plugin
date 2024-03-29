cmake_minimum_required(VERSION 3.9)
project(YAPJ)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "E:/dev/samp-json-plugin-YAPT/samp-json-plugin-YAPT/plugins/")

# MSVC-only flags
if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zc:preprocessor")
else()
    # x32 only
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m32")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32")
endif()
# Both MSVC and MSVC-like command-line interface compilers (Clang-cl)
if (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc")
endif()

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/third-party/samp-cmake)
include(AddSAMPPlugin)

include_directories(third-party)

add_samp_plugin(${PROJECT_NAME} src/main.cpp src/common.h src/plugin.cpp src/plugin.h src/plugin.def src/script.cpp src/script.h src/native_param.h src/json_watcher.cpp src/json_watcher.h)

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    # x32 only
    target_link_options(${PROJECT_NAME} PRIVATE /machine:x86)
endif()
