#include "hal_conv.h"
#include "chip_config.h"

// --- 1D Convolution Driver Functions ---

static inline __attribute__((always_inline)) void mmio_write64_fast(uintptr_t addr, uint64_t data) {
  *(volatile uint64_t *)addr = data;
}

static inline __attribute__((always_inline)) uint64_t mmio_read64_fast(uintptr_t addr) {
  return *(volatile uint64_t *)addr;
}

static inline __attribute__((always_inline)) uint64_t conv_read_cycles(void) {
  uint64_t c;
  asm volatile("rdcycle %0" : "=r"(c));
  return c;
}

static uint8_t g_conv_session_valid = 0U;
static uint32_t g_conv_session_input_length = 0U;
static uint8_t g_conv_session_kernel_length = 0U;

static inline int conv_kernel_meta(uint8_t kernel_length, size_t *kernel_packets, uint8_t *kernel_len_encoding) {
  if (kernel_length == 8U) {
    *kernel_packets = 4U;
    *kernel_len_encoding = 0U;
    return 0;
  }
  if (kernel_length == 16U) {
    *kernel_packets = 8U;
    *kernel_len_encoding = 1U;
    return 0;
  }
  return -1;
}

static inline void conv_write_kernel_packets(uint32_t *kernel, size_t kernel_packets) {
  for (size_t i = 0U; i < kernel_packets; ++i) {
    size_t element_idx = i * FP32_PER_PACKET;
    mmio_write64_fast((uintptr_t)(MMIO_BASE + CONV_KERNEL_ADDR), *((uint64_t*) (kernel + element_idx)));
  }
}

static bool dma_wait_till_inactive_timeout(int cycle_no_inflight, uint64_t timeout_cycles) {
  uint64_t start = conv_read_cycles();
  while ((conv_read_cycles() - start) < timeout_cycles) {
    volatile int t = 0;
    while ((t < cycle_no_inflight) && (dma_status() == 0)) {
      t++;
      if ((conv_read_cycles() - start) >= timeout_cycles) {
        return false;
      }
    }
    if (t == cycle_no_inflight) {
      return true;
    }
  }
  return false;
}

void start_conv() {
  reg_write8((uintptr_t)(MMIO_BASE + CONV_START_ADDR), 1);
}

uint8_t get_register_status() {
  return reg_read8((uintptr_t)(MMIO_BASE + CONV_STATUS_ADDR));
}

uint32_t get_register_out_count() {
  return reg_read32((uintptr_t)(MMIO_BASE + CONV_COUNT_ADDR));
}

uint8_t get_register_read_check() {
  return reg_read8((uintptr_t)(MMIO_BASE + READ_CHECK_ADDR));
}

void conv_init() {
  reg_write8((uintptr_t)(MMIO_BASE + CONV_START_ADDR), 0);  
  reg_write8((uintptr_t)(MMIO_BASE + CONV_CLEAR_ADDR), 1);   
  reg_write8((uintptr_t)(MMIO_BASE + CONV_MMIO_RESET), 1);
  g_conv_session_valid = 0U;
}

// This function now ONLY writes parameters (Length, Dilation, Kernel)
int conv_set_params_kernel_only(uint32_t input_length, uint16_t dilation, uint32_t* kernel, uint8_t kernel_length) {
  // Clear MMIO Reset, Clear Datapath, Stop
  reg_write8((uintptr_t)(MMIO_BASE + CONV_MMIO_RESET), 0);
  reg_write8((uintptr_t)(MMIO_BASE + CONV_CLEAR_ADDR), 0); 
  reg_write8((uintptr_t)(MMIO_BASE + CONV_START_ADDR), 0);  

  // Write parameters
  reg_write32((uintptr_t)(MMIO_BASE + CONV_LENGTH_ADDR), input_length);
  reg_write16((uintptr_t)(MMIO_BASE + CONV_DILATION_ADDR), dilation);

  size_t kernel_packets = 0U;
  uint8_t kernel_len_encoding = 0U;
  if (conv_kernel_meta(kernel_length, &kernel_packets, &kernel_len_encoding) != 0) {
    return -1;
  }

  // Write kernel data and kernel length encoding
  conv_write_kernel_packets(kernel, kernel_packets);
  reg_write8((uintptr_t)(MMIO_BASE + CONV_KERNEL_LEN_ADDR), kernel_len_encoding);

  return 0;
}

int conv_begin_preconfigured_session(uint32_t input_length, uint16_t dilation, uint8_t kernel_length) {
  size_t kernel_packets = 0U;
  uint8_t kernel_len_encoding = 0U;
  if (conv_kernel_meta(kernel_length, &kernel_packets, &kernel_len_encoding) != 0) {
    return -1;
  }

  (void)kernel_packets; // kernel packets are consumed by per-run load, but validate here.

  conv_init();

  // Deassert reset/clear and program fixed run configuration once.
  reg_write8((uintptr_t)(MMIO_BASE + CONV_MMIO_RESET), 0);
  reg_write8((uintptr_t)(MMIO_BASE + CONV_CLEAR_ADDR), 0);
  reg_write8((uintptr_t)(MMIO_BASE + CONV_START_ADDR), 0);

  reg_write32((uintptr_t)(MMIO_BASE + CONV_LENGTH_ADDR), input_length);
  reg_write16((uintptr_t)(MMIO_BASE + CONV_DILATION_ADDR), dilation);
  reg_write8((uintptr_t)(MMIO_BASE + CONV_KERNEL_LEN_ADDR), kernel_len_encoding);

  g_conv_session_input_length = input_length;
  g_conv_session_kernel_length = kernel_length;
  g_conv_session_valid = 1U;
  return 0;
}

// Separate function to stream input data (used repeatedly in the main loop)
void conv_stream_input_batch(uint32_t *input, size_t start_element, size_t num_packets) {
    for (size_t i = 0; i < num_packets; ++i) {
        size_t element_idx = start_element + (i * FP32_PER_PACKET);
        mmio_write64_fast((uintptr_t)(MMIO_BASE + CONV_INPUT_ADDR), *((uint64_t*) (input + element_idx)));
    }
}

// Separate function to read output data (used repeatedly in the main loop)
void conv_read_output_batch(uint32_t *output, size_t start_element, size_t num_packets) {
    for (size_t i = 0; i < num_packets; ++i) {
        size_t element_idx = start_element + (i * FP32_PER_PACKET);

        uint64_t current_out = mmio_read64_fast((uintptr_t)(MMIO_BASE + CONV_OUTPUT_ADDR));

        // Unpack the 64-bit word into two 32-bit FP32 elements
        uint32_t *unpacked = (uint32_t *) &current_out;
        output[element_idx]   = unpacked[0];
        output[element_idx + 1] = unpacked[1];
    }
}

// The main streaming driver function
uint8_t perform_convolution_1D(
    uint32_t* input, 
    uint32_t input_length, 
    uint32_t* kernel, 
    uint8_t kernel_length, 
    uint32_t* output, 
    uint16_t dilation
) {
    // 1. Initial setup
    conv_init();
    {
        int cfg_status = conv_set_params_kernel_only(input_length, dilation, kernel, kernel_length);
        if (cfg_status != 0) {
            return STATUS_INVALID;
        }
    }

    if ((input_length % FP32_PER_PACKET) != 0U || (kernel_length % FP32_PER_PACKET) != 0U) {
        return STATUS_INVALID;
    }

    // Calculate total packets
    size_t input_packets = input_length / FP32_PER_PACKET;
    size_t kernel_packets = kernel_length / FP32_PER_PACKET; 
    size_t output_packets = input_packets + kernel_packets;

    // Preload a bounded number of packets before start.
    // This follows the proven baremetal interleaving style while avoiding queue overfill.
    size_t preload_packets = input_packets;
    if (preload_packets > FIFO_CAPACITY_PACKETS) {
        preload_packets = FIFO_CAPACITY_PACKETS;
    }

    for (size_t i = 0; i < preload_packets; ++i) {
        size_t element_idx = i * FP32_PER_PACKET;
        mmio_write64_fast((uintptr_t)(MMIO_BASE + CONV_INPUT_ADDR), *((uint64_t*) (input + element_idx)));
    }

    // 2. Start the convolution engine
    start_conv();

    // 3. Stream with 1:1 interleaving and no out_count polling in the hot loop.
    //    Use write-then-read ordering to avoid blocking on the first output read.
    size_t in_packet_idx = preload_packets;
    for (size_t out_packet_idx = 0; out_packet_idx < output_packets; ++out_packet_idx) {
        if (in_packet_idx < input_packets) {
            size_t in_start_element = in_packet_idx * FP32_PER_PACKET;
            mmio_write64_fast((uintptr_t)(MMIO_BASE + CONV_INPUT_ADDR), *((uint64_t*) (input + in_start_element)));
            in_packet_idx++;
        }

        uint64_t current_out = mmio_read64_fast((uintptr_t)(MMIO_BASE + CONV_OUTPUT_ADDR));
        uint32_t *unpacked = (uint32_t *) &current_out;
        size_t out_start_element = out_packet_idx * FP32_PER_PACKET;
        output[out_start_element] = unpacked[0];
        output[out_start_element + 1U] = unpacked[1];
    }

    // 4. Final Status Read (Output fully drained)
    return get_register_status();
}

uint8_t perform_convolution_1D_preconfigured(
    uint32_t* input,
    uint32_t input_length,
    uint32_t* kernel,
    uint8_t kernel_length,
    uint32_t* output
) {
    if (g_conv_session_valid == 0U) {
        return STATUS_INVALID;
    }

    if (input_length != g_conv_session_input_length || kernel_length != g_conv_session_kernel_length) {
        return STATUS_INVALID;
    }

    if ((input_length % FP32_PER_PACKET) != 0U || (kernel_length % FP32_PER_PACKET) != 0U) {
        return STATUS_INVALID;
    }

    size_t kernel_packets = 0U;
    uint8_t kernel_len_encoding = 0U;
    if (conv_kernel_meta(kernel_length, &kernel_packets, &kernel_len_encoding) != 0) {
        return STATUS_INVALID;
    }

    // Per-run reset of run state, but keep MMIO reset and fixed params intact.
    reg_write8((uintptr_t)(MMIO_BASE + CONV_START_ADDR), 0);
    reg_write8((uintptr_t)(MMIO_BASE + CONV_CLEAR_ADDR), 1);
    reg_write8((uintptr_t)(MMIO_BASE + CONV_CLEAR_ADDR), 0);

    // Re-load kernel each run: start condition in FSM expects kernel queue activity.
    conv_write_kernel_packets(kernel, kernel_packets);
    reg_write8((uintptr_t)(MMIO_BASE + CONV_KERNEL_LEN_ADDR), kernel_len_encoding);

    size_t input_packets = input_length / FP32_PER_PACKET;
    size_t output_packets = input_packets + kernel_packets;

    size_t preload_packets = input_packets;
    if (preload_packets > FIFO_CAPACITY_PACKETS) {
        preload_packets = FIFO_CAPACITY_PACKETS;
    }

    for (size_t i = 0; i < preload_packets; ++i) {
        size_t element_idx = i * FP32_PER_PACKET;
        mmio_write64_fast((uintptr_t)(MMIO_BASE + CONV_INPUT_ADDR), *((uint64_t*) (input + element_idx)));
    }

    start_conv();

    size_t in_packet_idx = preload_packets;
    for (size_t out_packet_idx = 0; out_packet_idx < output_packets; ++out_packet_idx) {
        if (in_packet_idx < input_packets) {
            size_t in_start_element = in_packet_idx * FP32_PER_PACKET;
            mmio_write64_fast((uintptr_t)(MMIO_BASE + CONV_INPUT_ADDR), *((uint64_t*) (input + in_start_element)));
            in_packet_idx++;
        }

        uint64_t current_out = mmio_read64_fast((uintptr_t)(MMIO_BASE + CONV_OUTPUT_ADDR));
        uint32_t *unpacked = (uint32_t *) &current_out;
        size_t out_start_element = out_packet_idx * FP32_PER_PACKET;
        output[out_start_element] = unpacked[0];
        output[out_start_element + 1U] = unpacked[1];
    }

    return get_register_status();
}

// --- Utility 1D Convolution Driver Functions ---

void perform_naive_convolution_1D(uint32_t *arr, size_t arr_len, uint32_t *kernel, size_t kernel_len, size_t dilation, float *output) {
  size_t output_len = arr_len + (kernel_len - 1) * dilation;

    for (int i = 0; i < output_len; i++) {
        output[i] = 0.0f;

        for (int j = 0; j < kernel_len; j++) {
            int arr_index = i + j * dilation - (kernel_len - 1) * dilation;

            uint32_t item = 0;
            if (arr_index >= 0 && arr_index < arr_len) {
                item = arr[arr_index];
            }

            float float_input = *(float*)&item;
            float float_kernel = *(float*)&kernel[j]; 
            output[i] += float_input * float_kernel;
        }
    }
}

// NOTE: As of 12/9/25 the below get_register_status_human_readable() function cannot be included in drivers files because of the following:
// 1) you cannot use printf in driver files.
// 2) you cannot do general string manipulation.
// The above will generate this relocation truncated to fit error
// /bwrcq/C/louiejoshualabata/sp25-chips/software/baremetal-ide/platform/dsp25/src/hal_conv.c:200:(.text+0x5fa): relocation truncated to fit: R_RISCV_HI20 against `.LC0'
// collect2: error: ld returned 1 exit status

// NOTE: I believe this is due to how the sp25-baremetal-ide compiles .c into .riscv binaries because it is possible to do the above, (1) and (2). See /bwrcq/C/louiejoshualabata/sp25-chips/generators/dsp25-audio/baremetal_test/tests-dma/dma_utils.c and how this file is being included in this test /bwrcq/C/louiejoshualabata/sp25-chips/generators/dsp-1d-conv-sp25/baremetal_test/dma-temp.c


// #include <string.h>
// #include <stdio.h> // Required for sprintf

// void get_register_status_human_readable(char* buffer) {

  // if (buffer == NULL) {
    //     return; // Exit if the pointer is NULL
    // }
    
    // // 1. CLEAR THE BUFFER
    // buffer[0] = '\0'; 
    
    // uint8_t status = get_register_status();
    
    // // Pointer to the current end of the string, where new data will be written.
    // // It starts pointing to the beginning of the buffer.
    // char *write_ptr = buffer; 

    // // 2. Check each flag using bitwise AND (&)
    // // We use a temporary variable (len_written) to advance the write_ptr.
    // int len_written = 0;

    // if (status & STATUS_BUSY) {
    //     len_written = sprintf(write_ptr, "BUSY | ");
    //     write_ptr += len_written;
    // }
    // if (status & STATUS_COMPL) {
    //     len_written = sprintf(write_ptr, "COMPL | ");
    //     write_ptr += len_written;
    // }
    // if (status & STATUS_ERROR) {
    //     len_written = sprintf(write_ptr, "ERROR | ");
    //     write_ptr += len_written;
    // }
    // if (status & STATUS_INVALID) {
    //     len_written = sprintf(write_ptr, "INVALID | ");
    //     write_ptr += len_written;
    // }
    // if (status & STATUS_INFINITE) {
    //     len_written = sprintf(write_ptr, "INFINITE | ");
    //     write_ptr += len_written;
    // }
    // if (status & STATUS_OVERFLOW) {
    //     len_written = sprintf(write_ptr, "OVERFLOW | ");
    //     write_ptr += len_written;
    // }
    // if (status & STATUS_UNDERFLOW) {
    //     len_written = sprintf(write_ptr, "UNDERFLOW | ");
    //     write_ptr += len_written;
    // }
    // if (status & STATUS_INEXACT) {
    //     len_written = sprintf(write_ptr, "INEXACT | ");
    //     write_ptr += len_written;
    // }
    
    // // 3. Final string cleanup
    
    // // If the buffer pointer hasn't moved (only contains the initial '\0' or is empty)
    // if (write_ptr == buffer) { 
    //     if (status == 0x00) {
    //         sprintf(buffer, "IDLE/READY");
    //     } else {
    //         sprintf(buffer, "UNKNOWN STATUS");
    //     }
    // } else {
    //     // Remove the trailing " | " 
    //     // We know the length of the trailing part is 3 chars (" | ")
    //     // We just move the write_ptr back 3 positions and null-terminate there.
    //     write_ptr -= 3; 
    //     *write_ptr = '\0';
    // }
// }


bool dma_1dConvDriver(
    uint32_t *input_buffer_ptr,
    uint32_t *output_buffer_ptr,
    uint32_t *kernel_buffer_ptr,
    size_t    total_elements,    // number of FP32 elements in input (assume even)
    size_t    kernel_elements,   // number of FP32 elements in kernel (assume even)
    uint16_t  dilation,
    uint32_t  base_id
) {
    const uint64_t dma_timeout_cycles = 50000000ULL;

    // Packets = number of 64-bit beats (each beat carries 2 FP32s)
    size_t input_packets  = total_elements   / FP32_PER_PACKET;
    size_t kernel_packets = kernel_elements  / FP32_PER_PACKET;

    // Keep this as in your original hardware protocol
    size_t output_packets = input_packets + kernel_packets;

    // --- Reset / configure accelerator ---
    reg_write8(START_ADDR, 0);
    reg_write8(CLEAR_ADDR, 1);
    reg_write8(CLEAR_ADDR, 0);

    reg_write32(LENGTH_ADDR,  total_elements);
    reg_write16(DILATION_ADDR, dilation);

    // ================================
    // 1) DMA the kernel once
    // ================================
    dma_transaction_t k_trans = {
        .core            = DMA_KERNEL_CORE,
        .transaction_id  = base_id,
        .addr_r          = (uint64_t)(uintptr_t)kernel_buffer_ptr,  // from memory
        .addr_w          = KERNEL_ADDR,        // to kernel MMIO FIFO/port
        .inc_r           = DMA_WORD_INC,       // walk memory
        .inc_w           = 0,                  // fixed MMIO address
        .len             = kernel_packets,     // N x 64-bit beats
        .logw            = DMA_WORD_LOGW,      // 8 bytes
        .do_interrupt    = false,
        .do_address_gate = true
    };

    if (!set_DMA_P(k_trans.core, k_trans, true)) {
        return false;
    }
    start_DMA(k_trans.core, k_trans.transaction_id, NULL);

    // Wait once for kernel DMA to complete
    if (!dma_wait_till_inactive_timeout(DMA_IDLE_THRESHOLD, dma_timeout_cycles)) {
        return false;
    }

    // ================================
    // 2) Enable convolution engine
    // ================================
    reg_write8(START_ADDR, 1);

    // ================================
    // 3) DMA *all* output + input in big chunks
    // ================================

    // 3a) Output: accelerator → memory (read from OUTPUT_ADDR)
    dma_transaction_t tx_out = {
        .core            = DMA_OUTPUT_CORE,
        .transaction_id  = base_id + 1,
        .addr_r          = OUTPUT_ADDR,              // from MMIO FIFO
        .addr_w          = (uint64_t)(uintptr_t)output_buffer_ptr,  // to memory
        .inc_r           = 0,                        // fixed MMIO address
        .inc_w           = DMA_WORD_INC,             // walk memory
        .len             = output_packets,           // ALL output beats
        .logw            = DMA_WORD_LOGW,
        .do_interrupt    = false,
        .do_address_gate = true
    };
    if (!set_DMA_P(tx_out.core, tx_out, true)) {
        return false;
    }
    start_DMA(tx_out.core, tx_out.transaction_id, NULL);

    // 3b) Input: memory → accelerator (write to INPUT_ADDR)
    dma_transaction_t tx_in = {
        .core            = DMA_INPUT_CORE,
        .transaction_id  = base_id + 2,
        .addr_r          = (uint64_t)(uintptr_t)input_buffer_ptr,  // from memory
        .addr_w          = INPUT_ADDR,               // to MMIO FIFO
        .inc_r           = DMA_WORD_INC,             // walk memory
        .inc_w           = 0,                        // fixed MMIO address
        .len             = input_packets,            // ALL input beats
        .logw            = DMA_WORD_LOGW,
        .do_interrupt    = false,
        .do_address_gate = true
    };
    if (!set_DMA_P(tx_in.core, tx_in, true)) {
        return false;
    }
    start_DMA(tx_in.core, tx_in.transaction_id, NULL);

    // ================================
    // 4) Wait once for everything to finish
    // ================================
    if (!dma_wait_till_inactive_timeout(DMA_IDLE_THRESHOLD, dma_timeout_cycles)) {
        return false;
    }
    return true;
}
