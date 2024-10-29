
add_library(gpio STATIC gpio.c)

target_include_directories(gpio PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(gpio PUBLIC metal)
