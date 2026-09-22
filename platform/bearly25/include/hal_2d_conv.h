/**
 * \file    hal_2d_conv.h
 * \author  Nathan Carter | nathancarter@berkeley.edu
 * \brief   MMIO generic register operation driver for 2D Conv Engine.
 * \version 0.1
 * 
 * \copyright Copyright (c) 2025
 * 
 */

#ifndef __HAL_2D_CONV_H__
#define __HAL_2D_CONV_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

// Baremetal IDE Definitions //
#include "metal.h"

// Register Offset Definitions
#define CONV2D_STATUS_REG_OFFSET      0x00
#define CONV2D_READY_REG_OFFSET       0x08
#define CONV2D_SRC_ADDR_OFFSET        0x10
#define CONV2D_DEST_ADDR_OFFSET       0x20
#define CONV2D_INPUT_HEIGHT_OFFSET    0x40
#define CONV2D_INPUT_WIDTH_OFFSET     0x60
#define CONV2D_KERNEL_REG0_OFFSET     0x70
#define CONV2D_KERNEL_REG1_OFFSET     0x78
#define CONV2D_KERNEL_REG2_OFFSET     0x79
#define CONV2D_KERNEL_REG3_OFFSET     0x80
#define CONV2D_KERNEL_REG4_OFFSET     0x88
#define CONV2D_KERNEL_SIZE_OFFSET     0x90
#define CONV2D_USE_RELU_OFFSET        0x98
#define CONV2D_STRIDE_OFFSET          0xA0

#define MMIO_BASE        0x08808000

/**
 * \brief   Memory map of the 2D Convolution accelerator.
 */
typedef struct {
  __IO uint8_t  STATUS;        // 0x00: Status register (7 bits)
  __IO uint8_t  READY;         // 0x08: Ready register (1 bit)
  __IO uint64_t SRC_ADDR;      // 0x10: Source address register
  __IO uint64_t DEST_ADDR;     // 0x20: Destination address register
  __IO uint64_t INPUT_HEIGHT;  // 0x40: Input height register
  __IO uint64_t INPUT_WIDTH;   // 0x60: Input width register
  __IO uint64_t KERNEL_REG0;   // 0x70: Kernel register part 1 (8 bytes)
  __IO uint8_t  KERNEL_REG1;   // 0x78: Kernel register part 2 (1 byte)
  __IO uint64_t KERNEL_REG2;   // 0x79: Kernel register part 3 (7 bytes)
  __IO uint64_t KERNEL_REG3;   // 0x80: Kernel register part 4 (8 bytes)
  __IO uint8_t  KERNEL_REG4;   // 0x88: Kernel register part 5 (1 byte)
  __IO uint8_t  KERNEL_SIZE;   // 0x90: Kernel size register
  __IO uint8_t  USE_RELU;      // 0x98: Use ReLU register
  __IO uint8_t  STRIDE;        // 0xA0: Stride register
} Conv2D_Accel_Type;

// Methods //

void reg_write8(uintptr_t addr, uint8_t data);

uint8_t reg_read8(uintptr_t addr);

void reg_write16(uintptr_t addr, uint16_t data);

uint16_t reg_read16(uintptr_t addr);

void reg_write32(uintptr_t addr, uint32_t data);

uint32_t reg_read32(uintptr_t addr);

void reg_write64(unsigned long addr, uint64_t data);

uint64_t reg_read64(unsigned long addr);

// 2D Convolution Engine Functions
/**
 * \brief Initialize the 2D convolution engine
 * \param conv Pointer to the 2D convolution engine
 */
void conv2d_init(Conv2D_Accel_Type *conv);

/**
 * \brief Check if the 2D convolution engine is ready
 * \param conv Pointer to the 2D convolution engine
 * \return 1 if ready, 0 otherwise
 */
uint8_t conv2d_is_ready(Conv2D_Accel_Type *conv);

/**
 * \brief Configure the 2D convolution engine
 * \param conv Pointer to the 2D convolution engine
 * \param src_addr Source address of the input data
 * \param dest_addr Destination address for the output data
 * \param height Input height
 * \param width Input width
 * \param kernel_size Size of the kernel (must be odd)
 * \param use_relu Whether to use ReLU activation
 * \param stride Stride value for the convolution
 */
void conv2d_configure(Conv2D_Accel_Type *conv, uint64_t src_addr, uint64_t dest_addr, 
                       uint64_t height, uint64_t width, uint8_t kernel_size, 
                       uint8_t use_relu, uint8_t stride);

/**
 * \brief Set the kernel values for the 2D convolution
 * \param conv Pointer to the 2D convolution engine
 * \param kernel Pointer to the kernel values
 * \param size Size of the kernel (must be 3 or 5)
 */
void conv2d_set_kernel(Conv2D_Accel_Type *conv, int8_t *kernel, uint8_t size);

/**
 * \brief Start the 2D convolution operation
 * \param conv Pointer to the 2D convolution engine
 */
void conv2d_start(Conv2D_Accel_Type *conv);

/**
 * \brief Wait for the 2D convolution operation to complete
 * \param conv Pointer to the 2D convolution engine
 */
void conv2d_wait_complete(Conv2D_Accel_Type *conv);

/**
 * \brief Software reference convolution (valid convolution, int8 input/kernel -> int16 output)
 * \param input Pointer to input matrix in row-major order
 * \param inputWidth Input width
 * \param inputHeight Input height
 * \param kernel Pointer to flattened kernel values (size*size)
 * \param kernelSize Kernel size (must be 3 or 5)
 * \param stride Stride value (must be >= 1)
 * \param useReLU Whether to apply ReLU on output
 * \param output Pointer to output buffer in row-major order
 */
void convolve(const int8_t* input, uint16_t inputWidth, uint16_t inputHeight,
              const int8_t* kernel, uint8_t kernelSize, uint8_t stride,
              uint8_t useReLU, int16_t* output);

/**
 * \brief Simplified wrapper function for performing 2D convolution
 * \param srcAddrValue Source address of the input data
 * \param destAddrValue Destination address for the output data
 * \param inputHeightValue Input height
 * \param inputWidthValue Input width
 * \param kernel Pointer to the kernel values
 * \param kernelSizeValue Size of the kernel (must be 3 or 5)
 * \param useReLU Whether to use ReLU activation
 * \param strideValue Stride value for the convolution
 * This Function returns a uint8_t, which is a status flag (i.e. error flags)
 * 
 * Here's an example of how to call it:
 * 
  uint8_t status = perform_convolution((uint64_t)&testImage, (uint64_t)&outputImage, HEIGHT, WIDTH, (uint8_t*)kernel, 3, (uint8_t)0, 1);
  
  if (status != 0x0) {
    printf("Error Status: %p\n", status);
  }
 * 
 * If the status is 0x0, That means it was successful.
 */
 
uint8_t perform_convolution(uint64_t srcAddrValue,     uint64_t destAddrValue, 
                         uint16_t inputHeightValue, uint16_t inputWidthValue, 
                         uint8_t* kernel,           uint8_t kernelSizeValue, 
                         uint8_t useReLU,           uint8_t strideValue); 


#ifdef __cplusplus
}
#endif

#endif // __HAL_2D_CONV_H__
