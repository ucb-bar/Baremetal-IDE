#include "hal_DMA.h"
#include  "hal_mmio.h"


#define DMA_MMIO_Offset         64
int DMA_count(int num) {
    return reg_read32(DMA_COUNT + num*DMA_MMIO_Offset);
}


void set_DMAC(uint32_t num, uint64_t addr_r, uint64_t addr_w, uint16_t inc_r, uint16_t inc_w, uint16_t len, uint16_t logd) {
    reg_write64(DMA_SET_READ + num*DMA_MMIO_Offset, addr_r);
    reg_write64(DMA_SET_WRITE + num*DMA_MMIO_Offset, addr_w);

    reg_write16(DMA_INC_READ + num*DMA_MMIO_Offset, inc_r);
    reg_write16(DMA_INC_WRITE + num*DMA_MMIO_Offset, inc_w);

    reg_write16(DMA_SET_LEN + num*DMA_MMIO_Offset, len);
    reg_write8(DMA_SET_LOGD + num*DMA_MMIO_Offset, logd);
}


void set_DMAP(uint32_t num, uint64_t* addr_r, uint64_t* addr_w, uint64_t* addr_c, uint16_t inc_r, uint16_t inc_w, uint16_t len, uint8_t logd, uint32_t count) {
    reg_write64(DMA_SET_READ + num*DMA_MMIO_Offset, addr_r);
    reg_write64(DMA_SET_WRITE + num*DMA_MMIO_Offset, addr_w);
    reg_write64(DMA_SET_C + num*DMA_MMIO_Offset, addr_c);

    reg_write16(DMA_INC_READ + num*DMA_MMIO_Offset, inc_r);
    reg_write16(DMA_INC_WRITE + num*DMA_MMIO_Offset, inc_w);

    reg_write16(DMA_SET_LEN + num*DMA_MMIO_Offset, len);
    reg_write8(DMA_SET_LOGD + num*DMA_MMIO_Offset, logd);

    reg_write32(DMA_SET_D + num*DMA_MMIO_Offset, count);
}

void start_DMA(uint32_t num) {
    reg_write8(DMA_SET + num*DMA_MMIO_Offset, 1);
}

void reset_DMA() {
    reg_write8(DMA_BASE, 1);
    reg_write8(DMA_BASE, 0);
}

void enable_Crack(){
    reg_write8(DMA_CLOCKGATE, 0);
}

void disable_Crack(){
    reg_write8(DMA_CLOCKGATE, 1);
}
