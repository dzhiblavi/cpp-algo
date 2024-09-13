cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

project(
  cpp_algo
  VERSION 0.1
  LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(DLIB_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

include(./cmake/StandardProjectSettings.cmake)
include(./cmake/EnableCompilerWarnings.cmake)

add_subdirectory(src)

include(./cmake/Testing.cmake)

# option(DLIB_CODE_COVERAGE "Enable coverage reporting" OFF)
