/* Radiance tapeout ContingentSpad: 16 KiB at 0x180000000 (aliased above).
 * Run once after hardware reset; reserve its first 513 bytes for this demo.
 * Build: cmake --build build --target ltrace-spad-demo
 * Run the resulting examples/pmu-tests/ltrace-spad-demo.elf on a Radiance
 * tapeout configuration with WithRadianceRocket (TACIT encoder + DMA sink).
 */
#include "l_trace_encoder.h"

#define SPAD_ADDR 0x180000000ULL
#define CAPTURE_BYTES 512

static inline void fence_io(void) {
  __asm__ volatile ("fence iorw, iorw" ::: "memory");
}

static void settle(void) {
  /* Demo-only grace period for encoder drain / outstanding DMA responses.
   * Current DMA DONE does not guarantee either. This is not a completion API.
   */
  for (unsigned i = 0; i < 4096; ++i)
    __asm__ volatile ("nop" ::: "memory");
}

int main(void) {
  volatile uint8_t *spad = (volatile uint8_t *)SPAD_ADDR;
  LTraceEncoderType *encoder = l_trace_encoder_get(get_hart_id());
  LTraceSinkDmaType *dma = l_trace_sink_dma_get(get_hart_id());

  l_trace_encoder_stop(encoder);
  settle();
  l_trace_sink_dma_reset(dma);
  fence_io();
  for (unsigned i = 0; i <= CAPTURE_BYTES; ++i)
    spad[i] = 0xa5;
  fence_io();

  l_trace_sink_dma_configure_addr(dma, SPAD_ADDR, 0);
  l_trace_sink_dma_configure_max_size(dma, CAPTURE_BYTES);
  l_trace_encoder_configure_target(encoder, TARGET_DMA);
  l_trace_encoder_configure_branch_mode(encoder, BRANCH_MODE_TARGET);
  fence_io();

  volatile unsigned sum = 0;
  l_trace_encoder_start(encoder);
  for (unsigned i = 0; i < 16; ++i)
    sum += i;
  l_trace_encoder_stop(encoder);
  fence_io();
  settle();

  dma->TR_SK_DMA_FLUSH = 1;
  unsigned timeout = 1000000;
  while (!dma->TR_SK_DMA_FLUSH_DONE) {
    if (--timeout == 0) {
      printf("FAIL: DMA flush timeout\n");
      return 1;
    }
  }
  settle();
  fence_io();

  uint64_t count = dma->TR_SK_DMA_COUNT;
  if (sum != 120 || count == 0 || count >= CAPTURE_BYTES ||
      spad[CAPTURE_BYTES] != 0xa5) {
    printf("FAIL: sum=%u bytes=%llu guard=%02x\n",
           sum, (unsigned long long)count, (unsigned)spad[CAPTURE_BYTES]);
    return 1;
  }
  printf("TACIT spad demo: %llu bytes at 0x%llx, sum=%u\n",
         (unsigned long long)count, (unsigned long long)SPAD_ADDR, sum);
  for (uint64_t i = 0; i < count; ++i)
    printf("%02x%c", (unsigned)spad[i], ((i + 1) % 16) ? ' ' : '\n');
  printf("\n");
  return 0;
}
