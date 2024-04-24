#ifndef __RV_CLINT_H
#define __RV_CLINT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "rv_common.h"


/* Peripheral Struct Definition */
typedef struct {
  __IO uint32_t MSIP[4096];                     /** MSIP Registers (1 bit wide) */
  __IO uint64_t MTIMECMP[4095];                 /** MTIMECMP Registers */
  __IO uint64_t MTIME;                          /** Timer Register */
} CLINT_TypeDef;


static inline void HAL_CLINT_clearSoftwareInterrupt(CLINT_TypeDef *CLINTx, uint32_t hartid) {
  CLEAR_BITS(CLINTx->MSIP[hartid], 1U);
}

static inline void HAL_CLINT_triggerSoftwareInterrupt(CLINT_TypeDef *CLINTx, uint32_t hartid) {
  SET_BITS(CLINTx->MSIP[hartid], 1U);
}

uint64_t HAL_CLINT_getTime(CLINT_TypeDef *CLINTx);

void HAL_CLINT_setTimerInterruptTarget(CLINT_TypeDef *CLINTx, uint32_t hartid, uint64_t time);


#ifdef __cplusplus
}
#endif

#endif /* __RV_CLINT_H */
