add_executable(value_or_error_test static_tests.cpp small_tests.cpp tests.cpp)

target_link_libraries(
  value_or_error_test PUBLIC dlib::value_or_error gtest::gtest
                             project_warnings)

gtest_discover_tests(value_or_error_test)
