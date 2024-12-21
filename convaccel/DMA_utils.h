// DMA_utils.h
#ifndef DMA_UTILS_h
#define DMA_UTILS_h

#define DMA 0x8810000

// Address for general DMA registers
#define DMA_CLOCKGATE       DMA + 0x3
#define DMA_SET             DMA + 0x100

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

#define DMA_SET_D           DMA_SET + 0x28

uint64_t MMIO_Offset = 64;

int DMA_count(int num) {
    return reg_read32(DMA_COUNT + num*MMIO_Offset);
}


void set_DMAC(uint32_t num, uint64_t addr_r, uint64_t addr_w, uint16_t inc_r, uint16_t inc_w, uint16_t len, uint16_t logd) {
    reg_write64(DMA_SET_READ + num*MMIO_Offset, addr_r);
    reg_write64(DMA_SET_WRITE + num*MMIO_Offset, addr_w);

    reg_write16(DMA_INC_READ + num*MMIO_Offset, inc_r);
    reg_write16(DMA_INC_WRITE + num*MMIO_Offset, inc_w);

    reg_write16(DMA_SET_LEN + num*MMIO_Offset, len);
    reg_write8(DMA_SET_LOGD + num*MMIO_Offset, logd);
}


void set_DMAP(uint32_t num, uint64_t* addr_r, uint64_t* addr_w, uint64_t* addr_c, uint16_t inc_r, uint16_t inc_w, uint16_t len, uint8_t logd, uint32_t count) {
    reg_write64(DMA_SET_READ + num*MMIO_Offset, addr_r);
    reg_write64(DMA_SET_WRITE + num*MMIO_Offset, addr_w);
    reg_write64(DMA_SET_C + num*MMIO_Offset, addr_c);

    reg_write16(DMA_INC_READ + num*MMIO_Offset, inc_r);
    reg_write16(DMA_INC_WRITE + num*MMIO_Offset, inc_w);

    reg_write16(DMA_SET_LEN + num*MMIO_Offset, len);
    reg_write8(DMA_SET_LOGD + num*MMIO_Offset, logd);

    reg_write32(DMA_SET_D + num*MMIO_Offset, count);
}

void start_DMA(uint32_t num) {
    reg_write8(DMA_SET + num*MMIO_Offset, 1);
}

void reset_DMA() {
    reg_write8(DMA, 1);
    reg_write8(DMA, 0);
}

void enable_Crack(){
    reg_write8(DMA_CLOCKGATE, 0);
}

void disable_Crack(){
    reg_write8(DMA_CLOCKGATE, 1);
}


int checkval(int i, long unsigned int  poll, long unsigned int ref, long unsigned int addr) {
    if (poll != ref) {
        printf("[%d]Hardware result (1) %lx does not match reference value %lx at addr: [%lx]\n", i, poll, ref, addr);
        return 1;
    } else {
        printf("[%d]Success: (%lx)\n", i, poll, ref);
    }
    return 0;

}

#endif
