#include <stdint.h>
#include <stdio.h>
#include "riscv.h"
#include "l_trace_encoder.h"

/* Small buffer to force multiple wraps */
#define DMA_SIZE (256 * 1024)
#define SYNC_PERIOD    100

static volatile uint8_t dma_buffer_raw[DMA_SIZE + 64];
static volatile uint8_t *dma_buffer;

static void workload() {
    volatile int x = 0;
    for (int i = 0; i < 1000; i++) { x += i; }
}

int main() {
    dma_buffer = (volatile uint8_t *)(
        ((uintptr_t)dma_buffer_raw + 63) & ~(uintptr_t)63
    );
    printf("DMA buffer address: %p\n", dma_buffer);

    LTraceEncoderType *encoder = l_trace_encoder_get(get_hart_id());
    LTraceSinkDmaType *sink_dma = l_trace_sink_dma_get(get_hart_id());

    l_trace_sink_dma_reset(sink_dma);
    l_trace_sink_dma_configure_addr_and_size(sink_dma, (uint64_t)dma_buffer, DMA_SIZE, 0);
    l_trace_sink_dma_configure_mode(sink_dma, DMA_MODE_RING);
    l_trace_encoder_configure_target(encoder, TARGET_DMA);
    l_trace_encoder_configure_branch_mode(encoder, BRANCH_MODE_TARGET);
    l_trace_encoder_configure_sync_period(encoder, SYNC_PERIOD);
    l_trace_encoder_start(encoder);

    workload();

    l_trace_encoder_stop(encoder);

    uint64_t count = l_trace_sink_dma_get_count(sink_dma);
    uint32_t wrap  = l_trace_sink_dma_get_wrap_count(sink_dma);
    printf("bytes written (addr counter): %llu, wrap count: %u\n", count, wrap);

    if (wrap == 0) {
        printf("FAIL: expected at least one wrap in ring buffer mode\n");
        return 1;
    }
    /* addr_counter resets on wrap so it should be < DMA_SIZE */
    if (count >= DMA_SIZE) {
        printf("FAIL: addr counter %llu should be < %d after wrap\n", count, DMA_SIZE);
        return 1;
    }

    printf("PASS: wrapped %u times\n", wrap);
    return 0;
}