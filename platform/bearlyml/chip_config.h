#ifndef __CHIP_CONFIG_H
#define __CHIP_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#include "riscv.h"
#include "clint.h"
#include "spi.h"
#include "i2c.h"
#include "uart.h"


// ================================
//  System Clock
// ================================
// system clock frequency in Hz
#define SYS_CLK_FREQ   (100000000 / 2)

// CLINT time base frequency in Hz
#define MTIME_FREQ     100


// ================================
//  MMIO devices
// ================================
#define DEBUG_CONTROLLER_BASE   0x00000000U
#define ERROR_DEVICE_BASE       0x00003000U
#define BOOTSEL_BASE            0x00004000U
#define BOOT_SELECT_BASE        0x00004000U
#define BOOTROM_BASE            0x00010000U
#define LIBIF_ROM_BASE          0x00020000U
#define LIBIF_RAW_BASE          0x00030000U
#define RCC_BASE                0x00100000U
#define CLINT_BASE              0x02000000U
#define CACHE_CONTROLLER_BASE   0x02010000U
#define SCRATCH_BASE            0x08000000U
#define PLIC_BASE               0x0C000000U
#define GPIO_BASE               0x10012000U
#define UART_BASE               0x1001A000U
#define QSPI_BASE               0x10020000U
#define I2C_BASE                0x10024000U
#define QSPI_FLASH_BASE         0x20000000U
#define DRAM_BASE               0x80000000U

/* Peripheral Pointer Definition */
#define GPIOA_BASE              (GPIO_BASE)
#define UART0_BASE              (UART_BASE)
#define QSPI0_BASE              (QSPI_BASE)
#define SPI1_BASE               (QSPI_BASE + 0x1000U)
#define I2C0_BASE               (I2C_BASE)
#define I2C1_BASE               (I2C_BASE + 0x1000U)

/* Peripheral Structure Definition */
#define RCC                     ((RCC_Type *)RCC_BASE)
#define PLL                     ((PLL_Type *)PLL_BASE)
#define CLINT                   ((CLINT_Type *)CLINT_BASE)
#define PLIC                    ((PLIC_Type *)PLIC_BASE)
#define PLIC_CC                 ((PLIC_ContextControl_Type *)(PLIC_BASE + 0x00200000U))
#define GPIOA                   ((GPIO_Type *)GPIOA_BASE)
#define UART0                   ((UART_Type *)UART0_BASE)
#define QSPI0                   ((QSPI_Type *)QSPI0_BASE)
#define SPI1                    ((SPI_Type *)SPI1_BASE)
#define I2C0                    ((I2C_Type *)I2C0_BASE)
#define I2C1                    ((I2C_Type *)I2C1_BASE)


#ifdef __cplusplus
}
#endif

#endif // __CHIP_CONFIG_H
