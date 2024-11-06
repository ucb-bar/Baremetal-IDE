#ifndef __TRIGGER_H
#define __TRIGGER_H

#include "l_trace_encoder.h"
#include "clint.h"
#include "chip_config.h"
#include "pmu.h"
#define TIMER_INTERRUPT_INTERVAL 100

static inline void start_trigger(void) {
  LTraceEncoderType *encoder = l_trace_encoder_get(get_hart_id());
  l_trace_encoder_start(encoder);
  uint64_t curr_time = clint_get_time(CLINT);
  printf("start trigger at %lld\n", curr_time);

  PMU_COUNTER_ENABLE(3);
  PMU_EVENT_ENABLE(PMU_EVENT(0, LOAD), 3);
  PMU_COUNTER_RESET(3);

  PMU_COUNTER_ENABLE(4);
  PMU_EVENT_ENABLE(PMU_EVENT(0, STORE), 4);
  PMU_COUNTER_RESET(4);

  enable_global_interrupt();
  enable_timer_interrupt();
  clint_set_timer_interrupt_target(CLINT, get_hart_id(), curr_time + TIMER_INTERRUPT_INTERVAL);
}

// override weak implementation
void machine_timer_interrupt_callback() {
  uint64_t curr_time = clint_get_time(CLINT);
  printf("machine timer interrupt\n");
  uint64_t inst_ret = READ_CSR("minstret");
  printf("instruction retired: %lx\n", inst_ret);
  uint64_t load_count = PMU_COUNTER_READ_CLEAR(3);
  printf("load count: %lx\n", load_count);
  uint64_t store_count = PMU_COUNTER_READ_CLEAR(4);
  printf("store count: %lx\n", store_count);
  clint_set_timer_interrupt_target(CLINT, get_hart_id(), curr_time + TIMER_INTERRUPT_INTERVAL);
}

static inline void stop_trigger(void) {
  LTraceEncoderType *encoder = l_trace_encoder_get(get_hart_id());
  l_trace_encoder_stop(encoder);
  int64_t curr_time = clint_get_time(CLINT);
  printf("stop trigger at %lld\n", curr_time);
}

#endif /* __TRIGGER_H */