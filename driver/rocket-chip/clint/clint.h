#ifndef __LL_CLINT_H
#define __LL_CLINT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "rv_common.h"


#define CLINT_BASE               0x02000000U

/* Peripheral Struct Definition */
typedef struct {
  __IO uint32_t MSIP[4096];                     /** MSIP Registers (1 bit wide) */
  __IO uint64_t MTIMECMP[4095];                 /** MTIMECMP Registers */
  __IO uint64_t MTIME;                          /** Timer Register */
} CLINT_TypeDef;

#define CLINT                    ((CLINT_TypeDef *)CLINT_BASE)


#ifdef __cplusplus
}
#endif

#endif /* __LL_CLINT_H */
