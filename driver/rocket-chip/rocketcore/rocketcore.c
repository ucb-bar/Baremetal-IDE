
#include <rv/interrupt.h>

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

static inline void disableIRQ(InterruptType IRQn) {
  CLEAR_CSR_BITS("mie", 1U << (uint32_t)IRQn);
}

static inline void enableIRQ(InterruptType IRQn) {
  SET_CSR_BITS("mie", 1U << (uint32_t)IRQn);
}

static inline void clearIRQ(InterruptType IRQn) {
  CLEAR_CSR_BITS("mip", 1U << (uint32_t)IRQn);
}
