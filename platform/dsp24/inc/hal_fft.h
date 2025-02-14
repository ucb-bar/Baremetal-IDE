// FFT_utils.h
#ifndef FFT_UTILS_h
#define FFT_UTILS_h

#ifdef __cplusplus
extern "C" {
#endif

#include "hal_DMA.h"
#include  "hal_mmio.h"
#include "chip_config.h"

#define FFT_BUSY            FFT_BASE + 0x08L
#define FFT_LENGTH          FFT_BASE + 0x18L
#define FFT_INPUT           FFT_BASE + 0x20L
#define FFT_INPUT_READY     FFT_BASE + 0x28L
#define FFT_OUTPUT          FFT_BASE + 0x30L
#define FFT_OUTPUT_VALID    FFT_BASE + 0x38L
#define FFT_RESET           FFT_BASE + 0x40L

void reset_fft();

int fft_busy();

int fft_count_left();

void write_fft(int length, uint64_t* data);

void write_fft_dma(int dma_num, int length, uint64_t* data);

void read_fft_dma(int dma_num, int length, uint64_t* write_addr);

void read_fft_real_dma(int dma_num, int length, uint64_t* write_addr);

uint32_t read_fft();

#ifdef __cplusplus
}
#endif

#endif