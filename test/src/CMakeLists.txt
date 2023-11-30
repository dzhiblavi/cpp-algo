add_library(test-utility STATIC)
add_library(test::utility ALIAS test-utility)
target_sources(test-utility PRIVATE test_utility/random.cpp)
target_include_directories(test-utility PUBLIC ${DLIB_SOURCE_DIR}/test/include/)