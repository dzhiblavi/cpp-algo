add_library(coverage_flags INTERFACE)

target_compile_options(
  coverage_flags
  INTERFACE -O0 # no optimization
            -g # generate debug info
            --coverage # sets all required flags
)

target_link_options(
  coverage_flags INTERFACE --coverage # sets all required flags
)
