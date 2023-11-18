cc_library(
    name = "utility",
    hdrs = [
        "utility/nd_container.h",
        "utility/nd_helpers.h",
    ],
    include_prefix = "algo",
    visibility = ["//visibility:public"],
    deps = [
        "//src/utility",
    ],
)

cc_library(
    name = "common",
    hdrs = [
        "common/types.h",
    ],
    include_prefix = "algo",
    visibility = ["//visibility:public"],
)

cc_library(
    name = "treap",
    srcs = [
        "treap/node.cpp",
    ],
    hdrs = [
        "treap/adapter.h",
        "treap/node.h",
        "treap/treap.h",
    ],
    include_prefix = "algo",
    visibility = ["//visibility:public"],
)

cc_library(
    name = "aho_corasick",
    hdrs = [
        "aho_corasick/aho_corasick.h",
    ],
    include_prefix = "algo",
    visibility = ["//visibility:public"],
)

cc_library(
    name = "segment_tree",
    hdrs = [
        "segment_tree/segment_tree.h",
    ],
    include_prefix = "algo",
    visibility = ["//visibility:public"],
)

cc_library(
    name = "sparse_tree",
    hdrs = [
        "sparse_tree/sparse_tree.h",
    ],
    include_prefix = "algo",
    visibility = ["//visibility:public"],
)

cc_library(
    name = "fenwick_tree",
    hdrs = [
        "fenwick_tree/fenwick_tree.h",
    ],
    include_prefix = "algo",
    visibility = ["//visibility:public"],
    deps = [
        ":common",
        ":utility",
    ],
)

cc_library(
    name = "leftist_heap",
    hdrs = [
        "leftist_heap/leftist_heap.h",
    ],
    include_prefix = "algo",
    visibility = ["//visibility:public"],
)

cc_library(
    name = "kdtree",
    hdrs = [
        "kdtree/kdtree.h",
    ],
    include_prefix = "algo",
    visibility = ["//visibility:public"],
)

cc_library(
    name = "dsu",
    hdrs = [
        "dsu/dsu.hpp",
    ],
    include_prefix = "algo",
    visibility = ["//visibility:public"],
)

cc_library(
    name = "min_stack",
    hdrs = [
        "min_stack/min_queue.hpp",
        "min_stack/min_stack.hpp",
    ],
    include_prefix = "algo",
    visibility = ["//visibility:public"],
)
