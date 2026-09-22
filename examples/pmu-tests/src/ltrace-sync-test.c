#include <stdint.h>
#include <stdio.h>
#include "riscv.h"
#include "l_trace_encoder.h"

#define DMA_SIZE             (4 * 1024)
#define SYNC_START_HEADER    0x36
#define SYNC_PERIODIC_HEADER 0x56
#define SYNC_END_HEADER      0x76
#define SYNC_PERIOD_SHORT    100
#define SYNC_DISABLED        0

static volatile uint8_t dma_buffer_raw[DMA_SIZE + 64];
static volatile uint8_t *dma_buffer;
static int num_failures = 0;

typedef struct {
  int start;
  int periodic;
  int end;
} SyncCounts;

void check(int cond, const char *msg) {
  if (!cond) {
    printf("FAIL: %s\n", msg);
    num_failures++;
  }
}

SyncCounts count_sync_packets(volatile uint8_t *buf, uint64_t len) {
  SyncCounts c = {0, 0, 0};
  for (uint64_t i = 0; i < len; i++) {
    if (buf[i] == SYNC_START_HEADER)    c.start++;
    if (buf[i] == SYNC_PERIODIC_HEADER) c.periodic++;
    if (buf[i] == SYNC_END_HEADER)      c.end++;
  }
  return c;
}

static void workload() {
  volatile int x = 0;
  for (int i = 0; i < 1000; i++) { x += i; }
}

static void reset_and_configure(LTraceEncoderType *encoder, LTraceSinkDmaType *sink_dma, uint32_t sync_period) {
  l_trace_sink_dma_reset(sink_dma);
  l_trace_sink_dma_configure_addr_and_size(sink_dma, (uint64_t)dma_buffer, DMA_SIZE, 0);
  l_trace_sink_dma_configure_mode(sink_dma, DMA_MODE_OVERFLOW);
  l_trace_encoder_configure_sync_period(encoder, sync_period);
}

int main() {
  dma_buffer = (volatile uint8_t *)(
    ((uintptr_t)dma_buffer_raw + 63) & ~(uintptr_t)63
  );

  LTraceEncoderType *encoder  = l_trace_encoder_get(get_hart_id());
  LTraceSinkDmaType *sink_dma = l_trace_sink_dma_get(get_hart_id());

  l_trace_encoder_configure_target(encoder, TARGET_DMA);
  l_trace_encoder_configure_branch_mode(encoder, BRANCH_MODE_TARGET);

  // ---- TEST 2: short sync period - should produce periodic syncs ----
  printf("TEST: short sync period = %d\n", SYNC_PERIOD_SHORT);
  reset_and_configure(encoder, sink_dma, SYNC_PERIOD_SHORT);
  l_trace_encoder_start(encoder);
  workload();
  l_trace_encoder_stop(encoder);

  uint64_t count2 = l_trace_sink_dma_get_count(sink_dma);
  SyncCounts syncs2 = count_sync_packets(dma_buffer, count2);
  printf("bytes written: %llu, start: %d, periodic: %d, end: %d\n",
    count2, syncs2.start, syncs2.periodic, syncs2.end);
  check(count2 > 0,          "TEST: no bytes written");
  check(syncs2.start == 1,   "TEST: expected exactly 1 sync start");
  check(syncs2.end == 1,     "TEST: expected exactly 1 sync end");
  check(syncs2.periodic > 0, "TEST: no periodic syncs with short period");

  if (num_failures == 0) {
    printf("PASS\n");
  } else {
    printf("FAILED %d checks\n", num_failures);
  }
  return num_failures;
}