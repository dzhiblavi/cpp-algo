find_package(fmt)

add_executable(wptr_test wptr_test.cpp)
target_link_libraries(wptr_test dlib::wptr fmt::fmt gtest::gtest)
