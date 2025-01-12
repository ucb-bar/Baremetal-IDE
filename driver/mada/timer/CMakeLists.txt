
add_library(mada_timer STATIC mada_timer.c)

target_include_directories(mada_timer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(mada_timer PUBLIC rocketcore)
target_link_libraries(mada_timer PUBLIC metal)
