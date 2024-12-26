add_library(lbr STATIC lbr.c)

target_include_directories(lbr PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(lbr PUBLIC rocketcore)