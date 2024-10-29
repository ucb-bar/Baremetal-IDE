#ifndef __ROBO23_H
#define __ROBO23_H

#ifdef __cplusplus
extern "C" {
#endif

#include "rv_arch.h"
#include "rv_common.h"
#include "ll_clint.h"
#include "ll_core.h"
#include "ll_gpio.h"
#include "ll_i2c.h"
#include "ll_pll.h"
#include "ll_plic.h"
#include "ll_pwm.h"
#include "ll_rcc.h"
#include "ll_spi.h"
#include "ll_uart.h"
#include "ll_switchcap.h"


/* ================================ IRQ Definition ================================ */
typedef enum {
  UserSoftwareInterrupt         = 0,
  SupervisorSoftwareInterrupt   = 1,
  HypervisorSoftwareInterrupt   = 2,
  MachineSoftwareInterrupt      = 3,
  UserTimerInterrupt            = 4,
  SupervisorTimerInterrupt      = 5,
  HypervisorTimerInterrupt      = 6,
  MachineTimerInterrupt         = 7,
  UserExternalInterrupt         = 8,
  SupervisorExternalInterrupt   = 9,
  HypervisorExternalInterrupt   = 10,
  MachineExternalInterrupt      = 11,
} InterruptType;

typedef enum {
  UART0_IRQn                ,
} PLIC_IRQn_Type;


/* ================================ Memory Map Definition ================================ */
/* Peripheral Address Definition */
#define DEBUG_CONTROLLER_BASE   0x00000000U
#define BOOTSEL_BASE            0x00001000U
#define ERROR_DEVICE_BASE       0x00003000U
#define BOOTROM_BASE            0x00010000U
#define LBWIFROM_BASE           0x00020000U
#define CLINT_BASE              0x02000000U
#define CACHE_CONTROLLER_BASE   0x02010000U
#define SCRATCH_BASE            0x08000000U
#define PLIC_BASE               0x0C000000U
#define RCC_BASE                0x10000000U
#define PLL_BASE                0x10004000U
#define SWITCHCAP_BASE          0x10004800U
#define GPIO_BASE               0x10010000U
#define UART_BASE               0x10020000U
#define QSPI_BASE               0x10030000U
#define I2C_BASE                0x10040000U
#define PWM_BASE                0x10060000U
#define QSPI_FLASH_BASE         0x20000000U
#define DRAM_BASE               0x80000000U

/* Peripheral Pointer Definition */
#define GPIOA_BASE              (GPIO_BASE)
#define GPIOB_BASE              (GPIO_BASE + 0x1000U)
#define GPIOC_BASE              (GPIO_BASE + 0x2000U)
#define UART0_BASE              (UART_BASE)
#define UART1_BASE              (UART_BASE + 0x1000U)
#define UART2_BASE              (UART_BASE + 0x2000U)
#define QSPI0_BASE              (QSPI_BASE)
#define QSPI1_BASE              (QSPI_BASE + 0x1000U)
#define QSPI2_BASE              (QSPI_BASE + 0x2000U)
#define I2C0_BASE               (I2C_BASE)
#define I2C1_BASE               (I2C_BASE + 0x1000U)
#define PWM0_BASE               (PWM_BASE)           
#define PWM1_BASE               (PWM_BASE + 0x1000U)

/* Peripheral Structure Definition */
// #define DEBUG_CONTROLLER        (DEBUG_CONTROLLER_BASE)
// #define ERROR_DEVICE            (ERROR_DEVICE_BASE)
// #define BOOTSEL                 (BOOTSEL_BASE)
// #define CACHE_CONTROLLER        (CACHE_CONTROLLER_BASE)
#define PLIC                    ((PLIC_Type *)PLIC_BASE)
#define PLIC_CC                 ((PLIC_ContextControl_Type *)(PLIC_BASE + 0x00200000U))
#define CLINT                   ((CLINT_Type *)CLINT_BASE)
#define RCC                     ((RCC_Type *)RCC_BASE)
#define PLL                     ((PLL_Type *)PLL_BASE)
#define GPIOA                   ((GPIO_Type *)GPIOA_BASE)
#define GPIOB                   ((GPIO_Type *)GPIOB_BASE)
#define GPIOC                   ((GPIO_Type *)GPIOC_BASE)
#define UART0                   ((UART_Type *)UART0_BASE)
#define UART1                   ((UART_Type *)UART1_BASE)
#define UART2                   ((UART_Type *)UART2_BASE)
#define QSPI0                   ((QSPI_Type *)QSPI0_BASE)
#define QSPI1                   ((QSPI_Type *)QSPI1_BASE)
#define QSPI2                   ((QSPI_Type *)QSPI2_BASE)
#define I2C0                    ((I2C_Type *)I2C0_BASE)
#define I2C1                    ((I2C_Type *)I2C1_BASE)
#define PWM0                    ((PWM_Type *)PWM0_BASE)
#define PWM1                    ((PWM_Type *)PWM1_BASE)
#define SWITCHCAP               ((SWITCHCAP_Type *)SWITCHCAP_BASE)


#ifdef __cplusplus
}
#endif

#endif /* __ROBO23_H */