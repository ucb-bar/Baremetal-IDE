#include "hal_fft.h"
#include  "hal_mmio.h"

void reset_fft() {
    reg_write8(FFT_RESET, 1);
    reg_write8(FFT_RESET, 0);
}

int fft_busy() {
    return reg_read8(FFT_BUSY);
}

int fft_count_left() {
    return reg_read8(FFT_LENGTH);
}

void write_fft(int length, uint64_t* data) {
    reg_write32(FFT_LENGTH, length);

    for(int i=0; i<(length/2); i+=1) {
        reg_write64(FFT_INPUT, data[i]);
    }
}

void write_fft_dma(int dma_num, int length, uint64_t* data) {
    reg_write32(FFT_LENGTH, length);

    set_DMAC(dma_num, data, FFT_INPUT, 8, 0, length/2, 3);
    start_DMA(dma_num);
}

void read_fft_dma(int dma_num, int length, uint64_t* write_addr) {
    set_DMAC(dma_num, FFT_OUTPUT, write_addr, 0, 8, length, 2);
    start_DMA(dma_num);
}

void read_fft_real_dma(int dma_num, int length, uint64_t* write_addr) {
    set_DMAC(dma_num, FFT_OUTPUT, write_addr, 0, 4, length, 1);
    start_DMA(dma_num);
}

uint32_t read_fft() {
    return reg_read32(FFT_OUTPUT);
}