add_library(dlib-algo INTERFACE)
add_library(dlib::algo ALIAS dlib-algo)

target_include_directories(dlib-algo INTERFACE $<BUILD_INTERFACE:${DLIB_SOURCE_DIR}/include/algo/>)
target_include_directories(dlib-algo SYSTEM INTERFACE $<INSTALL_INTERFACE:$<INSTALL_PREFIX>/include/algo/>)