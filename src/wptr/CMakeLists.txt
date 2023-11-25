add_library(dlib-wptr INTERFACE)
add_library(dlib::wptr ALIAS dlib-wptr)

target_include_directories(dlib-wptr INTERFACE $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include/wptr/>)
target_include_directories(dlib-wptr SYSTEM INTERFACE $<INSTALL_INTERFACE:$<INSTALL_PREFIX>/include/wptr/>)