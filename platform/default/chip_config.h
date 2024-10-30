#ifndef __CHIP_CONFIG_H
#define __CHIP_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#include "riscv.h"
#include "clint.h"
#include "plic.h"
#include "htif.h"


// ================================
//  System Clock
// ================================
// system clock frequency in Hz
#define SYS_CLK_FREQ   100000000

// CLINT time base frequency in Hz
#define MTIME_FREQ     10000000


// ================================
//  MMIO devices
// ================================
#define CLINT_BASE                  0x02000000
#define PLIC_BASE                   0x0C000000

#define CLINT                      ((CLINT_Type *)CLINT_BASE)
#define PLIC                       ((PLIC_Type *)PLIC_BASE)
#define PLIC_CC                 ((PLIC_ContextControl_Type *)(PLIC_BASE + 0x00200000U))


#ifdef __cplusplus
}
#endif

#endif // __CHIP_CONFIG_H
