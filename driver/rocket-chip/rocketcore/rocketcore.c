
#include <stdint.h>
#include <stddef.h>

static inline size_t get_hart_id() {
  return READ_CSR("mhartid");
}

static inline uint64_t get_cycles() {
  return READ_CSR("mcycle");
}

static inline void disable_global_interrupt() {
  CLEAR_CSR_BITS("mstatus", 1U << 3U);
}

static inline void enable_global_interrupt() {
  SET_CSR_BITS("mstatus", 1U << 3U);
}

static inline void disable_irq(uint32_t IRQn) {
  CLEAR_CSR_BITS("mie", 1U << IRQn);
}

static inline void enable_irq(uint32_t IRQn) {
  SET_CSR_BITS("mie", 1U << IRQn);
}

static inline void clear_irq(uint32_t IRQn) {
  CLEAR_CSR_BITS("mip", 1U << IRQn);
}
