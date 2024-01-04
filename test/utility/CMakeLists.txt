function(dlib_add_test EXENAME FIRSTSOURCE)
  add_executable(unit.${EXENAME} ${FIRSTSOURCE} ${ARGN})
  target_link_libraries(unit.${EXENAME} PRIVATE dlib::utility test::utility
                                                GTest::gtest_main dlib::options)
  gtest_discover_tests(unit.${EXENAME})
endfunction(add_test)

dlib_add_test(co_resource co_resource/co_resource_test.cpp)
dlib_add_test(functional functional/func_test.cpp)
