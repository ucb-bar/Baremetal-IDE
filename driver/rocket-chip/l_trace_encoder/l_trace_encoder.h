#ifndef __L_TRACE_ENCODER_H
#define __L_TRACE_ENCODER_H

#include <stdio.h>

#include "metal.h"
#include "rocketcore.h"

typedef struct {
  __IO uint32_t TR_TE_CTRL; //0x00
  __I uint32_t TR_TE_INFO; //0x04
  __IO uint32_t TR_TE_BUBBLE[6]; //0x08-0x1C
  __IO uint32_t TR_TE_TARGET; //0x20
  __IO uint32_t TR_TE_BRANCH_MODE; //0x24
  __IO uint64_t TR_TE_STALL; //0x28
  __IO uint32_t TR_TE_SYNC_PERIOD; // 0x30
} LTraceEncoderType;

typedef struct {
  __IO uint64_t TR_SK_DMA_ADDR; // 0x00
  __I uint64_t TR_SK_DMA_COUNT; // 0x08
  __IO uint64_t TR_SK_DMA_MAX_SIZE; // 0x10
  __IO uint32_t TR_SK_DMA_RESET; // 0x18
  __IO uint32_t TR_SK_DMA_MODE; // 0x1C
  __IO uint32_t TR_SK_DMA_WRAP_COUNT; // 0x20
  __IO uint32_t TR_SK_DMA_SRC_READY_STALLED_COUNT;
} LTraceSinkDmaType;

// Trace Sink Targets
#define TARGET_PRINT 0x0
#define TARGET_DMA 0x1
#define L_TRACE_ENCODER_BASE_ADDRESS 0x3000000

// Trace Branch Mode
#define BRANCH_MODE_TARGET    0x0
#define BRANCH_MODE_RESERVED0 0x1
#define BRANCH_MODE_PREDICT   0x2
#define BRANCH_MODE_RESERVED1 0x3

// Trace DMA Mode
#define DMA_MODE_OVERFLOW    0
#define DMA_MODE_RING 1

// SBUS Bypass 
#define SBUS_BYPASS_ADDRESS 0x1000000000

#define L_TRACE_ENCODER0 ((LTraceEncoderType *)(L_TRACE_ENCODER_BASE_ADDRESS + 0x0000))
#define L_TRACE_ENCODER1 ((LTraceEncoderType *)(L_TRACE_ENCODER_BASE_ADDRESS + 0x1000))
#define L_TRACE_ENCODER2 ((LTraceEncoderType *)(L_TRACE_ENCODER_BASE_ADDRESS + 0x2000))
#define L_TRACE_ENCODER3 ((LTraceEncoderType *)(L_TRACE_ENCODER_BASE_ADDRESS + 0x3000))

#define L_TRACE_SINK_DMA_BASE_ADDRESS 0x3010000
#define L_TRACE_SINK_DMA0 ((LTraceSinkDmaType *)(L_TRACE_SINK_DMA_BASE_ADDRESS + 0x0000))
#define L_TRACE_SINK_DMA1 ((LTraceSinkDmaType *)(L_TRACE_SINK_DMA_BASE_ADDRESS + 0x1000))
#define L_TRACE_SINK_DMA2 ((LTraceSinkDmaType *)(L_TRACE_SINK_DMA_BASE_ADDRESS + 0x2000))
#define L_TRACE_SINK_DMA3 ((LTraceSinkDmaType *)(L_TRACE_SINK_DMA_BASE_ADDRESS + 0x3000))

#define SBUS_BYPASS_ADDRESS 0x1000000000

static inline LTraceEncoderType *l_trace_encoder_get(uint32_t hart_id) {
  return (LTraceEncoderType *)(L_TRACE_ENCODER_BASE_ADDRESS + hart_id * 0x1000);
}

static inline LTraceSinkDmaType *l_trace_sink_dma_get(uint32_t hart_id) {
  return (LTraceSinkDmaType *)(L_TRACE_SINK_DMA_BASE_ADDRESS + hart_id * 0x1000);
}

static inline void l_trace_encoder_start(LTraceEncoderType *encoder) {
  SET_BITS(encoder->TR_TE_CTRL, 0x1 << 1);
}

static inline void l_trace_encoder_stop(LTraceEncoderType *encoder) {
  CLEAR_BITS(encoder->TR_TE_CTRL, 0x1 << 1);
}

static inline void l_trace_encoder_configure_target(LTraceEncoderType *encoder, uint64_t target) {
  encoder->TR_TE_TARGET = target;
}

static inline void l_trace_encoder_configure_branch_mode(LTraceEncoderType *encoder, uint64_t branch_mode) {
  encoder->TR_TE_BRANCH_MODE = branch_mode;
}

static inline void l_trace_encoder_configure_sync_period(LTraceEncoderType *encoder, uint32_t period) {
  encoder->TR_TE_SYNC_PERIOD = period;
}

static inline void l_trace_sink_dma_configure_mode(LTraceSinkDmaType *sink_dma, uint32_t mode) {
  sink_dma->TR_SK_DMA_MODE = mode;
}

static inline void l_trace_sink_dma_configure_addr_and_size(LTraceSinkDmaType *sink_dma, uint64_t dma_addr, uint64_t dma_size, int bypass) {
  sink_dma->TR_SK_DMA_ADDR = bypass ? (SBUS_BYPASS_ADDRESS|dma_addr) : dma_addr;
  sink_dma->TR_SK_DMA_MAX_SIZE = dma_size;
}

static inline void l_trace_sink_dma_reset(LTraceSinkDmaType *sink_dma) {
  sink_dma->TR_SK_DMA_RESET = 1;
}

static inline uint64_t l_trace_sink_dma_get_count(LTraceSinkDmaType *sink_dma) {
  return sink_dma->TR_SK_DMA_COUNT;
}

static inline uint32_t l_trace_sink_dma_get_wrap_count(LTraceSinkDmaType *sink_dma) {
  return sink_dma->TR_SK_DMA_WRAP_COUNT;
}

static inline uint64_t l_trace_encoder_get_stall(LTraceEncoderType *encoder) {
  return encoder->TR_TE_STALL;
}

void l_trace_sink_dma_read(LTraceSinkDmaType *sink_dma, uint8_t *buffer);
#endif /* __L_TRACE_ENCODER_H */
