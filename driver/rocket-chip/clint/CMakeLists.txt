
add_library(rocket-chip-clint STATIC clint.c)

target_include_directories(rocket-chip-clint PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rocket-chip-clint PUBLIC hal-common)
