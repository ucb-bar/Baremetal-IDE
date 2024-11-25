#ifndef __TRIGGER_H
#define __TRIGGER_H

#include "l_trace_encoder.h"
#include "clint.h"
#include "chip_config.h"
#include "pmu.h"

// use timer interrupt to trigger the profiler
#define TIMER_INTERRUPT
// use PMU reading to report the profile
// #define USE_PMU_READING
// use LBR to report the trace
#define USE_LBR
// report the total time of the benchmark
#define REPORT_TOTAL_TIME
// use trace encoder to report the trace
// #define USE_L_TRACE

/* timer interrupt interval in milliseconds 
  only used when TIMER_INTERRUPT is defined
*/
#define TIMER_INTERRUPT_INTERVAL 100

static inline void start_trigger(void) {
  #ifdef USE_L_TRACE
    LTraceEncoderType *encoder = l_trace_encoder_get(get_hart_id());
    l_trace_encoder_start(encoder);
  #endif

  #ifdef REPORT_TOTAL_TIME
    uint64_t curr_time = clint_get_time(CLINT);
    printf("start trigger at %lld\n", curr_time);
  #endif

  #ifdef USE_PMU_READING
    PMU_EVENT_ENABLE(PMU_EVENT(1, LOAD_USE_INTERLOCK), 3);
    PMU_COUNTER_RESET(3);
    PMU_EVENT_ENABLE(PMU_EVENT(1, BRANCH_MISPREDICTION), 4);
    PMU_COUNTER_RESET(4);
  #endif

  #ifdef USE_LBR
    lbr_init();
  #endif

  #ifdef TIMER_INTERRUPT
    enable_global_interrupt();
    enable_timer_interrupt();
    clint_set_timer_interrupt_target(CLINT, get_hart_id(), curr_time + TIMER_INTERRUPT_INTERVAL);
  #endif
}

#ifdef TIMER_INTERRUPT
// override weak implementation
void machine_timer_interrupt_callback() {
  #ifdef USE_PMU_READING
    PMU_INHIBIT_ENABLE(3);
    PMU_INHIBIT_ENABLE(4);
    uint64_t curr_cycle = get_cycles();
    printf("curr_cycle: %lld\n", curr_cycle);
    uint64_t load_use_interlock_count = PMU_COUNTER_READ_CLEAR(3);
    printf("load use interlock count: %lld\n", load_use_interlock_count);
    uint64_t branch_misprediction_count = PMU_COUNTER_READ_CLEAR(4);
    printf("branch misprediction count: %lld\n", branch_misprediction_count);
    PMU_INHIBIT_DISABLE(3);
    PMU_INHIBIT_DISABLE(4);
  #endif

  #ifdef USE_LBR
    lbr_fetch_records();
  #endif

  uint64_t curr_time = clint_get_time(CLINT);
  clint_set_timer_interrupt_target(CLINT, get_hart_id(), curr_time + TIMER_INTERRUPT_INTERVAL);
}
#endif

static inline void stop_trigger(void) {
  #ifdef USE_LBR
    lbr_dump_records();
  #endif
  #ifdef USE_L_TRACE
    LTraceEncoderType *encoder = l_trace_encoder_get(get_hart_id());
    l_trace_encoder_stop(encoder);
  #endif

  #ifdef REPORT_TOTAL_TIME
    int64_t curr_time = clint_get_time(CLINT);
    printf("stop trigger at %lld\n", curr_time);
  #endif
}

#endif /* __TRIGGER_H */