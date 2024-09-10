
add_library(hal-common INTERFACE)

target_include_directories(hal-common INTERFACE ${CMAKE_SOURCE_DIR}/driver/common)
