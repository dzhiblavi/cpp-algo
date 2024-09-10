add_library(dlib-algo STATIC)
add_library(dlib::algo ALIAS dlib-algo)

target_include_directories(
  dlib-algo PUBLIC $<BUILD_INTERFACE:${DLIB_SOURCE_DIR}/include/algo/>)
target_include_directories(
  dlib-algo SYSTEM
  INTERFACE $<INSTALL_INTERFACE:$<INSTALL_PREFIX>/include/algo/>)

target_sources(dlib-algo PRIVATE ./suffix_array/suffix_array.cpp)
