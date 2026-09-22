/**
 * \file    hal_conv.h
 * \author  Louie Labata | louiejoshualabata@berkeley.edu
 * \brief   MMIO generic register operation driver for 1D Conv Engine.
 * \version 0.1
 * * \copyright Copyright (c) 2025
 * */

 #ifndef __HAL_CONV_H__
 #define __HAL_CONV_H__
 
 #ifdef __cplusplus
 extern "C" {
 #endif
 
 // Baremetal IDE Definitions //
 #include "metal.h"
 #include <stdint.h>
 #include <stddef.h> // For size_t, uintptr_t
 // #include <stdio.h>
 // #include <stdint.h>
 #include <string.h>
 
 // 1D Conv Base Address
 #define MMIO_BASE   0x08800000
 
 // Register Offset Definitions
 #define CONV_INPUT_ADDR   0x00
 #define CONV_OUTPUT_ADDR  0x20
 #define CONV_KERNEL_ADDR  0x40
 #define CONV_STATUS_ADDR  0x6A
 #define CONV_START_ADDR   0x6C
 #define CONV_CLEAR_ADDR   0x6D // Set to 1 to flush Datapath
 
 #define CONV_COUNT_ADDR   0x70
 #define CONV_LENGTH_ADDR  0x78
 #define CONV_DILATION_ADDR 0x7C
 
 #define READ_CHECK_ADDR   0x8D
 #define CONV_KERNEL_LEN_ADDR  0x8E
 #define CONV_MMIO_RESET   0x8F
 
 // Convolution Status Struct
 
 // Define the bit masks using an enum for clarity and maintainability
 typedef enum {
     STATUS_BUSY      = 1 << 0,  // Bit 0 (0x01)
     STATUS_COMPL     = 1 << 1,  // Bit 1 (0x02)
     STATUS_ERROR     = 1 << 2,  // Bit 2 (0x04)
     STATUS_INVALID   = 1 << 3,  // Bit 3 (0x08)
     STATUS_INFINITE  = 1 << 4,  // Bit 4 (0x10)
     STATUS_OVERFLOW  = 1 << 5,  // Bit 5 (0x20)
     STATUS_UNDERFLOW = 1 << 6,  // Bit 6 (0x40)
     STATUS_INEXACT   = 1 << 7   // Bit 7 (0x80)
 } ConvStatusBits;
 
 
 // Conservative software preload cap in 64-bit packets (2 FP32 each).
 // Keep this at 8 for compatibility with taped-out revisions that may stall
 // on larger pre-start queue fills.
 #define FIFO_CAPACITY_PACKETS 8
 #define FP32_PER_PACKET 2
 
 // --- Core Driver Functions ---
 
 /**
  * \brief Initializes the 1D Convolution Engine by stopping, clearing, and resetting the MMIO state.
  */
 void conv_init();
 
 /**
  * \brief Configures the convolution engine parameters and loads the kernel via MMIO.
  * * Note: Input data is streamed separately via conv_stream_input_batch.
  * \param input_length The total number of FP32 elements in the input buffer.
  * \param dilation The dilation factor.
  * \param kernel Pointer to the kernel array (FP32 elements).
  * \param kernel_length The number of FP32 elements in the kernel (must be 8 or 16).
  * \return int: 0 on success, -1 if kernel_length is invalid.
  */
 int conv_set_params_kernel_only(uint32_t input_length, uint16_t dilation, uint32_t* kernel, uint8_t kernel_length);

/**
 * \brief Begins a preconfigured streaming session for repeated runs.
 *
 * This performs one-time MMIO reset deassert, clear deassert, stop, and writes
 * fixed parameters (length, dilation, kernel length encoding). Use
 * perform_convolution_1D_preconfigured() for each chunk in the steady-state loop.
 *
 * \param input_length Total number of FP32 elements in each input run.
 * \param dilation Dilation factor used for all runs in this session.
 * \param kernel_length Number of FP32 elements in the kernel (8 or 16).
 * \return int: 0 on success, -1 on invalid kernel_length.
 */
int conv_begin_preconfigured_session(uint32_t input_length, uint16_t dilation, uint8_t kernel_length);
 
 /**
  * \brief Streams a batch of input data (2 FP32 per 64-bit packet) to the accelerator's FIFO via MMIO.
  * \param input Pointer to the input buffer.
  * \param start_element Starting FP32 element index in the input buffer for this batch.
  * \param num_packets Number of 64-bit packets (2 FP32 each) to stream.
  */
 void conv_stream_input_batch(uint32_t *input, size_t start_element, size_t num_packets);
 
 /**
  * \brief Reads a batch of output data (2 FP32 per 64-bit packet) from the accelerator's FIFO via MMIO.
  * \param output Pointer to the output buffer.
  * \param start_element Starting FP32 element index in the output buffer for this batch.
  * \param num_packets Number of 64-bit packets (2 FP32 each) to read.
  */
 void conv_read_output_batch(uint32_t *output, size_t start_element, size_t num_packets);
 
 
 /**
  * \brief Starts the 1D convolution engine by setting the START register.
  */
 void start_conv();
 
 
 // --- Register Access Functions ---
 
 /**
  * \brief Reads the 8-bit status register.
  * \return uint8_t: The status byte, reflecting flags like BUSY, COMPL, ERROR, etc.
  */
 uint8_t get_register_status();
 
 /**
  * \brief Reads the 32-bit output count register, indicating elements available in the output FIFO.
  * \return uint32_t: The number of available 64-bit packets (2 FP32) in the output FIFO.
  */
 uint32_t get_register_out_count();
 
 /**
  * \brief Reads the read check register.
  * \return uint8_t: Value of the READ_CHECK_ADDR register.
  */
 uint8_t get_register_read_check();
 
 
 /**
  * \brief Performs the 1D convolution operation by streaming data in batches via MMIO.
  * \param input Pointer to the input buffer (FP32 array).
  * \param input_length Total number of FP32 elements in the input.
  * \param kernel Pointer to the kernel buffer (FP32 array).
  * \param kernel_length Number of FP32 elements in the kernel (8 or 16).
  * \param output Pointer to the destination output buffer (FP32 array).
  * \param dilation The dilation factor.
  * \return uint8_t: The final status flag of the engine upon completion.
  */
 
  
 /**
 * @brief Perform a 1D convolution using the streaming MMIO hardware accelerator.
  *
  * This function initializes and configures the accelerator, preloads a bounded
  * number of input packets, starts the engine, then performs 1:1 interleaved
  * streaming in the hot loop:
  *   - write next input packet when available
  *   - read one output packet
  *
  * No output-count polling is used in the hot loop.
  *
  * Input and output data are transferred using MMIO-based functions:
  *    - raw 64-bit MMIO packet writes and reads
  *
  * Streaming Protocol Summary:
  *  - Input and kernel values must be packetized (FP32_PER_PACKET floats).
  *  - input_packets  = input_length  / FP32_PER_PACKET
  *  - kernel_packets = kernel_length / FP32_PER_PACKET
  *  - output_packets = input_packets + kernel_packets
  *  - Output is consumed as packets: 2 FP32 per packet.
  * 
  * FP32_PER_PACKET = 2 because the input/output vector queues are 64 bits wide so two FP32 numbers fit in one entry of the input/output vector queues
  *
  * 
  * \param input Pointer to the input buffer (FP32 array).
  * \param input_length Total number of FP32 elements in the input.
  * \param kernel Pointer to the kernel buffer (FP32 array).
  * \param kernel_length Number of FP32 elements in the kernel (8 or 16).
  * \param output Pointer to the destination output buffer (FP32 array).
  * \param dilation The dilation factor.
  * \return uint8_t: Hardware status register after output is fully drained.
  *
  * @note This is an MMIO-based (non-DMA) streaming driver.
  * @note The output size (in packets) is determined by the hardware protocol.
  * @note The function will block until all expected output packets are read.
  */
 
 uint8_t perform_convolution_1D(uint32_t* input, uint32_t input_length, uint32_t* kernel, uint8_t kernel_length, uint32_t* output, uint16_t dilation);

/**
 * @brief Perform one run using a preconfigured session.
 *
 * Expected usage:
 *  1) conv_begin_preconfigured_session(...)
 *  2) In a loop, call perform_convolution_1D_preconfigured(...)
 *
 * This function uses the same interleaved MMIO hot loop as perform_convolution_1D
 * but skips one-time parameter programming per run. Kernel data is still loaded
 * each run because the accelerator start path expects kernel queue activity.
 *
 * \param input Pointer to input FP32 buffer.
 * \param input_length Input FP32 element count (must match session length).
 * \param kernel Pointer to kernel FP32 buffer.
 * \param kernel_length Kernel FP32 element count (must match session length; 8 or 16).
 * \param output Pointer to output FP32 buffer.
 * \return uint8_t: Final hardware status, or STATUS_INVALID on session mismatch.
 */
uint8_t perform_convolution_1D_preconfigured(
    uint32_t* input,
    uint32_t input_length,
    uint32_t* kernel,
    uint8_t kernel_length,
    uint32_t* output);
 
 
 // --- Utility 1D Convolution Driver Functions (Golden Model) ---
 
 /**
  * \brief Computes the convolution of arr with the given kernel and dilation factor (naive C implementation).
  * * For border values, the array is assumed to be zero-extended.
  * * \param arr Pointer to the input array (uint32_t, treated as FP32).
  * \param arr_len Length of the input array.
  * \param kernel Pointer to the kernel array (uint32_t, treated as FP32).
  * \param kernel_len Length of the kernel array.
  * \param dilation Dilation factor.
  * \param output Pointer to the output array (float).
  */
 void perform_naive_convolution_1D(uint32_t *arr, size_t arr_len, uint32_t *kernel, size_t kernel_len, size_t dilation, float *output);
 
// ####################################################
// Copied from Isaac Tsang by LJ Labata- 1/10/26
// ####################################################

// Source: https://bwrcrepo.eecs.berkeley.edu/ee290c_ee194_intech22/dsp25-audio/-/blob/dp-fa25-dma/baremetal_test/tests-dma/1dconvdrive.h?ref_type=heads

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "hal_dma.h"

// --- Hardware Register Offsets ---
#define INPUT_ADDR          0x08800000
#define OUTPUT_ADDR         0x08800020
#define KERNEL_ADDR         0x08800040
#define START_ADDR          0x0880006C
#define CLEAR_ADDR          0x0880006D
#define LENGTH_ADDR         0x08800078
#define DILATION_ADDR       0x0880007C

// --- DMA Configuration ---
#define DMA_INPUT_CORE      3
#define DMA_KERNEL_CORE     2
#define DMA_OUTPUT_CORE     5
#define DMA_WORD_LOGW       3       // 8 bytes (64-bit)
#define DMA_WORD_INC        8       // increment in bytes for memory-side address

// Threshold: Cycles of silence required to consider the bus "done".
#define DMA_IDLE_THRESHOLD  100     

// 64-bit beat = 2 FP32
#define FP32_PER_PACKET         2

// TODO: needs documentation
bool dma_1dConvDriver(
    uint32_t *input_buffer_ptr,
    uint32_t *output_buffer_ptr,
    uint32_t *kernel_buffer_ptr,
    size_t    total_elements,    // number of FP32 elements in input (assume even)
    size_t    kernel_elements,   // number of FP32 elements in kernel (assume even)
    uint16_t  dilation,
    uint32_t  base_id
);

 // Deprecated or removed function (kept for reference of old structure)
 // Deprecated or removed functions (kept for reference of old structure)
 // int conv_set_params(uint32_t* input, uint32_t input_length, uint16_t dilation, uint32_t* kernel, uint8_t kernel_length);
 // uint8_t conv_read_output(uint32_t *output, uint32_t output_len);
 // void get_register_status_human_readable(char* buffer);
 



 #ifdef __cplusplus
 }
 #endif
 
 #endif // __HAL_CONV_H__
