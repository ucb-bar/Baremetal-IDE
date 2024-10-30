
add_library(l_trace_encoder STATIC l_trace_encoder.c)

target_include_directories(l_trace_encoder PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(l_trace_encoder PUBLIC rocketcore)
target_link_libraries(l_trace_encoder PUBLIC metal)
