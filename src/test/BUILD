cc_library(
    name = "tests_lib",
    hdrs = [
        "utils/compare_range_engines.h",
        "utils/range_query.h",
        "utils/generate.h",
    ],
    srcs = [
        "utils/generate.cpp",
    ],
    deps = [

    ],
)

cc_test(
    name = "unit_tests",
    srcs = [
        "aho_corasick_test.cpp",
        "segment_tree_test.cpp",
        "segment_vs_sparse_tree_test.cpp",
        "sparse_tree_test.cpp",
        "treap_test.cpp",
    ],
    deps = [
        ":tests_lib",
        "//src/algo:aho_corasick",
        "//src/algo:segment_tree",
        "//src/algo:sparse_tree",
        "//src/algo:treap",
        "@com_google_googletest//:gtest",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "benchmarks",
    srcs = [
        "sparse_tree_benchmark.cpp",
        "segment_tree_benchmark.cpp",
    ],
    deps = [
        ":tests_lib",
        "//src/algo:aho_corasick",
        "//src/algo:segment_tree",
        "//src/algo:sparse_tree",
        "//src/algo:treap",
        "@com_google_benchmark//:benchmark",
        "@com_google_benchmark//:benchmark_main",
    ],
)

