#include "hal_2d_conv.h"
#include "chip_config.h"
#include <limits.h>
#include <string.h>

void reg_write8(uintptr_t addr, uint8_t data) {
	volatile uint8_t *ptr = (volatile uint8_t *) addr;
	*ptr = data;
}

uint8_t reg_read8(uintptr_t addr) {
	volatile uint8_t *ptr = (volatile uint8_t *) addr;
	return *ptr & 0xFF;
}

void reg_write16(uintptr_t addr, uint16_t data) {
	volatile uint16_t *ptr = (volatile uint16_t *) addr;
	*ptr = data;
}

uint16_t reg_read16(uintptr_t addr) {
	volatile uint16_t *ptr = (volatile uint16_t *) addr;
	return *ptr & 0xFFFF;
}

void reg_write32(uintptr_t addr, uint32_t data) {
	volatile uint32_t *ptr = (volatile uint32_t *) addr;
	*ptr = data;
}

uint32_t reg_read32(uintptr_t addr) {
	volatile uint32_t *ptr = (volatile uint32_t *) addr;
	return *ptr & 0xFFFFFFFF;
}

void reg_write64(unsigned long addr, uint64_t data) {
	volatile uint64_t *ptr = (volatile uint64_t *) addr;
	*ptr = data;
}

uint64_t reg_read64(unsigned long addr) {
	volatile uint64_t *ptr = (volatile uint64_t *) addr;
	return *ptr;
}

/**
 * Initialize the 2D convolution engine, set everything to 0 except for Ready. 
 * Ready is set to 0 to start a convolution operation.
 */
void conv2d_init(Conv2D_Accel_Type *conv) {
    // Reset all registers to default values
    reg_write8((uintptr_t)&conv->STATUS, 0);
    reg_write8((uintptr_t)&conv->READY, 1);
    reg_write64((uintptr_t)&conv->SRC_ADDR, 0);
    reg_write64((uintptr_t)&conv->DEST_ADDR, 0);
    reg_write64((uintptr_t)&conv->INPUT_HEIGHT, 0);
    reg_write64((uintptr_t)&conv->INPUT_WIDTH, 0);
    reg_write64((uintptr_t)&conv->KERNEL_REG0, 0);
    reg_write8((uintptr_t)&conv->KERNEL_REG1, 0);
    reg_write64((uintptr_t)&conv->KERNEL_REG2, 0);
    reg_write64((uintptr_t)&conv->KERNEL_REG3, 0);
    reg_write8((uintptr_t)&conv->KERNEL_REG4, 0);
    reg_write8((uintptr_t)&conv->KERNEL_SIZE, 3); // 3 = Default kernel size of 3x3
    reg_write8((uintptr_t)&conv->USE_RELU, 0);
    reg_write8((uintptr_t)&conv->STRIDE, 0); // 0 = Default stride of 1
}

/**
 * Check if the 2D convolution engine is ready
 * ready bit is set to 0 in order to start convolution
 * When ready == 1, convolution is finished
 */
uint8_t conv2d_is_ready(Conv2D_Accel_Type *conv) {
    // read the ready bit
    return (reg_read8((uintptr_t)&conv->READY) & 0x01);
}

/**
 * Configure the 2D convolution engine
 */
void conv2d_configure(Conv2D_Accel_Type *conv, uint64_t src_addr, uint64_t dest_addr, 
                     uint64_t height, uint64_t width, uint8_t kernel_size, 
                     uint8_t use_relu, uint8_t stride) {
    // Configure the engine with the provided parameters
    reg_write64((uintptr_t)&conv->SRC_ADDR, src_addr);
    reg_write64((uintptr_t)&conv->DEST_ADDR, dest_addr);
    reg_write64((uintptr_t)&conv->INPUT_HEIGHT, height);
    reg_write64((uintptr_t)&conv->INPUT_WIDTH, width);
    reg_write8((uintptr_t)&conv->KERNEL_SIZE, kernel_size);
    reg_write8((uintptr_t)&conv->USE_RELU, use_relu ? 1 : 0);
    reg_write8((uintptr_t)&conv->STRIDE, stride);
}

/**
 * Set the kernel values for the 2D convolution
 */
void conv2d_set_kernel(Conv2D_Accel_Type *conv, int8_t *kernel, uint8_t size) {
    // Check that the kernel size is either 3 or 5
    if ((size != 3) && (size != 5)) {
        return; // Invalid kernel size
    }
    
    // Set the kernel size
    reg_write8((uintptr_t)&conv->KERNEL_SIZE, size);
    
    if (size == 3) {
        // For 3x3 kernel, we only use KERNEL_REG0 and KERNEL_REG1
        // First 8 bytes (KERNEL_REG0)
        uint64_t kernel_reg_value = 0;
        /* take the i-th kernel value
        * shift mask it with 0xFF to make sure it is 8 bits
        * then shift it left by 8*i bits
        * then OR it with the previous value
        * OR with previous value: 00000001 | 00000010 00000000 = 00000010 00000001
        */
        for (int i = 0; i < 8; i++) {
            kernel_reg_value |= ((uint64_t)(kernel[i] & 0xFF)) << (8 * i);
        }


        reg_write64((uintptr_t)&conv->KERNEL_REG0, kernel_reg_value);
        
        // Next byte (KERNEL_REG1)
        reg_write8((uintptr_t)&conv->KERNEL_REG1, kernel[8] & 0xFF);
        
        // Clear the remaining registers
        // For KERNEL_REG2, we need to write 7 bytes carefully
        // just write 8bits at a time
        for (int i = 0; i < 7; i++) {
            reg_write8((uintptr_t)&conv->KERNEL_REG2 + i, 0);
        }
        // Set the remaining registers to 0
        reg_write64((uintptr_t)&conv->KERNEL_REG3, 0);
        reg_write8((uintptr_t)&conv->KERNEL_REG4, 0);
    } else { //IF KERNEL SIZE IS 5

        // For 5x5 kernel, place all alues
        // First 8 bytes (KERNEL_REG0)
        uint64_t kernel_reg_value = 0;
        for (int i = 0; i < 8; i++) {
            kernel_reg_value |= ((uint64_t)(kernel[i] & 0xFF)) << (8 * i);
        }
        reg_write64((uintptr_t)&conv->KERNEL_REG0, kernel_reg_value);
        
        // Next byte (KERNEL_REG1)
        reg_write8((uintptr_t)&conv->KERNEL_REG1, kernel[8] & 0xFF);
        
        // Next 7 bytes (KERNEL_REG2)
        for (int i = 0; i < 7; i++) {
            reg_write8((uintptr_t)&conv->KERNEL_REG2 + i, kernel[9 + i] & 0xFF);
        }
        
        // Next 8 bytes (KERNEL_REG3)
        kernel_reg_value = 0;
        for (int i = 0; i < 8; i++) {
            kernel_reg_value |= ((uint64_t)(kernel[16 + i] & 0xFF)) << (8 * i);
        }
        reg_write64((uintptr_t)&conv->KERNEL_REG3, kernel_reg_value);
        
        // Final byte (KERNEL_REG4)
        reg_write8((uintptr_t)&conv->KERNEL_REG4, kernel[24] & 0xFF);
    }
}

/**
 * Start the 2D convolution operation
 */
void conv2d_start(Conv2D_Accel_Type *conv) {
    // Set the READY register to start the operation
    reg_write8((uintptr_t)&conv->READY, 0);
}

/**
 * Wait for the 2D convolution operation to complete
 * This is just a safety precaution:    
 * If the operation does not complete, we can force the engine to finish.
 */
void conv2d_wait_complete(Conv2D_Accel_Type *conv) {
    // Add a counter for timeout
    int timeout_counter = 0;
    const int TIMEOUT_MAX = 100000000; 
    
    // Poll the ready register until the operation is complete or timeout
    while (!conv2d_is_ready(conv) && timeout_counter < TIMEOUT_MAX) {
        timeout_counter++;
    }
    
    if (timeout_counter >= TIMEOUT_MAX) {
        // Using register access directly instead of printf to avoid string constant issues
        // That might be causing the relocation error
        reg_write8((uintptr_t)&conv->READY, 0x01); // Force the convolution engine to finish.
    }
}

void convolve(const int8_t* input, uint16_t inputWidth, uint16_t inputHeight,
              const int8_t* kernel, uint8_t kernelSize, uint8_t stride,
              uint8_t useReLU, int16_t* output) {
    if (input == NULL || kernel == NULL || output == NULL) {
        return;
    }
    if ((kernelSize != 3u && kernelSize != 5u) || stride == 0u) {
        return;
    }
    if (inputWidth < kernelSize || inputHeight < kernelSize) {
        return;
    }

    const uint16_t outHeight = (uint16_t)((inputHeight - kernelSize) / stride + 1u);
    const uint16_t outWidth = (uint16_t)((inputWidth - kernelSize) / stride + 1u);

    for (uint16_t oh = 0; oh < outHeight; ++oh) {
        for (uint16_t ow = 0; ow < outWidth; ++ow) {
            int32_t acc = 0;
            const uint16_t inRowBase = (uint16_t)(oh * stride);
            const uint16_t inColBase = (uint16_t)(ow * stride);

            for (uint16_t kh = 0; kh < kernelSize; ++kh) {
                const uint16_t inRow = (uint16_t)(inRowBase + kh);
                const uint16_t kRow = (uint16_t)(kh * kernelSize);
                for (uint16_t kw = 0; kw < kernelSize; ++kw) {
                    const uint16_t inCol = (uint16_t)(inColBase + kw);
                    const int32_t a = (int32_t)input[(size_t)inRow * inputWidth + inCol];
                    const int32_t b = (int32_t)kernel[(size_t)kRow + kw];
                    acc += a * b;
                }
            }

            if (useReLU && acc < 0) {
                acc = 0;
            }

            if (acc > INT16_MAX) {
                acc = INT16_MAX;
            } else if (acc < INT16_MIN) {
                acc = INT16_MIN;
            }

            output[(size_t)oh * outWidth + ow] = (int16_t)acc;
        }
    }
}

// /**
//  * Simplified wrapper function for performing 2D convolution
//  * This function combines all the necessary steps into a single call
//  */
// void perform_convolution(uint64_t src_addr, uint64_t dest_addr,
//                         uint64_t height, uint64_t width,
//                         int8_t *kernel, uint8_t kernel_size,
//                         uint8_t use_relu, uint8_t stride) {
//     // Get accelerator pointer
//     Conv2D_Accel_Type *conv = CONV2D;
    
//     // Initialize and configure
//     conv2d_init(conv);
//     conv2d_configure(conv, src_addr, dest_addr, height, width, 
//                     kernel_size, use_relu, stride);
    
//     // Set kernel values
//     conv2d_set_kernel(conv, kernel, kernel_size);
    
//     // Start and wait
//     conv2d_start(conv);
//     conv2d_wait_complete(conv);
// }




/* USE THIS VERSION*/

uint8_t perform_convolution(uint64_t srcAddrValue,     uint64_t destAddrValue, 
                            uint16_t inputHeightValue, uint16_t inputWidthValue, 
                            uint8_t* kernel,           uint8_t kernelSizeValue, 
                            uint8_t useReLU,           uint8_t strideValue) {


    // Hardware/MMIO registers
    volatile uint64_t* statusPtr = (volatile uint64_t*)(MMIO_BASE + 0x0);
    volatile uint8_t*  readyPtr = (volatile uint8_t*) (MMIO_BASE + 0x08);
    volatile uint64_t* srcAddrPtr = (volatile uint64_t*)(MMIO_BASE + 0x10);
    volatile uint64_t* destAddrPtr = (volatile uint64_t*)(MMIO_BASE + 0x20);
    volatile uint64_t* inputHeightPtr = (volatile uint64_t*)(MMIO_BASE + 0x40);
    volatile uint64_t* inputWidthPtr = (volatile uint64_t*)(MMIO_BASE + 0x60);
    volatile uint64_t* kernelRegPtr = (volatile uint64_t*)(MMIO_BASE + 0x70);
    volatile uint8_t*  kernelRegSize = (volatile uint8_t*) (MMIO_BASE + 0x90);
    volatile uint8_t*  useReLUPtr = (volatile uint8_t*) (MMIO_BASE + 0x98);
    volatile uint8_t*  stridePtr = (volatile uint8_t*) (MMIO_BASE + 0xA0);
    //uint8_t  statusValue;                        
    uint8_t  readyValue = 0;

    // Connect Hardware Regs with args
    *srcAddrPtr     = srcAddrValue;
    *destAddrPtr    = destAddrValue;
    // kernel is a pointer; copy kernelSizeValue*kernelSizeValue bytes
    memcpy((void*)kernelRegPtr, kernel, (size_t)(kernelSizeValue * kernelSizeValue));
    asm volatile("fence");
    *inputHeightPtr = inputHeightValue;
    *inputWidthPtr  = inputWidthValue;
    *kernelRegSize  = kernelSizeValue;
    *useReLUPtr     = useReLU;
    *stridePtr      = strideValue;

    asm volatile("fence iorw, iorw" ::: "memory");
    // THIS STARTS THE CONVO
    *readyPtr = readyValue;

    while (*readyPtr == 0);

    asm volatile("fence iorw, iorw" ::: "memory");                          
                            
    return *statusPtr;
}
