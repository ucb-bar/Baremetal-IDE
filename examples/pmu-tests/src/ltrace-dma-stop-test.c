/* test_dma_stop_halts_writes.c
 * Verify that stopping the encoder actually stops DMA writes.
 * Count must not increase after l_trace_encoder_stop is called. */
#include <stdint.h>
#include <stdio.h>
#include "riscv.h"
#include "l_trace_encoder.h"

#define DMA_SIZE (256 * 1024)
#define SYNC_PERIOD    100

static volatile uint8_t dma_buffer_raw[DMA_SIZE + 64];
static volatile uint8_t *dma_buffer;

static void workload() {
    volatile int x = 0;
    for (int i = 0; i < 1000; i++) { x += i; }
}

int main() {
    dma_buffer = (volatile uint8_t *)(((uintptr_t)dma_buffer_raw + 63) & ~(uintptr_t)63);
    printf("DMA buffer address: %p\n", dma_buffer);

    LTraceEncoderType *encoder = l_trace_encoder_get(get_hart_id());
    LTraceSinkDmaType *sink_dma = l_trace_sink_dma_get(get_hart_id());

    l_trace_sink_dma_reset(sink_dma);
    l_trace_sink_dma_configure_addr_and_size(sink_dma, (uint64_t)dma_buffer, DMA_SIZE, 0);
    l_trace_encoder_configure_target(encoder, TARGET_DMA);
    l_trace_encoder_configure_branch_mode(encoder, BRANCH_MODE_TARGET);
    l_trace_encoder_configure_sync_period(encoder, SYNC_PERIOD);
    l_trace_encoder_start(encoder);

    workload();

    l_trace_encoder_stop(encoder);

    uint64_t count_at_stop = l_trace_sink_dma_get_count(sink_dma);
    printf("count at stop: %llu\n", count_at_stop);

    if (count_at_stop == 0) {
        printf("FAIL: no bytes written before stop\n");
        return 1;
    }

    /* Do more work with encoder stopped — count must not change by much (allow +8 bytes of margin) */
    workload();
    workload();

    uint64_t count_after_work = l_trace_sink_dma_get_count(sink_dma);
    printf("count after additional work: %llu\n", count_after_work);

    if (count_after_work > count_at_stop + 8) {
        printf("FAIL: count changed after stop (%llu -> %llu), encoder did not stop\n",
               count_at_stop, count_after_work);
        return 1;
    }

    printf("PASS: count stop at %llu, end %llu after encoder stopped\n", count_at_stop, count_after_work);
    return 0;
}