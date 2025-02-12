#include "hal_mmio.h"

void reg_write8(uintptr_t addr, uint8_t data) {
	volatile uint8_t *ptr = (volatile uint8_t *) addr;
	*ptr = data;
}

uint8_t reg_read8(uintptr_t addr) {
	volatile uint8_t *ptr = (volatile uint8_t *) addr;
	return *ptr;
}

void reg_write16(uintptr_t addr, uint16_t data) {
	volatile uint16_t *ptr = (volatile uint16_t *) addr;
	*ptr = data;
}

uint16_t reg_read16(uintptr_t addr) {
	volatile uint16_t *ptr = (volatile uint16_t *) addr;
	return *ptr;
}

void reg_write32(uintptr_t addr, uint32_t data) {
	volatile uint32_t *ptr = (volatile uint32_t *) addr;
	*ptr = data;
}

uint32_t reg_read32(uintptr_t addr) {
	volatile uint32_t *ptr = (volatile uint32_t *) addr;
	return *ptr;
}

void reg_write64(unsigned long addr, uint64_t data) {
	volatile uint64_t *ptr = (volatile uint64_t *) addr;
	*ptr = data;
}

uint64_t reg_read64(unsigned long addr) {
	volatile uint64_t *ptr = (volatile uint64_t *) addr;
	return *ptr;
}
