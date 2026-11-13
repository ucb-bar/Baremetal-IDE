/**
 * @file hal_dma.h
 * @author Jasmine Angle / angle@berkeley.edu
 * @brief 
 * @version 0.1
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef __HAL_DMA_H
#define __HAL_DMA_H

#ifdef __cplusplus
extern "C" {
#endif

// #include "rv_common.h"
#include "metal.h"
#include "ll_dma.h"

#define DMA0                    ((DMA_Type*) 0x8800000)
#define DMA1                    ((DMA_Type*) 0x8802000)
#define DMA2                    ((DMA_Type*) 0x8804000)
#define DMA3                    ((DMA_Type*) 0x8808000)

typedef enum {
  MODE_COPY = 0x0,
  MODE_MAC  = 0x2
} DMA_Operation;

typedef enum {
  DMA_OK,
  DMA_BADMODE,
  DMA_CNTERR,
  DMA_DENYR,
  DMA_CORRUPTR,
  DMA_DENYW
} DMA_Status;

/* Returns the DMA_Status value corresponding with the given DMA engine's status register. */
DMA_Status get_status(DMA_Type* DMAX);

/* Returns whether the given DMA engine's last operation was completed successfully */
static inline uint8_t dma_operation_complete(DMA_Type* DMAX) {
  return READ_BITS(DMAX->STATUS, DMA_COMPL_MSK) != 0;
}

/* Returns whether the given DMA engine has an operation in progress */
static inline uint8_t dma_operation_inprogress(DMA_Type* DMAX) {
  return READ_BITS(DMAX->STATUS, DMA_INPROG_MSK) != 0;
}

/* Returns whether the given DMA engine has an operation in progress */
static inline uint8_t dma_operation_inprogress_and_not_error(DMA_Type* DMAX) {
  return READ_BITS(DMAX->STATUS, DMA_INPROG_MSK) != 0 && READ_BITS(DMAX->STATUS, DMA_ERR_MSK) == 0;
}

/* Returns whether the given DMA engine last operation errored */
static inline uint8_t dma_operation_errored(DMA_Type* DMAX) {
  return READ_BITS(DMAX->STATUS, DMA_ERR_MSK) != 0;
}

/* Initializes a block copy from SRC to DST on DMA engine
   DMAX to transfer COUNT cache blocks and with stride
   SRC_STRIDE between copies. SRC and DST must be cache
   block aligned and SRC_STRIDE must be a multiple of a
   cache block size. Waits for any previous operations to
   complete before initiating current operation. Non blocking
   and returns immediately after initiating operation. */
void dma_init_memcpy(DMA_Type* DMAX, void* src, void* dst, uint64_t src_stride, uint32_t count);

/* Initializes a saturating matrix-vector product of int8s
   where SRC is a pointer to a matrix of int8s, OPERAND is
   a pointer to a vector of int8s to multiply with, STRIDE
   is the stride of the input matrix, and COUNT is the numnber
   of matrix-vector multiplies to perform. SRC must be cache
   block aligned, SRC_STRIDE must be a multiple of a cache
   block size, and COUNT must be <= 32 */
void dma_init_MAC(DMA_Type* DMAX, void* src, int8_t* operand, uint64_t src_stride, uint32_t count);

/* Synchronously waits for DMA operation to complete and returns the DMA
   Status Code. Refer to BearlyML23 Doc for interpretation of error codes*/
DMA_Status dma_await_result(DMA_Type* DMAX);

/* Synchronously waits for MAC operation to complete, copies result to
   *dst, and returns the DMA Status Code. Refer to BearlyML23 Doc
   for interpretation of error codes*/
DMA_Status dma_get_MAC_result(DMA_Type* DMAX, int16_t* dst, uint32_t count);

#ifdef __cplusplus
}
#endif

#endif /* __HAL_DMA_H */
