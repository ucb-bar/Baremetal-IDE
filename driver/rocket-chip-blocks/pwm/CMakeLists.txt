
add_library(pwm STATIC pwm.c)

target_include_directories(pwm PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(pwm PUBLIC metal)
