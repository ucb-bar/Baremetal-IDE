
add_library(rocketcore STATIC rocketcore.c)

target_include_directories(rocketcore PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rocketcore PUBLIC metal)