#include "l_trace_encoder.h"

int main(int argc, char **argv) {
  LTraceEncoderType *encoder = l_trace_encoder_get(get_hart_id());
  l_trace_encoder_configure_branch_mode(encoder, BRANCH_MODE_PREDICT);
  // l_trace_encoder_configure_branch_mode(encoder, BRANCH_MODE_TARGET);
  l_trace_encoder_start(encoder);

  printf("Hello, world from %d\n", get_hart_id());

  l_trace_encoder_stop(encoder);
}
