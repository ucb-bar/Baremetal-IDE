#include "rvv_matmul.h"

#include <stdio.h>
#include <stdlib.h>

#include <riscv_vector.h>
// rank_k_reduce_6x16(
//     K : size,
//     A : f32[6, K] @DRAM,
//     B : f32[K, 16] @DRAM,
//     C : f32[6, 16] @DRAM
// )
void rank_k_reduce_6x16( void *ctxt, int_fast32_t K, const float* A, const float* B, float* C ) {
for (int_fast32_t i = 0; i < 6; i++) {
  for (int_fast32_t j = 0; j < 16; j++) {
    for (int_fast32_t k = 0; k < K; k++) {
      C[i * 16 + j] += A[i * K + k] * B[k * 16 + j];
    }
  }
}
}

// rank_k_reduce_6x16_scheduled(
//     K : size,
//     A : f32[6, K] @DRAM,
//     B : f32[K, 16] @DRAM,
//     C : f32[6, 16] @DRAM
// )
void rank_k_reduce_6x16_scheduled( void *ctxt, int_fast32_t K, const float* A, const float* B, float* C ) {
register vfloat32m1_t C_reg[6][4];
for (int_fast32_t i0 = 0; i0 < 6; i0++) {
  for (int_fast32_t i2 = 0; i2 < 4; i2++) {
    C_reg[i0][i2] = __riscv_vle32_v_f32m1(&C[(i0) * (16) + 4 * i2],(4));
  }
}
for (int_fast32_t k = 0; k < K; k++) {
  register vfloat32m1_t B_reg[4];
  for (int_fast32_t io = 0; io < 4; io++) {
    B_reg[io] = __riscv_vle32_v_f32m1(&B[(k) * (16) + 4 * io],(4));
  }
  for (int_fast32_t i = 0; i < 6; i++) {
    register vfloat32m1_t A_reg;
    A_reg = __riscv_vfmv_v_f_f32m1(A[(i) * K + k],(4));
    for (int_fast32_t jo = 0; jo < 4; jo++) {
      C_reg[i][jo] = __riscv_vfmacc_vv_f32m1(C_reg[i][jo], A_reg, B_reg[jo],(4));
    }
  }
}
for (int_fast32_t i0 = 0; i0 < 6; i0++) {
  for (int_fast32_t i2 = 0; i2 < 4; i2++) {
    __riscv_vse32_v_f32m1(&C[(i0) * (16) + 4 * i2], C_reg[i0][i2],(4));
  }
}
}


/* relying on the following instruction..."
rvv_broadcast_4xf32(dst,src,vl)
{dst_data} = __riscv_vfmv_v_f_f32m1({src_data},{vl});
*/

/* relying on the following instruction..."
rvv_vfmacc_4xf32_4xf32(dst,lhs,rhs,vl)
{dst_data} = __riscv_vfmacc_vv_f32m1({dst_data}, {lhs_data}, {rhs_data},{vl});
*/

/* relying on the following instruction..."
rvv_vld_4xf32(dst,src,vl)
{dst_data} = __riscv_vle32_v_f32m1(&{src_data},{vl});
*/

/* relying on the following instruction..."
rvv_vst_4xf32(dst,src,vl)
__riscv_vse32_v_f32m1(&{dst_data}, {src_data},{vl});
*/
