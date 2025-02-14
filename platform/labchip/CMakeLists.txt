########################################################################################################################
# Baremetal Platform Configuration for default chip
########################################################################################################################

add_library(chip-config STATIC chip.c ${PLATFORM_FILES})

# set terminal device (printf, scanf, etc.) to HTIF
set(TERMINAL_DEVICE_UART0   ON    PARENT_SCOPE)
target_compile_definitions(chip-config PUBLIC -D TERMINAL_DEVICE_UART0)

set(PLATFORM_INCLUDE
  ${CMAKE_CURRENT_SOURCE_DIR}/inc)
target_include_directories(chip-config PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${PLATFORM_INCLUDE})


target_link_libraries(chip-config PUBLIC rocketcore)
target_link_libraries(chip-config PUBLIC clint)
target_link_libraries(chip-config PUBLIC uart)
