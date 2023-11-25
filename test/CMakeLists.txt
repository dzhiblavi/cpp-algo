include(gtest)
include(benchmark)
enable_testing()

add_library(test-utility STATIC)
add_library(test::utility ALIAS test-utility)
target_sources(test-utility PRIVATE test_utility/random.cpp)
target_include_directories(test-utility PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/)

add_subdirectory(algo)
add_subdirectory(graph)
add_subdirectory(value_or_error)
add_subdirectory(wptr)