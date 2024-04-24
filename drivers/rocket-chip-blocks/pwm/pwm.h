#ifndef __HAL_PWM_H
#define __HAL_PWM_H

#ifdef __cplusplus
extern "C" {
#endif

#include "rv_common.h"


typedef struct {
  __IO uint32_t PWM_CFG;
  uint32_t RESERVED0;
  __IO uint32_t PWM_CTR;
  uint32_t RESERVED1;
  __IO uint32_t PWM_PWMS;
  uint32_t RESERVED2[3];
  __IO uint32_t PWM_CMP0;
  __IO uint32_t PWM_CMP1;
  __IO uint32_t PWM_CMP2;
  __IO uint32_t PWM_CMP3;
} PWM_TypeDef;


#ifdef __cplusplus
}
#endif

#endif /* __HAL_PWM_H */
