cc_library(
    name = "utility",
    hdrs = [
        "utility/fixture.h",
        "utility/graph_types.h",
        "utility/small_graphs.h",
    ],
    include_prefix = "test/graph",
    deps = [
        "//src/algo/graph",
        "//src/test:utility",
        "@com_google_googletest//:gtest",
    ],
)

cc_test(
    name = "unit_tests",
    srcs = [
        "algo/bridges_test.cpp",
        "algo/cutpoints_test.cpp",
        "algo/connected_test.cpp",
        "algo/reachable_test.cpp",
        "algo/top_sort_test.cpp",
        "algo/cycle_test.cpp",
        "algo/dijkstra_test.cpp",
        # "repr/adj_matrix_test.cpp",
    ],
    deps = [
        ":utility",
        "//src/algo/graph:algo",
        "@com_google_googletest//:gtest_main",
    ],
)
