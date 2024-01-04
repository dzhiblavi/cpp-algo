FetchContent_Declare(
  glog
  URL https://github.com/google/glog/archive/refs/tags/v0.6.0.zip
  GIT_SHALLOW TRUE
  GIT_PROGRESS TRUE)

FetchContent_MakeAvailable(glog)
