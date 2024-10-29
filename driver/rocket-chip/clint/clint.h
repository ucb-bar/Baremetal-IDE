#ifndef __LL_CLINT_H
#define __LL_CLINT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "metal.h"

/* Peripheral Struct Definition */
typedef struct {
  __IO uint32_t MSIP[4096];                     /** MSIP Registers (1 bit wide) */
  __IO uint64_t MTIMECMP[4095];                 /** MTIMECMP Registers */
  __IO uint64_t MTIME;                          /** Timer Register */
} CLINT_TypeDef;


static inline void CLINT_clearSoftwareInterrupt(CLINT_TypeDef *clint, uint32_t hartid) {
  CLEAR_BITS(clint->MSIP[hartid], 1U);
}

static inline void CLINT_triggerSoftwareInterrupt(CLINT_TypeDef *clint, uint32_t hartid) {
  SET_BITS(clint->MSIP[hartid], 1U);
}

/**
 * @brief Get current time
 * 
 * This function returns the current time as stated in the mtime register.
 * 
 * @return uint64_t current time
 */
uint64_t CLINT_getTime(CLINT_TypeDef *clint);

/**
 * @brief Set target interrupt time
 * 
 * This function sets the target interrupt time for the given hart.
 * 
 * @param hartid hart ID
 * @param time time to set
 */
void CLINT_setTimerInterruptTarget(CLINT_TypeDef *clint, uint32_t hartid, uint64_t time);


#ifdef __cplusplus
}
#endif

#endif /* __LL_CLINT_H */
