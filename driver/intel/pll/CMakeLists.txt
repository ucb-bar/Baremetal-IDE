
add_library(intel-pll STATIC pll.c)

target_include_directories(intel-pll PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(intel-pll PUBLIC metal)
