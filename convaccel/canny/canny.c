#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

#define PI 3.14159265359
#define HEIGHT 256
#define WIDTH 256
#define MAX_KERNEL_SIZE 31  // This will support sigma up to 5.0

void convolution_1D_char(unsigned char *arr, size_t arr_len, float *kernel, size_t kernel_len, size_t dilation, unsigned char *output) {
  /*

  Computes the convolution of arr with the given kernel and dilation factor and stores the result in output, specifically 
  based on the implementation of the convolution block. The first value in the output array is computed with the kernel's 
  left element aligned with the array's left element.

  arr:        pointer to input array      INT8 array
  arr_len:    length of input array       
  kernel:     pointer to kernel array     FP16 array (represented as uint16_t)
  kernel_len: length of kernel array 
  dilation:   dilation factor
  output:     pointer to output array     FP16 array (represented as uint16_t)

  Example input and output: 

  arr:        {1, 2, 3, 4}
  arr_len:    4
  kernel:     {-1, 1, -1}
  kernel_len: 3
  dilation:   1

  output: {-2, -3, -1, -4} ({-1*1 + 1*2 + -1*3, -1*2 + 1*3 + -1*4, -1*3 + 1*4 + -1*0, -1*4 + 1*0 + -1*0})

  For border values (at the end), we assume the array is zero-extended to fit the length of the kernel (including dilation).
  */

    int y = 0;
    for (int x = 1; x < WIDTH - 1; x++) {
        float sum = 0;
        // for (int ky = -1; ky <= 1; ky++) {
            for (int kx = -1; kx <= 1; kx++) {
                sum += arr[y*WIDTH + (x+kx)] * kernel[kx+1];
            }
        // }
        output[y*WIDTH + x] = (unsigned char)sum;
    }
}

void convolution_1D_int(unsigned char *arr, size_t arr_len, int *kernel, size_t kernel_len, size_t dilation, int *output) {
  /*

  Computes the convolution of arr with the given kernel and dilation factor and stores the result in output, specifically 
  based on the implementation of the convolution block. The first value in the output array is computed with the kernel's 
  left element aligned with the array's left element.

  arr:        pointer to input array      INT8 array
  arr_len:    length of input array       
  kernel:     pointer to kernel array     FP16 array (represented as uint16_t)
  kernel_len: length of kernel array 
  dilation:   dilation factor
  output:     pointer to output array     FP16 array (represented as uint16_t)

  Example input and output: 

  arr:        {1, 2, 3, 4}
  arr_len:    4
  kernel:     {-1, 1, -1}
  kernel_len: 3
  dilation:   1

  output: {-2, -3, -1, -4} ({-1*1 + 1*2 + -1*3, -1*2 + 1*3 + -1*4, -1*3 + 1*4 + -1*0, -1*4 + 1*0 + -1*0})

  For border values (at the end), we assume the array is zero-extended to fit the length of the kernel (including dilation).
  */

    int y = 0;
    for (int x = 1; x < WIDTH - 1; x++) {
        int g = 0;
        // for (int ky = -1; ky <= 1; ky++) {
            for (int kx = -1; kx <= 1; kx++) {
                g += arr[y*WIDTH + (x+kx)] * kernel[kx+1];
            }
        // }
        output[y*WIDTH + x] = g;
    }
}

void twoDimCharConv(unsigned char *in_arr, unsigned char *test_out, float kernel[3][8], int width, int height) {
    uint32_t in_len[1] = {WIDTH};
    uint16_t in_dilation[1] = {1};

    unsigned char test_out1[WIDTH * HEIGHT];
    for (int i = 0; i < 8; i++) {
        test_out1[(HEIGHT - 2)*WIDTH + i] = 0;
        test_out1[(HEIGHT - 1)*WIDTH + i] = 0;
    }
    unsigned char test_out2[WIDTH * HEIGHT];
    for (int i = 0; i < 8; i++) {
        test_out2[0*WIDTH + i] = 0;
        test_out2[(HEIGHT - 1)*WIDTH + i] = 0;
    }
    unsigned char test_out3[WIDTH * HEIGHT];
    for (int i = 0; i < 8; i++) {
        test_out3[0*WIDTH + i] = 0;
        test_out3[1*WIDTH + i] = 0;
    }

    for (int y = 0; y < HEIGHT - 2; y++) {
        // puts("Starting test");
        // uint16_t in_kernel1[8] = {0x2D00, 0x3000, 0x2D00, 0, 0, 0, 0, 0}; // {1/16, 2/16, 1/16, 0, 0, 0, 0, 0} in FP16
        // float in_kernel1[8] = {1/16.0, 2/16.0, 1/16.0, 0, 0, 0, 0, 0};
                                                                    

        // puts("Setting values of MMIO registers");
        // for (int x = 0; x < WIDTH; x += 8) {
        //     reg_write64(INPUT_ADDR, *((uint64_t*) (in_arr + x)));             // 64 bits: 8 INT8s
        // }
        // reg_write32(LENGTH_ADDR, in_len[0]);
        // reg_write16(DILATION_ADDR, in_dilation[0]);
        // reg_write64(KERNEL_ADDR, *((uint64_t*) in_kernel1));         // 64 bits: 4 FP16s
        // reg_write64(KERNEL_ADDR, *((uint64_t*) (in_kernel1 + 4)));   // 64 bits: 4 FP16s (Total 8)

        // puts("Starting Convolution");
        // reg_write8(START_ADDR, 1);

        // puts("Waiting for convolution to complete");
        

        
        // printf("Input (INT8): ");
        // for (int i = 0; i < 8; i++) {
        //     printf("%d ", in_arr[y*WIDTH + i]);
        // }

        // printf("\nTest Output (FP16 binary): ");
        // for (int i = 0; i < 2; i++) {
        //     uint64_t current_out = reg_read64(OUTPUT_ADDR);         // the OUTPUT is a queue, each read gives one FP16
        //     // each output is 4 stacked FP16 so do bit shifting to seperate the results
        //     for (int j = 0; j < 4; j++) {
        //         test_out1[y*WIDTH + i*4 + j] = (current_out >> (j*16)) & 0xFFFF;
        //         printf("0x%"PRIx64" ", test_out1[y*WIDTH + i*4 + j]);
        //     }
        // }
        convolution_1D_char(in_arr + y*WIDTH, in_len[0], kernel[0], 8, in_dilation[0], test_out1 + y*WIDTH);
        // for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                printf("%d, ", test_out1[0*width + x]);
            }
            printf("\n");
        // }
    }

    for (int y = 1; y < HEIGHT - 1; y++) {
        // puts("Starting test");
        // uint16_t in_kernel2[8] = {0x3000, 0x3400, 0x3000, 0, 0, 0, 0, 0}; // {2/16, 4/16, 2/16, 0, 0, 0, 0, 0} in FP16
        float in_kernel2[8] = {2/16.0, 4/16.0, 2/16.0, 0, 0, 0, 0, 0};
                                                                    

        // puts("Setting values of MMIO registers");
        // for (int x = 0; x < WIDTH; x += 8) {
        //     reg_write64(INPUT_ADDR, *((uint64_t*) (in_arr + y*WIDTH + x)));             // 64 bits: 8 INT8s
        // }
        // reg_write32(LENGTH_ADDR, in_len[0]);
        // reg_write16(DILATION_ADDR, in_dilation[0]);
        // reg_write64(KERNEL_ADDR, *((uint64_t*) in_kernel2));         // 64 bits: 4 FP16s
        // reg_write64(KERNEL_ADDR, *((uint64_t*) (in_kernel2 + 4)));   // 64 bits: 4 FP16s (Total 8)

        // puts("Starting Convolution");
        // reg_write8(START_ADDR, 1);

        // puts("Waiting for convolution to complete");
        
        // printf("Input (INT8): ");
        // for (int i = 0; i < 8; i++) {
        //     printf("%d ", in_arr[y*WIDTH + i]);
        // }

        // printf("\nTest Output (FP16 binary): ");
        // for (int i = 0; i < 2; i++) {
        //     uint64_t current_out = reg_read64(OUTPUT_ADDR);         // the OUTPUT is a queue, each read gives one FP16
        //     // each output is 4 stacked FP16 so do bit shifting to seperate the results
        //     for (int j = 0; j < 4; j++) {
        //         test_out2[y*WIDTH + i*4 + j] = (current_out >> (j*16)) & 0xFFFF;
        //         printf("0x%"PRIx64" ", test_out2[y*WIDTH + i*4 + j]);
        //     }
        // }
        convolution_1D_char(in_arr + y*WIDTH, in_len[0], kernel[1], 8, in_dilation[0], test_out2 + y*WIDTH);
        // for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                printf("%d, ", test_out2[1*width + x]);
            }
            printf("\n");
        // }
    }

    for (int y = 2; y < HEIGHT; y++) {
        // puts("Starting test");
        // uint16_t in_kernel3[8] = {0x2D00, 0x3000, 0x2D00, 0, 0, 0, 0, 0}; // {1/16, 2/16, 1/16, 0, 0, 0, 0, 0} in FP16
        float in_kernel3[8] = {1/16.0, 2/16.0, 1/16.0, 0, 0, 0, 0, 0};
                                                                    

        // puts("Setting values of MMIO registers");
        // for (int x = 0; x < WIDTH; x += 8) {
        //     reg_write64(INPUT_ADDR, *((uint64_t*) (in_arr + y*WIDTH + x)));             // 64 bits: 8 INT8s
        // }
        // reg_write32(LENGTH_ADDR, in_len[0]);
        // reg_write16(DILATION_ADDR, in_dilation[0]);
        // reg_write64(KERNEL_ADDR, *((uint64_t*) in_kernel3));         // 64 bits: 4 FP16s
        // reg_write64(KERNEL_ADDR, *((uint64_t*) (in_kernel3 + 4)));   // 64 bits: 4 FP16s (Total 8)

        // puts("Starting Convolution");
        // reg_write8(START_ADDR, 1);

        // puts("Waiting for convolution to complete");
        
        // printf("Input (INT8): ");
        // for (int i = 0; i < 8; i++) {
        //     printf("%d ", in_arr[y*WIDTH + i]);
        // }

        // printf("\nTest Output (FP16 binary): ");
        // for (int i = 0; i < 2; i++) {
        //     uint64_t current_out = reg_read64(OUTPUT_ADDR);         // the OUTPUT is a queue, each read gives one FP16
        //     // each output is 4 stacked FP16 so do bit shifting to seperate the results
        //     for (int j = 0; j < 4; j++) {
        //         test_out3[y*WIDTH + i*4 + j] = (current_out >> (j*16)) & 0xFFFF;
        //         printf("0x%"PRIx64" ", test_out3[y*WIDTH + i*4 + j]);
        //     }
        // }
        convolution_1D_char(in_arr + y*WIDTH, in_len[0], kernel[2], 8, in_dilation[0], test_out3 + y*WIDTH);
        // for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                printf("%d, ", test_out3[2*width + x]);
            }
            printf("\n");
        // }
    }

    // uint16_t test_out[WIDTH * HEIGHT];
    for (int y = 1; y < height - 1; y++) {
        for (int x = 0; x < width; x++) {
            test_out[y*WIDTH+x] = test_out1[(y-1)*WIDTH+x] + test_out2[y*WIDTH+x] + test_out3[(y+1)*WIDTH+x];
        }
    }
}

void twoDimIntConv(unsigned char *in_arr, int *test_out, int kernel[3][8], int width, int height) {
    uint32_t in_len[1] = {WIDTH};
    uint16_t in_dilation[1] = {1};

    int test_out1[WIDTH * HEIGHT];
    for (int i = 0; i < 8; i++) {
        test_out1[(HEIGHT - 2)*WIDTH + i] = 0;
        test_out1[(HEIGHT - 1)*WIDTH + i] = 0;
    }
    int test_out2[WIDTH * HEIGHT];
    for (int i = 0; i < 8; i++) {
        test_out2[0*WIDTH + i] = 0;
        test_out2[(HEIGHT - 1)*WIDTH + i] = 0;
    }
    int test_out3[WIDTH * HEIGHT];
    for (int i = 0; i < 8; i++) {
        test_out3[0*WIDTH + i] = 0;
        test_out3[1*WIDTH + i] = 0;
    }

    for (int y = 0; y < HEIGHT - 2; y++) {
        // puts("Starting test");
        // uint16_t in_kernel1[8] = {0x2D00, 0x3000, 0x2D00, 0, 0, 0, 0, 0}; // {1/16, 2/16, 1/16, 0, 0, 0, 0, 0} in FP16
        // float in_kernel1[8] = {-1, 0, 1, 0, 0, 0, 0, 0};
                                                                    

        // puts("Setting values of MMIO registers");
        // for (int x = 0; x < WIDTH; x += 8) {
        //     reg_write64(INPUT_ADDR, *((uint64_t*) (in_arr + x)));             // 64 bits: 8 INT8s
        // }
        // reg_write32(LENGTH_ADDR, in_len[0]);
        // reg_write16(DILATION_ADDR, in_dilation[0]);
        // reg_write64(KERNEL_ADDR, *((uint64_t*) in_kernel1));         // 64 bits: 4 FP16s
        // reg_write64(KERNEL_ADDR, *((uint64_t*) (in_kernel1 + 4)));   // 64 bits: 4 FP16s (Total 8)

        // puts("Starting Convolution");
        // reg_write8(START_ADDR, 1);

        // puts("Waiting for convolution to complete");
        

        
        // printf("Input (INT8): ");
        // for (int i = 0; i < 8; i++) {
        //     printf("%d ", in_arr[y*WIDTH + i]);
        // }

        // printf("\nTest Output (FP16 binary): ");
        // for (int i = 0; i < 2; i++) {
        //     uint64_t current_out = reg_read64(OUTPUT_ADDR);         // the OUTPUT is a queue, each read gives one FP16
        //     // each output is 4 stacked FP16 so do bit shifting to seperate the results
        //     for (int j = 0; j < 4; j++) {
        //         test_out1[y*WIDTH + i*4 + j] = (current_out >> (j*16)) & 0xFFFF;
        //         printf("0x%"PRIx64" ", test_out1[y*WIDTH + i*4 + j]);
        //     }
        // }
        convolution_1D_int(in_arr + y*WIDTH, in_len[0], kernel[0], 8, in_dilation[0], test_out1 + y*WIDTH);
        // for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                printf("%d, ", test_out1[0*width + x]);
            }
            printf("\n");
        // }
    }

    for (int y = 1; y < HEIGHT - 1; y++) {
        // puts("Starting test");
        // uint16_t in_kernel2[8] = {0x3000, 0x3400, 0x3000, 0, 0, 0, 0, 0}; // {2/16, 4/16, 2/16, 0, 0, 0, 0, 0} in FP16
        // float in_kernel2[8] = {-2, 0, 2, 0, 0, 0, 0, 0};
                                                                    

        // puts("Setting values of MMIO registers");
        // for (int x = 0; x < WIDTH; x += 8) {
        //     reg_write64(INPUT_ADDR, *((uint64_t*) (in_arr + y*WIDTH + x)));             // 64 bits: 8 INT8s
        // }
        // reg_write32(LENGTH_ADDR, in_len[0]);
        // reg_write16(DILATION_ADDR, in_dilation[0]);
        // reg_write64(KERNEL_ADDR, *((uint64_t*) in_kernel2));         // 64 bits: 4 FP16s
        // reg_write64(KERNEL_ADDR, *((uint64_t*) (in_kernel2 + 4)));   // 64 bits: 4 FP16s (Total 8)

        // puts("Starting Convolution");
        // reg_write8(START_ADDR, 1);

        // puts("Waiting for convolution to complete");
        
        // printf("Input (INT8): ");
        // for (int i = 0; i < 8; i++) {
        //     printf("%d ", in_arr[y*WIDTH + i]);
        // }

        // printf("\nTest Output (FP16 binary): ");
        // for (int i = 0; i < 2; i++) {
        //     uint64_t current_out = reg_read64(OUTPUT_ADDR);         // the OUTPUT is a queue, each read gives one FP16
        //     // each output is 4 stacked FP16 so do bit shifting to seperate the results
        //     for (int j = 0; j < 4; j++) {
        //         test_out2[y*WIDTH + i*4 + j] = (current_out >> (j*16)) & 0xFFFF;
        //         printf("0x%"PRIx64" ", test_out2[y*WIDTH + i*4 + j]);
        //     }
        // }
        convolution_1D_int(in_arr + y*WIDTH, in_len[0], kernel[1], 8, in_dilation[0], test_out2 + y*WIDTH);
        // for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                printf("%d, ", test_out2[1*width + x]);
            }
            printf("\n");
        // }
    }

    for (int y = 2; y < HEIGHT; y++) {
        // puts("Starting test");
        // uint16_t in_kernel3[8] = {0x2D00, 0x3000, 0x2D00, 0, 0, 0, 0, 0}; // {1/16, 2/16, 1/16, 0, 0, 0, 0, 0} in FP16
        // float in_kernel3[8] = {-1, 0, -1, 0, 0, 0, 0, 0};
                                                                    

        // puts("Setting values of MMIO registers");
        // for (int x = 0; x < WIDTH; x += 8) {
        //     reg_write64(INPUT_ADDR, *((uint64_t*) (in_arr + y*WIDTH + x)));             // 64 bits: 8 INT8s
        // }
        // reg_write32(LENGTH_ADDR, in_len[0]);
        // reg_write16(DILATION_ADDR, in_dilation[0]);
        // reg_write64(KERNEL_ADDR, *((uint64_t*) in_kernel3));         // 64 bits: 4 FP16s
        // reg_write64(KERNEL_ADDR, *((uint64_t*) (in_kernel3 + 4)));   // 64 bits: 4 FP16s (Total 8)

        // puts("Starting Convolution");
        // reg_write8(START_ADDR, 1);

        // puts("Waiting for convolution to complete");
        
        // printf("Input (INT8): ");
        // for (int i = 0; i < 8; i++) {
        //     printf("%d ", in_arr[y*WIDTH + i]);
        // }

        // printf("\nTest Output (FP16 binary): ");
        // for (int i = 0; i < 2; i++) {
        //     uint64_t current_out = reg_read64(OUTPUT_ADDR);         // the OUTPUT is a queue, each read gives one FP16
        //     // each output is 4 stacked FP16 so do bit shifting to seperate the results
        //     for (int j = 0; j < 4; j++) {
        //         test_out3[y*WIDTH + i*4 + j] = (current_out >> (j*16)) & 0xFFFF;
        //         printf("0x%"PRIx64" ", test_out3[y*WIDTH + i*4 + j]);
        //     }
        // }
        convolution_1D_int(in_arr + y*WIDTH, in_len[0], kernel[2], 8, in_dilation[0], test_out3 + y*WIDTH);
        // for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                printf("%d, ", test_out3[2*width + x]);
            }
            printf("\n");
        // }
    }

    // uint16_t test_out[WIDTH * HEIGHT];
    for (int y = 1; y < height - 1; y++) {
        for (int x = 0; x < width; x++) {
            test_out[y*WIDTH+x] = test_out1[(y-1)*WIDTH+x] + test_out2[y*WIDTH+x] + test_out3[(y+1)*WIDTH+x];
        }
    }
}

// Gaussian Blur implementation
void gaussianBlur(unsigned char* src, unsigned char* dst) {
    float kernel[3][8] = {{1/16.0, 2/16.0, 1/16.0, 0, 0, 0, 0, 0},
                          {2/16.0, 4/16.0, 2/16.0, 0, 0, 0, 0, 0},
                          {1/16.0, 2/16.0, 1/16.0, 0, 0, 0, 0, 0}};
    twoDimCharConv(src, dst, kernel, WIDTH, HEIGHT);
}

// Sobel operator implementation
void sobelOperator(unsigned char* src, unsigned char* gradientMagnitude, unsigned char* gradientDirection) {
    int sobelX[3][8] = {{-1, 0, 1, 0, 0, 0, 0, 0},
                            {-2, 0, 2, 0, 0, 0, 0, 0},
                            {-1, 0, 1, 0, 0, 0, 0, 0}};
    int sobelY[3][8] = {{-1, -2, -1, 0, 0, 0, 0, 0},
                            {0, 0, 0, 0, 0, 0, 0, 0},
                            {1, 2, 1, 0, 0, 0, 0, 0}};

    int gradientX[WIDTH * HEIGHT];
    int gradientY[WIDTH * HEIGHT];

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
            float gx = gradientX[idx];
            float gy = gradientY[idx];
            gradientMagnitude[idx] = (unsigned char)sqrt(gx*gx + gy*gy);
            gradientDirection[idx] = (unsigned char)(atan2(gy, gx) * 180.0 / PI);
        }
    }
}

// Non-maximum suppression implementation
void nonMaxSuppression(unsigned char* gradientMagnitude, unsigned char* gradientDirection, unsigned char* nms) {
    for (int y = 1; y < HEIGHT - 1; y++) {
        for (int x = 1; x < WIDTH - 1; x++) {
            int idx = y * WIDTH + x;
            float angle = gradientDirection[idx];
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

// Double threshold implementation
void doubleThreshold(unsigned char* src, unsigned char* dst, float highThreshold, float lowThreshold) {
    const unsigned char high = highThreshold * 255;
    const unsigned char low = lowThreshold * 255;
    
    for (int i = 0; i < WIDTH * HEIGHT; i++) {
        if (src[i] >= high)
            dst[i] = 255;
        else if (src[i] >= low)
            dst[i] = 128;
        else
            dst[i] = 0;
    }
}

// Hysteresis implementation
void hysteresis(unsigned char* src, unsigned char* dst) {
    memcpy(dst, src, WIDTH * HEIGHT);
    int changed;
    
    do {
        changed = 0;
        for (int y = 1; y < HEIGHT - 1; y++) {
            for (int x = 1; x < WIDTH - 1; x++) {
                int idx = y * WIDTH + x;
                if (dst[idx] == 128) {
                    for (int dy = -1; dy <= 1; dy++) {
                        for (int dx = -1; dx <= 1; dx++) {
                            if (dx == 0 && dy == 0) continue;
                            int neighborIdx = (y + dy) * WIDTH + (x + dx);
                            if (dst[neighborIdx] == 255) {
                                dst[idx] = 255;
                                changed = 1;
                                break;
                            }
                        }
                        if (dst[idx] == 255) break;
                    }
                    if (dst[idx] == 128) {
                        dst[idx] = 0;
                    }
                }
            }
        }
    } while (changed);
}

void subjectIdentification(unsigned char* src, int position[2]) {
    float xSums[HEIGHT] = {0};
    float xIndices[HEIGHT] = {0};
    float ySums[WIDTH] = {0};
    float yIndices[HEIGHT] = {0};
    printf("%d %d \n", xSums[0], ySums[0]);
    for (int y = 1; y < HEIGHT - 1; y++) {
        for (int x = 1; x < WIDTH - 1; x++) {
            if (src[y * WIDTH + x] == 255) {
                xSums[y] += x;
                xIndices[y] += 1;
                ySums[x] += y;
                yIndices[x] += 1;
            }
        }
    }

    float xSum = 0;
    for (int i = 0; i < HEIGHT; i++) {
        if (xIndices[i] != 0) {
            xSum += xSums[i] / xIndices[i];
        }
    }

    float ySum = 0;
    for (int i = 0; i < WIDTH; i++) {
        if (yIndices[i] != 0) {
            ySum += ySums[i] / yIndices[i];
        }
    }

    position[0] = (int) (xSum / HEIGHT);
    position[1] = (int) (ySum / WIDTH);
}
