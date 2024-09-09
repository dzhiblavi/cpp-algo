option(DLIB_BUILD_TESTING "Build tests" ON)

if(DLIB_BUILD_TESTS AND (PROJECT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR))
  enable_testing()
  add_subdirectory(test)
endif()
