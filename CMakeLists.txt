cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

project(
  cpp_algo
  VERSION 0.1
  LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(DLIB_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_FIND_LIBRARY_SUFFIXES ".a" ".so")
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake"
                      "${PROJECT_SOURCE_DIR}/third_party" ${CMAKE_MODULE_PATH})

option(DLIB_CODE_COVERAGE "Enable coverage reporting" OFF)
option(DLIB_BUILD_TESTING "Build tests" ON)

include(options)
add_subdirectory(src)

if(DLIB_BUILD_TESTING AND (PROJECT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR))
  enable_testing()
  add_subdirectory(test)
endif()
