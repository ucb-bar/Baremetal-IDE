
#################################
# Build
#################################

add_executable(blinky-2
  src/main.c
)

target_include_directories(blinky-2 PUBLIC include)

target_link_libraries(blinky-2 PRIVATE 
  -L${CMAKE_BINARY_DIR}/glossy -Wl,--whole-archive glossy -Wl,--no-whole-archive
)

