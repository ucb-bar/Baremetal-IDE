########################################################################################################################
# Baremetal Platform Configuration for BearlyML chip
########################################################################################################################


add_library(chip-bearlyml-config INTERFACE)

# BearlyML does not support serial TileLink interface
# so HTIF is not supported
set(TERMINAL_DEVICE_UART0        ON)

# set(SYS_CLK_FREQ                50000000)         # for board with 100 MHz crystal
set(SYS_CLK_FREQ                12500000)         # for board with 25 MHz crystal
set(MTIME_FREQ                  100000)

set(DEBUG_CONTROLLER_BASE       0x00000000)
set(ERROR_DEVICE_BASE           0x00003000)
set(BOOTSEL_BASE                0x00004000)
set(BOOT_SELECT_BASE            0x00004000)
set(BOOTROM_BASE                0x00010000)
set(LIBIF_ROM_BASE              0x00020000)
set(LIBIF_RAW_BASE              0x00030000)
set(PRCI_BASE                   0x00100000)
set(CLINT_BASE                  0x02000000)
set(CACHE_CONTROLLER_BASE       0x02010000)
set(SCRATCH_BASE                0x08000000)
set(PLIC_BASE                   0x0C000000)
set(GPIO_BASE                   0x10012000)
set(UART_BASE                   0x1001A000)
set(QSPI_BASE                   0x10020000)
set(I2C_BASE                    0x10024000)
set(QSPI_FLASH_BASE             0x20000000)
set(DRAM_BASE                   0x80000000)

set(GPIOA_BASE                  0x10012000)
set(UART0_BASE                  0x1001A000)
set(QSPI0_BASE                  0x10020000)
set(SPI1_BASE                   0x10021000)
set(I2C0_BASE                   0x10024000)
set(I2C1_BASE                   0x10025000)


target_compile_definitions(chip-bearlyml-config INTERFACE -D SYS_CLK_FREQ=${SYS_CLK_FREQ})
target_compile_definitions(chip-bearlyml-config INTERFACE -D MTIME_FREQ=${MTIME_FREQ})

target_compile_definitions(chip-bearlyml-config INTERFACE -D CLINT_BASE=${CLINT_BASE})
target_compile_definitions(chip-bearlyml-config INTERFACE -D PLIC_BASE=${PLIC_BASE})
target_compile_definitions(chip-bearlyml-config INTERFACE -D GPIOA_BASE=${GPIOA_BASE})
target_compile_definitions(chip-bearlyml-config INTERFACE -D UART0_BASE=${UART0_BASE})
target_compile_definitions(chip-bearlyml-config INTERFACE -D QSPI0_BASE=${QSPI0_BASE})
target_compile_definitions(chip-bearlyml-config INTERFACE -D SPI1_BASE=${SPI1_BASE})
target_compile_definitions(chip-bearlyml-config INTERFACE -D I2C0_BASE=${I2C0_BASE})
target_compile_definitions(chip-bearlyml-config INTERFACE -D I2C1_BASE=${I2C1_BASE})


target_link_libraries(chip-bearlyml-config INTERFACE clint)
target_link_libraries(chip-bearlyml-config INTERFACE plic)
target_link_libraries(chip-bearlyml-config INTERFACE gpio)
target_link_libraries(chip-bearlyml-config INTERFACE uart)
