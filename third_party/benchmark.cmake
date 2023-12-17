include(FetchContent)
set(FETCHCONTENT_QUIET FALSE)

FetchContent_Declare(
  googlebenchmark
  SYSTEM
  GIT_REPOSITORY https://github.com/google/benchmark.git
  GIT_TAG main
)

FetchContent_MakeAvailable(googlebenchmark)
