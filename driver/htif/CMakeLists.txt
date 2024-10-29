
add_library(htif STATIC htif.c)

target_include_directories(htif PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(htif PUBLIC metal)
