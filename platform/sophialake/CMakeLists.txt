########################################################################################################################
# Baremetal Platform Configuration for SophiaLake (RocketSophiaLakeConfig soft core)
########################################################################################################################

add_library(chip-config STATIC chip.c)

# set terminal device (printf, scanf, etc.) to UART0, on the FT-LINK in PMOD J7
set(TERMINAL_DEVICE_UART0   ON    PARENT_SCOPE)
target_compile_definitions(chip-config PUBLIC -D TERMINAL_DEVICE_UART0)

target_include_directories(chip-config PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(chip-config PUBLIC rocketcore)
target_link_libraries(chip-config PUBLIC clint)
target_link_libraries(chip-config PUBLIC plic)
target_link_libraries(chip-config PUBLIC uart)
