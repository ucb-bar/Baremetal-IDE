/*
This file contains the c wrapper for the operations the filter can do in hardware.
More detail on the functions is available on the wiki here:
https://bwrcrepo.eecs.berkeley.edu/ee290c_ee194_intech22/robochip-kalman-filter/-/wikis/home
*/

#include <inttypes.h>
#include <stdio.h>
#include "rocc.h"

#define LOAD_FUNCT7 0
#define STORE_FUNCT7 1
#define DOT_PROD_FUNCT7 2
#define ADD_FUNCT7 3
#define SUB_FUNCT7 4
#define MUL_FUNCT7 5
#define DIV_FUNCT7 6
#define SQRT_FUNCT7 7
#define FILL_FUNCT7 8
#define SET_OUT_FUNCT7 9
#define SET_SIZE_FUNCT7 10

#define NUM_REGISTERS 4

// addr := rs2
// v0 := rs1
// v1 := rs2

/** vec[v0] = M[addr] */
#define K_LOAD(rs1, rs2) \
  ROCC_INSTRUCTION_SS(1, rs1, rs2, LOAD_FUNCT7)

/** M[addr] = vec[v0] */
#define K_STORE(rs1, rs2) \
  ROCC_INSTRUCTION_SS(1, rs1, rs2, STORE_FUNCT7)

/** R[rd] = vec[v0] dot vec[v1] */
#define K_DOT_PROD(rd, rs1, rs2) \
  ROCC_INSTRUCTION_DSS(1, rd, rs1, rs2, DOT_PROD_FUNCT7)

/** vec[vout] = vec[v0] + vec[v1] */
#define K_ADD(rs1, rs2) \
  ROCC_INSTRUCTION_SS(1, rs1, rs2, ADD_FUNCT7)

/** vec[vout] = vec[v0] - vec[v1] */
#define K_SUB(rs1, rs2) \
  ROCC_INSTRUCTION_SS(1, rs1, rs2, SUB_FUNCT7)

/** vec[vout] = vec[v0] * vec[v1] (elementwise) */
#define K_MUL(rs1, rs2) \
  ROCC_INSTRUCTION_SS(1, rs1, rs2, MUL_FUNCT7)

/** vec[vout] = vec[v0] / vec[v1] (elementwise) */
#define K_DIV(rs1, rs2) \
  ROCC_INSTRUCTION_SS(1, rs1, rs2, DIV_FUNCT7)

/** vec[vout] = sqrt(vec[v0]) */
#define K_SQRT(rs1) \
  ROCC_INSTRUCTION_S(1, rs1, SQRT_FUNCT7)

/** vec[vout] = rs1 (repeated) */
#define K_FILL(rs1) \
  ROCC_INSTRUCTION_S(1, rs1, FILL_FUNCT7)

/** vout = v0 */
#define K_SET_OUT(rs1) \
  ROCC_INSTRUCTION_S(1, rs1, SET_OUT_FUNCT7)

/** size = rs1 */
#define K_SET_SIZE(rs1) \
  ROCC_INSTRUCTION_S(1, rs1, SET_SIZE_FUNCT7)