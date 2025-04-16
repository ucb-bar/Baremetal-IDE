#ifndef __PWM_H
#define __PWM_H

#ifdef __cplusplus
extern "C" {
#endif

#include "metal.h"

#define PWM_PWMSCALE_POS                        (0U)
#define PWM_PWMSCALE_MSK                        (0xFU << PWM_PWMSCALE_POS)
#define PWM_PWMSTICKY_POS                       (8U)
#define PWM_PWMSTICKY_MSK                       (0x1U << PWM_PWMSTICKY_POS)
#define PWM_PWMZEROCMP_POS                      (9U)
#define PWM_PWMZEROCMP_MSK                      (0x1U << PWM_PWMZEROCMP_POS)
#define PWM_PWMDEGLITCH_POS                     (10U)
#define PWM_PWMDEGLITCH_MSK                     (0x1U << PWM_PWMDEGLITCH_POS)
#define PWM_PWMENALWAYS_POS                     (12U)
#define PWM_PWMENALWAYS_MSK                     (0x1U << PWM_PWMENALWAYS_POS)
#define PWM_PWMENONESHOT_POS                    (13U)
#define PWM_PWMENONESHOT_MSK                    (0x1U << PWM_PWMENONESHOT_POS)
#define PWM_PWMCMP0CENTER_POS                   (16U)
#define PWM_PWMCMP0CENTER_MSK                   (0x1U << PWM_PWMCMP0CENTER_POS)
#define PWM_PWMCMP1CENTER_POS                   (17U)
#define PWM_PWMCMP1CENTER_MSK                   (0x1U << PWM_PWMCMP1CENTER_POS)
#define PWM_PWMCMP2CENTER_POS                   (18U)
#define PWM_PWMCMP2CENTER_MSK                   (0x1U << PWM_PWMCMP2CENTER_POS)
#define PWM_PWMCMP3CENTER_POS                   (19U)
#define PWM_PWMCMP3CENTER_MSK                   (0x1U << PWM_PWMCMP3CENTER_POS)
#define PWM_PWMCMP0GANG_POS                     (24U)
#define PWM_PWMCMP0GANG_MSK                     (0x1U << PWM_PWMCMP0GANG_POS)
#define PWM_PWMCMP1GANG_POS                     (25U)
#define PWM_PWMCMP1GANG_MSK                     (0x1U << PWM_PWMCMP1GANG_POS)
#define PWM_PWMCMP2GANG_POS                     (26U)
#define PWM_PWMCMP2GANG_MSK                     (0x1U << PWM_PWMCMP2GANG_POS)
#define PWM_PWMCMP3GANG_POS                     (27U)
#define PWM_PWMCMP3GANG_MSK                     (0x1U << PWM_PWMCMP3GANG_POS)
#define PWM_PWMCMP0IP_POS                       (28U)
#define PWM_PWMCMP0IP_MSK                       (0x1U << PWM_PWMCMP0IP_POS)
#define PWM_PWMCMP1IP_POS                       (29U)
#define PWM_PWMCMP1IP_MSK                       (0x1U << PWM_PWMCMP1IP_POS)
#define PWM_PWMCMP2IP_POS                       (30U)
#define PWM_PWMCMP2IP_MSK                       (0x1U << PWM_PWMCMP2IP_POS)
#define PWM_PWMCMP3IP_POS                       (31U)
#define PWM_PWMCMP3IP_MSK                       (0x1U << PWM_PWMCMP3IP_POS)

#define PWM_CMP_REG_POS                         (0U)
#define PWM_CMP_REG_MSK                         (0xFFFFU << PWM_PWMCMP3IP_POS)


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
} PWM_Type;


typedef struct {
  uint8_t pwmscale        : 4;
  uint8_t RESERVED        : 4;
  uint8_t pwmsticky       : 1;
  uint8_t pwmzerocmp      : 1;
  uint8_t pwmdeglitch     : 1;
  uint8_t RESERVED1       : 1;
  uint8_t pwmenalways     : 1;
  uint8_t pwmenoneshot    : 1;
  uint8_t RESERVED2       : 2;
  uint8_t pwmcmp0center   : 1;
  uint8_t pwmcmp1center   : 1;
  uint8_t pwmcmp2center   : 1;
  uint8_t pwmcmp3center   : 1;
  uint8_t RESERVED3       : 4;
  uint8_t pwmcmp0gang     : 1;
  uint8_t pwmcmp1gang     : 1;
  uint8_t pwmcmp2gang     : 1;
  uint8_t pwmcmp3gang     : 1;
  uint8_t pwmcmp0ip       : 1;
  uint8_t pwmcmp1ip       : 1;
  uint8_t pwmcmp2ip       : 1;
  uint8_t pwmcmp3ip       : 1;
} PWM_InitType __attribute__((packed));

static inline void pwm_enable(PWM_Type *PWMx) {
  SET_BITS(PWMx->PWM_CFG, PWM_PWMENALWAYS_MSK);
  SET_BITS(PWMx->PWM_CFG, PWM_PWMZEROCMP_MSK);
  SET_BITS(PWMx->PWM_CFG, PWM_PWMDEGLITCH_MSK);
}

static inline void pwm_disable(PWM_Type *PWMx) {
  CLEAR_BITS(PWMx->PWM_CFG, PWM_PWMENALWAYS_MSK);
}

static inline void pwm_set_scale(PWM_Type *PWMx, uint32_t value) {
  WRITE_BITS(PWMx->PWM_CFG, PWM_PWMSCALE_MSK, value << PWM_PWMSCALE_POS);
}

static inline void pwm_set_compare_value(PWM_Type *PWMx, uint32_t idx,
                                       uint32_t value) {
  switch (idx) {
  case 0:
    SET_BITS(PWMx->PWM_CFG, PWM_PWMCMP0IP_MSK);
    PWMx->PWM_CMP0 = value;
    break;
  case 1:
    SET_BITS(PWMx->PWM_CFG, PWM_PWMCMP1IP_MSK);
    PWMx->PWM_CMP1 = value;
    break;
  case 2:
    SET_BITS(PWMx->PWM_CFG, PWM_PWMCMP2IP_MSK);
    PWMx->PWM_CMP2 = value;
    break;
  case 3:
    SET_BITS(PWMx->PWM_CFG, PWM_PWMCMP3IP_MSK);
    PWMx->PWM_CMP3 = value;
    break;
  }
}

static inline void pwm_trigger_oneshot(PWM_Type *PWMx, uint32_t idx) {
  SET_BITS(PWMx->PWM_CFG, PWM_PWMENONESHOT_MSK);
}

void pwm_init(PWM_Type *PWMx, PWM_InitType *PWM_init);

void pwm_stop(PWM_Type *PWMx, uint32_t idx);

void pwm_set_frequency(PWM_Type *PWMx, uint32_t idx, uint32_t freq);

uint32_t pwm_get_frequency(PWM_Type *PWMx, uint32_t idx);

void pwm_set_duty_cycle(PWM_Type *PWMx, uint32_t idx, uint32_t duty, uint32_t freq, int phase_corr);

uint32_t pwm_get_duty_cycle(PWM_Type *PWMx, uint32_t idx);

void pwm_trigger(PWM_Type *PWMx, uint32_t idx);



#ifdef __cplusplus
}
#endif

#endif /* __PWM_H */
