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
#include "hal_i2s.h"
#include "hal_dma.h"
// #include "hal_wavelet.h"
#include "hal_conv.h"


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
#define RCC_BASE                    0x00100000U
#define CLINT_BASE                  0x02000000U
#define PLIC_BASE                   0x0C000000U
#define PWM_BASE                    0x10060000U
#define GPIOA_BASE                  0x10010000U
#define GPIOB_BASE                  0x10011000U

#define UART0_BASE                  0x10020000U
#define I2S_BASE                    0x10042000U
#define PLL_BASE                    0x00140000U
#define WAVELET_BASE                0x08810000U
#define DMA_BASE                    0x08812000U

#define RCC_CLOCK_SELECTOR          ((ClockSel_Type*)(RCC_BASE + 0x30000))
#define CLINT                       ((CLINT_Type *)CLINT_BASE)
#define PLIC                        ((PLIC_Type *)PLIC_BASE)
#define PLIC_CC                     ((PLIC_ContextControl_Type *)(PLIC_BASE + 0x00200000U))
#define PLL                         ((PLL_Type *)PLL_BASE)
#define UART0                       ((UART_Type *)UART0_BASE)
#define PWM0_BASE                   (PWM_BASE)
#define GPIOA                       ((GPIO_Type *)GPIOA_BASE)
#define GPIOB                       ((GPIO_Type *)GPIOB_BASE)

// #define CONV_BASE                   0x08800000U
// #define CONV1D                  ((ConvAccel_Type *)CONV_BASE)

#ifdef __cplusplus
}
#endif

#endif // __CHIP_CONFIG_H
