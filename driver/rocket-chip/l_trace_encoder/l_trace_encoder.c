#include "l_trace_encoder.h"

void l_trace_encoder_start(LTraceEncoderType *encoder) {
  SET_BITS(encoder->TR_TE_CTRL, 0x1 << 1);
}

void l_trace_encoder_stop(LTraceEncoderType *encoder) {
  CLEAR_BITS(encoder->TR_TE_CTRL, 0x1 << 1);
}

