#ifndef __MADA_TIMER_H
#define __MADA_TIMER_H

#include "metal.h"

/*
register map
0x00: control and status
0x04: counter value (r)
0x08: auto-reload value (rw)
0x0c: prescaler value (rw)
0x10: capture and control 0 (rw)
0x14: capture and control 1 (rw)
0x18: capture and control 2 (rw)
0x1c: capture and control 3 (rw)
*/

typedef struct {
    uint32_t MD_TM_CSR;
    uint32_t MD_TM_COUNTER_VAL;
    uint32_t MD_TM_AUTO_RELAOD;
    uint32_t MD_TM_PRESCALER;
    uint32_t MD_TM_CCR0;
    uint32_t MD_TM_CCR1;
    uint32_t MD_TM_CCR2;
} MadaTimerType;

#define MADA_ADDR 0x11000000
#define MADA_ENCODER0 ((MadaTimerType*)MADA_ADDR)

static inline void mada_timer_enable(MadaTimerType *timer){
    timer->MD_TM_CSR = 0x1;
}

static inline void mada_timer_disable(MadaTimerType *timer){
    timer->MD_TM_CSR = 0x0;
}

static inline uint32_t mada_timer_read_raw_counter(MadaTimerType *timer){
    return timer->MD_TM_COUNTER_VAL;
}

static inline void mada_timer_configure(MadaTimerType* timer, uint32_t auto_reload, uint32_t prescaler){
    timer->MD_TM_AUTO_RELAOD = auto_reload;
    timer->MD_TM_PRESCALER = prescaler;
}

static inline void mada_timer_pwm_set(MadaTimerType* timer, uint32_t ccr0, uint32_t ccr1, uint32_t ccr2){
    timer->MD_TM_CCR0 = ccr0;
    timer->MD_TM_CCR1 = ccr1;
    timer->MD_TM_CCR2 = ccr2;
}

#endif
