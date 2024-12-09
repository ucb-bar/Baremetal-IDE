/**
 * @file hal_pwm.c
 * @author -Ethan Gao / eygao@berkeley.edu
 * @brief
 * @version 0.1
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "pwm.h"

extern uint64_t sys_clk_freq;

uint32_t log2_bitwise(uint32_t n) { 
    uint32_t log = 0; 
 
    while (n >>= 1) {  // Right shift n until it becomes 0 
        log++; 
    } 
    return log;  // log will be the position of the highest set bit 
} 

void pwm_init(PWM_Type *PWMx, PWM_InitType *PWM_init) {
  PWMx->PWM_CFG = *((uint32_t*) PWM_init);
  PWMx->PWM_CMP0 = 0;
  PWMx->PWM_CMP1 = 0;
  PWMx->PWM_CMP2 = 0;
  PWMx->PWM_CMP3 = 0;
}

void pwm_stop(PWM_Type *PWMx, uint32_t idx) {
  // TODO: implementation
}

void pwm_set_frequency(PWM_Type *PWMx, uint32_t idx, uint32_t freq) {
  // TODO: implementation
  // PWM frequency = System clock / 2^pwmscale
  uint16_t pwmscale = log2_bitwise(sys_clk_freq / freq);
  pwm_set_scale(PWMx, pwmscale);
}

uint32_t pwm_get_frequency(PWM_Type *PWMx, uint32_t idx) {
  // TODO: implementation
  uint32_t pwmscale = READ_BITS(PWMx->PWM_CFG, PWM_PWMSCALE_MSK);
  return sys_clk_freq / 1<<pwmscale;
  // return 0;
}

void pwm_set_duty_cycle(PWM_Type *PWMx, uint32_t idx, uint32_t duty, int phase_corr) {
  // TODO: implementation

  uint32_t cmpvalue = duty * 655; //pwm_get_frequency(PWMx, idx)
  pwm_set_compare_value(PWMx, idx, cmpvalue);
}

uint32_t pwm_get_duty_cycle(PWM_Type *PWMx, uint32_t idx) {
  // TODO: implementation
  switch (idx) {
  case 0:
    return PWMx->PWM_CMP0 / 655;
    break;
  case 1:
    return PWMx->PWM_CMP1 / 655;
    break;
  case 2:
    return PWMx->PWM_CMP2 / 655;
    break;
  case 3:
    return PWMx->PWM_CMP3 / 655;
    break;
  }
  return 0; 
}

void pwm_trigger(PWM_Type *PWMx, uint32_t idx) {
  // TODO: implementation
}
