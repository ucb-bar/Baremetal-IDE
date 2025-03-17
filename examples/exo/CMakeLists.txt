add_library(rvv_matmul STATIC src/rvv_matmul.c)

add_executable(exo src/main.c)

target_link_libraries(exo 
  PUBLIC rvv_matmul
  PUBLIC l_trace_encoder
  PRIVATE -L${CMAKE_BINARY_DIR}/glossy -Wl,--whole-archive glossy -Wl,--no-whole-archive
)
