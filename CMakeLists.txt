cmake_minimum_required(VERSION 2.8.2)

project (MiniSTL)

set (MiniSTL_VERSION_MAJOR 0)
set (MiniSTL_VERSION_MINOR 1)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -g")

if( COMMAND CMAKE_POLICY )
   CMAKE_POLICY( SET CMP0008 NEW )
endif( COMMAND CMAKE_POLICY )

configure_file(
    "${PROJECT_SOURCE_DIR}/MiniSTLConfig.h.in"
    "${PROJECT_BINARY_DIR}/MiniSTLConfig.h"
    )

option (NO_NS
    "do not use namespace" OFF)

include_directories("${PROJECT_BINARY_DIR}")

add_subdirectory(ministl)

file(GLOB unittest_src
    "test/*.h"
    "test/*.cc"
    )

if (NOT CMAKE_BUILD_TYPE)
    set (CMAKE_BUILD_TYPE Debug)
endif()

# EP_PREFIX: External Projet Prefix
#set_directory_properties(PROPERTIES EP_PREFIX ${CMAKE_BINARY_DIR}/deps)

# //// copy from github.com/google/googletest
# Copy .in to right place
configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
# Run cmake and build in target dir
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()

# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory(${CMAKE_BINARY_DIR}/googletest-src
                 ${CMAKE_BINARY_DIR}/googletest-build
                 EXCLUDE_FROM_ALL)

# The gtest/gtest_main targets carry header search path
# dependencies automatically when using CMake 2.8.11 or
# later. Otherwise we have to add them here ourselves.
if (CMAKE_VERSION VERSION_LESS 2.8.11)
  include_directories("${gtest_SOURCE_DIR}/include")
endif()

set(Pthread "-pthread")

add_executable(main_test ${unittest_src})
target_link_libraries(
    main_test
    gtest_main
    #debug ${binary_dir}/debug/${CMAKE_FIND_LIBRARY_PREFIXES}gtest${Suffix}
    #optimized ${binary_dir}/release/${CMAKE_FIND_LIBRARY_PREFIXES}gtest${Suffix}
    ${Pthread})
add_test(NAME ministl_test COMMAND main_test)
