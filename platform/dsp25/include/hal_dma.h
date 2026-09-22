/**
 * \file    hal_dma.h
 * \brief   DSP'25 DMA driver.
 */

#ifndef __HAL_DMA_H__
#define __HAL_DMA_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "metal.h"
#include "hal_mmio.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "encoding.h"

/*
 * DMA transaction descriptor written into a channel slot.
 *
 * Address/stride units are bytes.
 * `len` is the number of packets, each packet size = 2^logw bytes.
 */
typedef struct {
  uint8_t core;
  uint16_t transaction_id;
  uint8_t transaction_priority;
  uint8_t peripheral_id;
  uint64_t addr_r;
  uint64_t addr_w;
  uint16_t inc_r;
  uint16_t inc_w;
  uint16_t len;
  uint8_t logw;
  bool do_interrupt;
  bool do_address_gate;
} dma_transaction_t;

typedef struct {
  bool valid;
  uint8_t core_id;
  uint16_t transaction_id;
  bool is_error;
  uint64_t address;
} dma_interrupt_t;

#ifndef DMA_CORE_CHANNEL_COUNT
#define DMA_CORE_CHANNEL_COUNT       (2U)
#endif

#ifndef DMA_PERIPHERAL_CHANNEL_COUNT
#define DMA_PERIPHERAL_CHANNEL_COUNT (5U)
#endif

/* Channel numbering is global:
 * - core channels are [0, DMA_CORE_CHANNEL_COUNT)
 * - peripheral channels are [DMA_CORE_CHANNEL_COUNT, DMA_TOTAL_CHANNEL_COUNT)
 */
#define DMA_TOTAL_CHANNEL_COUNT (DMA_CORE_CHANNEL_COUNT + DMA_PERIPHERAL_CHANNEL_COUNT)

#define DMA_MODE_INTERRUPT_EN   (0x1U)
#define DMA_MODE_ADDRESS_GATING (0x2U)

#define ALWAYS_PRINT    0
#define PRINT_ON_ERROR  1
#define NEVER_PRINT     2

/* DMA setup and programming */
void setup_interrupts(void);
bool set_DMA_C(uint32_t channel, dma_transaction_t transaction, bool retry);
bool set_DMA_P(uint32_t channel, dma_transaction_t transaction, bool retry);
void start_DMA(uint32_t channel, uint16_t transaction_id, bool *finished);

/* DMA status/control */
int dma_status(void);
void dma_reset(void);
void dma_wait_till_inactive(int cycle_no_inflight);
void dma_wait_till_interrupt(bool *finished);
void dma_wait_till_done(size_t mhartid, bool *finished);

/* Optional polling helper for interrupt MMIO slots */
bool dma_poll_interrupt(size_t mhartid, dma_interrupt_t *out);

/* Test helpers */
bool check_val8(int i, unsigned int ref, unsigned long addr, int print);
bool check_val16(int i, unsigned int ref, unsigned long addr, int print);
bool check_val32(int i, unsigned int ref, unsigned long addr, int print);
bool check_val64(int i, unsigned long ref, unsigned long addr, int print);

size_t ticks(void);
void end_dma(void);

#ifdef __cplusplus
}
#endif

#endif /* __HAL_DMA_H__ */
