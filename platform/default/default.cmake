########################################################################################################################
# Baremetal Platform Configuration for default chip
########################################################################################################################

add_library(chip-default-config INTERFACE)

# set terminal device (printf, scanf, etc.) to HTIF
set(TERMINAL_DEVICE_HTIF         ON)

# ================================
#  System Clock
# ================================
set(SYS_CLK_FREQ   100000000)               # set system clock frequency to 100 MHz
set(MTIME_FREQ     10000000)                # set CLINT time base frequency to 10 MHz


# ================================
#  MMIO devices
# ================================
set(CLINT_BASE                  0x02000000)
set(PLIC_BASE                   0x0C000000)


target_compile_definitions(chip-default-config INTERFACE -D SYS_CLK_FREQ=${SYS_CLK_FREQ})
target_compile_definitions(chip-default-config INTERFACE -D MTIME_FREQ=${MTIME_FREQ})

target_compile_definitions(chip-default-config INTERFACE -D CLINT_BASE=${CLINT_BASE})
target_compile_definitions(chip-default-config INTERFACE -D PLIC_BASE=${PLIC_BASE})

target_link_libraries(chip-default-config INTERFACE clint)
target_link_libraries(chip-default-config INTERFACE htif)

