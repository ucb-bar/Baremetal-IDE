// This file contains the C wrapper for the operations the accelerator can do in hardware.
#include <inttypes.h>
#include <stdio.h>
#include "rocc.h"

#define LOAD_FUNCT7 0
#define STORE_FUNCT7 1
#define QUANTIZE_FUNCT7 2
#define SET_SCALE_FACTOR_FUNCT7 3
#define DOT_PROD_FUNCT7 4
#define RELU_FUNCT7 5
#define SOFTMAX_FUNCT7 6
#define LAYERNORM_FUNCT7 7

// M = CPU memory
// R = CPU RegFile
// vec = Vector RegFile
// Q = Quantization Register

// vec[rs1] = M[rs2]
#define V_LOAD(rs1, rs2) \
  ROCC_INSTRUCTION_SS(1, rs1, rs2, LOAD_FUNCT7)

// M[rs2] = vec[rs1]
#define V_STORE(rs1, rs2) \
  ROCC_INSTRUCTION_SS(1, rs1, rs2, STORE_FUNCT7)

#define QUANTIZE(rd, rs1, rs2) \
  ROCC_INSTRUCTION_DSS(1, rd, rs1, rs2, QUANTIZE_FUNCT7)

// Q[0] = rs1, Q[1] = rs2
#define SET_SCALE_FACTOR(rs1, rs2) \
  ROCC_INSTRUCTION_SS(1, rs1, rs2, SET_SCALE_FACTOR_FUNCT7)

// R[rd] = vec[rs1] dot vec[rs2]
#define V_DOT_PROD(rd, rs1, rs2) \
  ROCC_INSTRUCTION_DSS(1, rd, rs1, rs2, DOT_PROD_FUNCT7)

// vec[vout] = ReLU(vec[rs1]) (elementwise)
#define V_RELU(rs1) \
  ROCC_INSTRUCTION_S(1, rs1, RELU_FUNCT7)

// vec[vout] = SoftMax(vec[rs1]) (elementwise)
#define V_SOFTMAX(rs1) \
  ROCC_INSTRUCTION_S(1, rs1, SOFTMAX_FUNCT7)

  // vec[vout] = LayerNorm(vec[rs1]) (elementwise)
#define V_LAYERNORM(rs1) \
  ROCC_INSTRUCTION_S(1, rs1, LAYERNORM_FUNCT7)
