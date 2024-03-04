add_library(dlib-di INTERFACE)
add_library(dlib::di ALIAS dlib-di)

target_include_directories(dlib-di INTERFACE $<BUILD_INTERFACE:${DLIB_SOURCE_DIR}/include/di/>)
target_include_directories(dlib-di SYSTEM INTERFACE $<INSTALL_INTERFACE:$<INSTALL_PREFIX>/include/di/>)
target_link_libraries(dlib-di INTERFACE dlib::utility)
