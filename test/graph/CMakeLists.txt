function(dlib_add_test EXENAME FIRSTSOURCE)
  add_executable(unit.${EXENAME} ${FIRSTSOURCE} ${ARGN})
  target_link_libraries(
    unit.${EXENAME} PRIVATE dlib::graph dlib::utility test::utility
                            GTest::gtest_main dlib::options)
  gtest_discover_tests(unit.${EXENAME})
endfunction(dlib_add_test)

dlib_add_test(bridges_test algo/bridges_test.cpp)
dlib_add_test(connected_test algo/connected_test.cpp)
dlib_add_test(cutpoints_test algo/cutpoints_test.cpp)
dlib_add_test(cycle_test algo/cycle_test.cpp)
dlib_add_test(dijkstra_test algo/dijkstra_test.cpp)
dlib_add_test(reachable_test algo/reachable_test.cpp)
dlib_add_test(top_sort_test algo/top_sort_test.cpp)

dlib_add_test(adj_matrix_test repr/adj_matrix_test.cpp)
