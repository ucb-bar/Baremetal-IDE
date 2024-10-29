
add_library(spi STATIC spi.c)

target_include_directories(spi PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(spi PUBLIC metal)
