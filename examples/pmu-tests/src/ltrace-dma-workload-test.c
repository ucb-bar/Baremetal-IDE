/* test_dma_count_scales.c
 * Verify that a larger workload produces more trace bytes than a smaller one.
 * More iterations = more branches = more packets = more bytes. */
#include <stdint.h>
#include <stdio.h>
#include "riscv.h"
#include "l_trace_encoder.h"

#define DMA_SIZE (512 * 1024)
#define SYNC_PERIOD    100
#define SMALL_ITERS    1000
#define LARGE_ITERS    10000

static volatile uint8_t dma_buffer_raw[DMA_SIZE + 64];
static volatile uint8_t *dma_buffer;

static void workload(int iters) {
    volatile int x = 0;
    for (int i = 0; i < iters; i++) { x += i; }
}

static uint64_t run_trace(LTraceEncoderType *encoder, LTraceSinkDmaType *sink_dma,
                           int iters) {
    l_trace_sink_dma_reset(sink_dma);
    l_trace_sink_dma_configure_addr_and_size(sink_dma, (uint64_t)dma_buffer, DMA_SIZE, 0);
    l_trace_encoder_configure_target(encoder, TARGET_DMA);
    l_trace_encoder_configure_branch_mode(encoder, BRANCH_MODE_TARGET);
    l_trace_encoder_configure_sync_period(encoder, SYNC_PERIOD);
    l_trace_encoder_start(encoder);
    workload(iters);
    l_trace_encoder_stop(encoder);
    return l_trace_sink_dma_get_count(sink_dma);
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

    workload(SMALL_ITERS);

    uint64_t count_small = l_trace_sink_dma_get_count(sink_dma);
    printf("small workload (%llu iters):  %llu bytes\n", SMALL_ITERS, count_small);

    workload(LARGE_ITERS);
    uint64_t count_large = l_trace_sink_dma_get_count(sink_dma) - count_small; 
    printf("large workload (%llu iters): %llu bytes\n", LARGE_ITERS, count_large);

    if (count_small == 0) {
        printf("FAIL: small workload produced no bytes\n");
        return 1;
    }
    if (count_large == 0) {
        printf("FAIL: large workload produced no bytes\n");
        return 1;
    }
    if (count_large <= count_small) {
        printf("FAIL: larger workload produced fewer or equal bytes (%llu <= %llu)\n",
               count_large, count_small);
        return 1;
    }

    printf("PASS: count scaled correctly (%llu -> %llu)\n", count_small, count_large);
    return 0;
}