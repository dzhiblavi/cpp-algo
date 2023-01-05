load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")

http_archive(
    name = "com_google_googletest",
    urls = ["https://github.com/google/googletest/archive/refs/tags/release-1.12.1.zip"],
    strip_prefix = "googletest-release-1.12.1",
)

git_repository(
    name = "com_google_benchmark",
    commit = "d572f4777349d43653b21d6c2fc63020ab326db2",
    remote = "https://github.com/google/benchmark.git",
    shallow_since = "1668175263 +0000",
)

#git_repository(
    #name = "com_github_nelhage_rules_boost",
    #shallow_since = "1630079166 -0700",
    #commit = "fce83babe3f6287bccb45d2df013a309fa3194b8",
    #remote = "https://github.com/nelhage/rules_boost",
#)
#load("@com_github_nelhage_rules_boost//:boost/boost.bzl", "boost_deps")
#boost_deps()
