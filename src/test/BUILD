cc_library(
    name = "tests_lib",
    srcs = [
        "utils/generate.cpp",
    ],
    hdrs = [
        "utils/benchmark.h",
        "utils/compare_range_engines.h",
        "utils/generate.h",
        "utils/range_query.h",
    ],
    deps = [
        "//src/algo:common",
        "//src/algo:utility",
        "@com_google_benchmark//:benchmark",
        "@com_google_googletest//:gtest",
    ],
)

cc_test(
    name = "unit_tests",
    srcs = [
        # "aho_corasick_test.cpp",
        "fenwick_tree_test.cpp",
        "segment_tree_test.cpp",
        "segment_vs_other_tree_test.cpp",
        "sparse_tree_test.cpp",
        # "treap_test.cpp",
    ],
    deps = [
        ":tests_lib",
        # "//src/algo:aho_corasick",
        "//src/algo:fenwick_tree",
        "//src/algo:segment_tree",
        "//src/algo:sparse_tree",
        # "//src/algo:treap",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "benchmarks",
    srcs = [
        "fenwick_tree_benchmark.cpp",
        "segment_tree_benchmark.cpp",
        "sparse_tree_benchmark.cpp",
    ],
    deps = [
        ":tests_lib",
        # "//src/algo:aho_corasick",
        "//src/algo:fenwick_tree",
        "//src/algo:segment_tree",
        "//src/algo:sparse_tree",
        # "//src/algo:treap",
        "@com_google_benchmark//:benchmark_main",
    ],
)
