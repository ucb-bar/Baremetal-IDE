#include <stdint.h>
#include "riscv.h"
#include "riscv_encoding.h"
#include "l_trace_encoder.h"
#include "math.h"
// #include "mada_timer.h"

#define NUM_ITERS 60 // 0 to 2pi, 60 steps

void FOC_invParkTransform(float *v_alpha, float *v_beta, float v_q, float v_d, float sin_theta, float cos_theta) {
  *v_alpha  = -(sin_theta * v_q) + (cos_theta * v_d);
  *v_beta   =  (cos_theta * v_q) + (sin_theta * v_d);
}

void FOC_invClarkSVPWM(float *v_a, float *v_b, float *v_c, float v_alpha, float v_beta) {
  float v_a_phase = v_alpha;
  float v_b_phase = (-.5f * v_alpha) + ((sqrtf(3.f)/2.f) * v_beta);
  float v_c_phase = (-.5f * v_alpha) - ((sqrtf(3.f)/2.f) * v_beta);

  float v_neutral = .5f * (fmaxf(fmaxf(v_a_phase, v_b_phase), v_c_phase) + fminf(fminf(v_a_phase, v_b_phase), v_c_phase));

  *v_a = v_a_phase - v_neutral;
  *v_b = v_b_phase - v_neutral;
  *v_c = v_c_phase - v_neutral;
}

void FOC_update(float *v_a, float *v_b, float *v_c, float vq, float vd) {
  float v_alpha, v_beta;
  FOC_invParkTransform(&v_alpha, &v_beta, vq, vd, sin(vq), cos(vq));
  FOC_invClarkSVPWM(v_a, v_b, v_c, v_alpha, v_beta);
}

int main(int argc, char **argv) {
  LTraceEncoderType *encoder = l_trace_encoder_get(get_hart_id());
  l_trace_encoder_configure_target(encoder, TARGET_PRINT);
  l_trace_encoder_start(encoder);

  for (int i = 0; i < 10; i++) {
    __asm__("nop");
  }

  // warmup
  // float v_a = 0, v_b = 0, v_c = 0;
  // FOC_update(&v_a, &v_b, &v_c, 0, 0);

  for (int i = 0; i < NUM_ITERS; i++) {
    float vq = 2 * M_PI * i / NUM_ITERS;
    float vd = 0;
    float v_alpha, v_beta, v_a, v_b, v_c;
    for (int j = 0; j < 2; j++) {
      FOC_update(&v_a, &v_b, &v_c, vq, vd);
    }
  }

  l_trace_encoder_stop(encoder);

  // spin for a bit, to make sure the trace buffer is flushed
  for (int i = 0; i < 10; i++) {
    __asm__("nop");
  }
}
