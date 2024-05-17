
#ifndef __EXAMPLECHIP_H
#define __EXAMPLECHIP_H

#ifdef __cplusplus
extern "C" {
#endif

#include "rv_arch.h"
#include "rv_common.h"
#include "ll_clint.h"
#include "ll_core.h"
#include "ll_gpio.h"
#include "ll_i2c.h"
#include "ll_plic.h"
#include "ll_pwm.h"
#include "ll_spi.h"
#include "ll_uart.h"


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

// Interrupt map (2 harts 45 interrupts):
//   [1, 1] => uart_0
//   [2, 2] => uart_1
//   [3, 3] => uart_2
//   [4, 27] => gpio_0
//   [28, 43] => gpio_1
//   [44, 45] => gpio_2

typedef enum {
  NO_IRQn,
  UART0_IRQn,
  UART1_IRQn,
  UART2_IRQn,
  GPIOA_PIN0_IRQn,
  GPIOA_PIN1_IRQn,
  GPIOA_PIN2_IRQn,
  GPIOA_PIN3_IRQn,
  GPIOA_PIN4_IRQn,
  GPIOA_PIN5_IRQn,
  GPIOA_PIN6_IRQn,
  GPIOA_PIN7_IRQn,
  GPIOA_PIN8_IRQn,
  GPIOA_PIN9_IRQn,
  GPIOA_PIN10_IRQn,
  GPIOA_PIN11_IRQn,
  GPIOA_PIN12_IRQn,
  GPIOA_PIN13_IRQn,
  GPIOA_PIN14_IRQn,
  GPIOA_PIN15_IRQn,
  GPIOA_PIN16_IRQn,
  GPIOA_PIN17_IRQn,
  GPIOA_PIN18_IRQn,
  GPIOA_PIN19_IRQn,
  GPIOA_PIN20_IRQn,
  GPIOA_PIN21_IRQn,
  GPIOA_PIN22_IRQn,
  GPIOA_PIN23_IRQn,
  GPIOB_PIN0_IRQn,
  GPIOB_PIN1_IRQn,
} PLIC_IRQn_Type;

/* ================================ Memory Map Definition ================================ */
/* Peripheral Address Definition */
#define DEBUG_CONTROLLER_BASE   0x00000000U
#define BOOT_SELECT_BASE        0x00001000U
#define ERROR_DEVICE_BASE       0x00003000U
#define BOOTROM_BASE            0x00010000U
#define CLINT_BASE              0x02000000U
#define SCRATCH_BASE            0x08000000U
#define PLIC_BASE               0x0C000000U
#define RCC_BASE                0x10000000U
#define GPIO_BASE               0x10010000U
#define UART_BASE               0x10020000U
#define QSPI_BASE               0x10030000U
#define I2C_BASE                0x10040000U
#define PWM_BASE                0x10050000U
#define QSPI_FLASH_BASE         0x20000000U
#define DRAM_BASE               0x80000000U

/* Peripheral Pointer Definition */
#define GPIOA_BASE              (GPIO_BASE + 0x0000U)
#define GPIOB_BASE              (GPIO_BASE + 0x1000U)
#define GPIOC_BASE              (GPIO_BASE + 0x2000U)
#define UART0_BASE              (UART_BASE + 0x0000U)
#define UART1_BASE              (UART_BASE + 0x1000U)
#define QSPI0_BASE              (QSPI_BASE + 0x0000U)
#define SPI1_BASE               (QSPI_BASE + 0x1000U)
#define I2C0_BASE               (I2C_BASE + 0x0000U)
#define I2C1_BASE               (I2C_BASE + 0x1000U)

/* Peripheral Structure Definition */
// #define DEBUG_CONTROLLER        ((DEBUG_CONTROLLER_TypeDef *)DEBUG_CONTROLLER_BASE)
// #define ERROR_DEVICE            ((ERROR_DEVICE_TypeDef *)ERROR_DEVICE_BASE)
// #define BOOT_SEL                ((BOOT_SEL_TypeDef *)BOOT_SEL_BASE)
#define CLINT                   ((CLINT_TypeDef *)CLINT_BASE)
#define PLIC                    ((PLIC_TypeDef *)PLIC_BASE)
#define PLIC_CC                 ((PLIC_ContextControl_TypeDef *)(PLIC_BASE + 0x00200000U))
// #define RCC                     ((RCC_TypeDef *)RCC_BASE)
#define GPIOA                   ((GPIO_TypeDef *)GPIOA_BASE)
#define GPIOB                   ((GPIO_TypeDef *)GPIOB_BASE)
#define GPIOC                   ((GPIO_TypeDef *)GPIOC_BASE)
#define UART0                   ((UART_TypeDef *)UART0_BASE)
#define UART1                   ((UART_TypeDef *)UART1_BASE)
#define QSPI0                   ((QSPI_TypeDef *)QSPI0_BASE)
#define SPI1                    ((SPI_TypeDef *)SPI1_BASE)
#define I2C0                    ((I2C_TypeDef *)I2C0_BASE)
#define I2C1                    ((I2C_TypeDef *)I2C1_BASE)


#ifdef __cplusplus
}
#endif

#endif /* __EXAMPLECHIP_H */
