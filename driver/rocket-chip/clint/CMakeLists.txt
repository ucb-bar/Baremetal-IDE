
add_library(clint STATIC clint.c)

target_include_directories(clint PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(clint PUBLIC metal)

message(STATUS "Including CLINT to target")

set(CLINT_ENABLED ON)
