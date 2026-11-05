# CMakeLists definitions for target `hello_world`.

#################################
# Build Configuration
#################################

# Source Files
add_executable(hello_world
  src/main.c
)

# Header Files
target_include_directories(hello_world PUBLIC include)

#################################
# Dependencies
#################################

target_link_libraries(hello_world PRIVATE
  -L${CMAKE_BINARY_DIR}/glossy -Wl,--whole-archive glossy -Wl,--no-whole-archive
)

target_link_libraries(hello_world PRIVATE chip-config)
target_link_libraries(hello_world PRIVATE rocketcore)

if (PROF_COV)
  target_link_libraries(hello_world PRIVATE gcov)
endif()
