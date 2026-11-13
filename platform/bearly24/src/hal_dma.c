#include "hal_dma.h"

DMA_Status get_status(DMA_Type* DMAX) {
  if (READ_BITS(DMAX->STATUS, DMA_BADMODE_MSK))
    return DMA_BADMODE;
  else if (READ_BITS(DMAX->STATUS, DMA_CNTERR_MSK))
    return DMA_CNTERR;
  else if (READ_BITS(DMAX->STATUS, DMA_DENYR_MSK))
    return DMA_DENYR;
  else if (READ_BITS(DMAX->STATUS, DMA_CORRUPTR_MSK))
    return DMA_CORRUPTR;
  else if (READ_BITS(DMAX->STATUS, DMA_DENYW_MSK))
    return DMA_DENYW;
  return DMA_OK;
}

void dma_init_memcpy(DMA_Type* DMAX, void* src, void* dst, uint64_t src_stride, uint32_t count) {
  while (dma_operation_inprogress_and_not_error(DMAX));
  
  DMAX->SRC_ADDR = (uint64_t) src;
  DMAX->DEST_ADDR = (uint64_t) dst;
  DMAX->SRCSTRIDE = src_stride;
  DMAX->MODE = MODE_COPY;
  DMAX->COUNT = count;
}

void dma_init_MAC(DMA_Type* DMAX, void* src, int8_t* operand, uint64_t src_stride, uint32_t count) {
  while (dma_operation_inprogress_and_not_error(DMAX));

  uint64_t* op = (uint64_t*) operand;
  for (size_t i = 0; i < 8; i++)
    DMAX->OPERAND_REG[i] = op[i];
  DMAX->SRC_ADDR = (uint64_t) src;
  DMAX->SRCSTRIDE = src_stride;
  DMAX->MODE = MODE_MAC;
  DMAX->COUNT = count;

}

DMA_Status dma_await_result(DMA_Type* DMAX) {
  while (dma_operation_inprogress_and_not_error(DMAX));
  if (dma_operation_complete(DMAX))
    return DMA_OK;
  else
    return get_status(DMAX);
}

DMA_Status dma_get_MAC_result(DMA_Type* DMAX, int16_t* dst, uint32_t count) {
  while (dma_operation_inprogress_and_not_error(DMAX));
  if (count > 32)
    count = 32;
  
  if (dma_operation_complete(DMAX)){
    for (size_t i = 0; i < count; i++)
      dst[i] = DMAX->DEST_REG[i];
    return DMA_OK;
  }
  else {
    for (size_t i = 0; i < count; i++)
        dst[i] = -1;
    return get_status(DMAX);
  }
}
