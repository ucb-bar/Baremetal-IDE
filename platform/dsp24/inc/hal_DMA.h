// DMA_utils.h
#ifndef DMA_UTILS_h
#define DMA_UTILS_h

#ifdef __cplusplus
extern "C" {
#endif

#include "chip_config.h"
#include  "hal_mmio.h"

// Address for general DMA registers
#define DMA_CLOCKGATE       DMA_BASE + 0x3
#define DMA_SET             DMA_BASE + 0x100

// Addresses for each DMA channel
#define DMA_SET_LOGD        DMA_SET + 0x1
#define DMA_READY           DMA_SET + 0x2
#define DMA_COUNT           DMA_SET + 0x4

#define DMA_SET_READ        DMA_SET + 0x8
#define DMA_SET_WRITE       DMA_SET + 0x10
#define DMA_SET_C           DMA_SET + 0x18

#define DMA_SET_LEN         DMA_SET + 0x20
#define DMA_INC_READ        DMA_SET + 0x22
#define DMA_INC_WRITE       DMA_SET + 0x24

#define DMA_SET_D           DMA_SET + 0x2

int DMA_count(int num);


void set_DMAC(uint32_t num, uint64_t addr_r, uint64_t addr_w, uint16_t inc_r, uint16_t inc_w, uint16_t len, uint16_t logd);


void set_DMAP(uint32_t num, uint64_t* addr_r, uint64_t* addr_w, uint64_t* addr_c, uint16_t inc_r, uint16_t inc_w, uint16_t len, uint8_t logd, uint32_t count);

void start_DMA(uint32_t num);

void reset_DMA();

void enable_Crack();

void disable_Crack();

#ifdef __cplusplus
}
#endif

#endif