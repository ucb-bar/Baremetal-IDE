#include "hal_ope_benchmark.h"
#include "rocc.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>

// ===== RoCC Instruction Configuration =====

// Functional tags
#define FCTN7_ACC 0b00
#define FCTN7_EXTRACT 0b01
#define FCTN7_ZERO 0b10
#define FCTN7_LOAD 0b11

// Rocket source register numbers for RoCC interface
#define ROCC_RS1_REG_N 11
#define ROCC_RS2_REG_N 12

#define REG_STR_HELPER(x) #x
#define REG_STR(x) REG_STR_HELPER(x)
#define REG_VAR(name, reg_num) register uint64_t name asm("x" REG_STR(reg_num))

// ===== Utility Functions =====

static inline unsigned long read_cycles(void) {
  unsigned long cycles;
  asm volatile("rdcycle %0" : "=r"(cycles));
  return cycles;
}

static inline int round_up_8(int x) {
  return ((x + 7) / 8) * 8;
}

static inline size_t align_up_8(size_t bytes) {
  return ((bytes + 7) / 8) * 8;
}

// ===== Low-Level RoCC Operations =====

#define OP_ZERO() ROCC_INSTRUCTION(OPE_CUSTOM, FCTN7_ZERO)

static inline void OP_ACC_L(int8_t* U, int8_t* V, int L) {
  REG_VAR(rs1, ROCC_RS1_REG_N) = (uint64_t) U;
  REG_VAR(rs2, ROCC_RS2_REG_N) = (uint64_t) V;

  switch(L) {
    case 1: { ROCC_INSTRUCTION_SS(OPE_CUSTOM, rs1, rs2, FCTN7_ACC | (0<<2)); break; }
    case 2: { ROCC_INSTRUCTION_SS(OPE_CUSTOM, rs1, rs2, FCTN7_ACC | (1<<2)); break; }
    case 3: { ROCC_INSTRUCTION_SS(OPE_CUSTOM, rs1, rs2, FCTN7_ACC | (2<<2)); break; }
    case 4: { ROCC_INSTRUCTION_SS(OPE_CUSTOM, rs1, rs2, FCTN7_ACC | (3<<2)); break; }
    case 5: { ROCC_INSTRUCTION_SS(OPE_CUSTOM, rs1, rs2, FCTN7_ACC | (4<<2)); break; }
    case 6: { ROCC_INSTRUCTION_SS(OPE_CUSTOM, rs1, rs2, FCTN7_ACC | (5<<2)); break; }
    case 7: { ROCC_INSTRUCTION_SS(OPE_CUSTOM, rs1, rs2, FCTN7_ACC | (6<<2)); break; }
    case 8: { ROCC_INSTRUCTION_SS(OPE_CUSTOM, rs1, rs2, FCTN7_ACC | (7<<2)); break; }
    case 9: { ROCC_INSTRUCTION_SS(OPE_CUSTOM, rs1, rs2, FCTN7_ACC | (8<<2)); break; }
    case 10: { ROCC_INSTRUCTION_SS(OPE_CUSTOM, rs1, rs2, FCTN7_ACC | (9<<2)); break; }
    case 11: { ROCC_INSTRUCTION_SS(OPE_CUSTOM, rs1, rs2, FCTN7_ACC | (10<<2)); break; }
    case 12: { ROCC_INSTRUCTION_SS(OPE_CUSTOM, rs1, rs2, FCTN7_ACC | (11<<2)); break; }
    case 13: { ROCC_INSTRUCTION_SS(OPE_CUSTOM, rs1, rs2, FCTN7_ACC | (12<<2)); break; }
    case 14: { ROCC_INSTRUCTION_SS(OPE_CUSTOM, rs1, rs2, FCTN7_ACC | (13<<2)); break; }
    case 15: { ROCC_INSTRUCTION_SS(OPE_CUSTOM, rs1, rs2, FCTN7_ACC | (14<<2)); break; }
    case 16: { ROCC_INSTRUCTION_SS(OPE_CUSTOM, rs1, rs2, FCTN7_ACC | (15<<2)); break; }
    case 17: { ROCC_INSTRUCTION_SS(OPE_CUSTOM, rs1, rs2, FCTN7_ACC | (16<<2)); break; }
    case 18: { ROCC_INSTRUCTION_SS(OPE_CUSTOM, rs1, rs2, FCTN7_ACC | (17<<2)); break; }
    case 19: { ROCC_INSTRUCTION_SS(OPE_CUSTOM, rs1, rs2, FCTN7_ACC | (18<<2)); break; }
    case 20: { ROCC_INSTRUCTION_SS(OPE_CUSTOM, rs1, rs2, FCTN7_ACC | (19<<2)); break; }
    case 21: { ROCC_INSTRUCTION_SS(OPE_CUSTOM, rs1, rs2, FCTN7_ACC | (20<<2)); break; }
    case 22: { ROCC_INSTRUCTION_SS(OPE_CUSTOM, rs1, rs2, FCTN7_ACC | (21<<2)); break; }
    case 23: { ROCC_INSTRUCTION_SS(OPE_CUSTOM, rs1, rs2, FCTN7_ACC | (22<<2)); break; }
    case 24: { ROCC_INSTRUCTION_SS(OPE_CUSTOM, rs1, rs2, FCTN7_ACC | (23<<2)); break; }
    case 25: { ROCC_INSTRUCTION_SS(OPE_CUSTOM, rs1, rs2, FCTN7_ACC | (24<<2)); break; }
    case 26: { ROCC_INSTRUCTION_SS(OPE_CUSTOM, rs1, rs2, FCTN7_ACC | (25<<2)); break; }
    case 27: { ROCC_INSTRUCTION_SS(OPE_CUSTOM, rs1, rs2, FCTN7_ACC | (26<<2)); break; }
    case 28: { ROCC_INSTRUCTION_SS(OPE_CUSTOM, rs1, rs2, FCTN7_ACC | (27<<2)); break; }
    case 29: { ROCC_INSTRUCTION_SS(OPE_CUSTOM, rs1, rs2, FCTN7_ACC | (28<<2)); break; }
    case 30: { ROCC_INSTRUCTION_SS(OPE_CUSTOM, rs1, rs2, FCTN7_ACC | (29<<2)); break; }
    case 31: { ROCC_INSTRUCTION_SS(OPE_CUSTOM, rs1, rs2, FCTN7_ACC | (30<<2)); break; }
    case 32: { ROCC_INSTRUCTION_SS(OPE_CUSTOM, rs1, rs2, FCTN7_ACC | (31<<2)); break; }
  }
}

#define _OP_EXT_S_T(rs1, rs2) \
  ROCC_INSTRUCTION_SS(OPE_CUSTOM, rs1,rs2, FCTN7_EXTRACT | (1 << 2) | (1 << 3))
#define _OP_EXT_NS_T(rs2) \
  ROCC_INSTRUCTION_SS(OPE_CUSTOM, 0,rs2, FCTN7_EXTRACT | (1 << 2) | (0 << 3))
#define _OP_EXT_S_NT(rs1, rs2) \
  ROCC_INSTRUCTION_SS(OPE_CUSTOM, rs1,rs2, FCTN7_EXTRACT | (0 << 2) | (1 << 3))
#define _OP_EXT_NS_NT(rs2) \
  ROCC_INSTRUCTION_SS(OPE_CUSTOM, 0,rs2, FCTN7_EXTRACT | (0 << 2) | (0 << 3))

static inline void OP_EXT_STRIDE(int32_t* arr, int stride_elements, bool transposed) {
  REG_VAR(rs2, ROCC_RS2_REG_N) = (uint64_t) arr;

  if (stride_elements == 0 || stride_elements == 8){
    if (transposed) { _OP_EXT_NS_T(rs2);  }
    else { _OP_EXT_NS_NT(rs2); }
    asm volatile("fence w, r" ::: "memory");
  } else {
    REG_VAR(rs1, ROCC_RS1_REG_N) = (uint64_t) stride_elements;
    if (transposed) { _OP_EXT_S_T(rs1, rs2);  }
    else { _OP_EXT_S_NT(rs1, rs2); }
    asm volatile("fence w, r" ::: "memory");
  }
}

// ===== Matrix Utility Functions =====

ope_mat8_t* ope_mat8_init(int rows, int cols, ope_mat_init_t init_method) {
  // Round up to nearest multiple of eight
  int rowsU = ((rows + 7) / 8) * 8;
  int colsU = ((cols + 7) / 8) * 8;

  size_t total_size = sizeof(ope_mat8_t) + rowsU * colsU * sizeof(int8_t);
  // Ensure size is a multiple of alignment (8)
  total_size = ((total_size + 7) / 8) * 8;
  ope_mat8_t* mat = (ope_mat8_t*)aligned_alloc(8, total_size);
  if (!mat) {
    return NULL;
  }

  mat->rows = rows;
  mat->cols = cols;
  mat->rowsU = rowsU;
  mat->colsU = colsU;

  // Zero-initialize if requested or if padding is needed
  if (init_method == OPE_MAT_ZERO || (rowsU != rows || colsU != cols)) {
    memset(mat->data, 0, sizeof(int8_t) * rowsU * colsU);
  }
  return mat;
}

ope_mat32_t* ope_mat32_init(int rows, int cols, ope_mat_init_t init_method) {
  // Round up to nearest multiple of eight
  int rowsU = ((rows + 7) / 8) * 8;
  int colsU = ((cols + 7) / 8) * 8;

  size_t total_size = sizeof(ope_mat32_t) + rowsU * colsU * sizeof(int32_t);
  // Ensure size is a multiple of alignment (8)
  total_size = ((total_size + 7) / 8) * 8;
  ope_mat32_t* mat = (ope_mat32_t*)aligned_alloc(8, total_size);
  if (!mat) {
    return NULL;
  }

  mat->rows = rows;
  mat->cols = cols;
  mat->rowsU = rowsU;
  mat->colsU = colsU;

  // Zero-initialize if requested or if padding is needed
  if (init_method == OPE_MAT_ZERO || (rowsU != rows || colsU != cols)) {
    memset(mat->data, 0, sizeof(int32_t) * rowsU * colsU);
  }
  return mat;
}

void ope_mat8_free(ope_mat8_t* mat) {
  free(mat);
}

void ope_mat32_free(ope_mat32_t* mat) {
  free(mat);
}

void ope_mat32_transpose_inplace(ope_mat32_t* mat) {
  assert(mat->rows == mat->cols);
  for (int i = 0; i < mat->rows; i++) {
    for (int j = i + 1; j < mat->cols; j++) {
      int32_t tmp = mat->data[i * mat->colsU + j];
      mat->data[i * mat->colsU + j] = mat->data[j * mat->colsU + i];
      mat->data[j * mat->colsU + i] = tmp;
    }
  }
}

size_t ope_packed_a_size_bytes(int rows, int cols) {
  assert(rows >= 0 && cols >= 0);
  int rowsU = round_up_8(rows);
  int colsU = round_up_8(cols);
  return align_up_8((size_t)rowsU * (size_t)colsU * sizeof(int8_t));
}

size_t ope_packed_b_size_bytes(int rows, int cols) {
  assert(rows >= 0 && cols >= 0);
  int rowsU = round_up_8(rows);
  int colsU = round_up_8(cols);
  return align_up_8((size_t)rowsU * (size_t)colsU * sizeof(int8_t));
}

// ===== Pre-allocated Workspace for Remap Buffers =====
static int8_t *g_workspace_A = NULL;
static int8_t *g_workspace_B = NULL;
static size_t g_workspace_size = 0;

static int acquire_pack_buffers(size_t size_A, size_t size_B,
                                int8_t** A_T, int8_t** B_remap, bool* use_workspace) {
  size_t max_size = (size_A > size_B) ? size_A : size_B;
  *use_workspace = (g_workspace_A && g_workspace_B && g_workspace_size >= max_size);

  if (*use_workspace) {
    *A_T = g_workspace_A;
    *B_remap = g_workspace_B;
    return 0;
  }

  *A_T = aligned_alloc(8, size_A);
  if (!*A_T) {
    return -1;
  }

  *B_remap = aligned_alloc(8, size_B);
  if (!*B_remap) {
    free(*A_T);
    *A_T = NULL;
    return -1;
  }

  return 0;
}

static void release_pack_buffers(int8_t* A_T, int8_t* B_remap, bool use_workspace) {
  if (!use_workspace) {
    free(A_T);
    free(B_remap);
  }
}

void ope_init_workspace(int max_M, int max_N, int max_K) {
  // Round up to multiples of 8
  int mU = round_up_8(max_M);
  int nU = round_up_8(max_N);
  int kU = round_up_8(max_K);
  
  // Size for A remap: mU * kU, Size for B remap: kU * nU
  size_t size_A = (size_t)mU * (size_t)kU;
  size_t size_B = (size_t)kU * (size_t)nU;
  size_t max_size = (size_A > size_B) ? size_A : size_B;
  
  // Ensure multiple of 8 for aligned_alloc
  max_size = align_up_8(max_size);
  
  if (g_workspace_A) free(g_workspace_A);
  if (g_workspace_B) free(g_workspace_B);
  
  g_workspace_A = aligned_alloc(8, max_size);
  g_workspace_B = aligned_alloc(8, max_size);
  g_workspace_size = max_size;
}

void ope_free_workspace(void) {
  if (g_workspace_A) { free(g_workspace_A); g_workspace_A = NULL; }
  if (g_workspace_B) { free(g_workspace_B); g_workspace_B = NULL; }
  g_workspace_size = 0;
}

// ===== Matrix Remapping Functions =====
void ope_remap_matrix_A(const ope_mat8_t* A, int8_t* A_T) {
  int rowsU = A->rowsU;
  int colsU = A->colsU;

  for (int chunk = 0; chunk < rowsU / 8; chunk++) {
    for (int r = 0; r < 8; r++) {
      int actualRow = chunk * 8 + r;
      for (int c = 0; c < colsU; c++) {
        // Column-major (transposed) with 8-element alignment
        A_T[(chunk * colsU * 8) + (c * 8) + r] = A->data[actualRow * A->colsU + c];
      }
    }
  }
}

void ope_remap_matrix_B(const ope_mat8_t* B, int8_t* B_remap) {
  int rowsU = B->rowsU;
  int colsU = B->colsU;

  for (int chunk = 0; chunk < colsU / 8; chunk++) {
    for (int r = 0; r < rowsU; r++) {
      for (int c = 0; c < 8; c++) {
        int actualCol = chunk * 8 + c;
        // Row-major with 8-element alignment
        B_remap[(chunk * rowsU * 8) + (r * 8) + c] = B->data[r * B->colsU + actualCol];
      }
    }
  }
}

// ===== OPE Matrix Multiplication Functions =====

long ope_matmul_8x8(ope_mat8_t* A, ope_mat8_t* B, ope_mat32_t* out) {
  assert(A->rowsU == 8);
  assert(A->colsU == 8);
  assert(B->rowsU == 8);
  assert(B->colsU == 8);
  assert(A->cols == B->rows);

  // Inline transpose of A (avoids remap overhead for single tile)
  int8_t A_T[8 * 8] __attribute__((aligned(8)));
  memset(A_T, 0, 8 * 8 * sizeof(int8_t));
  for (int i = 0; i < A->rows; i++) {
    for (int j = 0; j < A->cols; j++) {
      A_T[j * 8 + i] = A->data[i * A->colsU + j];
    }
  }

  unsigned long t0 = read_cycles();

  OP_ZERO();
  OP_ACC_L((int8_t*)&A_T, (int8_t*)&B->data, A->cols);
  OP_EXT_STRIDE((int32_t*)&out->data, 8, OPE_EXT_FLIP);

  unsigned long t1 = read_cycles();

  return t1 - t0;
}

long ope_matmul_16x16(int8_t* A_T, int8_t* B_remap, ope_mat32_t* out) {
  assert(out->rowsU == 16 && out->colsU == 16);
  unsigned long t0 = read_cycles();

#pragma GCC unroll 2
  for (int i = 0; i < 2; i++) {
#pragma GCC unroll 2
    for (int j = 0; j < 2; j++) {
      OP_ZERO();
      OP_ACC_L((int8_t*)(A_T + (i * 16 * 8)),
               (int8_t*)(B_remap + (j * 16 * 8)), 16);

      int32_t* addr = &out->data[(i * 8) * 16 + (j * 8)];
      OP_EXT_STRIDE(addr, 16, OPE_EXT_FLIP);
    }
  }
  asm volatile("fence w, r" ::: "memory");

  unsigned long t1 = read_cycles();
  return t1 - t0;
}

long ope_matmul_32x32(int8_t* A_T, int8_t* B_remap, ope_mat32_t* out) {
  assert(out->rowsU == 32 && out->colsU == 32);
  unsigned long t0 = read_cycles();

#pragma GCC unroll 4
  for (int i = 0; i < 4; i++) {
#pragma GCC unroll 4
    for (int j = 0; j < 4; j++) {
      OP_ZERO();
      OP_ACC_L(A_T + (i * 32 * 8), B_remap + (j * 32 * 8), 32);

      int32_t* addr = &out->data[(i * 8) * 32 + (j * 8)];
      OP_EXT_STRIDE(addr, 32, OPE_EXT_FLIP);
    }
  }
  asm volatile("fence w, r" ::: "memory");

  unsigned long t1 = read_cycles();
  return t1 - t0;
}

long ope_matmul_64x64(int8_t* A_T, int8_t* B_remap, ope_mat32_t* out) {
  assert(out->rowsU == 64 && out->colsU == 64);

  unsigned long t0 = read_cycles();

#pragma GCC unroll 8
  for (int i = 0; i < 8; i++) {
#pragma GCC unroll 8
    for (int j = 0; j < 8; j++) {
      OP_ZERO();
      OP_ACC_L(A_T + (i * 64 * 8), B_remap + (j * 64 * 8), 32);
      OP_ACC_L(A_T + (i * 64 * 8) + (32 * 8),
               B_remap + (j * 64 * 8) + (32 * 8), 32);

      int32_t* addr = &out->data[(i * 8) * 64 + (j * 8)];
      OP_EXT_STRIDE(addr, 64, OPE_EXT_FLIP);
    }
  }
  asm volatile("fence w, r" ::: "memory");

  unsigned long t1 = read_cycles();
  return t1 - t0;
}

static long ope_matmul_8x8_packed(int8_t* A_T, int8_t* B_remap, ope_mat32_t* out) {
  assert(out->rowsU == 8 && out->colsU == 8);
  unsigned long t0 = read_cycles();
  OP_ZERO();
  OP_ACC_L(A_T, B_remap, 8);
  OP_EXT_STRIDE((int32_t*)&out->data, 8, OPE_EXT_FLIP);
  unsigned long t1 = read_cycles();
  return t1 - t0;
}

long ope_matmul_square_packed(int8_t* A_T, int8_t* B_remap, int kU, ope_mat32_t* out) {
  assert(A_T && B_remap && out);
  assert(kU > 0 && (kU % 8) == 0);
  assert(out->rowsU == out->colsU);
  assert(out->rowsU == kU);

  switch (kU) {
    case 8:
      return ope_matmul_8x8_packed(A_T, B_remap, out);
    case 16:
      return ope_matmul_16x16(A_T, B_remap, out);
    case 32:
      return ope_matmul_32x32(A_T, B_remap, out);
    case 64:
      return ope_matmul_64x64(A_T, B_remap, out);
    default: {
      // Generic tiled implementation for sizes > 64
      // Process in 8x8 output tiles, accumulating K dimension in chunks of 32
      register int stride = kU;
      unsigned long t0 = read_cycles();

      for (int i = 0; i < kU / 8; i++) {
        for (int j = 0; j < kU / 8; j++) {
          OP_ZERO();

          // Accumulate along K dimension in chunks of up to 32
          for (int k_ofs = 0, k_rem = kU; k_rem > 0;) {
            int L = MIN(32, k_rem);
            OP_ACC_L(A_T + (i * kU * 8) + k_ofs, B_remap + (j * kU * 8) + k_ofs, L);
            k_rem -= L;
            k_ofs += L * 8;
          }

          int32_t* addr = &out->data[(i * 8) * out->colsU + (j * 8)];
          OP_EXT_STRIDE(addr, stride, OPE_EXT_FLIP);
        }
      }

      asm volatile("fence w, rw" ::: "memory");
      unsigned long t1 = read_cycles();
      return t1 - t0;
    }
  }
}

long ope_matmul_arb_packed(int8_t* A_T, int8_t* B_remap, int mU, int nU, int kU, ope_mat32_t* out) {
  assert(A_T && B_remap && out);
  assert(mU > 0 && nU > 0 && kU > 0);
  assert((mU % 8) == 0 && (nU % 8) == 0 && (kU % 8) == 0);
  assert(out->rowsU == mU);
  assert(out->colsU == nU);

  if (mU == kU && kU == nU) {
    return ope_matmul_square_packed(A_T, B_remap, kU, out);
  }

  register int stride = nU;
  unsigned long t0 = read_cycles();

  for (int i = 0; i < mU / 8; i++) {
    for (int j = 0; j < nU / 8; j++) {
      OP_ZERO();

      if (kU <= 32) {
        OP_ACC_L(A_T + (i * kU * 8), B_remap + (j * kU * 8), kU);
      } else if (kU <= 64) {
        OP_ACC_L(A_T + (i * kU * 8), B_remap + (j * kU * 8), 32);
        OP_ACC_L(A_T + (i * kU * 8) + 32 * 8, B_remap + (j * kU * 8) + 32 * 8, kU - 32);
      } else {
        for (int k_ofs = 0, k_rem = kU; k_rem > 0;) {
          int L = MIN(32, k_rem);
          OP_ACC_L(A_T + (i * kU * 8) + k_ofs, B_remap + (j * kU * 8) + k_ofs, L);
          k_rem -= L;
          k_ofs += L * 8;
        }
      }

      int32_t* addr = &out->data[(i * 8) * out->colsU + (j * 8)];
      OP_EXT_STRIDE(addr, stride, OPE_EXT_FLIP);
    }
  }

  asm volatile("fence w, rw" ::: "memory");
  unsigned long t1 = read_cycles();
  return t1 - t0;
}

long ope_matmul_square(ope_mat8_t* A, ope_mat8_t* B, ope_mat32_t* out) {
  assert(A->rows == A->cols);
  assert(B->rows == B->cols);
  assert(out->rows == out->cols);
  assert(A->cols == B->rows);

  int kU = A->colsU;
  if (kU == 8) return ope_matmul_8x8(A, B, out);

  size_t size_A = ope_packed_a_size_bytes(A->rows, A->cols);
  size_t size_B = ope_packed_b_size_bytes(B->rows, B->cols);

  int8_t* A_T = NULL;
  int8_t* B_remap = NULL;
  bool use_workspace = false;
  if (acquire_pack_buffers(size_A, size_B, &A_T, &B_remap, &use_workspace) != 0) {
    return -1;
  }

  ope_remap_matrix_A(A, A_T);
  ope_remap_matrix_B(B, B_remap);
  unsigned long cycles = ope_matmul_square_packed(A_T, B_remap, kU, out);

  release_pack_buffers(A_T, B_remap, use_workspace);
  return cycles;
}

long ope_matmul_arb(ope_mat8_t* A, ope_mat8_t* B, ope_mat32_t* out) {
  assert(A->rows == out->rows);
  assert(B->cols == out->cols);
  assert(A->cols == B->rows);

  int mU = A->rowsU;
  int kU = A->colsU;
  int nU = B->colsU;

  // Square case keeps the existing fast path behavior.
  if (mU == kU && kU == nU) {
    if (mU == 8) return ope_matmul_8x8(A, B, out);
    return ope_matmul_square(A, B, out);
  }

  size_t size_A = ope_packed_a_size_bytes(A->rows, A->cols);
  size_t size_B = ope_packed_b_size_bytes(B->rows, B->cols);

  int8_t* A_T = NULL;
  int8_t* B_remap = NULL;
  bool use_workspace = false;
  if (acquire_pack_buffers(size_A, size_B, &A_T, &B_remap, &use_workspace) != 0) {
    return -1;
  }

  ope_remap_matrix_A(A, A_T);
  ope_remap_matrix_B(B, B_remap);
  unsigned long cycles = ope_matmul_arb_packed(A_T, B_remap, mU, nU, kU, out);

  release_pack_buffers(A_T, B_remap, use_workspace);
  return cycles;
}
