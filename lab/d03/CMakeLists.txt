
#################################
# Build
#################################

add_executable(hello
  src/main.c
)

target_include_directories(hello PUBLIC include)

target_link_libraries(hello PRIVATE 
  -L${CMAKE_BINARY_DIR}/glossy -Wl,--whole-archive glossy -Wl,--no-whole-archive
)

