# L_FLAGS += -l
#################################
# Build
#################################

add_executable(wikisort
  src/main.c
)

add_library(beebsc STATIC src/beebsc.c)

target_link_libraries(wikisort PUBLIC beebsc)
target_link_libraries(wikisort PUBLIC l_trace_encoder)

target_include_directories(wikisort PUBLIC inc)
target_include_directories(beebsc PUBLIC inc)

target_link_libraries(wikisort PRIVATE 
-L${CMAKE_BINARY_DIR}/glossy -Wl,--whole-archive glossy -Wl,--no-whole-archive -lm
)

