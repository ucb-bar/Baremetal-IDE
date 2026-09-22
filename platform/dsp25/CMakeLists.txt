########################################################################################################################
# Baremetal Platform Configuration for DSP'25
########################################################################################################################

file(GLOB_RECURSE PLATFORM_FILES 
  ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c 
  ${CMAKE_CURRENT_SOURCE_DIR}/src/*.S     # Added for trap_entry.S
)
add_library(chip-config STATIC chip.c ${PLATFORM_FILES})

# Terminal device for printf/scanf. On real silicon (PLATFORM=CHIP) it's the UART. Under simulation
# (PLATFORM=SIMS, e.g. Spike) the UART MMIO isn't modeled, so route the console through HTIF (the
# fesvr tohost/fromhost proxy) instead — that's what makes printf show up under `spike`.
string(TOUPPER "${PLATFORM}" _DSP25_PLATFORM_UPPER)
if (_DSP25_PLATFORM_UPPER STREQUAL "SIMS")
  target_compile_definitions(chip-config PUBLIC -D TERMINAL_DEVICE_HTIF)
  message(STATUS "dsp25: TERMINAL_DEVICE_HTIF (SIMS/Spike console)")
else()
  set(TERMINAL_DEVICE_UART0    ON    PARENT_SCOPE)
  target_compile_definitions(chip-config PUBLIC -D TERMINAL_DEVICE_UART0)
endif()

set(PLATFORM_INCLUDE
  ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(chip-config PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${PLATFORM_INCLUDE})

target_link_libraries(chip-config PUBLIC rocketcore)
target_link_libraries(chip-config PUBLIC clint)
target_link_libraries(chip-config PUBLIC plic)
target_link_libraries(chip-config PUBLIC htif)
target_link_libraries(chip-config PUBLIC uart)
target_link_libraries(chip-config PUBLIC spi)
target_link_libraries(chip-config PUBLIC i2c)
target_link_libraries(chip-config PUBLIC gpio)
target_link_libraries(chip-config PUBLIC pwm)
target_link_libraries(chip-config PUBLIC intel-pll)
