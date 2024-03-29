function(dlib_add_test EXENAME FIRSTSOURCE)
  add_executable(unit.${EXENAME} ${FIRSTSOURCE} ${ARGN})
  target_link_libraries(unit.${EXENAME} PRIVATE dlib::algo test::utility
                                                GTest::gtest_main dlib::options)
  gtest_discover_tests(unit.${EXENAME})
endfunction(dlib_add_test)

function(dlib_add_bench EXENAME FIRSTSOURCE)
  add_executable(bench.${EXENAME} ${FIRSTSOURCE} ${ARGN})
  target_link_libraries(
    bench.${EXENAME} PRIVATE dlib::algo test::utility benchmark::benchmark
                             dlib::options)
endfunction(dlib_add_bench)

dlib_add_test(aho_corasick_test aho_corasick/aho_corasick_test.cpp)
dlib_add_test(fenwick_tree_test fenwick_tree/fenwick_tree_test.cpp)
dlib_add_test(kdtree_test kdtree/kdtree_test.cpp)
dlib_add_test(leftist_heap_test leftist_heap/leftist_heap_test.cpp)
dlib_add_test(min_stack_test min_stack/min_stack_test.cpp)
dlib_add_test(segment_tree_test segment_tree/segment_tree_test.cpp)
dlib_add_test(segment_vs_other_tree_test
              segment_tree/segment_vs_other_tree_test.cpp)
dlib_add_test(sparse_tree_test sparse_tree/sparse_tree_test.cpp)
dlib_add_test(treap_test treap/treap_test.cpp)

dlib_add_bench(fenwick_tree_benchmark fenwick_tree/fenwick_tree_benchmark.cpp)
dlib_add_bench(kdtree_benchmark kdtree/kdtree_benchmark.cpp)
dlib_add_bench(segment_tree_benchmark segment_tree/segment_tree_benchmark.cpp)
dlib_add_bench(sparse_tree_benchmark sparse_tree/sparse_tree_benchmark.cpp)
