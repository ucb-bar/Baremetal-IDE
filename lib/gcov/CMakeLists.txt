add_library(gcov STATIC gcov_public.c gcov_printf.c gcov_gcc.c)

target_include_directories(gcov PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
