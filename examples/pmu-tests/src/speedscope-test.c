#include <stdint.h>
#include "riscv.h"
#include "riscv_encoding.h"
#include "l_trace_encoder.h"
#include "math.h"

#define NUM_ITERS 10 // 0 to 2pi, 10 steps

int add_kernel(int a, int b) {
  if (b == 0) {
    return a;
  }
  return add_kernel(a, b - 1) + 1;
}

int workload(int a, int b) {
  return add_kernel(a, b);
}

int main(int argc, char **argv) {
  LTraceEncoderType *encoder = l_trace_encoder_get(get_hart_id());
  l_trace_encoder_configure_target(encoder, TARGET_PRINT);
  l_trace_encoder_start(encoder);

  for (int i = 0; i < 10; i++) {
    __asm__("nop");
  }

  for (int i = 0; i < 10; i++) {
    volatile int x = workload(1, i);
  }

  for (int i = 0; i < 10; i++) {
    volatile float x = sin(i);
  }

  printf("Hello, world! %d\n", 0);

  l_trace_encoder_stop(encoder);

  // spin for a bit, to make sure the trace buffer is flushed
  for (int i = 0; i < 10; i++) {
    __asm__("nop");
  }
}
