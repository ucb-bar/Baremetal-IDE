#include "hal_fft.h"

void reset_fft() {
    *(uint8_t*)FFT_RESET = 1;
    *(uint8_t*)FFT_RESET = 0;
}

int fft_busy() {
    return *(uint8_t*)FFT_BUSY;
}

int fft_count_left() {
    return *(uint8_t*)FFT_LENGTH;
}

void write_fft(int length, uint64_t* data) {
    *(uint32_t*)FFT_LENGTH = length;

    for(int i=0; i<(length/2); i+=1) {
        *(uint64_t*)FFT_INPUT = data[i];
    }
}

void write_fft_dma(int dma_num, int length, uint64_t* data) {
    *(uint32_t*)FFT_LENGTH = length;

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
    return *(uint32_t*)FFT_OUTPUT;
}