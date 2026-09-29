#################################
# Build
#################################

add_executable(blinky
  main.c
)

target_compile_options(blinky PRIVATE -O0)

target_link_libraries(blinky PRIVATE
  -L${CMAKE_BINARY_DIR}/glossy -Wl,--whole-archive glossy -Wl,--no-whole-archive
)
