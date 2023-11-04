cc_library(
    name = "utility",
    srcs = [
        "utility/random.cpp",
    ],
    hdrs = [
        "utility/random.h",
        "utility/type_list.h",
        "utility/zip_view.h",
    ],
    include_prefix = "test",
    # visibility = ["//test:__subpackages__"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "rq_utils",
    hdrs = [
        "rq_utils/benchmark.h",
        "rq_utils/compare_range_engines.h",
        "rq_utils/generate.h",
        "rq_utils/range_query.h",
    ],
    include_prefix = "test",
    deps = [
        ":utility",
        "//src/algo:common",
        "//src/algo:utility",
        "@com_google_benchmark//:benchmark",
        "@com_google_googletest//:gtest",
    ],
)

cc_test(
    name = "rq_unit_tests",
    srcs = [
        "aho_corasick/aho_corasick_test.cpp",
        "fenwick_tree/fenwick_tree_test.cpp",
        "kdtree/kdtree_test.cpp",
        "leftist_heap/leftist_heap_test.cpp",
        "min_stack/min_stack_test.cpp",
        "segment_tree/segment_tree_test.cpp",
        "segment_tree/segment_vs_other_tree_test.cpp",
        "sparse_tree/sparse_tree_test.cpp",
        "treap/treap_test.cpp",
    ],
    deps = [
        ":rq_utils",
        "//src/algo:aho_corasick",
        "//src/algo:fenwick_tree",
        "//src/algo:kdtree",
        "//src/algo:leftist_heap",
        "//src/algo:min_stack",
        "//src/algo:segment_tree",
        "//src/algo:sparse_tree",
        "//src/algo:treap",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "rq_benchmarks",
    srcs = [
        "fenwick_tree/fenwick_tree_benchmark.cpp",
        "kdtree/kdtree_benchmark.cpp",
        "segment_tree/segment_tree_benchmark.cpp",
        "sparse_tree/sparse_tree_benchmark.cpp",
    ],
    deps = [
        ":rq_utils",
        "//src/algo:aho_corasick",
        "//src/algo:fenwick_tree",
        "//src/algo:kdtree",
        "//src/algo:segment_tree",
        "//src/algo:sparse_tree",
        "//src/algo:treap",
        "@com_google_benchmark//:benchmark_main",
    ],
)
