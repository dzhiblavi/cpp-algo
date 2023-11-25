include(FetchContent)
set(FETCHCONTENT_QUIET FALSE)

FetchContent_Declare(
  googlebenchmark
  GIT_REPOSITORY https://github.com/google/benchmark.git
  GIT_TAG main
)

FetchContent_MakeAvailable(googlebenchmark)