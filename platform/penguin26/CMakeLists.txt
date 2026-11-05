########################################################################################################################
# Baremetal Platform Configuration for the penguin26 tapeout chip (chipyard EE290SimConfig)
########################################################################################################################

file(GLOB_RECURSE PLATFORM_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c )
add_library(chip-config STATIC chip.c ${PLATFORM_FILES})

target_compile_options(chip-config PUBLIC ${ARCH_FLAGS} ${SPEC_FLAGS})
target_link_options(chip-config PUBLIC ${ARCH_FLAGS} ${SPEC_FLAGS})

# set terminal device (printf, scanf, etc.) based on runtime platform
string(TOUPPER "${PLATFORM}" PLATFORM_UPPER)
if (PLATFORM_UPPER STREQUAL "SIMS")
  set(TERMINAL_DEVICE_HTIF    ON     PARENT_SCOPE)
  set(TERMINAL_DEVICE_UART0   OFF    PARENT_SCOPE)
  target_compile_definitions(chip-config PUBLIC -D TERMINAL_DEVICE_HTIF)
else()
  set(TERMINAL_DEVICE_UART0   ON     PARENT_SCOPE)
  set(TERMINAL_DEVICE_HTIF    OFF    PARENT_SCOPE)
  target_compile_definitions(chip-config PUBLIC -D TERMINAL_DEVICE_UART0)
  # UART0 must be initialized before stdio works: crt0 calls __wrap_main (src/uart_stdio.c), which
  # runs uart_init() and then the program's main(), so every program prints over UART0.
  target_link_options(chip-config PUBLIC -Wl,--wrap=main)
endif()

# Shuttle has no LBR CSR (0x401): keep glossy's trap vector from touching it, otherwise every
# trap re-traps inside trap_vector and recurses until the stack overwrites memory.
target_compile_definitions(chip-config PUBLIC -D GLOSSY_NO_LBR_CSR)

set(PLATFORM_INCLUDE
  ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(chip-config PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${PLATFORM_INCLUDE})

# Drivers for the devices this chip has (see chip_config.h / the EE290SimConfig DTS):
# Shuttle core, CLINT, PLIC, HTIF (sims), SiFive UART0, TACIT trace encoder + sink DMA.
target_link_libraries(chip-config PUBLIC rocketcore)
target_link_libraries(chip-config PUBLIC clint)
target_link_libraries(chip-config PUBLIC plic)
target_link_libraries(chip-config PUBLIC htif)
target_link_libraries(chip-config PUBLIC uart)
target_link_libraries(chip-config PUBLIC l_trace_encoder)
