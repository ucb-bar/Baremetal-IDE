#ifndef __CHIP_CONFIG_H
#define __CHIP_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#include "riscv.h"
#include "clint.h"
#include "plic.h"
#include "htif.h"
#include "spi.h"
#include "i2c.h"
#include "uart.h"
#include "gpio.h"
#include "pwm.h"
#include "pll.h"

// ================================
//  Platform Drivers
// ================================
#include "hal_mmio.h"
#include "hal_ope.h"


// ================================
//  System Clock
// ================================
// system clock frequency in Hz
#define SYS_CLK_FREQ   50000000

// CLINT time base frequency in Hz
#define MTIME_FREQ     50000


// ================================
//  MMIO devices
// ================================
// #define DEBUG_CONTROLLER_BASE   0x00000000U
// #define ERROR_DEVICE_BASE       0x00003000U
// #define BOOTSEL_BASE            0x00004000U
// #define BOOT_SELECT_BASE        0x00004000U
// #define LIBIF_ROM_BASE          0x00020000U
// #define LIBIF_RAW_BASE          0x00030000U
#define RCC_BASE                 0x00130000U
#define CLINT_BASE               0x02000000U
// #define CACHE_CONTROLLER_BASE   0x02010000U
#define PLIC_BASE                0x0C000000U
#define GPIO_BASE                0x10010000U
#define GPIOC_BASE                0x10012000U
#define UART_BASE                0x10020000U
#define QSPI_BASE                0x10030000U
#define I2C_BASE                 0x10040000U
#define PLL_BASE                 0x00140000U
// #define TCM_BASE                 0x78000000U
#define TCM_BASE                 0x08010000U

#define RCC_CLOCK_SELECTOR       ((ClockSel_Type*)(RCC_BASE))
#define CLOCK_SELECTOR          ((ClockSel_Type*)RCC_CLOCK_SELECTOR)
#define CLINT                    ((CLINT_Type *)CLINT_BASE)
#define PLIC                     ((PLIC_Type *)PLIC_BASE)
#define PLIC_CC                  ((PLIC_ContextControl_Type *)(PLIC_BASE + 0x00200000U))

#define PLL                      ((PLL_Type *)PLL_BASE)

#define UART0_BASE               (UART_BASE)
#define UART0                    ((UART_Type *)UART0_BASE)
#define UART1_BASE               0x10021000U
#define UART1                    ((UART_Type *)UART1_BASE)

#define CONV2D_BASE              0x08808000U
#define CONV2D                   ((Conv2D_Accel_Type *)CONV2D_BASE)

#define GPIOA_BASE              (GPIO_BASE)
#define GPIOA                   ((GPIO_Type *)GPIOA_BASE)
// #define GPIOC_BASE              (GPIOC_BASE)
#define GPIOC                   ((GPIO_Type *)GPIOC_BASE)

#define QSPI0_BASE              (QSPI_BASE)
#define QSPI0                   ((QSPI_Type *)QSPI0_BASE)

#define I2C0_BASE               (I2C_BASE)
#define I2C0                    ((I2C_Type *)I2C0_BASE)


#ifdef __cplusplus
}
#endif

#endif // __CHIP_CONFIG_H