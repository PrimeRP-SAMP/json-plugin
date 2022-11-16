cmake_minimum_required(VERSION 3.23)
project(YAPJ_test)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "E:/dev/samp-json-plugin-YAPT/samp-json-plugin-YAPT/plugins/")
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/third-party/samp-cmake)
include(AddSAMPPlugin)

include_directories(third-party)

add_samp_plugin(${PROJECT_NAME} main.cpp common.h plugin.cpp plugin.h plugin.def script.cpp script.h native_param.h)
