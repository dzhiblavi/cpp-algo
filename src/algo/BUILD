cc_library(
    name = "utility",
    hdrs = [
        "utility/nd_container.h",
        "utility/nd_helpers.h",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "common",
    hdrs = [
        "common/types.h",
    ],
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
