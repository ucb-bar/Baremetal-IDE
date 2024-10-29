#ifndef __LL_SWITCHCAP_H
#define __LL_SWITCHCAP_H

#ifdef __cplusplus
extern "C" {
#endif

#include "rv_common.h"


typedef struct {
  __IO uint64_t DIVISOR;
  __IO uint32_t DEAD_TIME;
} SWITCHCAP_Type;

#ifdef __cplusplus
}
#endif

#endif /* __LL_SWITCHCAP_H */