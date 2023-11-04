load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")

http_archive(
    name = "com_google_googletest",
    strip_prefix = "googletest-release-1.12.1",
    urls = ["https://github.com/google/googletest/archive/refs/tags/release-1.12.1.zip"],
)

git_repository(
    name = "com_google_benchmark",
    commit = "d572f4777349d43653b21d6c2fc63020ab326db2",
    remote = "https://github.com/google/benchmark.git",
    shallow_since = "1668175263 +0000",
)

# git_repository(
#     name = "com_github_nelhage_rules_boost",
#     commit = "4ab574f9a84b42b1809978114a4664184716f4bf",
#     remote = "https://github.com/nelhage/rules_boost",
#     shallow_since = "1630079166 -0700",
# )
# load("@com_github_nelhage_rules_boost//:boost/boost.bzl", "boost_deps")
# boost_deps()

http_archive(
    name = "com_github_nelhage_rules_boost",
    strip_prefix = "rules_boost-4ab574f9a84b42b1809978114a4664184716f4bf",
    url = "https://github.com/nelhage/rules_boost/archive/4ab574f9a84b42b1809978114a4664184716f4bf.tar.gz",
)

load("@com_github_nelhage_rules_boost//:boost/boost.bzl", "boost_deps")

boost_deps()
