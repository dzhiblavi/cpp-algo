include(range-v3)

add_library(dlib-graph INTERFACE)
add_library(dlib::graph ALIAS dlib-graph)

target_include_directories(dlib-graph INTERFACE $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include/graph/>)
target_include_directories(dlib-graph SYSTEM INTERFACE $<INSTALL_INTERFACE:$<INSTALL_PREFIX>/include/graph/>)
target_link_libraries(dlib-graph INTERFACE range-v3)