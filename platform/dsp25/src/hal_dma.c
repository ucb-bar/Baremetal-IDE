#include "hal_dma.h"
#include "chip_config.h"
#include <stdio.h>

#define DMA_MMIO_BASE        ((uintptr_t)DMA_BASE)
#define DMA_RESET_REG        (DMA_MMIO_BASE + 0x00UL)
#define DMA_INFLIGHT_STATUS  (DMA_MMIO_BASE + 0x01UL)

#define CHANNEL_BASE         (DMA_MMIO_BASE + 0x100UL)
#define CHANNEL_OFFSET       (64UL)

#define DMA_START            (CHANNEL_BASE + 0x00UL)
#define DMA_READY            (CHANNEL_BASE + 0x02UL)
#define DMA_FIFO_LENGTH      (CHANNEL_BASE + 0x03UL)
#define DMA_CORE_ID          (CHANNEL_BASE + 0x04UL)
#define DMA_TRANSACTION_ID   (CHANNEL_BASE + 0x08UL)
#define DMA_PERIPHERAL_ID    (CHANNEL_BASE + 0x0AUL)
#define DMA_TRANSACTION_PRI  (CHANNEL_BASE + 0x0CUL)
#define DMA_MODE             (CHANNEL_BASE + 0x0EUL)
#define DMA_ADDR_R           (CHANNEL_BASE + 0x10UL)
#define DMA_ADDR_W           (CHANNEL_BASE + 0x18UL)
#define DMA_LEN              (CHANNEL_BASE + 0x20UL)
#define DMA_LOGW             (CHANNEL_BASE + 0x22UL)
#define DMA_INC_R            (CHANNEL_BASE + 0x24UL)
#define DMA_INC_W            (CHANNEL_BASE + 0x26UL)
#define DMA_BUSY             (CHANNEL_BASE + 0x28UL)

#define INTERRUPT_BASE       (DMA_MMIO_BASE + 0x10UL)
#define INTERRUPT_OFFSET     (16UL)

#define DMA_INT_SERVICED     (INTERRUPT_BASE + 0x00UL)
#define DMA_INT_VALID        (INTERRUPT_BASE + 0x01UL)
#define DMA_INT_TID          (INTERRUPT_BASE + 0x02UL)
#define DMA_INT_IS_ERROR     (INTERRUPT_BASE + 0x04UL)
#define DMA_INT_ADDRESS      (INTERRUPT_BASE + 0x08UL)

#define DMA_INTERRUPT_ID_CORE_0  (4U)
#define DMA_INTERRUPT_ID_CORE_1  (5U)

#define PLIC_ENABLE_CORE_0   (PLIC_BASE + 0x2000UL)
#define PLIC_ENABLE_CORE_1   (PLIC_BASE + 0x2080UL)
#define PLIC_CLAIM_CORE_0    (PLIC_BASE + 0x200004UL)
#define PLIC_CLAIM_CORE_1    (PLIC_BASE + 0x201004UL)

#define PLIC_PRIO_DMA_CORE_0 (PLIC_BASE + (4UL * DMA_INTERRUPT_ID_CORE_0))
#define PLIC_PRIO_DMA_CORE_1 (PLIC_BASE + (4UL * DMA_INTERRUPT_ID_CORE_1))

#define DMA_MAX_TRACKED_TRANSACTIONS 64

typedef struct {
  bool in_use;
  uint16_t transaction_id;
  volatile bool *complete;
} dma_completion_slot_t;

static dma_completion_slot_t g_completion_slots[DMA_MAX_TRACKED_TRANSACTIONS];

static uintptr_t dma_channel_offset(uint32_t channel) {
  return ((uintptr_t)channel) * CHANNEL_OFFSET;
}

static bool dma_channel_is_core(uint32_t channel) {
  return channel < DMA_CORE_CHANNEL_COUNT;
}

static bool dma_channel_is_peripheral(uint32_t channel) {
  return (channel >= DMA_CORE_CHANNEL_COUNT) && (channel < DMA_TOTAL_CHANNEL_COUNT);
}

static uintptr_t dma_core_offset(size_t mhartid) {
  return ((uintptr_t)mhartid) * INTERRUPT_OFFSET;
}

static bool tracker_add(uint16_t transaction_id, volatile bool *complete) {
  size_t i;

  if (complete == 0) {
    return false;
  }

  for (i = 0; i < DMA_MAX_TRACKED_TRANSACTIONS; ++i) {
    if (g_completion_slots[i].in_use && g_completion_slots[i].transaction_id == transaction_id) {
      g_completion_slots[i].complete = complete;
      return true;
    }
  }

  for (i = 0; i < DMA_MAX_TRACKED_TRANSACTIONS; ++i) {
    if (!g_completion_slots[i].in_use) {
      g_completion_slots[i].in_use = true;
      g_completion_slots[i].transaction_id = transaction_id;
      g_completion_slots[i].complete = complete;
      return true;
    }
  }

  return false;
}

static bool tracker_complete(uint16_t transaction_id) {
  size_t i;

  for (i = 0; i < DMA_MAX_TRACKED_TRANSACTIONS; ++i) {
    if (g_completion_slots[i].in_use && g_completion_slots[i].transaction_id == transaction_id) {
      if (g_completion_slots[i].complete != 0) {
        *(g_completion_slots[i].complete) = true;
      }
      g_completion_slots[i].in_use = false;
      g_completion_slots[i].complete = 0;
      return true;
    }
  }

  return false;
}

static void tracker_clear(void) {
  size_t i;

  for (i = 0; i < DMA_MAX_TRACKED_TRANSACTIONS; ++i) {
    g_completion_slots[i].in_use = false;
    g_completion_slots[i].transaction_id = 0;
    g_completion_slots[i].complete = 0;
  }
}

static uint8_t mode_from_transaction(dma_transaction_t transaction) {
  uint8_t mode = 0;

  if (transaction.do_interrupt) {
    mode |= DMA_MODE_INTERRUPT_EN;
  }
  if (transaction.do_address_gate) {
    mode |= DMA_MODE_ADDRESS_GATING;
  }

  return mode;
}

static uint8_t default_mode_for_channel(uint32_t channel) {
  if (dma_channel_is_core(channel)) {
    return DMA_MODE_INTERRUPT_EN;
  }
  return (uint8_t)(DMA_MODE_INTERRUPT_EN | DMA_MODE_ADDRESS_GATING);
}

static bool set_DMA_common(uint32_t channel, dma_transaction_t transaction, bool retry) {
  uintptr_t channel_offset = dma_channel_offset(channel);
  uint32_t lock_val;

  do {
    lock_val = reg_amo_swap32(DMA_BUSY + channel_offset, 1U);
    if (!retry && lock_val != 0U) {
      return false;
    }
  } while (lock_val != 0U);

  reg_write8(DMA_CORE_ID + channel_offset, transaction.core);
  reg_write16(DMA_TRANSACTION_ID + channel_offset, transaction.transaction_id);
  reg_write8(DMA_TRANSACTION_PRI + channel_offset, transaction.transaction_priority);
  reg_write8(DMA_LOGW + channel_offset, transaction.logw);
  reg_write64(DMA_ADDR_R + channel_offset, transaction.addr_r);
  reg_write64(DMA_ADDR_W + channel_offset, transaction.addr_w);
  reg_write16(DMA_LEN + channel_offset, transaction.len);

  return true;
}

void setup_interrupts(void) {
  reg_write32(PLIC_ENABLE_CORE_0, (1U << DMA_INTERRUPT_ID_CORE_0));
  reg_write32(PLIC_ENABLE_CORE_1, (1U << DMA_INTERRUPT_ID_CORE_1));

  reg_write32(PLIC_PRIO_DMA_CORE_0, 5U);
  reg_write32(PLIC_PRIO_DMA_CORE_1, 5U);

  set_csr(mie, MIP_MEIP);
  set_csr(mstatus, MSTATUS_MIE);
}

void machine_external_interrupt_callback(void) {
  size_t mhartid = read_csr(mhartid);
  uintptr_t plic_claim_addr;
  uint32_t expected_irq;
  uintptr_t core_offset;
  uint32_t irq_id;

  if (mhartid == 0U) {
    plic_claim_addr = PLIC_CLAIM_CORE_0;
    expected_irq = DMA_INTERRUPT_ID_CORE_0;
  } else if (mhartid == 1U) {
    plic_claim_addr = PLIC_CLAIM_CORE_1;
    expected_irq = DMA_INTERRUPT_ID_CORE_1;
  } else {
    return;
  }

  irq_id = reg_read32(plic_claim_addr);
  core_offset = dma_core_offset(mhartid);

  if (irq_id == expected_irq) {
    uint16_t tid = reg_read16(DMA_INT_TID + core_offset);

    reg_write8(DMA_INT_SERVICED + core_offset, 1U);
    tracker_complete(tid);
  }

  if (irq_id != 0U) {
    reg_write32(plic_claim_addr, irq_id);
  }
}

bool set_DMA_C(uint32_t channel, dma_transaction_t transaction, bool retry) {
  uintptr_t channel_offset = dma_channel_offset(channel);
  uint8_t mode = mode_from_transaction(transaction);

  if (!dma_channel_is_core(channel)) {
    printf("[dma] set_DMA_C invalid core channel %u (core channels: 0-%u)\n",
           (unsigned)channel,
           (unsigned)(DMA_CORE_CHANNEL_COUNT - 1U));
    return false;
  }

  if (!set_DMA_common(channel, transaction, retry)) {
    return false;
  }

  reg_write16(DMA_INC_R + channel_offset, transaction.inc_r);
  reg_write16(DMA_INC_W + channel_offset, transaction.inc_w);
  if (mode != default_mode_for_channel(channel)) {
    reg_write8(DMA_MODE + channel_offset, mode);
  }

  return true;
}

bool set_DMA_P(uint32_t channel, dma_transaction_t transaction, bool retry) {
  uintptr_t channel_offset = dma_channel_offset(channel);
  uint8_t mode = mode_from_transaction(transaction);

  if (!dma_channel_is_peripheral(channel)) {
    printf("[dma] set_DMA_P invalid peripheral channel %u (peripheral channels: %u-%u)\n",
           (unsigned)channel,
           (unsigned)DMA_CORE_CHANNEL_COUNT,
           (unsigned)(DMA_TOTAL_CHANNEL_COUNT - 1U));
    return false;
  }

  if (!set_DMA_common(channel, transaction, retry)) {
    return false;
  }

  if (transaction.inc_r != 0U) {
    reg_write16(DMA_INC_R + channel_offset, transaction.inc_r);
  }
  if (transaction.inc_w != 0U) {
    reg_write16(DMA_INC_W + channel_offset, transaction.inc_w);
  }
  reg_write8(DMA_PERIPHERAL_ID + channel_offset, transaction.peripheral_id);
  if (mode != default_mode_for_channel(channel)) {
    reg_write8(DMA_MODE + channel_offset, mode);
  }

  return true;
}

void start_DMA(uint32_t channel, uint16_t transaction_id, bool *finished) {
  uintptr_t channel_offset = dma_channel_offset(channel);

  if (finished != 0) {
    *finished = false;
    (void)tracker_add(transaction_id, (volatile bool *)finished);
  }

  reg_write8(DMA_START + channel_offset, 1U);
}

int dma_status(void) {
  return (int)reg_read8(DMA_INFLIGHT_STATUS);
}

void dma_reset(void) {
  /* Issue a reset pulse; holding this bit high can keep DMA logic in reset. */
  reg_write8(DMA_RESET_REG, 1U);
  reg_write8(DMA_RESET_REG, 0U);
}

void dma_wait_till_inactive(int cycle_no_inflight) {
  while (1) {
    volatile int t = 0;
    while ((t < cycle_no_inflight) && (dma_status() == 0)) {
      t++;
    }
    if (t == cycle_no_inflight) {
      break;
    }
  }
}

void dma_wait_till_interrupt(bool *finished) {
  volatile int spin = 0;

  if (finished == 0) {
    return;
  }

  while (!(*finished)) {
    spin = spin + 1;
  }
}

bool dma_poll_interrupt(size_t mhartid, dma_interrupt_t *out) {
  uintptr_t core_offset = dma_core_offset(mhartid);
  bool valid = reg_read8(DMA_INT_VALID + core_offset) != 0;
  uint16_t tid;

  if (!valid) {
    return false;
  }

  tid = reg_read16(DMA_INT_TID + core_offset);

  if (out != 0) {
    out->valid = true;
    out->core_id = (uint8_t)mhartid;
    out->transaction_id = tid;
    out->is_error = (reg_read8(DMA_INT_IS_ERROR + core_offset) != 0);
    out->address = reg_read64(DMA_INT_ADDRESS + core_offset);
  }

  reg_write8(DMA_INT_SERVICED + core_offset, 1U);
  tracker_complete(tid);

  return true;
}

void dma_wait_till_done(size_t mhartid, bool *finished) {
  if (finished == 0) {
    while (!dma_poll_interrupt(mhartid, 0)) {
      ;
    }
    return;
  }

  while (!(*finished)) {
    (void)dma_poll_interrupt(mhartid, 0);
  }
}

#define CHECK_VAL_BODY(bits) \
  do { \
    unsigned int poll = reg_read##bits(addr); \
    if (poll != ref) { \
      if (print == PRINT_ON_ERROR || print == ALWAYS_PRINT) { \
        printf("[%d] 0x%x != 0x%x at [0x%lx]\n", i, poll, ref, addr); \
      } \
      return true; \
    } \
    if (print == ALWAYS_PRINT) { \
      printf("[%d] OK: 0x%x\n", i, ref); \
    } \
    return false; \
  } while (0)

bool check_val8(int i, unsigned int ref, unsigned long addr, int print) {
  CHECK_VAL_BODY(8);
}

bool check_val16(int i, unsigned int ref, unsigned long addr, int print) {
  CHECK_VAL_BODY(16);
}

bool check_val32(int i, unsigned int ref, unsigned long addr, int print) {
  CHECK_VAL_BODY(32);
}

bool check_val64(int i, unsigned long ref, unsigned long addr, int print) {
  unsigned long poll = reg_read64(addr);

  if (poll != ref) {
    if (print == PRINT_ON_ERROR || print == ALWAYS_PRINT) {
      printf("[%d] 0x%lx != 0x%lx at [0x%lx]\n", i, poll, ref, addr);
    }
    return true;
  }

  if (print == ALWAYS_PRINT) {
    printf("[%d] OK: 0x%lx\n", i, ref);
  }

  return false;
}

size_t ticks(void) {
  return read_csr(mcycle);
}

void end_dma(void) {
  tracker_clear();
}
