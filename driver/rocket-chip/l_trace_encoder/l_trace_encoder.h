#ifndef __L_TRACE_ENCODER_H
#define __L_TRACE_ENCODER_H

#include "metal.h"
#include "rocketcore.h"

typedef struct {
  uint32_t TR_TE_CTRL;
} LTraceEncoderType;

#define L_TRACE_ENCODER_BASE_ADDRESS 0x10000000

#define L_TRACE_ENCODER0 ((LTraceEncoderType *)(L_TRACE_ENCODER_BASE_ADDRESS + 0x0000))
#define L_TRACE_ENCODER1 ((LTraceEncoderType *)(L_TRACE_ENCODER_BASE_ADDRESS + 0x1000))
#define L_TRACE_ENCODER2 ((LTraceEncoderType *)(L_TRACE_ENCODER_BASE_ADDRESS + 0x2000))
#define L_TRACE_ENCODER3 ((LTraceEncoderType *)(L_TRACE_ENCODER_BASE_ADDRESS + 0x3000))

static inline LTraceEncoderType *l_trace_encoder_get(uint32_t hart_id) {
  return (LTraceEncoderType *)(L_TRACE_ENCODER_BASE_ADDRESS + hart_id * 0x1000);
}

void l_trace_encoder_start(LTraceEncoderType *encoder);

void l_trace_encoder_stop(LTraceEncoderType *encoder);

#endif /* __L_TRACE_ENCODER_H */