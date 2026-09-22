/* test_dma_overflow.c - Verify overflow mode stops writing at max size */
#include <stdint.h>
#include <stdio.h>
#include "riscv.h"
#include "l_trace_encoder.h"

/* Small buffer to force overflow */
#define DMA_SIZE (256 * 1024)
__attribute__((aligned(64), section(".noinit"))) static volatile uint8_t dma_buffer[DMA_SIZE];
#define SYNC_PERIOD    100

static void workload() {
    volatile int x = 0;
    for (int i = 0; i < 1000; i++) { x += i; }
}

int main() {
    LTraceEncoderType *encoder = l_trace_encoder_get(get_hart_id());
    LTraceSinkDmaType *sink_dma = l_trace_sink_dma_get(get_hart_id());

    l_trace_sink_dma_reset(sink_dma);
    /* Set max size smaller than buffer to test overflow boundary */
    l_trace_sink_dma_configure_addr_and_size(sink_dma, (uint64_t)dma_buffer, DMA_SIZE, 0);
    l_trace_sink_dma_configure_mode(sink_dma, DMA_MODE_OVERFLOW);
    l_trace_encoder_configure_target(encoder, TARGET_DMA);
    l_trace_encoder_configure_branch_mode(encoder, BRANCH_MODE_TARGET);
    l_trace_encoder_configure_sync_period(encoder, SYNC_PERIOD);
    l_trace_encoder_start(encoder);

    workload();

    l_trace_encoder_stop(encoder);

    uint64_t count = l_trace_sink_dma_get_count(sink_dma);
    uint32_t wrap  = l_trace_sink_dma_get_wrap_count(sink_dma);
    printf("bytes written: %llu, wrap count: %u\n", count, wrap);

    /* In overflow mode, count must never exceed DMA_SIZE */
    if (count > DMA_SIZE) {
        printf("FAIL: overflow mode wrote %llu bytes, exceeding max %d\n", count, DMA_SIZE);
        return 1;
    }
    /* Wrap count must be 0 in overflow mode */
    if (wrap != 0) {
        printf("FAIL: wrap count should be 0 in overflow mode, got %u\n", wrap);
        return 1;
    }
    if (count == 0) {
        printf("FAIL: no bytes written\n");
        return 1;
    }

    printf("PASS\n");
    return 0;
}