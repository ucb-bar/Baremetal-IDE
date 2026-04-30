/* test_dma_target_print_no_dma.c
 * Verify that when the encoder target is set to TARGET_PRINT,
 * no bytes are written to the DMA buffer. */
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

    /* Route to PRINT, not DMA */
    l_trace_encoder_configure_target(encoder, TARGET_PRINT);
    l_trace_encoder_configure_branch_mode(encoder, BRANCH_MODE_TARGET);
    l_trace_encoder_start(encoder);

    workload();

    l_trace_encoder_stop(encoder);

    uint64_t count = l_trace_sink_dma_get_count(sink_dma);
    printf("bytes written to DMA: %llu (expected 0)\n", count);

    if (count != 0) {
        printf("FAIL: TARGET_PRINT wrote %llu bytes to DMA, expected 0\n", count);
        return 1;
    }

    /* Now switch to DMA and verify it works after having used PRINT */
    l_trace_sink_dma_reset(sink_dma);
    l_trace_sink_dma_configure_addr_and_size(sink_dma, (uint64_t)dma_buffer, DMA_SIZE, 0);
    l_trace_encoder_configure_target(encoder, TARGET_DMA);
    l_trace_encoder_start(encoder);

    workload();

    l_trace_encoder_stop(encoder);

    uint64_t count_dma = l_trace_sink_dma_get_count(sink_dma);
    printf("bytes written after switching to DMA: %llu\n", count_dma);

    if (count_dma == 0) {
        printf("FAIL: DMA produced no bytes after switching from PRINT\n");
        return 1;
    }

    printf("PASS\n");
    return 0;
}