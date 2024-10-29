
add_library(plic STATIC plic.c)

target_include_directories(plic PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(plic PUBLIC metal)
