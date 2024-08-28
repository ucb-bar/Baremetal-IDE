/**
 * @file hal_dma.h
 * @author -Ethan Gao / eygao@berkeley.edu
 * @brief 
 * @version 0.1
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef __HAL_DMA_H
#define __HAL_DMA_H

#ifdef __cplusplus
extern "C" {
#endif

#include "ll_dma.h"

typedef enum {
  MODE_COPY = 0x0,
  MODE_MAC  = 0x2
} DMA_Operation;

typedef enum {
  DMA_OK,
  DMA_BADMODE,
  DMA_SRCALIGN,
  DMA_DSTALIGN,
  DMA_STRALIGN,
  DMA_CNTERR,
  DMA_DENYR,
  DMA_CORRUPTR,
  DMA_DENYW
} DMA_Status;

/* Returns whether the given DMA engine's last operation was completed successfully */
static inline uint8_t HAL_DMA_operation_complete(DMA_TypeDef* DMAX) {
  return READ_BITS(DMAX->STATUS, DMA_COMPL_MSK) != 0;
}

/* Returns whether the given DMA engine has an operation in progress */
static inline uint8_t HAL_DMA_operation_inprogress(DMA_TypeDef* DMAX) {
  return READ_BITS(DMAX->STATUS, DMA_INPROG_MSK) != 0;
}

/* Returns whether the given DMA engine last operation errored */
static inline uint8_t HAL_DMA_operation_errored(DMA_TypeDef* DMAX) {
  return READ_BITS(DMAX->STATUS, DMA_INPROG_MSK) != 0;
}

/* Initializes a block copy from SRC to DST on DMA engine
   DMAX to transfer COUNT cache blocks and with stride
   SRC_STRIDE between copies. SRC and DST must be cache
   block aligned and SRC_STRIDE must be a multiple of a
   cache block size. Waits for any previous operations to
   complete before initiating current operation. Non blocking
   and returns immediately after initiating operation. */
void HAL_DMA_init_memcpy(DMA_TypeDef* DMAX, void* src, void* dst, uint64_t src_stride, uint32_t count);

/* Initializes a saturating matrix-vector product of int8s
   where SRC is a pointer to a matrix of int8s, OPERAND is
   a pointer to a vector of int8s to multiply with, STRIDE
   is the stride of the input matrix, and COUNT is the numnber
   of matrix-vector multiplies to perform. SRC must be cache
   block aligned, SRC_STRIDE must be a multiple of a cache
   block size, and COUNT must be <= 32 */
void HAL_DMA_init_MAC(DMA_TypeDef* DMAX, void* src, int8_t* operand, uint64_t src_stride, uint32_t count);

/* Synchronously waits for DMA operation to complete and returns the DMA
   Status Code. Refer to BearlyML23 Doc for interpretation of error codes*/
DMA_Status HAL_DMA_await_result(DMA_TypeDef* DMAX);

/* Synchronously waits for MAC operation to complete, copies result to
   *dst, and returns the DMA Status Code. Refer to BearlyML23 Doc
   for interpretation of error codes*/
DMA_Status HAL_DMA_get_MAC_result(DMA_TypeDef* DMAX, int16_t* dst, uint32_t count);

#ifdef __cplusplus
}
#endif

#endif /* __LL_DMA_H */