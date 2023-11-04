cc_library(
    name = "utility",
    hdrs = [
        "utility/queue.h",
    ],
    include_prefix = "algo/graph",
    visibility = ["//visibility:public"],
)

cc_library(
    name = "repr",
    hdrs = [
        "repr/adj_list/adj_list.h",
        "repr/adj_matrix/adj_matrix.h",
        "repr/adj_matrix/repr.h",
        "repr/adj_matrix/transposed.h",
        "repr/concepts.h",
        "repr/storage_type.h",
        "repr/traits.h",
    ],
    include_prefix = "algo/graph",
    visibility = ["//visibility:public"],
    deps = [
        ":utility",
    ],
)

cc_library(
    name = "algo",
    hdrs = [
        "algo/bridges.h",
        "algo/connected.h",
        "algo/cutpoints.h",
        "algo/cycle.h",
        "algo/dijkstra.h",
        "algo/reachable.h",
        "algo/top_sort.h",
    ],
    include_prefix = "algo/graph",
    visibility = ["//visibility:public"],
    deps = [
        ":repr",
    ],
)

cc_library(
    name = "graph",
    include_prefix = "algo/graph",
    visibility = ["//visibility:public"],
    deps = [
        ":algo",
        ":repr",
    ],
)
