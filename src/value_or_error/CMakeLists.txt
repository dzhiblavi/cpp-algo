add_library(dlib-value_or_error INTERFACE)
add_library(dlib::value_or_error ALIAS dlib-value_or_error)

target_include_directories(dlib-value_or_error INTERFACE $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include/value_or_error/>)
target_include_directories(dlib-value_or_error SYSTEM INTERFACE $<INSTALL_INTERFACE:$<INSTALL_PREFIX>/include/value_or_error/>)
target_link_libraries(dlib-value_or_error INTERFACE dlib::utility)