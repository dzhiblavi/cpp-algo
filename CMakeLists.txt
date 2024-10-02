cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

project(cpp_algo VERSION 0.1 LANGUAGES CXX)

set(DLIB_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

include(./cmake/CCache.cmake)
include(./cmake/Platform.cmake)
include(./cmake/Development.cmake)
include(./cmake/CompileOptions.cmake)
include(./cmake/Coverage.cmake)
include(./cmake/Sanitizers.cmake)
include(./cmake/WarningsAsErrors.cmake)

add_subdirectory(src)

include(./cmake/Testing.cmake)
