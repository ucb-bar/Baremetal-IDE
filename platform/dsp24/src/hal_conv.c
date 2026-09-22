#include "hal_conv.h"

int set_conv_params(int len, uint16_t in_dilation, uint16_t* in_kernel) {
    *(uint32_t*)CONV_LENGTH_ADDR = len;
    *(uint8_t*)CONV_ISFLOAT_ADDR = 0;
    *(uint16_t*)CONV_DILATION_ADDR = in_dilation;
    *(uint64_t*)CONV_KERNEL_ADDR = *((uint64_t*) in_kernel);         // 64 bits: 4 FP16s
    *(uint64_t*)CONV_KERNEL_ADDR = *((uint64_t*) (in_kernel + 4));   // 64 bits: 4 FP16s (Total 8)
}

void write_conv_dma(int dma_num, int length, uint64_t* data) {
    set_DMAC(dma_num, (uint64_t*) data, CONV_BASE, 8, 0, length/8, 3);
    start_DMA(dma_num);
}

void read_conv_dma(int dma_num, int length, uint64_t* write_addr) {
    set_DMAC(dma_num, CONV_OUTPUT_ADDR, write_addr, 0, 8, length/4, 3);
    start_DMA(dma_num);
}

void read_conv_dma_p(int dma_num, int length, uint64_t* write_addr) {
    set_DMAP(dma_num, CONV_OUTPUT_ADDR, write_addr, READ_CHECK_ADDR, 0, 8, length/4, 3, 0);
    start_DMA(dma_num);
}

void start_conv() {
    *(uint8_t*)CONV_START_ADDR = 1;
}

