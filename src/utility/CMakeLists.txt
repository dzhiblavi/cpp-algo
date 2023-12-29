add_library(dlib-utility INTERFACE)
add_library(dlib::utility ALIAS dlib-utility)

target_include_directories(dlib-utility INTERFACE $<BUILD_INTERFACE:${DLIB_SOURCE_DIR}/include/utility/>)
target_include_directories(dlib-utility SYSTEM INTERFACE $<INSTALL_INTERFACE:$<INSTALL_PREFIX>/include/utility/>)
