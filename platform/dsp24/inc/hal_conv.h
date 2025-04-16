// CONV_utils.h
#ifndef CONV_UTILS_h
#define CONV_UTILS_h


#ifdef __cplusplus
extern "C" {
#endif

#include  "hal_mmio.h"
#include "chip_config.h"


// Addresses for Convolution
#define CONV_BASE 0x08800000U
#define CONV_OUTPUT_ADDR     CONV_BASE + 0x20
#define CONV_KERNEL_ADDR     CONV_BASE + 0x40
#define CONV_START_ADDR      CONV_BASE + 0x6C
#define CONV_LENGTH_ADDR     CONV_BASE + 0x78
#define CONV_DILATION_ADDR   CONV_BASE + 0x7C
#define CONV_ISFLOAT_ADDR    CONV_BASE + 0x8E

#define READ_CHECK_ADDR   0x0880008D

int set_conv_params(int len, uint16_t in_dilation, uint16_t* in_kernel);

void write_conv_dma(int dma_num, int length, uint64_t* data);

void read_conv_dma(int dma_num, int length, uint64_t* write_addr);

void read_conv_dma_p(int dma_num, int length, uint64_t* write_addr);

void start_conv();

#ifdef __cplusplus
}
#endif

#endif