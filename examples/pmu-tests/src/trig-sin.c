#include <stdint.h>
#include "riscv.h"
#include "riscv_encoding.h"
#include "l_trace_encoder.h"
#include "math.h"

#define NUM_ITERS 10 // 0 to 2pi, 10 steps

int main(int argc, char **argv) {
  LTraceEncoderType *encoder = l_trace_encoder_get(get_hart_id());
  l_trace_encoder_configure_target(encoder, TARGET_PRINT);
  l_trace_encoder_start(encoder);

  for (int i = 0; i < NUM_ITERS; i++) {
    volatile float x = i * (2 * M_PI / NUM_ITERS);
    volatile float y = sin(x);
    printf("x: %f, y: %f\n", x, y);
  }

  l_trace_encoder_stop(encoder);
}
