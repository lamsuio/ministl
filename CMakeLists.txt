cmake_minimum_required(VERSION 2.6)
project (MiniSTL)
set (MiniSTL_VERSION_MAJOR 0)
set (MiniSTL_VERSION_MINOR 1)

configure_file(
    "${PROJECT_SOURCE_DIR}/MiniSTLConfig.h.in"
    "${PROJECT_BINARY_DIR}/MiniSTLConfig.h"
    )

include_directories("${PROJECT_BINARY_DIR}")

add_subdirectory(ministl)

add_executable(Example example.cc)



