cmake_minimum_required(VERSION 2.6)
project (MiniSTL)
set (MiniSTL_VERSION_MAJOR 0)
set (MiniSTL_VERSION_MINOR 1)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

configure_file(
    "${PROJECT_SOURCE_DIR}/MiniSTLConfig.h.in"
    "${PROJECT_BINARY_DIR}/MiniSTLConfig.h"
    )

option (NO_NS
    "do not use namespace" OFF)

include_directories("${PROJECT_BINARY_DIR}")

add_subdirectory(ministl)

# create main.cc which uses gtest
file (WRITE test/main.cc "#include \"gtest/gtest.h\"\n\n")
file (APPEND test/main.cc "TEST(A, B) {SUCCEED(); }\n")
file (APPEND test/main.cc "int main(int argc, char **argv) {")
file (APPEND test/main.cc "testing::InitGoogleTest(&argc, argv);")
file (APPEND test/main.cc "return RUN_ALL_TESTS();}\n")

file(GLOB unittest_src
    "test/*.h"
    "test/*.cc"
    )

# including external project module
include(ExternalProject)

if (NOT CMAKE_BUILD_TYPE)
    set (CMAKE_BUILD_TYPE Release)
endif()

set_directory_properties(PROPERTIES EP_PREFIX ${CMAKE_BINARY_DIR}/deps)

# add gtest now
ExternalProject_Add(
    googletest
    GIT_REPOSITORY https://github.com/svn2github/googletest.git
    TIMEOUT 10
    # FIXME: PATH_COMMAND
    CMAKE_ARGS -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
               -Dgtest_force_shared_crt=ON
               -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG:PATH=debug
               -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE:PATH=release
    # Disable install step
    INSTALL_COMMAND ""
    LOG_DOWNLOAD ON
    LOG_CONFIGURE ON
    LOG_BUILD ON)

ExternalProject_Get_Property(googletest source_dir)
include_directories(${source_dir}/include)

add_executable(main_test ${unittest_src})
add_dependencies(main_test googletest)

ExternalProject_Get_Property(googletest binary_dir)
set(Suffix ".a")
set(Pthread "-pthread")
target_link_libraries(
    main_test
    debug ${binary_dir}/debug/${CMAKE_FIND_LIBRARY_PREFIXES}gtest${Suffix}
    optimized ${binary_dir}/release/${CMAKE_FIND_LIBRARY_PREFIXES}gtest${Suffix}
    ${Pthread})


