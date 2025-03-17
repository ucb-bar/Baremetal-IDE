#include <float.h>
#include <riscv_vector.h>

#include "rvv_matmul.h"
#include "l_trace_encoder.h"

#define K 2048
// static float A[6 * K];
// static float B[K * 16];
// static float C[6 * 16];

static float x[K];
static float y[K];

void nn_max1d_f32_rvv_m1(float *y, const float *x, size_t n) {
  float *x_data = x;
  vfloat32m1_t vec_max = __riscv_vfmv_s_f_f32m1(-FLT_MAX, 1);
  while (n > 0) {
    size_t vl = __riscv_vsetvl_e32m1(n);
    vfloat32m1_t vec_x = __riscv_vle32_v_f32m1(x_data, vl);
    vec_max = __riscv_vfredmax_vs_f32m1_f32m1(vec_x, vec_max, vl);
    x_data += vl;
    n -= vl;
  }
  *y = __riscv_vfmv_f_s_f32m1_f32(vec_max);
}

void nn_max1d_f32_rvv_m2(float *y, const float *x, size_t n) {
  float *x_data = x;
  vfloat32m1_t vec_max = __riscv_vfmv_s_f_f32m1(-FLT_MAX, 1);
  while (n > 0) {
    size_t vl = __riscv_vsetvl_e32m2(n);
    vfloat32m2_t vec_x = __riscv_vle32_v_f32m2(x_data, vl);
    vec_max = __riscv_vfredmax_vs_f32m2_f32m1(vec_x, vec_max, vl);
    x_data += vl;
    n -= vl;
  }
  *y = __riscv_vfmv_f_s_f32m1_f32(vec_max);
}

void nn_max1d_f32_rvv_m4(float *y, const float *x, size_t n) {
  float *x_data = x;
  vfloat32m1_t vec_max = __riscv_vfmv_s_f_f32m1(-FLT_MAX, 1);
  while (n > 0) {
    size_t vl = __riscv_vsetvl_e32m4(n);
    vfloat32m4_t vec_x = __riscv_vle32_v_f32m4(x_data, vl);
    vec_max = __riscv_vfredmax_vs_f32m4_f32m1(vec_x, vec_max, vl);
    x_data += vl;
    n -= vl;
  }
  *y = __riscv_vfmv_f_s_f32m1_f32(vec_max);
}

void nn_max1d_f32_rvv_m8(float *y, const float *x, size_t n) {
  float *x_data = x;

  vfloat32m1_t vec_max = __riscv_vfmv_s_f_f32m1(-FLT_MAX, 1);

  while (n > 0) {
    size_t vl = __riscv_vsetvl_e32m8(n);
    vfloat32m8_t vec_x = __riscv_vle32_v_f32m8(x_data, vl);
    vec_max = __riscv_vfredmax_vs_f32m8_f32m1(vec_x, vec_max, vl);
    x_data += vl;
    n -= vl;
  }
  *y = __riscv_vfmv_f_s_f32m1_f32(vec_max);
}

void nn_max1d_f32_scalar(float *y, const float *x, size_t n) {
  *y = -FLT_MAX;
  for (size_t i = 0; i < n; i += 1) {
    float val = x[i];
    *y = val > *y ? val : *y;
  }
}

void initialize() {
  // for (int i = 0; i < 6; i++) {
  //   for (int j = 0; j < K; j++) {
  //     A[i * K + j] = 3.2;
  //   }
  // }
  // for (int i = 0; i < K; i++) {
  //   for (int j = 0; j < 16; j++) {
  //     B[i * 16 + j] = 0.2;
  //   }
  // }
  // for (int i = 0; i < 6; i++) {
  //   for (int j = 0; j < 16; j++) {
  //     C[i * 16 + j] = 0.0;
  //   }
  // }
  for (int i = 0; i < K; i++) {
    x[i] = 0.2;
  }
  for (int i = 0; i < K; i++) {
    y[i] = 0.0;
  }
  return;
}

int main(int argc, char **argv) {

  // enable vector operation
  unsigned long mstatus;
  asm volatile("csrr %0, mstatus" : "=r"(mstatus));
  mstatus |= 0x00000400 | 0x00004000 | 0x00010000;
  asm volatile("csrw mstatus, %0"::"r"(mstatus));

  LTraceEncoderType *encoder = l_trace_encoder_get(get_hart_id());
  l_trace_encoder_configure_branch_mode(encoder, BRANCH_MODE_PREDICT);
  l_trace_encoder_start(encoder);

  uint64_t start = get_cycles();
  for (int i = 0; i < 1; i++)
    nn_max1d_f32_scalar(x, y, K);
  uint64_t end = get_cycles();

  uint64_t msec = (end - start);
  printf("Time taken for original max: %d cycles\n", msec);

  start = get_cycles();
  for (int i = 0; i < 1; i++)
    nn_max1d_f32_rvv_m1(x, y, K);
  end = get_cycles();

  msec = (end - start);
  printf("Time taken for rvv max m1: %d cycles\n", msec);

  start = get_cycles();
  for (int i = 0; i < 1; i++)
    nn_max1d_f32_rvv_m2(x, y, K);
  end = get_cycles();

  msec = (end - start);
  printf("Time taken for rvv max m2: %d cycles\n", msec);

  start = get_cycles();
  for (int i = 0; i < 1; i++)
    nn_max1d_f32_rvv_m4(x, y, K);
  end = get_cycles();

  msec = (end - start);
  printf("Time taken for rvv max m4: %d cycles\n", msec);

  // Calling scheduled matmul
  start = get_cycles();
  for (int i = 0; i < 1; i++)
    nn_max1d_f32_rvv_m8(x, y, K);
  end = get_cycles();

  msec = (end - start);
  printf("Time taken for rvv max m8: %d cycles\n", msec);

  return (0);

  l_trace_encoder_stop(encoder);
}
