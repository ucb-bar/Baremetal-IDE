
#ifndef __EAGLEX_H
#define __EAGLEX_H

#ifdef __cplusplus
extern "C" {
#endif

#include "rv_arch.h"
#include "rv_common.h"
#include "ll_clint.h"
#include "ll_core.h"
#include "ll_gpio.h"
#include "ll_plic.h"
#include "ll_rcc.h"
#include "ll_uart.h"



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
#define RCC_BASE                0x00110000U
#define CLINT_BASE              0x02000000U
#define PLIC_BASE               0x0C000000U
#define SCRATCH_BASE            0x50000000U
#define DRAM_BASE               0x80000000U

/* Peripheral Pointer Definition */
#define GPIOA_BASE              (GPIO_BASE + 0x0000U)
#define SPI0_BASE               (SPI_BASE + 0x0000U)
#define I2C0_BASE               (I2C_BASE + 0x0000U)
#define UART0_BASE              (UART_BASE + 0x0000U)


#define CLINT                   ((CLINT_TypeDef *)CLINT_BASE)
#define PLIC                    ((PLIC_TypeDef *)PLIC_BASE)
#define RCC                     ((RCC_TypeDef *)RCC_BASE)
#define PLIC_EXTRA              ((PLIC_Extra_TypeDef *)(PLIC_BASE + 0x00200000U))
#define GPIOA                   ((GPIO_TypeDef *)GPIOA_BASE)
#define UART0                   ((UART_TypeDef *)UART0_BASE)
#define SPI0                    ((SPI_TypeDef *)SPI0_BASE)
#define I2C0                    ((I2C_TypeDef *)I2C0_BASE)


/* Peripheral Bit Field Definition */
#define CLINT_MSIPx_MSIPx_POS                   (0U)
#define CLINT_MSIPx_MSIPx_MSK                   (0x1U << CLINT_MSIPx_MSIPx_POS)
#define CLINT_MTIMECMPx_MTIMECMPx_POS           (0U)
#define CLINT_MTIMECMPx_MTIMECMPx_MSK           (0xFFFFFFFFFFFFFFFFUL << CLINT_MTIMECMPx_MTIMECMPx_POS)
#define CLINT_MTIME_MTIME_POS                   (0U)
#define CLINT_MTIME_MTIME_MSK                   (0xFFFFFFFFFFFFFFFFUL << CLINT_MTIME_MTIME_POS)

#define RCC_TILE0_RESET_TILE0_RESET_POS         (0U)
#define RCC_TILE0_RESET_TILE0_RESET_MSK         (0x1U << RCC_TILE0_RESET_TILE0_RESET_POS)

#define UART_TXDATA_DATA_POS                    (0U)
#define UART_TXDATA_DATA_MSK                    (0xFFU << UART_TXDATA_DATA_POS)
#define UART_TXDATA_FULL_POS                    (31U)
#define UART_TXDATA_FULL_MSK                    (0x1U << UART_TXDATA_FULL_POS)
#define UART_RXDATA_DATA_POS                    (0U)
#define UART_RXDATA_DATA_MSK                    (0xFFU << UART_RXDATA_DATA_POS)
#define UART_RXDATA_EMPTY_POS                   (31U)
#define UART_RXDATA_EMPTY_MSK                   (0x1U << UART_RXDATA_EMPTY_POS)
#define UART_TXCTRL_TXEN_POS                    (0U)
#define UART_TXCTRL_TXEN_MSK                    (0x1U << UART_TXCTRL_TXEN_POS)
#define UART_TXCTRL_NSTOP_POS                   (1U)
#define UART_TXCTRL_NSTOP_MSK                   (0x1U << UART_TXCTRL_NSTOP_POS)
#define UART_TXCTRL_TXCNT_POS                   (16U)
#define UART_TXCTRL_TXCNT_MSK                   (0x7U << UART_RXCTRL_RXCNT_POS)
#define UART_RXCTRL_RXEN_POS                    (0U)
#define UART_RXCTRL_RXEN_MSK                    (0x1U << UART_RXCTRL_RXEN_POS)
#define UART_RXCTRL_RXCNT_POS                   (16U)
#define UART_RXCTRL_RXCNT_MSK                   (0x7U << UART_RXCTRL_RXCNT_POS)
#define UART_IE_TXWM_POS                        (0U)
#define UART_IE_TXWM_MSK                        (0x1U << UART_IE_TXWM_POS)
#define UART_IE_RXWM_POS                        (1U)
#define UART_IE_RXWM_MSK                        (0x1U << UART_IE_RXWM_POS)
#define UART_IP_TXWM_POS                        (0U)
#define UART_IP_TXWM_MSK                        (0x1U << UART_IP_TXWM_POS)
#define UART_IP_RXWM_POS                        (1U)
#define UART_IP_RXWM_MSK                        (0x1U << UART_IP_RXWM_POS)
#define UART_DIV_DIV_POS                        (0U)
#define UART_DIV_DIV_MSK                        (0xFFU << UART_DIV_DIV_POS)

#define I2C_CTRL_IEN_POS                        (6U)
#define I2C_CTRL_IEN_MSK                        (0x1U << I2C_CTRL_IEN_POS)
#define I2C_CTRL_EN_POS                         (7U)
#define I2C_CTRL_EN_MSK                         (0x1U << I2C_CTRL_EN_POS)
#define I2C_STAT_CMD_IF_POS                     (0U)
#define I2C_STAT_CMD_IF_MSK                     (0x1U << I2C_STAT_CMD_IF_POS)
#define I2C_STAT_CMD_TIP_POS                    (1U)
#define I2C_STAT_CMD_TIP_MSK                    (0x1U << I2C_STAT_CMD_TIP_POS)
#define I2C_STAT_CMD_ACK_POS                    (3U)
#define I2C_STAT_CMD_ACK_MSK                    (0x1U << I2C_STAT_CMD_ACK_POS)
#define I2C_STAT_CMD_WR_POS                     (4U)
#define I2C_STAT_CMD_WR_MSK                     (0x1U << I2C_STAT_CMD_WR_POS)
#define I2C_STAT_CMD_RD_POS                     (5U)
#define I2C_STAT_CMD_RD_MSK                     (0x1U << I2C_STAT_CMD_RD_POS)
#define I2C_STAT_CMD_BUSY_STO_POS               (6U)
#define I2C_STAT_CMD_BUSY_STO_MSK               (0x1U << I2C_STAT_CMD_BUSY_STO_POS)
#define I2C_STAT_CMD_RXACK_STA_POS              (7U)
#define I2C_STAT_CMD_RXACK_STA_MSK              (0x1U << I2C_STAT_CMD_RXACK_STA_POS)


#define SPI_SCKDIV_DIV_POS                      (0U)
#define SPI_SCKDIV_DIV_MSK                      (0x7FFU << SPI_SCKDIV_DIV_POS)
#define SPI_SCKMODE_PHA_POS                     (0U)
#define SPI_SCKMODE_PHA_MSK                     (0x1U << SPI_SCKMODE_PHA_POS)
#define SPI_SCKMODE_POL_POS                     (1U)
#define SPI_SCKMODE_POL_MSK                     (0x1U << SPI_SCKMODE_POL_POS)
#define SPI_CSID_CSID_POS                       (0U)
#define SPI_CSID_CSID_MSK                       (0xFFFFFFFFU << SPI_CSID_CSID_POS)
#define SPI_CSDEF_CSDEF_POS                     (0U)
#define SPI_CSDEF_CSDEF_MSK                     (0xFFFFFFFFU << SPI_CSDEF_CSDEF_POS)
#define SPI_CSMODE_MODE_POS                     (0U)
#define SPI_CSMODE_MODE_MSK                     (0x3U << SPI_CSMODE_MODE_POS)
#define SPI_TXDATA_DATA_POS                     (0U)
#define SPI_TXDATA_DATA_MSK                     (0xFFU << SPI_TXDATA_DATA_POS)
#define SPI_TXDATA_FULL_POS                     (31U)
#define SPI_TXDATA_FULL_MSK                     (0x1U << SPI_TXDATA_FULL_POS)
#define SPI_RXDATA_DATA_POS                     (0U)
#define SPI_RXDATA_DATA_MSK                     (0xFFU << SPI_RXDATA_DATA_POS)
#define SPI_RXDATA_EMPTY_POS                    (31U)
#define SPI_RXDATA_EMPTY_MSK                    (0x1U << SPI_RXDATA_EMPTY_POS)

#ifdef __cplusplus
}
#endif

#endif /* __EAGLEX_H */