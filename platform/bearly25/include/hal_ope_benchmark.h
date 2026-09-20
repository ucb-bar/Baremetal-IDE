#ifndef HAL_OPE_H
#define HAL_OPE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifndef OPE_CUSTOM
#define OPE_CUSTOM 0
#endif

// When set to 1, the hardware writes out tiles in transposed form
#ifndef OPE_EXT_FLIP
#define OPE_EXT_FLIP 1
#endif

// If set to 1, the driver will issue per-tile fences in OP_EXT_STRIDE
#ifndef OPE_TILE_FENCE
#define OPE_TILE_FENCE 0
#endif

#ifndef MIN
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#endif

typedef struct {
  int rows;
  int cols;
  int rowsU;
  int colsU;
  int8_t data[];
} ope_mat8_t;

typedef struct {
  int rows;
  int cols;
  int rowsU;
  int colsU;
  int32_t data[];
} ope_mat32_t;

typedef enum {
  OPE_MAT_NONE = 0,
  OPE_MAT_ZERO
} ope_mat_init_t;

// Pre-allocate workspace for remap buffers
void ope_init_workspace(int max_M, int max_N, int max_K);

// Free workspace if previously allocated
void ope_free_workspace(void);

// Utility Functions
ope_mat8_t* ope_mat8_init (int rows, int cols, ope_mat_init_t init_method);
ope_mat32_t* ope_mat32_init(int rows, int cols, ope_mat_init_t init_method);
void ope_mat8_free (ope_mat8_t*  mat);
void ope_mat32_free(ope_mat32_t* mat);
void ope_mat32_transpose_inplace(ope_mat32_t* mat);

// Packed/remapped buffer sizing helpers (returned size is 8-byte aligned)
size_t ope_packed_a_size_bytes(int rows, int cols);
size_t ope_packed_b_size_bytes(int rows, int cols);

// Remap row-major matrices into OPE packed layout
void ope_remap_matrix_A(const ope_mat8_t* restrict A, int8_t* restrict A_T);
void ope_remap_matrix_B(const ope_mat8_t* restrict B, int8_t* restrict B_remap);

// Compute from pre-remapped buffers
long ope_matmul_square_packed(int8_t* A_T, int8_t* B_remap, int kU, ope_mat32_t* out);
long ope_matmul_arb_packed(int8_t* A_T, int8_t* B_remap, int mU, int nU, int kU, ope_mat32_t* out);

long ope_matmul_8x8 (ope_mat8_t* A, ope_mat8_t* B, ope_mat32_t* out);
long ope_matmul_16x16(int8_t* A_T, int8_t* B_remap, ope_mat32_t* out);
long ope_matmul_32x32(int8_t* A_T, int8_t* B_remap, ope_mat32_t* out);
long ope_matmul_64x64(int8_t* A_T, int8_t* B_remap, ope_mat32_t* out);

// Backward-compatible wrappers (pack + compute)
long ope_matmul_square(ope_mat8_t* A, ope_mat8_t* B, ope_mat32_t* out);
long ope_matmul_arb (ope_mat8_t* A, ope_mat8_t* B, ope_mat32_t* out);

#ifdef __cplusplus
}
#endif

#endif // HAL_OPE_H
