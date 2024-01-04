include(FetchContent)
set(FETCHCONTENT_QUIET FALSE)

FetchContent_Declare(
  googlebenchmark
  SYSTEM
  GIT_REPOSITORY https://github.com/google/benchmark.git
  GIT_TAG main)

set(BENCHMARK_ENABLE_TESTING OFF)
FetchContent_MakeAvailable(googlebenchmark)
