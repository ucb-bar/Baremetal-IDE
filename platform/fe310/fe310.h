
#ifndef __FE310_H
#define __FE310_H

#ifdef __cplusplus
extern "C" {
#endif

#include <rv/arch.h>
#include <rv.h>
#include "gpio.h"
// #include "i2c.h"
// #include "pwm.h"
// #include "spi.h"
#include "uart.h"

/* ================================ IRQ Definition ================================ */

typedef enum {
  AON_IRQn                      = 1,
  AON_RTC_IRQn                  = 2,
  UART0_IRQn                    = 3,
  UART1_IRQn                    = 4,
  QSPI0_IRQn                    = 5,
  SPI1_IRQn                     = 6,
  SPI2_IRQn                     = 7,
  GPIOA_0_IRQn                  = 8,
  GPIOA_1_IRQn                  = 9,
  GPIOA_2_IRQn                  = 10,
  GPIOA_3_IRQn                  = 11,
  GPIOA_4_IRQn                  = 12,
  GPIOA_5_IRQn                  = 13,
  GPIOA_6_IRQn                  = 14,
  GPIOA_7_IRQn                  = 15,
  GPIOA_8_IRQn                  = 16,
  GPIOA_9_IRQn                  = 17,
  GPIOA_10_IRQn                 = 18,
  GPIOA_11_IRQn                 = 19,
  GPIOA_12_IRQn                 = 20,
  GPIOA_13_IRQn                 = 21,
  GPIOA_14_IRQn                 = 22,
  GPIOA_15_IRQn                 = 23,
  GPIOA_16_IRQn                 = 24,
  GPIOA_17_IRQn                 = 25,
  GPIOA_18_IRQn                 = 26,
  GPIOA_19_IRQn                 = 27,
  GPIOA_20_IRQn                 = 28,
  GPIOA_21_IRQn                 = 29,
  GPIOA_22_IRQn                 = 30,
  GPIOA_23_IRQn                 = 31,
  GPIOA_24_IRQn                 = 32,
  GPIOA_25_IRQn                 = 33,
  GPIOA_26_IRQn                 = 34,
  GPIOA_27_IRQn                 = 35,
  GPIOA_28_IRQn                 = 36,
  GPIOA_29_IRQn                 = 37,
  GPIOA_30_IRQn                 = 38,
  GPIOA_31_IRQn                 = 39,
  PWM0_0_IRQn                   = 40,
  PWM0_1_IRQn                   = 41,
  PWM0_2_IRQn                   = 42,
  PWM0_3_IRQn                   = 43,
  PWM1_0_IRQn                   = 44,
  PWM1_1_IRQn                   = 45,
  PWM1_2_IRQn                   = 46,
  PWM1_3_IRQn                   = 47,
  PWM2_0_IRQn                   = 48,
  PWM2_1_IRQn                   = 49,
  PWM2_2_IRQn                   = 50,
  PWM2_3_IRQn                   = 51,
  I2C0_IRQn                     = 52,
} PLIC_IRQn_Type;


/* ================================ Memory Map Definition ================================ */
/* Peripheral Address Definition */
#define DEBUG_CONTROLLER_BASE   0x00000000U
#define MODE_SELECT_BASE        0x00001000U
#define ERROR_DEVICE_BASE       0x00003000U
#define BOOTROM_BASE            0x00010000U
#define OTP_BASE                0x00020000U
#define CLINT_BASE              0x02000000U
#define ITIM_BASE               0x08000000U
#define PLIC_BASE               0x0C000000U
#define AON_BASE                0x10000000U
#define PRCI_BASE               0x10008000U
#define OPT_CTRL_BASE           0x10010000U
#define GPIO_BASE               0x10012000U
#define UART_BASE               0x10013000U
#define QSPI_BASE               0x10014000U
#define PWM_BASE                0x10015000U
#define I2C_BASE                0x10016000U
#define QSPI_FLASH_BASE         0x20000000U
#define DTIM_BASE               0x80000000U

/* Peripheral Pointer Definition */
#define GPIOA_BASE              (GPIO_BASE)
#define UART0_BASE              (UART_BASE)
#define UART1_BASE              (UART_BASE + 0x10000U)
#define QSPI0_BASE              (QSPI_BASE)
#define SPI1_BASE               (QSPI_BASE + 0x10000U)
#define SPI2_BASE               (QSPI_BASE + 0x20000U)
#define PWM0_BASE               (PWM_BASE)
#define PWM1_BASE               (PWM_BASE + 0x10000U)
#define PWM2_BASE               (PWM_BASE + 0x20000U)
#define I2C0_BASE               (I2C_BASE)

/* Peripheral Structure Definition */
// #define DEBUG_CONTROLLER        ((DEBUG_CONTROLLER_Type *)DEBUG_CONTROLLER_BASE)
// #define MODE_SELECT             ((BOOTSEL_Type *)MODE_SELECT_BASE)
// #define ERROR_DEVICE            ((ERROR_DEVICE_Type *)ERROR_DEVICE_BASE)
#define GPIOA                   ((GPIO_Type *)GPIOA_BASE)
#define UART0                   ((UART_Type *)UART0_BASE)
#define UART1                   ((UART_Type *)UART1_BASE)
#define UART2                   ((UART_Type *)UART2_BASE)
#define QSPI0                   ((QSPI_Type *)QSPI0_BASE)
#define SPI1                    ((SPI_Type *)SPI1_BASE)
#define SPI2                    ((SPI_Type *)SPI2_BASE)
#define PWM0                    ((PWM_Type *)PWM0_BASE)
#define PWM1                    ((PWM_Type *)PWM1_BASE)
#define PWM2                    ((PWM_Type *)PWM2_BASE)
#define I2C0                    ((I2C_Type *)I2C0_BASE)

/**
 * System Clock Configuration
 */
#define HXTAL_FREQ          18000000                        /** crystal or external clock frequency in Hz */
#define SYS_CLK_FREQ        HXTAL_FREQ                      /** system clock frequency in Hz */
#define MTIME_TIMEBASE      32768                           /** tick per second */
#define MTIME_FREQ          MTIME_TIMEBASE


#ifdef __cplusplus
}
#endif

#endif /* __FE310_H */
