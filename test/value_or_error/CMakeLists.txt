add_executable(value_or_error_test static_tests.cpp small_tests.cpp tests.cpp)

target_link_libraries(
  value_or_error_test PUBLIC dlib::value_or_error GTest::gtest_main
                             dlib::options)

gtest_discover_tests(value_or_error_test)
