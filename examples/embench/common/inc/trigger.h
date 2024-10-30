#ifndef __TRIGGER_H
#define __TRIGGER_H

#include "l_trace_encoder.h"

static inline void start_trigger(void) {
  LTraceEncoderType *encoder = l_trace_encoder_get(get_hart_id());
  l_trace_encoder_start(encoder);
}

static inline void stop_trigger(void) {
  LTraceEncoderType *encoder = l_trace_encoder_get(get_hart_id());
  l_trace_encoder_stop(encoder);
}

#endif /* __TRIGGER_H */