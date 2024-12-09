#include <stdint.h>
#include "riscv.h"
#include "riscv_encoding.h"
#include "l_trace_encoder.h"

#define NUM_ITERS 100

#define USE_L_TRACE_DMA

__attribute__((aligned(64), section(".noinit"))) static volatile uint8_t dma_buffer[512 * 1024];

int main(int argc, char **argv) {
  // 64 aligned
  LTraceEncoderType *encoder = l_trace_encoder_get(get_hart_id());
  #ifdef USE_L_TRACE_DMA
    LTraceSinkDmaType *sink_dma = l_trace_sink_dma_get(get_hart_id());
    l_trace_sink_dma_configure_addr(sink_dma, (uint64_t)dma_buffer);
    l_trace_encoder_configure_target(encoder, TARGET_DMA);
  #else
    l_trace_encoder_configure_target(encoder, TARGET_PRINT);
  #endif
  l_trace_encoder_start(encoder);
  volatile int c;
  // do some dummy loops
  for (int i = 0; i < NUM_ITERS; i++) {
    c += i;
  }
  // do some different loops
  for (int i = 0; i < NUM_ITERS; i++) {
    c -= i;
  }
  l_trace_encoder_stop(encoder);
  #ifdef USE_L_TRACE_DMA
    l_trace_sink_dma_read(sink_dma, dma_buffer);
  #endif
}
