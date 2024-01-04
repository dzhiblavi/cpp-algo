add_library(compile_flags INTERFACE)

target_compile_options(
  compile_flags
  INTERFACE -Wall
            -Wextra
            -Wno-array-bounds
            # -Wformat=2 -Wswitch-default -Wcast-align -Wpointer-arith
            # -Wbad-function-cast -Wstrict-prototypes -Wundef -Wnested-externs
            # -Wcast-qual -Wshadow -Wwrite-strings -Wconversion
            # -Wunreachable-code -Wstrict-aliasing=2 -ffloat-store -fno-common
            # -fstrict-aliasing -pedantic
)
