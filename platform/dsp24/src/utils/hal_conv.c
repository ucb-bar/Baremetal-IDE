#include "hal_conv.h"
#include  "hal_mmio.h"

int set_conv_params(int len, uint16_t in_dilation, uint16_t* in_kernel) {
    reg_write32(CONV_LENGTH_ADDR, len);
    reg_write8(CONV_ISFLOAT_ADDR, 0);
    reg_write16(CONV_DILATION_ADDR, in_dilation);
    reg_write64(CONV_KERNEL_ADDR, *((uint64_t*) in_kernel));         // 64 bits: 4 FP16s
    reg_write64(CONV_KERNEL_ADDR, *((uint64_t*) (in_kernel + 4)));   // 64 bits: 4 FP16s (Total 8)
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
    reg_write8(CONV_START_ADDR, 1);
}

