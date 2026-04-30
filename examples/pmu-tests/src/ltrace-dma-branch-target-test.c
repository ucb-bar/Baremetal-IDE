#include <stdint.h>
#include <stdio.h>
#include "riscv.h"
#include "l_trace_encoder.h"

#define DMA_SIZE (256 * 1024)
#define SYNC_PERIOD   100

static volatile uint8_t dma_buffer_raw[DMA_SIZE + 64];
static volatile uint8_t *dma_buffer;

/* Branch-heavy workload */
static void workload() {
    volatile int x = 0;
    for (int i = 0; i < 1000; i++) {
        if (i % 2 == 0) x += i;
        else             x -= i;
    }
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
    l_trace_encoder_configure_target(encoder, TARGET_DMA);
    l_trace_encoder_configure_branch_mode(encoder, BRANCH_MODE_TARGET);
    l_trace_encoder_configure_sync_period(encoder, SYNC_PERIOD);
    l_trace_encoder_start(encoder);

    workload();

    l_trace_encoder_stop(encoder);

    uint64_t count = l_trace_sink_dma_get_count(sink_dma);
    printf("bytes written = %llu\n", count);

    if (count == 0) {
        printf("FAIL: produced no output\n");
        return 1;
    }

    printf("PASS\n");
    return 0;
}