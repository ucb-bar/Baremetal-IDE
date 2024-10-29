
add_library(poweroff STATIC poweroff.c)

target_include_directories(poweroff PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(poweroff PUBLIC metal)
