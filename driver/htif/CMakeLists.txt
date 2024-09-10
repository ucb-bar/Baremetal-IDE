
add_library(htif STATIC htif.c)

target_compile_definitions(htif PUBLIC -DDEVICE_TERMINAL_HTIF)

target_include_directories(htif PUBLIC ${CMAKE_SOURCE_DIR}/driver/htif)

target_link_libraries(htif PUBLIC hal-common)
