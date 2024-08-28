
#ifndef __[chip]_H
#define __[chip]_H

#ifdef __cplusplus
extern "C" {
#endif
/* Feel free to modify header file based on memory map of chip and verify address. 
   System clock settings can be changed below.*/

/* Include custom headers from inc here. */
#include "inc/ll_rcc.h"

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
#define GPIO_BASE               0x10010000U
#define UART_BASE               0x10020000U
#define QSPI_BASE               0x10030000U
#define I2C_BASE                0x10040000U
#define PWM_BASE                0x10060000U
#define QSPI_FLASH_BASE         0x20000000U
#define DRAM_BASE               0x80000000U

/* Peripheral Pointer Definition */
#define GPIOA_BASE              (GPIO_BASE)
#define UART0_BASE              (UART_BASE)
#define QSPI0_BASE              (QSPI_BASE)
#define I2C0_BASE               (I2C_BASE)
#define PWM0_BASE               (PWM_BASE)           

/* Peripheral Structure Definition */
#define DEBUG_CONTROLLER        (DEBUG_CONTROLLER_BASE)
#define ERROR_DEVICE            (ERROR_DEVICE_BASE)
#define BOOTSEL                 (BOOTSEL_BASE)
#define CACHE_CONTROLLER        (CACHE_CONTROLLER_BASE)
#define PLIC                    ((PLIC_TypeDef *)PLIC_BASE)
#define PLIC_CC                 ((PLIC_ContextControl_TypeDef *)(PLIC_BASE + 0x00200000U))
#define CLINT                   ((CLINT_TypeDef *)CLINT_BASE)
#define RCC                     ((RCC_TypeDef *)RCC_BASE)
#define GPIOA                   ((GPIO_TypeDef *)GPIOA_BASE)
#define UART0                   ((UART_TypeDef *)UART0_BASE)
#define QSPI0                   ((QSPI_TypeDef *)QSPI0_BASE)
#define I2C0                    ((I2C_TypeDef *)I2C0_BASE)
#define PWM0                    ((PWM_TypeDef *)PWM0_BASE)

/**
 * System Clock Configuration
 */
#define HXTAL_FREQ     100000000                   /** crystal or external clock frequency in Hz */
#define SYS_CLK_FREQ  HXTAL_FREQ                /** system clock frequency in Hz */
#define MTIME_TIMEBASE      1                               /** tick per milliseconds */
#define MTIME_FREQ    (SYS_CLK_FREQ / 100000)     // tick per milliseconds


// weak definition of clock frequency
__attribute__((weak)) uint64_t sys_clk_freq = SYS_CLK_FREQ;
__attribute__((weak)) uint64_t mtime_freq = MTIME_FREQ;

#ifdef __cplusplus
}
#endif

#endif /* __[chip]_H */