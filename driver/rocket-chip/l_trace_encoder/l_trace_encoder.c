#include "l_trace_encoder.h"

void l_trace_sink_dma_configure_addr(LTraceSinkDmaType *sink_dma, uint64_t dma_addr) {
  sink_dma->TR_SK_DMA_ADDR = dma_addr;
}

void l_trace_sink_dma_read(LTraceSinkDmaType *sink_dma, uint8_t *buffer) {
  sink_dma->TR_SK_DMA_FLUSH = 1;
  while (sink_dma->TR_SK_DMA_FLUSH_DONE == 0) {
    ;
  }
  // printf("[l_trace_sink_dma_read] flush done\n");
  uint64_t count = sink_dma->TR_SK_DMA_COUNT;
  printf("[l_trace_sink_dma_read] count: %lld\n", count);
  for (uint8_t i = 0; i < count; i++) {
    printf("%02x ", buffer[i]);
  }
  printf("\n");
}

void l_trace_encoder_start(LTraceEncoderType *encoder) {
  SET_BITS(encoder->TR_TE_CTRL, 0x1 << 1);
}

void l_trace_encoder_configure_target(LTraceEncoderType *encoder, uint64_t target) {
  encoder->TR_TE_TARGET = target;
}

void l_trace_encoder_stop(LTraceEncoderType *encoder) {
  CLEAR_BITS(encoder->TR_TE_CTRL, 0x1 << 1);
}

