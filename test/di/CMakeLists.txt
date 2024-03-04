function(dlib_add_test EXENAME FIRSTSOURCE)
  add_executable(unit.${EXENAME} ${FIRSTSOURCE} ${ARGN})
  target_link_libraries(unit.${EXENAME} PRIVATE dlib::di test::utility
                                                GTest::gtest_main dlib::options)
  gtest_discover_tests(unit.${EXENAME})
endfunction(add_test)

dlib_add_test(di di_test.cpp)
