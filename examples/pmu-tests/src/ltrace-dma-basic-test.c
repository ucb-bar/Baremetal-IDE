#include <stdint.h>
#include <stdio.h>
#include "riscv.h"
#include "l_trace_encoder.h"

#define DMA_SIZE (256 * 1024)
#define DMA_ALIGN 64
#define SYNC_PERIOD    100

static volatile uint8_t dma_buffer_raw[DMA_SIZE + DMA_ALIGN] __attribute__((aligned(64)));;
static volatile uint8_t *dma_buffer;

static void workload() {
    volatile int x = 0;
    for (int i = 0; i < 1000; i++) { x += i; }
}

int main() {
    dma_buffer = dma_buffer_raw; 
    printf("DMA buffer address: %p\n", dma_buffer);

    LTraceEncoderType *encoder = l_trace_encoder_get(get_hart_id());
    LTraceSinkDmaType *sink_dma = l_trace_sink_dma_get(get_hart_id());

    l_trace_sink_dma_configure_addr_and_size(sink_dma, (uint64_t)dma_buffer, DMA_SIZE, 0);
    l_trace_encoder_configure_target(encoder, TARGET_DMA);
    l_trace_encoder_configure_branch_mode(encoder, BRANCH_MODE_TARGET);
    l_trace_encoder_configure_sync_period(encoder, SYNC_PERIOD);
    l_trace_encoder_start(encoder);

    workload();

    l_trace_encoder_stop(encoder);

    uint64_t count = l_trace_sink_dma_get_count(sink_dma);
    printf("bytes written: %llu\n", count);

    if (count == 0) {
        printf("FAIL: no bytes written\n");
        return 1;
    }
    if (dma_buffer[0] != 0x36) {
        printf("FAIL: expected sync header 0x36, got 0x%02x\n", dma_buffer[0]);
        return 1;
    }
    if (count > DMA_SIZE) {
        printf("FAIL: count %llu exceeds buffer size %d\n", count, DMA_SIZE);
        return 1;
    }

    printf("PASS\n");
    return 0;
}