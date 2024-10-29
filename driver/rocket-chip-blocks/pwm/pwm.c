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

void pwm_init(PWM_Type *PWMx, PWM_InitType *PWM_init) {
  PWMx->PWM_CFG = *((uint32_t*) PWM_init);
}

void pwm_stop(PWM_Type *PWMx, uint32_t idx) {
  // TODO: implementation
}

void pwm_set_frequency(PWM_Type *PWMx, uint32_t idx, uint32_t freq) {
  // TODO: implementation
}

uint32_t pwm_get_frequency(PWM_Type *PWMx, uint32_t idx) {
  // TODO: implementation
  return 0;
}

void pwm_set_duty_cycle(PWM_Type *PWMx, uint32_t idx, uint32_t duty, int phase_corr) {
  // TODO: implementation
}

uint32_t pwm_get_duty_cycle(PWM_Type *PWMx, uint32_t idx) {
  // TODO: implementation
  return 0; 
}

void pwm_trigger(PWM_Type *PWMx, uint32_t idx) {
  // TODO: implementation
}