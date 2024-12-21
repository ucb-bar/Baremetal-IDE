#include <string.h>
#include <math.h>
#include <stdint.h>

#define PI 3.14159265359
#define HEIGHT 256
#define WIDTH 256
#define MAX_KERNEL_SIZE 31  // This will support sigma up to 5.0

#define INPUT_ADDR      0x08800000
#define OUTPUT_ADDR     0x08800020
#define KERNEL_ADDR     0x08800040
#define START_ADDR      0x0880006C
#define LENGTH_ADDR     0x08800078
#define DILATION_ADDR   0x0880007C

// Optimized version of double threshold using RISC-V Vector instructions
void doubleThreshold(unsigned char* src, unsigned char* dst,
                    float highThreshold, float lowThreshold) {
    const unsigned char high = highThreshold * 255;
    const unsigned char low = lowThreshold * 255;
    register uint8_t strong_edge = 255;
    register uint8_t weak_edge = 128;
    
    uint8_t* src_ptr = src;
    uint8_t* dst_ptr = dst;
    uint8_t* const src_end = src_ptr + (HEIGHT * WIDTH);
    
    while (src_ptr + 16 < src_end) {
        asm volatile(
            "vsetvli zero, %0, e8, m8, ta, ma\n\t"
            "vle8.v v8, (%1)\n\t"
            // Compare with high threshold into v0
            "vmsgeu.vx v0, v8, %2\n\t"         
            // Initialize and set strong edges
            "vmv.v.x v16, x0\n\t"              
            "vmerge.vxm v16, v16, %3, v0\n\t"  
            // Compare with low threshold
            "vmsgeu.vx v1, v8, %4\n\t"         
            // Save strong edge mask
            "vmv.v.v v2, v0\n\t"               
            // Set v0 = low AND NOT high directly
            "vmandn.mm v0, v1, v2\n\t"         
            // Set weak edges
            "vmerge.vxm v16, v16, %5, v0\n\t"  
            // Store results
            "vse8.v v16, (%6)\n\t"
            "vle8.v v8, (%7)"                  
            :
            : "r"(16), "r"(src_ptr), 
              "r"(high), "r"(strong_edge), 
              "r"(low), "r"(weak_edge), 
              "r"(dst_ptr), "r"(src_ptr + 16)
            : "v0", "v1", "v2", "v8", "v16", "memory"
        );

        src_ptr += 16;
        dst_ptr += 16;
    }

    // Handle remaining elements
    if (src_ptr < src_end) {
        size_t vl;
        asm volatile(
            "vsetvli %0, %1, e8, m8, ta, ma\n\t"
            "vle8.v v8, (%2)\n\t"
            // Compare with high threshold into v0
            "vmsgeu.vx v0, v8, %3\n\t"
            // Initialize and set strong edges
            "vmv.v.x v16, x0\n\t"
            "vmerge.vxm v16, v16, %4, v0\n\t"
            // Compare with low threshold
            "vmsgeu.vx v1, v8, %5\n\t"
            // Save strong edge mask
            "vmv.v.v v2, v0\n\t"
            // Set v0 = low AND NOT high directly
            "vmandn.mm v0, v1, v2\n\t"
            // Set weak edges
            "vmerge.vxm v16, v16, %6, v0\n\t"
            // Store results
            "vse8.v v16, (%7)"
            : "=r"(vl)
            : "r"((size_t)(src_end - src_ptr)), "r"(src_ptr),
              "r"(high), "r"(strong_edge),
              "r"(low), "r"(weak_edge), "r"(dst_ptr)
            : "v0", "v1", "v2", "v8", "v16", "memory"
        );
    }
}

// Gaussian Blur implementation
// void gaussianBlur(unsigned char* src, unsigned char* dst) {
//     uint32_t in_len[1] = {WIDTH};
//     uint16_t in_dilation[1] = {1};

//     unsigned char test_out1[WIDTH * HEIGHT];
//     for (int i = 0; i < 8; i++) {
//         test_out1[(HEIGHT - 2)*WIDTH + i] = 0;
//         test_out1[(HEIGHT - 1)*WIDTH + i] = 0;
//     }
//     unsigned char test_out2[WIDTH * HEIGHT];
//     for (int i = 0; i < 8; i++) {
//         test_out2[0*WIDTH + i] = 0;
//         test_out2[(HEIGHT - 1)*WIDTH + i] = 0;
//     }
//     unsigned char test_out3[WIDTH * HEIGHT];
//     for (int i = 0; i < 8; i++) {
//         test_out3[0*WIDTH + i] = 0;
//         test_out3[1*WIDTH + i] = 0;
//     }

//     for (int y = 0; y < HEIGHT - 2; y++) {
//         puts("\r\nStarting test");
//         reg_write8(RESET_ADDR, 1);

//         uint32_t in_len[1] = {WIDTH};
//         uint16_t in_dilation[1] = {1};
//         uint16_t in_kernel[16] = {0x2D00, 0x3000, 0x2D00, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // {1/16, 2/16, 1/16, 0, 0, 0, 0, 0} in FP16              

//         puts("\r\nSetting values of MMIO registers");
//         set_conv_params(16, 1, ((uint64_t*) in_kernel));                  
//         write_conv_dma(0, WIDTH, src + y*WIDTH);

//         uint64_t cpu_start_cycles = READ_CSR("mcycle");
//         puts("\r\nStarting Convolution");
//         asm volatile ("fence");
//         start_conv();

//         uint64_t cpu_end_cycles = READ_CSR("mcycle");

//         asm volatile ("fence");

//         puts("\r\nWaiting for convolution to complete");
        
//         printf("\r\nInput (INT8): ");
//         for (int i = 0; i < 16; i++) {
//             printf("%d ", (src + y*WIDTH)[i]);
//         }

//         read_conv_dma(0, WIDTH, ((uint64_t*) (test_out1 + y*WIDTH)));
//     }

//     for (int y = 1; y < HEIGHT - 1; y++) {
//         puts("\r\nStarting test");
//         reg_write8(RESET_ADDR, 1);

//         uint32_t in_len[1] = {WIDTH};
//         uint16_t in_dilation[1] = {1};
//         uint16_t in_kernel[16] = {0x3000, 0x3400, 0x3000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // {2/16, 4/16, 2/16, 0, 0, 0, 0, 0} in FP16              

//         puts("\r\nSetting values of MMIO registers");
//         set_conv_params(16, 1, ((uint64_t*) in_kernel));                  
//         write_conv_dma(0, WIDTH, src + y*WIDTH);

//         uint64_t cpu_start_cycles = READ_CSR("mcycle");
//         puts("\r\nStarting Convolution");
//         asm volatile ("fence");
//         start_conv();

//         uint64_t cpu_end_cycles = READ_CSR("mcycle");

//         asm volatile ("fence");

//         puts("\r\nWaiting for convolution to complete");
        
//         printf("\r\nInput (INT8): ");
//         for (int i = 0; i < 16; i++) {
//             printf("%d ", (src + y*WIDTH)[i]);
//         }

//         read_conv_dma(0, WIDTH, ((uint64_t*) (test_out3 + y*WIDTH)));
//     }

//     for (int y = 2; y < HEIGHT; y++) {
//         puts("\r\nStarting test");
//         reg_write8(RESET_ADDR, 1);

//         uint32_t in_len[1] = {WIDTH};
//         uint16_t in_dilation[1] = {1};
//         uint16_t in_kernel[16] = {0x2D00, 0x3000, 0x2D00, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // {1/16, 2/16, 1/16, 0, 0, 0, 0, 0} in FP16              

//         puts("\r\nSetting values of MMIO registers");
//         set_conv_params(16, 1, ((uint64_t*) in_kernel));                  
//         write_conv_dma(0, WIDTH, src + y*WIDTH);

//         uint64_t cpu_start_cycles = READ_CSR("mcycle");
//         puts("\r\nStarting Convolution");
//         asm volatile ("fence");
//         start_conv();

//         uint64_t cpu_end_cycles = READ_CSR("mcycle");

//         asm volatile ("fence");

//         puts("\r\nWaiting for convolution to complete");
        
//         printf("\r\nInput (INT8): ");
//         for (int i = 0; i < 16; i++) {
//             printf("%d ", (src + y*WIDTH)[i]);
//         }

//         read_conv_dma(0, WIDTH, ((uint64_t*) (test_out3 + y*WIDTH)));
//     }

//     for (int y = 1; y < height - 1; y++) {
//         for (int x = 0; x < width; x++) {
//             dst[y*WIDTH+x] = test_out1[(y-1)*WIDTH+x] + test_out2[y*WIDTH+x] + test_out3[(y+1)*WIDTH+x];
//         }
//     }
// }

void convolution_1D_int(int8_t *in_arr, size_t arr_len, uint16_t *kernel, size_t kernel_len, size_t dilation, uint16_t *test_out) {
    puts("\r\nStarting test");
    reg_write8(RESET_ADDR, 1);

    // https://bwrcrepo.eecs.berkeley.edu/ee290c_ee194_intech22/sp24-chips/-/wikis/digital/dsp24/Programming-Interfaces#convolution-accelerator
    // reg_write8(INPUT_TYPE_ADDR, 0);
    // int8_t in_arr[16] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
    uint32_t in_len[1] = {kernel_len};
    uint16_t in_dilation[1] = {dilation};
    // uint16_t in_kernel[8] = {0x4000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000}; // {2, 0, 0, 0, 0, 0, 0, 0} in FP16              

    puts("\r\nSetting values of MMIO registers");
    set_conv_params(kernel_len, 1, ((uint64_t*) in_kernel));                  
    write_conv_dma(0, WIDTH, in_arr);

    uint64_t cpu_start_cycles = READ_CSR("mcycle");
    puts("\r\nStarting Convolution");
    asm volatile ("fence");
    start_conv();

    uint64_t cpu_end_cycles = READ_CSR("mcycle");

    asm volatile ("fence");

    puts("\r\nWaiting for convolution to complete");
    
    printf("\r\nInput (INT8): ");
    for (int i = 0; i < 16; i++) {
        printf("%d ", in_arr[i]);
    }
}

void twoDimIntConv(int8_t *in_arr, uint16_t *test_out, uint16_t kernel[3][8], int width, int height) {
    uint32_t in_len[1] = {WIDTH};
    uint16_t in_dilation[1] = {1};

    uint16_t test_out1[WIDTH * HEIGHT];
    for (int i = 0; i < 8; i++) {
        test_out1[(HEIGHT - 2)*WIDTH + i] = 0;
        test_out1[(HEIGHT - 1)*WIDTH + i] = 0;
    }
    uint16_t test_out2[WIDTH * HEIGHT];
    for (int i = 0; i < 8; i++) {
        test_out2[0*WIDTH + i] = 0;
        test_out2[(HEIGHT - 1)*WIDTH + i] = 0;
    }
    uint16_t test_out3[WIDTH * HEIGHT];
    for (int i = 0; i < 8; i++) {
        test_out3[0*WIDTH + i] = 0;
        test_out3[1*WIDTH + i] = 0;
    }

    for (int y = 0; y < HEIGHT - 2; y++) {
        convolution_1D_int(in_arr + y*WIDTH, in_len[0], kernel[0], 8, in_dilation[0], test_out1 + y*WIDTH);
    }

    for (int y = 1; y < HEIGHT - 1; y++) {
        convolution_1D_int(in_arr + y*WIDTH, in_len[0], kernel[1], 8, in_dilation[0], test_out2 + y*WIDTH);
    }

    for (int y = 2; y < HEIGHT; y++) {
        convolution_1D_int(in_arr + y*WIDTH, in_len[0], kernel[2], 8, in_dilation[0], test_out3 + y*WIDTH);
    }

    for (int y = 1; y < height - 1; y++) {
        for (int x = 0; x < width; x++) {
            test_out[y*WIDTH+x] = test_out1[(y-1)*WIDTH+x] + test_out2[y*WIDTH+x] + test_out3[(y+1)*WIDTH+x];
        }
    }
}

// Sobel operator implementation
void sobelOperator(int8_t* src, unsigned char* gradientMagnitude, unsigned char* gradientDirection) {
    uint16_t sobelX[3][8] = {{0xBC00, 0x0000, 0x3C00, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000},
                            {0xC000, 0x0000, 0x4000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000},
                            {0xBC00, 0x0000, 0x3C00, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000}};
    uint16_t sobelY[3][8] = {{0xBC00, 0xC000, 0xBC00, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000},
                            {0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000},
                            {0x3C00, 0x4000, 0x3C00, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000}};

    uint16_t gradientX[WIDTH * HEIGHT];
    uint16_t gradientY[WIDTH * HEIGHT];

    twoDimIntConv(src, gradientX, sobelX, WIDTH, HEIGHT);
    twoDimIntConv(src, gradientY, sobelY, WIDTH, HEIGHT);

    // const int Gx[3][3] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
    // const int Gy[3][3] = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}};

    for (int y = 1; y < HEIGHT - 1; y++) {
        for (int x = 1; x < WIDTH - 1; x++) {
            // float gx = 0, gy = 0;
            
            // for (int i = -1; i <= 1; i++) {
            //     for (int j = -1; j <= 1; j++) {
            //         int idx = (y + i) * WIDTH + (x + j);
            //         gx += src[idx] * Gx[i+1][j+1];
            //         gy += src[idx] * Gy[i+1][j+1];
            //     }
            // }

            int idx = y * WIDTH + x;
            float gx = f16_int(gradientX[idx]);
            float gy = f16_int(gradientY[idx]);
            gradientMagnitude[idx] = (unsigned char)sqrt(gx*gx + gy*gy);
            gradientDirection[idx] = (unsigned char)(atan2(gy, gx) * 180.0 / PI);
        }
    }
}

// Non-maximum suppression implementation
void nonMaxSuppression(unsigned char* gradientMagnitude,
                      unsigned char* gradientDirection,
                      unsigned char* nms) {
    for (int y = 1; y < HEIGHT - 1; y++) {
        for (int x = 1; x < WIDTH - 1; x++) {
            int idx = y * WIDTH + x;
            float angle = (gradientDirection[idx] * 360.0 / 255.0) - 180;
            float mag = gradientMagnitude[idx];
            
            if (angle < 0) angle += 180;
            
            float mag1, mag2;
            
            if ((angle >= 0 && angle < 22.5) || (angle >= 157.5 && angle <= 180)) {
                mag1 = gradientMagnitude[idx-1];
                mag2 = gradientMagnitude[idx+1];
            }
            else if (angle >= 22.5 && angle < 67.5) {
                mag1 = gradientMagnitude[(y-1)*WIDTH + (x+1)];
                mag2 = gradientMagnitude[(y+1)*WIDTH + (x-1)];
            }
            else if (angle >= 67.5 && angle < 112.5) {
                mag1 = gradientMagnitude[(y-1)*WIDTH + x];
                mag2 = gradientMagnitude[(y+1)*WIDTH + x];
            }
            else {
                mag1 = gradientMagnitude[(y-1)*WIDTH + (x-1)];
                mag2 = gradientMagnitude[(y+1)*WIDTH + (x+1)];
            }
            
            nms[idx] = (mag >= mag1 && mag >= mag2) ? mag : 0;
        }
    }
}

// Hysteresis implementation
void hysteresis(unsigned char* src,
                unsigned char* dst) {
    const unsigned char STRONG = 255;
    const unsigned char WEAK = 128;
    
    // First pass: mark strong edges
    for (int y = 1; y < HEIGHT - 1; y++) {
        for (int x = 1; x < WIDTH - 1; x++) {
            int idx = y * WIDTH + x;
            if (src[idx] == STRONG) {
                dst[idx] = STRONG;
                // Check 8-connected neighbors
                for (int i = -1; i <= 1; i++) {
                    for (int j = -1; j <= 1; j++) {
                        if (i == 0 && j == 0) continue;
                        int neighborIdx = (y + i) * WIDTH + (x + j);
                        if (src[neighborIdx] == WEAK) {
                            dst[neighborIdx] = STRONG;
                        }
                    }
                }
            }
        }
    }
}
