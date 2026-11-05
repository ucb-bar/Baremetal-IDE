# CMakeLists definitions for target `atlas_example`: a minimal Atlas NPU test to copy when writing a
# new penguin26 test (see README "Writing a new test").

#################################
# Build Configuration
#################################

# Source Files (src/sum.S is the Atlas program in source form, for reference; it is not compiled)
add_executable(atlas_example
  src/main.c
)

# Header Files
target_include_directories(atlas_example PUBLIC include)

#################################
# Dependencies
#################################

# glossy: startup code, stdio (HTIF for PLATFORM=SIMS, UART0 for PLATFORM=CHIP) and exit
target_link_libraries(atlas_example PRIVATE
  -L${CMAKE_BINARY_DIR}/glossy -Wl,--whole-archive glossy -Wl,--no-whole-archive
)

# chip-config: chip_config.h (addresses) and the chip drivers, including hal_atlas
target_link_libraries(atlas_example PRIVATE chip-config)
target_link_libraries(atlas_example PRIVATE rocketcore)

if (PROF_COV)
  target_link_libraries(atlas_example PRIVATE gcov)
endif()
