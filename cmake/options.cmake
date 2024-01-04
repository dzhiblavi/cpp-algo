add_library(dlib-options INTERFACE)
add_library(dlib::options ALIAS dlib-options)

include(compile_flags)
target_link_libraries(dlib-options INTERFACE compile_flags)

if(DLIB_CODE_COVERAGE)
  include(coverage)
  target_link_libraries(dlib-options INTERFACE coverage_flags)

  add_custom_target(
    gcov
    COMMAND ${CMAKE_MAKE_PROGRAM} test
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

  # Create the lcov target. Run result tests with 'make lcov'
  add_custom_target(lcov COMMAND mkdir -p lcoverage)
  add_custom_command(
    TARGET lcov
    COMMAND echo "=================== LCOV ===================="
    COMMAND echo "-- Passing lcov tool under code coverage"
    COMMAND lcov --gcov /usr/bin/gcov-11 --capture --directory ../ --output-file
            lcoverage/main_coverage.info
    COMMAND echo "-- Generating HTML output files"
    COMMAND genhtml lcoverage/main_coverage.info --output-directory lcoverage)

  # Make sure to clean up the coverage folder
  set_property(
    DIRECTORY
    APPEND
    PROPERTY ADDITIONAL_MAKE_CLEAN_FILES gcoverage)

  # Create the gcov-clean target. This cleans the build as well as generated
  # .gcda and .gcno files.
  add_custom_target(
    init
    COMMAND ${CMAKE_MAKE_PROGRAM} clean
    COMMAND rm -f ${OBJECT_DIR}/*.gcno
    COMMAND rm -f ${OBJECT_DIR}/*.gcda
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
endif()
