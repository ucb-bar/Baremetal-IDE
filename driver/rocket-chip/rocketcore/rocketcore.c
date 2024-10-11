
#include <stdint.h>
#include <stddef.h>

static inline size_t getHartId() {
  return READ_CSR("mhartid");
}

static inline uint64_t getCycles() {
  return READ_CSR("mcycle");
}

static inline void disableGlobalInterrupt() {
  CLEAR_CSR_BITS("mstatus", 1U << 3U);
}

static inline void enableGlobalInterrupt() {
  SET_CSR_BITS("mstatus", 1U << 3U);
}

static inline void disableIRQ(uint32_t IRQn) {
  CLEAR_CSR_BITS("mie", 1U << IRQn);
}

static inline void enableIRQ(uint32_t IRQn) {
  SET_CSR_BITS("mie", 1U << IRQn);
}

static inline void clearIRQ(uint32_t IRQn) {
  CLEAR_CSR_BITS("mip", 1U << IRQn);
}
