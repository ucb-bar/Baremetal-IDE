#include "l_trace_encoder.h"

void l_trace_sink_dma_read(LTraceSinkDmaType *sink_dma, uint8_t *buffer) {
  sink_dma->TR_SK_DMA_FLUSH = 1;
  while (sink_dma->TR_SK_DMA_FLUSH_DONE == 0) {
    // printf("waiting for flush done\n");
  }
  uint64_t count = sink_dma->TR_SK_DMA_COUNT;
  printf("[l_trace_sink_dma_read] count: %llu, max_size: %llu\n",
         count, sink_dma->TR_SK_DMA_MAX_SIZE);

  // Keep output bounded so UART doesn't dominate sim time.
  const uint64_t max_dump = 1024;
  uint64_t dump_count = (count < max_dump) ? count : max_dump;
  if (count > max_dump) {
    printf("[l_trace_sink_dma_read] dumping first %llu bytes only\n", dump_count);
  }

  for (uint64_t i = 0; i < dump_count; i++) {
    if ((i & 0xF) == 0) {
      printf("\n%08llx: ", i);
    }
    printf("%02x ", (uint8_t)buffer[i]);
  }
  printf("\n\n");
}
