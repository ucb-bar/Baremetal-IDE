
add_library(ns16550a STATIC ns16550a.S uart.c)

target_include_directories(ns16550a PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(ns16550a PUBLIC metal)
