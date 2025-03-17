#include <stdint.h>
#include "riscv.h"
#include "pmu.h"
#include "riscv_encoding.h"
#include "l_trace_encoder.h"

#define NUM_ITERS 100

// #define USE_L_TRACE_DMA

static volatile uint32_t array[NUM_ITERS];

int main(int argc, char **argv) {
  // enable counter enable
  PMU_COUNTER_ENABLE(3);
  // configure event3 and reset counter3
  printf("---configure event3---\n");
  PMU_EVENT_ENABLE(PMU_EVENT(0, LOAD), 3);
  LTraceEncoderType *encoder = l_trace_encoder_get(get_hart_id());
  l_trace_encoder_configure_target(encoder, TARGET_PRINT);
  l_trace_encoder_enable_hpm(encoder, 1 << 3);
  l_trace_encoder_configure_hpm_interval(encoder, 100); // report every 100 cycles
  PMU_COUNTER_RESET(3);
  l_trace_encoder_start(encoder);
  // do some dummy loop with load
  for (int i = 0; i < NUM_ITERS; i++) {
    array[i] = i;
  }
  // do some different loops
  for (int i = 0; i < NUM_ITERS; i++) {
    array[i] -= i;
  }
  l_trace_encoder_stop(encoder);
  PMU_INHIBIT_ENABLE(3);

  // read counter
  uint32_t counter = PMU_COUNTER_READ(3);
  printf("counter: %d\n", counter);

}
