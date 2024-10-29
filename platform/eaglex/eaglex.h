
#ifndef __EAGLEX_H
#define __EAGLEX_H

#ifdef __cplusplus
extern "C" {
#endif

#include "rv_arch.h"
#include "rv.h"
#include "uart.h"



/* IRQ Definition */
typedef enum {
  UserSoftware_IRQn         = 0,
  SupervisorSoftware_IRQn   = 1,
  HypervisorSoftware_IRQn   = 2,
  MachineSoftware_IRQn      = 3,
  UserTimer_IRQn            = 4,
  SupervisorTimer_IRQn      = 5,
  HypervisorTimer_IRQn      = 6,
  MachineTimer_IRQn         = 7,
  UserExternal_IRQn         = 8,
  SupervisorExternal_IRQn   = 9,
  HypervisorExternal_IRQn   = 10,
  MachineExternal_IRQn      = 11,
} InterruptType;



/* Memory Map Definition */
#define GPIO_BASE               0x00009000U
#define SPI_BASE                0x0000A000U
#define I2C_BASE                0x0000B000U
#define UART_BASE               0x0000C000U
#define PLL_BASE                0x00101000U
#define PRCI_BASE               0x00110000U
#define CLINT_BASE              0x02000000U
#define PLIC_BASE               0x0C000000U
#define SCRATCH_BASE            0x50000000U
#define DRAM_BASE               0x80000000U

/* Peripheral Pointer Definition */
#define GPIOA_BASE              (GPIO_BASE + 0x0000U)
#define SPI0_BASE               (SPI_BASE + 0x0000U)
#define I2C0_BASE               (I2C_BASE + 0x0000U)
#define UART0_BASE              (UART_BASE + 0x0000U)

#define UART0                   ((UART_Type *)UART0_BASE)

/**
 * System Clock Configuration
 */
#define HXTAL_FREQ          100000000                       /** crystal or external clock frequency in Hz */
#define SYS_CLK_FREQ        HXTAL_FREQ                      /** system clock frequency in Hz */
#define MTIME_FREQ          (SYS_CLK_FREQ / 100)            /** mtime clock frequency in Hz */

// weak definition of clock frequency
__attribute__((weak)) uint64_t sys_clk_freq = SYS_CLK_FREQ;
__attribute__((weak)) uint64_t mtime_freq = MTIME_FREQ;


#ifdef __cplusplus
}
#endif

#endif /* __EAGLEX_H */