include(FetchContent)
set(FETCHCONTENT_QUIET FALSE)

set(BOOST_ENABLE_CMAKE ON)
set(BOOST_USE_STATIC_LIBS ON)
set(BOOST_USE_DEBUG_LIBS OFF)
set(BOOST_USE_RELEASE_LIBS ON)
set(BOOST_USE_MULTITHREADED ON)
set(BOOST_USE_STATIC_RUNTIME ON)
set(BOOST_NO_BOOST_CMAKE ON)

set(BOOST_INCLUDE_LIBRARIES filesystem program_options timer thread python)

FetchContent_Declare(
  Boost
  GIT_REPOSITORY https://github.com/boostorg/boost.git
  GIT_TAG boost-1.80.0
  GIT_SHALLOW TRUE
  GIT_PROGRESS TRUE)

FetchContent_MakeAvailable(Boost)
