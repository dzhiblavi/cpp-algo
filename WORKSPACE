load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")

http_archive(
    name = "com_google_googletest",
    sha256 = "24564e3b712d3eb30ac9a85d92f7d720f60cc0173730ac166f27dda7fed76cb2",
    strip_prefix = "googletest-release-1.12.1",
    urls = ["https://github.com/google/googletest/archive/refs/tags/release-1.12.1.zip"],
)

git_repository(
    name = "com_google_benchmark",
    commit = "d572f4777349d43653b21d6c2fc63020ab326db2",
    remote = "https://github.com/google/benchmark.git",
    shallow_since = "1668175263 +0000",
)

git_repository(
    name = "ranges-v3",
    commit = "a81477931a8aa2ad025c6bda0609f38e09e4d7ec",
    remote = "https://github.com/ericniebler/range-v3.git",
    shallow_since = "1655847311 -0700",
)
