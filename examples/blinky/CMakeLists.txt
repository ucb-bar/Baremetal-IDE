
#################################
# Build
#################################

add_executable(blinky
  src/main.c
)

target_include_directories(blinky PUBLIC inc)

target_link_libraries(blinky PRIVATE 
  -L${CMAKE_BINARY_DIR}/glossy -Wl,--whole-archive glossy -Wl,--no-whole-archive
)

