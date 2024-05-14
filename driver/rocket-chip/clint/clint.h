#ifndef __LL_CLINT_H
#define __LL_CLINT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "rv.h"


#define CLINT_BASE               0x02000000U

/* Peripheral Struct Definition */
typedef struct {
  __IO uint32_t MSIP[4096];                     /** MSIP Registers (1 bit wide) */
  __IO uint64_t MTIMECMP[4095];                 /** MTIMECMP Registers */
  __IO uint64_t MTIME;                          /** Timer Register */
} CLINT_TypeDef;

#define CLINT                    ((CLINT_TypeDef *)CLINT_BASE)


static inline void CLINT_clearSoftwareInterrupt(uint32_t hartid) {
  CLEAR_BITS(CLINT->MSIP[hartid], 1U);
}

static inline void CLINT_triggerSoftwareInterrupt(uint32_t hartid) {
  SET_BITS(CLINT->MSIP[hartid], 1U);
}

/**
 * @brief Get current time
 * 
 * This function returns the current time as stated in the mtime register.
 * 
 * @return uint64_t current time
 */
uint64_t CLINT_getTime();

/**
 * @brief Set target interrupt time
 * 
 * This function sets the target interrupt time for the given hart.
 * 
 * @param hartid hart ID
 * @param time time to set
 */
void CLINT_setTimerInterruptTarget(uint32_t hartid, uint64_t time);


#ifdef __cplusplus
}
#endif

#endif /* __LL_CLINT_H */
