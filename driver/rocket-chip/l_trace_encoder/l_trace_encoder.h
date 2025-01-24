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
  __IO uint32_t TR_TE_HPM_COUNTER; //0x24
  __IO uint32_t TR_TE_HPM_TIME; //0x28
} LTraceEncoderType;

typedef struct {
  __IO uint32_t TR_SK_DMA_FLUSH;
  __I uint32_t TR_SK_DMA_FLUSH_DONE;
  __IO uint64_t TR_SK_DMA_ADDR;
  __I uint64_t TR_SK_DMA_COUNT;
} LTraceSinkDmaType;

#define TARGET_PRINT 0x0
#define TARGET_DMA 0x1
#define L_TRACE_ENCODER_BASE_ADDRESS 0x3000000

#define L_TRACE_ENCODER0 ((LTraceEncoderType *)(L_TRACE_ENCODER_BASE_ADDRESS + 0x0000))
#define L_TRACE_ENCODER1 ((LTraceEncoderType *)(L_TRACE_ENCODER_BASE_ADDRESS + 0x1000))
#define L_TRACE_ENCODER2 ((LTraceEncoderType *)(L_TRACE_ENCODER_BASE_ADDRESS + 0x2000))
#define L_TRACE_ENCODER3 ((LTraceEncoderType *)(L_TRACE_ENCODER_BASE_ADDRESS + 0x3000))

#define L_TRACE_SINK_DMA_BASE_ADDRESS 0x3010000
#define L_TRACE_SINK_DMA0 ((LTraceSinkDmaType *)(L_TRACE_SINK_DMA_BASE_ADDRESS + 0x0000))
#define L_TRACE_SINK_DMA1 ((LTraceSinkDmaType *)(L_TRACE_SINK_DMA_BASE_ADDRESS + 0x1000))
#define L_TRACE_SINK_DMA2 ((LTraceSinkDmaType *)(L_TRACE_SINK_DMA_BASE_ADDRESS + 0x2000))
#define L_TRACE_SINK_DMA3 ((LTraceSinkDmaType *)(L_TRACE_SINK_DMA_BASE_ADDRESS + 0x3000))

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

void l_trace_encoder_configure_target(LTraceEncoderType *encoder, uint64_t target);
void l_trace_sink_dma_configure_addr(LTraceSinkDmaType *sink_dma, uint64_t dma_addr);
void l_trace_sink_dma_read(LTraceSinkDmaType *sink_dma, uint8_t *buffer);
void l_trace_encoder_configure_hpm_counter_en(LTraceEncoderType *encoder, uint32_t hpm_counter);
void l_trace_encoder_configure_hpm_counter_time(LTraceEncoderType *encoder, uint32_t time);
#endif /* __L_TRACE_ENCODER_H */
