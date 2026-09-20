########################################################################################################################
# Baremetal Platform Configuration for BearlyML'25
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
