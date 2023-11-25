include(fmt)

add_executable(wptr_test wptr_test.cpp)
target_link_libraries(wptr_test dlib::wptr fmt::fmt GTest::gtest_main)