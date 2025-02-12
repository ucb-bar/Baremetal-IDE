
#################################
# Build
#################################

add_executable(encoders 
  src/main.c
)

target_include_directories(encoders PUBLIC include)

target_link_libraries(encoders PRIVATE 
  -L${CMAKE_BINARY_DIR}/glossy -Wl,--whole-archive glossy -Wl,--no-whole-archive -Wl,--defsym=__boot_hart=$2 -Wl,--defsym=__main=main
  m
)
