/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "chip_config.h"
#include "hal_conv.h"
// #include "../../../tests/mmio.h"
// #include <math.h>

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define PI 3.14159265359
#define HEIGHT 16
#define WIDTH 16
#define MAX_KERNEL_SIZE 31  // This will support sigma up to 5.0

#define INPUT_ADDR      0x08800000
#define OUTPUT_ADDR     0x08800020
#define KERNEL_ADDR     0x08800040
#define START_ADDR      0x0880006C
#define LENGTH_ADDR     0x08800078
#define DILATION_ADDR   0x0880007C
#define INPUT_TYPE_ADDR 0x0880008E
#define RESET_ADDR      0x0880008E

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */


/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN PUC */


void app_init() {
  // torch::executor::runtime_init();
}

// read cycles at current timestamp
uint64_t read_cycles() { 
  uint64_t cycles; 
  asm volatile ("rdcycle %0":"=r" (cycles));
  return cycles;
}

// Compare two arrays and return number of differences
int compareArrays(uint8_t* arr1, uint8_t* arr2, int size) {
    int differences = 0;
    for (int i = 0; i < size; i++) {
        if (arr1[i] != arr2[i]) {
            printf("Difference at index %d: %d vs %d\n", i, arr1[i], arr2[i]);
            differences++;
        }
    }
    return differences;
}

// SATURN Double threshold implementation
void doubleThreshold_opt(uint8_t* src, uint8_t* dst, int size, float highThreshold, float lowThreshold) {
    const unsigned char high = highThreshold * 255;
    const unsigned char low = lowThreshold * 255;
    register uint8_t strong_edge = 255;
    register uint8_t weak_edge = 128;
    
    unsigned char* const src_end = src + size;
    
    while (src + 16 < src_end) {
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
            : "r"(16), "r"(src), 
              "r"(high), "r"(strong_edge), 
              "r"(low), "r"(weak_edge), 
              "r"(dst), "r"(src + 16)
            : "v0", "v1", "v2", "v8", "v16", "memory"
        );

        src += 16;
        dst += 16;
    }

    // Handle remaining elements
    if (src < src_end) {
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
            : "r"((size_t)(src_end - src)), "r"(src),
              "r"(high), "r"(strong_edge),
              "r"(low), "r"(weak_edge), "r"(dst)
            : "v0", "v1", "v2", "v8", "v16", "memory"
        );
    }
}

// Double threshold implementation
void doubleThreshold(uint8_t* src, uint8_t* dst, int size, float highThreshold, float lowThreshold) {
    unsigned char high = highThreshold * 255;
    unsigned char low = lowThreshold * 255;
    
    for (int i = 0; i < size; i++) {
        if (src[i] >= high) {
            dst[i] = 255;  // Strong edge
        } else if (src[i] >= low) {
            dst[i] = 128;  // Weak edge
        } else {
            dst[i] = 0;    // Non-edge
        }
    }
}

void benchmark_double_threshold(){

  int vector_len = 256;
  uint8_t input[vector_len] = {};
  uint8_t saturn_output[vector_len];
  uint8_t rocket_output[vector_len];
  
  // Initialize test pattern
  for (int i = 0; i < vector_len; i++) {
    input[i] = (unsigned char)(rand() % 256);
  }

  float highThreshold = 0.7f;
  float lowThreshold = 0.3f;
  
  uint64_t saturn_start_cycles = read_cycles();
  doubleThreshold_opt(input, saturn_output, vector_len, highThreshold, lowThreshold);
  uint64_t saturn_end_cycles = read_cycles();
  uint64_t saturn_cycles = saturn_end_cycles - saturn_start_cycles;


  uint64_t rocket_start_cycles = read_cycles();
  doubleThreshold(input, rocket_output, vector_len, highThreshold, lowThreshold);
  uint64_t rocket_end_cycles = read_cycles();
  uint64_t rocket_cycles = rocket_end_cycles - rocket_start_cycles;

  
  compareArrays(saturn_output, rocket_output, vector_len);

  printf("ROCKET took %ld cycles", rocket_cycles);
  printf("SATURN took %ld cycles", saturn_cycles);
  printf("saturn was better by %ld cycles than rocket on double threshold", rocket_cycles - saturn_cycles);
}

// ROCKET Non-maximum suppression implementation
void nonMaxSuppression(uint8_t* magnitudes, uint8_t* directions, uint8_t* output, int width, int height) {
    // Clear output
    memset(output, 0, width * height);
    
    for (int y = 1; y < height - 1; y++) {
        for (int x = 1; x < width - 1; x++) {
            float angle = directions[y * width + x];
            float mag = magnitudes[y * width + x];
            
            // Normalize angle to 0-180 degrees
            if (angle < 0) angle += 180;
            
            float mag1 = 0, mag2 = 0;
            
            // 0 degrees (horizontal)
            if ((angle >= 0 && angle < 22.5) || (angle >= 157.5 && angle <= 180)) {
                mag1 = magnitudes[y * width + (x + 1)];
                mag2 = magnitudes[y * width + (x - 1)];
            }
            // 45 degrees (diagonal)
            else if (angle >= 22.5 && angle < 67.5) {
                mag1 = magnitudes[(y - 1) * width + (x + 1)];
                mag2 = magnitudes[(y + 1) * width + (x - 1)];
            }
            // 90 degrees (vertical)
            else if (angle >= 67.5 && angle < 112.5) {
                mag1 = magnitudes[(y - 1) * width + x];
                mag2 = magnitudes[(y + 1) * width + x];
            }
            // 135 degrees (diagonal)
            else if (angle >= 112.5 && angle < 157.5) {
                mag1 = magnitudes[(y - 1) * width + (x - 1)];
                mag2 = magnitudes[(y + 1) * width + (x + 1)];
            }
            
            // Non-maximum suppression
            if (mag >= mag1 && mag >= mag2) {
                output[y * width + x] = magnitudes[y * width + x];
            }
        }
    }
}

// // SATURN Non Max Suppresion Implementation
// void nonMaxSuppression_opt(uint8_t* magnitudes, uint8_t* directions, uint8_t* output, int width, int height) {
//     // Process each row except borders
//     for (int y = 1; y < height - 1; y++) {
//         int row_offset = y * width;
//         int vl;
        
//         // Process each pixel in the row except borders
//         for (int x = 1; x < width - 1; x += vl) {
//             // Calculate vector length for this iteration
//             int remaining = width - 1 - x;
//             asm volatile(
//                 "vsetvli %0, %1, e8, m2, ta, ma"
//                 : "=r"(vl)
//                 : "r"(remaining)
//             );

//             // Load current magnitudes
//             asm volatile(
//                 "vle8.v v8, (%0)"
//                 :
//                 : "r"(magnitudes + row_offset + x)
//                 : "v8"
//             );

//             // Load directions
//             asm volatile(
//                 "vle8.v v10, (%0)"
//                 :
//                 : "r"(directions + row_offset + x)
//                 : "v10"
//             );

//             // Load neighbor magnitudes
//             asm volatile(
//                 "vle8.v v12, (%0)"  // left neighbors
//                 :
//                 : "r"(magnitudes + row_offset + x - 1)
//                 : "v12"
//             );

//             asm volatile(
//                 "vle8.v v14, (%0)"  // right neighbors
//                 :
//                 : "r"(magnitudes + row_offset + x + 1)
//                 : "v14"
//             );

//             asm volatile(
//                 "vle8.v v16, (%0)"  // top neighbors
//                 :
//                 : "r"(magnitudes + row_offset - width + x)
//                 : "v16"
//             );

//             asm volatile(
//                 "vle8.v v18, (%0)"  // bottom neighbors
//                 :
//                 : "r"(magnitudes + row_offset + width + x)
//                 : "v18"
//             );

//             // Initialize result vector
//             asm volatile(
//                 "vmv.v.x v20, zero"
//                 :
//                 :
//                 : "v20"
//             );

//             // Check horizontal edges (direction < 64 or >= 192)
//             uint8_t dir_thresh1 = 64;
//             uint8_t dir_thresh2 = 192;
//             asm volatile(
//                 "vmslt.vx v0, v10, %0\n\t"      // direction < 64
//                 "vmsgeu.vx v1, v10, %1\n\t"      // direction >= 192
//                 "vmor.mm v2, v0, v1\n\t"         // combine masks
//                 "vmsgeu.vv v3, v8, v12\n\t"      // mag >= left
//                 "vmsgeu.vv v4, v8, v14\n\t"      // mag >= right
//                 "vmand.mm v5, v3, v4\n\t"        // combine magnitude comparisons
//                 "vmand.mm v0, v5, v2\n\t"        // final horizontal edge mask
//                 "vmerge.vvm v20, v20, v8, v0"    // keep magnitude if edge
//                 :
//                 : "r"(dir_thresh1), "r"(dir_thresh2)
//                 : "v0", "v1", "v2", "v3", "v4", "v5", "v20"
//             );

//             // Check vertical edges (64 <= direction < 192)
//             asm volatile(
//                 "vmsgeu.vx v0, v10, %0\n\t"      // direction >= 64
//                 "vmslt.vx v1, v10, %1\n\t"       // direction < 192
//                 "vmand.mm v2, v0, v1\n\t"        // combine masks
//                 "vmsgeu.vv v3, v8, v16\n\t"      // mag >= top
//                 "vmsgeu.vv v4, v8, v18\n\t"      // mag >= bottom
//                 "vmand.mm v5, v3, v4\n\t"        // combine magnitude comparisons
//                 "vmand.mm v0, v5, v2\n\t"        // final vertical edge mask
//                 "vmerge.vvm v20, v20, v8, v0"    // keep magnitude if edge
//                 :
//                 : "r"(dir_thresh1), "r"(dir_thresh2)
//                 : "v0", "v1", "v2", "v3", "v4", "v5", "v20"
//             );

//             // Store results
//             asm volatile(
//                 "vse8.v v20, (%0)"
//                 :
//                 : "r"(output + row_offset + x)
//                 : "memory"
//             );
//         }
//     }
// }

// void benchmark_non_max_suppression() {
//     const int width = 16;
//     const int height = 16;
//     const int size = width * height;
    
//     // Use stack arrays instead of malloc
//     uint8_t magnitudes[256];    // 16x16 = 256
//     uint8_t directions[256];
//     uint8_t saturn_output[256];
//     uint8_t rocket_output[256];
    
//     // Initialize test pattern
//     for (int i = 0; i < size; i++) {
//         magnitudes[i] = (uint8_t)(rand() % 256);
//         directions[i] = (uint8_t)(rand() % 256);  // 0-255 maps to 0-360 degrees
//     }
    
//     // Clear outputs
//     memset(saturn_output, 0, size);
//     memset(rocket_output, 0, size);
    
//     uint64_t saturn_start_cycles = read_cycles();
//     nonMaxSuppression_opt(magnitudes, directions, saturn_output);
//     uint64_t saturn_end_cycles = read_cycles();
//     uint64_t saturn_cycles = saturn_end_cycles - saturn_start_cycles;

//     uint64_t rocket_start_cycles = read_cycles();
//     nonMaxSuppression(magnitudes, directions, rocket_output, width, height);
//     uint64_t rocket_end_cycles = read_cycles();
//     uint64_t rocket_cycles = rocket_end_cycles - rocket_start_cycles;
    
//     uint64_t differences = compareArrays(saturn_output, rocket_output, size);

//     printf("ROCKET took %ld cycles", rocket_cycles);
//     printf("SATURN took %ld cycles", saturn_cycles);
//     printf("SATURN was better by %ld cycles than ROCKET on non-max suppression\n", rocket_cycles - saturn_cycles);
// }

// // Non-maximum suppression implementation using RISC-V Vector instructions
// void nonMaxSuppression_opt(unsigned char* gradientMagnitude, unsigned char* gradientDirection, unsigned char* nms) {
//     memset(nms, 0, HEIGHT * WIDTH);  // Initialize output array
    
//     // Process inner pixels (excluding borders)
//     for (int y = 1; y < HEIGHT - 1; y++) {
//         for (int x = 1; x < WIDTH - 1; x += 16) {  // Process 16 pixels at a time
//             int remaining = WIDTH - 1 - x;
//             int vlen = (remaining < 16) ? remaining : 16;
            
//             // Calculate base indices for current position
//             int base_idx = y * WIDTH + x;
            
//             // Load current magnitudes and directions
//             asm volatile(
//                 "vsetvli zero, %0, e8, m1, ta, ma\n\t"
//                 "vle8.v v0, (%1)\n\t"          // Load magnitudes
//                 "vle8.v v1, (%2)"              // Load directions
//                 :
//                 : "r"(vlen), 
//                   "r"(&gradientMagnitude[base_idx]),
//                   "r"(&gradientDirection[base_idx])
//                 : "v0", "v1"
//             );
            
//             // Convert direction to angles (0-180)
//             asm volatile(
//                 "vfcvt.f.xu.v v2, v1\n\t"      // Convert to float
//                 "vfmul.vf v2, v2, %0\n\t"      // Multiply by (360/255)
//                 "vfsub.vf v2, v2, %1\n\t"      // Subtract 180 to get -180 to 180 range
//                 "vfadd.vf v2, v2, %2\n\t"      // Add 180 to those < 0
//                 "vfsgnjx.vv v3, v2, v2\n\t"    // Get absolute value
//                 "vmflt.vf v4, v2, %3\n\t"      // Check if < 0
//                 "vfmerge.vfm v2, v2, %2, v4"   // Add 180 to negative values
//                 : 
//                 : "f"(360.0f/255.0f), "f"(180.0f), "f"(180.0f), "f"(0.0f)
//                 : "v2", "v3", "v4"
//             );
            
//             // Create masks for different angle ranges
//             asm volatile(
//                 // Horizontal edges (0° or 180°)
//                 "vmfle.vf v3, v2, %0\n\t"      // angle <= 22.5
//                 "vmfge.vf v4, v2, %1\n\t"      // angle >= 157.5
//                 "vmor.mm v5, v3, v4\n\t"       // Combine masks
                
//                 // 45° edges (22.5° to 67.5°)
//                 "vmfge.vf v6, v2, %2\n\t"      // angle >= 22.5
//                 "vmfle.vf v7, v2, %3\n\t"      // angle <= 67.5
//                 "vmand.mm v6, v6, v7\n\t"      // Combine range
                
//                 // Vertical edges (67.5° to 112.5°)
//                 "vmfge.vf v7, v2, %4\n\t"      // angle >= 67.5
//                 "vmfle.vf v8, v2, %5\n\t"      // angle <= 112.5
//                 "vmand.mm v7, v7, v8\n\t"      // Combine range
                
//                 // 135° edges (112.5° to 157.5°)
//                 "vmfge.vf v8, v2, %6\n\t"      // angle >= 112.5
//                 "vmfle.vf v9, v2, %7\n\t"      // angle <= 157.5
//                 "vmand.mm v8, v8, v9"          // Combine range
//                 :
//                 : "f"(22.5f), "f"(157.5f), 
//                   "f"(22.5f), "f"(67.5f),
//                   "f"(67.5f), "f"(112.5f),
//                   "f"(112.5f), "f"(157.5f)
//                 : "v3", "v4", "v5", "v6", "v7", "v8", "v9"
//             );
            
//             // Load neighbor magnitudes
//             asm volatile(
//                 "vle8.v v10, (%0)\n\t"         // Left neighbors
//                 "vle8.v v11, (%1)\n\t"         // Right neighbors
//                 "vle8.v v12, (%2)\n\t"         // Top neighbors
//                 "vle8.v v13, (%3)\n\t"         // Bottom neighbors
//                 "vle8.v v14, (%4)\n\t"         // Top-right neighbors
//                 "vle8.v v15, (%5)\n\t"         // Bottom-left neighbors
//                 "vle8.v v16, (%6)\n\t"         // Top-left neighbors
//                 "vle8.v v17, (%7)"             // Bottom-right neighbors
//                 :
//                 : "r"(&gradientMagnitude[base_idx - 1]),
//                   "r"(&gradientMagnitude[base_idx + 1]),
//                   "r"(&gradientMagnitude[base_idx - WIDTH]),
//                   "r"(&gradientMagnitude[base_idx + WIDTH]),
//                   "r"(&gradientMagnitude[base_idx - WIDTH + 1]),
//                   "r"(&gradientMagnitude[base_idx + WIDTH - 1]),
//                   "r"(&gradientMagnitude[base_idx - WIDTH - 1]),
//                   "r"(&gradientMagnitude[base_idx + WIDTH + 1])
//                 : "v10", "v11", "v12", "v13", "v14", "v15", "v16", "v17"
//             );
            
//             // Compare with neighbors based on angle
//             asm volatile(
//                 // Initialize result vector with zeros
//                 "vmv.v.x v20, x0\n\t"
                
//                 // Horizontal edges (compare with left/right)
//                 "vmsltu.vv v21, v10, v0\n\t"    // mag > left
//                 "vmsltu.vv v22, v11, v0\n\t"    // mag > right
//                 "vmand.mm v23, v21, v22\n\t"    // both conditions true
//                 "vmand.mm v23, v23, v5\n\t"     // apply horizontal mask
//                 "vmerge.vvm v20, v20, v0, v0\n\t"  // set result if conditions met
                
//                 // 45° edges (compare with top-right/bottom-left)
//                 "vmsltu.vv v21, v14, v0\n\t"    // mag > top-right
//                 "vmsltu.vv v22, v15, v0\n\t"    // mag > bottom-left
//                 "vmand.mm v23, v21, v22\n\t"    // both conditions true
//                 "vmand.mm v23, v23, v6\n\t"     // apply 45° mask
//                 "vmerge.vvm v20, v20, v0, v0\n\t"  // set result if conditions met
                
//                 // Vertical edges (compare with top/bottom)
//                 "vmsltu.vv v21, v12, v0\n\t"    // mag > top
//                 "vmsltu.vv v22, v13, v0\n\t"    // mag > bottom
//                 "vmand.mm v23, v21, v22\n\t"    // both conditions true
//                 "vmand.mm v23, v23, v7\n\t"     // apply vertical mask
//                 "vmerge.vvm v20, v20, v0, v0\n\t"  // set result if conditions met
                
//                 // 135° edges (compare with top-left/bottom-right)
//                 "vmsltu.vv v21, v16, v0\n\t"    // mag > top-left
//                 "vmsltu.vv v22, v17, v0\n\t"    // mag > bottom-right
//                 "vmand.mm v23, v21, v22\n\t"    // both conditions true
//                 "vmand.mm v23, v23, v8\n\t"     // apply 135° mask
//                 "vmerge.vvm v20, v20, v0, v0"    // set result if conditions met
//                 :
//                 :
//                 : "v20", "v21", "v22", "v23"
//             );
            
//             // Store results
//             asm volatile(
//                 "vse8.v v20, (%0)"
//                 :
//                 : "r"(&nms[base_idx])
//                 : "memory"
//             );
//         }
//     }
// }

// naive C 1D conv
void convolution_1D(unsigned char *arr, size_t arr_len, float *kernel, size_t kernel_len, size_t dilation, unsigned char *output) {
    int y = 0;
    for (int x = 1; x < WIDTH - 1; x++) {
        float sum = 0;
            for (int kx = -1; kx <= 1; kx++) {
                sum += arr[y*WIDTH + (x+kx)] * kernel[kx+1];
            }
        // }
        output[y*WIDTH + x] = (unsigned char)sum;
    }
}

// Gaussian Blur implementation
void gaussian_blur(unsigned char* src, unsigned char* dst) {
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
        float in_kernel1[8] = {1/16.0, 2/16.0, 1/16.0, 0, 0, 0, 0, 0};
        convolution_1D(src + y*WIDTH, in_len[0], in_kernel1, 8, in_dilation[0], test_out1 + y*WIDTH);
        for (int x = 0; x < WIDTH; x++) {
            printf("%d, ", test_out1[0*WIDTH + x]);
        }
        printf("\n");
    }

    for (int y = 1; y < HEIGHT - 1; y++) {
        float in_kernel2[8] = {2/16.0, 4/16.0, 2/16.0, 0, 0, 0, 0, 0};
        convolution_1D(src + y*WIDTH, in_len[0], in_kernel2, 8, in_dilation[0], test_out2 + y*WIDTH);
        for (int x = 0; x < WIDTH; x++) {
            printf("%d, ", test_out2[1*WIDTH + x]);
        }
        printf("\n");
    }

    for (int y = 2; y < HEIGHT; y++) {
        float in_kernel3[8] = {1/16.0, 2/16.0, 1/16.0, 0, 0, 0, 0, 0};
        convolution_1D(src + y*WIDTH, in_len[0], in_kernel3, 8, in_dilation[0], test_out3 + y*WIDTH);
        for (int x = 0; x < WIDTH; x++) {
            printf("%d, ", test_out3[2*WIDTH + x]);
        }
        printf("\n");
    }

    for (int y = 1; y < HEIGHT - 1; y++) {
        for (int x = 0; x < WIDTH; x++) {
            dst[y*WIDTH+x] = test_out1[(y-1)*WIDTH+x] + test_out2[y*WIDTH+x] + test_out3[(y+1)*WIDTH+x];
        }
    }
}

// Gaussian Blur implementation
void gaussian_blur_opt(unsigned char* src, unsigned char* dst) {
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
        puts("Starting test");
        uint16_t in_kernel1[8] = {0x2D00, 0x3000, 0x2D00, 0, 0, 0, 0, 0}; // {1/16, 2/16, 1/16, 0, 0, 0, 0, 0} in FP16

        puts("Setting values of MMIO registers");
        for (int x = 0; x < WIDTH; x += 8) {
            reg_write64(INPUT_ADDR, *((uint64_t*) (src + y*WIDTH + x)));             // 64 bits: 8 INT8s
        }
        reg_write32(LENGTH_ADDR, in_len[0]);
        reg_write16(DILATION_ADDR, in_dilation[0]);
        reg_write64(KERNEL_ADDR, *((uint64_t*) in_kernel1));         // 64 bits: 4 FP16s
        reg_write64(KERNEL_ADDR, *((uint64_t*) (in_kernel1 + 4)));   // 64 bits: 4 FP16s (Total 8)

        puts("Starting Convolution");
        reg_write8(START_ADDR, 1);

        puts("Waiting for convolution to complete");
        
        printf("Input (INT8): ");
        for (int i = 0; i < 8; i++) {
            printf("%d ", src[y*WIDTH + i]);
        }

        printf("\nTest Output (FP16 binary): ");
        for (int i = 0; i < 2; i++) {
            uint64_t current_out = reg_read64(OUTPUT_ADDR);         // the OUTPUT is a queue, each read gives one FP16
            // each output is 4 stacked FP16 so do bit shifting to seperate the results
            for (int j = 0; j < 4; j++) {
                test_out1[y*WIDTH + i*4 + j] = (current_out >> (j*16)) & 0xFFFF;
                printf("0x%"PRIx64" ", test_out1[y*WIDTH + i*4 + j]);
            }
        }
    }

    for (int y = 1; y < HEIGHT - 1; y++) {
        puts("Starting test");
        uint16_t in_kernel2[8] = {0x3000, 0x3400, 0x3000, 0, 0, 0, 0, 0}; // {2/16, 4/16, 2/16, 0, 0, 0, 0, 0} in FP16
                                                                    

        puts("Setting values of MMIO registers");
        for (int x = 0; x < WIDTH; x += 8) {
            reg_write64(INPUT_ADDR, *((uint64_t*) (src + y*WIDTH + x)));             // 64 bits: 8 INT8s
        }
        reg_write32(LENGTH_ADDR, in_len[0]);
        reg_write16(DILATION_ADDR, in_dilation[0]);
        reg_write64(KERNEL_ADDR, *((uint64_t*) in_kernel2));         // 64 bits: 4 FP16s
        reg_write64(KERNEL_ADDR, *((uint64_t*) (in_kernel2 + 4)));   // 64 bits: 4 FP16s (Total 8)

        puts("Starting Convolution");
        reg_write8(START_ADDR, 1);

        puts("Waiting for convolution to complete");
        
        printf("Input (INT8): ");
        for (int i = 0; i < 8; i++) {
            printf("%d ", src[y*WIDTH + i]);
        }

        printf("\nTest Output (FP16 binary): ");
        for (int i = 0; i < 2; i++) {
            uint64_t current_out = reg_read64(OUTPUT_ADDR);         // the OUTPUT is a queue, each read gives one FP16
            // each output is 4 stacked FP16 so do bit shifting to seperate the results
            for (int j = 0; j < 4; j++) {
                test_out2[y*WIDTH + i*4 + j] = (current_out >> (j*16)) & 0xFFFF;
                printf("0x%"PRIx64" ", test_out2[y*WIDTH + i*4 + j]);
            }
        }
    }

    for (int y = 2; y < HEIGHT; y++) {
        puts("Starting test");
        uint16_t in_kernel3[8] = {0x2D00, 0x3000, 0x2D00, 0, 0, 0, 0, 0}; // {1/16, 2/16, 1/16, 0, 0, 0, 0, 0} in FP16
                                                                    

        puts("Setting values of MMIO registers");
        for (int x = 0; x < WIDTH; x += 8) {
            reg_write64(INPUT_ADDR, *((uint64_t*) (src + y*WIDTH + x)));             // 64 bits: 8 INT8s
        }
        reg_write32(LENGTH_ADDR, in_len[0]);
        reg_write16(DILATION_ADDR, in_dilation[0]);
        reg_write64(KERNEL_ADDR, *((uint64_t*) in_kernel3));         // 64 bits: 4 FP16s
        reg_write64(KERNEL_ADDR, *((uint64_t*) (in_kernel3 + 4)));   // 64 bits: 4 FP16s (Total 8)

        puts("Starting Convolution");
        reg_write8(START_ADDR, 1);

        puts("Waiting for convolution to complete");
        
        printf("Input (INT8): ");
        for (int i = 0; i < 8; i++) {
            printf("%d ", src[y*WIDTH + i]);
        }

        printf("\nTest Output (FP16 binary): ");
        for (int i = 0; i < 2; i++) {
            uint64_t current_out = reg_read64(OUTPUT_ADDR);         // the OUTPUT is a queue, each read gives one FP16
            // each output is 4 stacked FP16 so do bit shifting to seperate the results
            for (int j = 0; j < 4; j++) {
                test_out3[y*WIDTH + i*4 + j] = (current_out >> (j*16)) & 0xFFFF;
                printf("0x%"PRIx64" ", test_out3[y*WIDTH + i*4 + j]);
            }
        }
    }

    for (int y = 1; y < height - 1; y++) {
        for (int x = 0; x < width; x++) {
            dst[y*WIDTH+x] = test_out1[(y-1)*WIDTH+x] + test_out2[y*WIDTH+x] + test_out3[(y+1)*WIDTH+x];
        }
    }
}

void compare_gaussian_blur() {
    uint8_t test_image[WIDTH * HEIGHT];
    uint8_t naive_result[WIDTH * HEIGHT];
    uint8_t opt_result[WIDTH * HEIGHT];
    
    // Initialize test image with some pattern
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            test_image[y*WIDTH + x] = (x + y) % 256;  // Create a gradient pattern
        }
    }
    
    // Run both implementations
    gaussian_blur(test_image, naive_result);  // Naive implementation
    gaussian_blur_opt(test_image, opt_result); // Optimized implementation
    
    // Compare results
    int mismatch_count = 0;
    for (int y = 1; y < HEIGHT - 1; y++) {
        for (int x = 1; x < WIDTH - 1; x++) {
            int idx = y*WIDTH + x;
            if (naive_result[idx] != opt_result[idx]) {
                mismatch_count++;
                if (mismatch_count <= 10) {  // Print first 10 mismatches
                    printf("Mismatch at (%d,%d): naive=%d, opt=%d, diff=%d\n", 
                            x, y, naive_result[idx], opt_result[idx],
                            (int)naive_result[idx] - (int)opt_result[idx]);
                }
            }
        }
    }
    
    if (mismatch_count == 0) {
        printf("SUCCESS: Both implementations produce identical results!\n");
    } else {
        printf("FAIL: Found %d mismatches between implementations.\n", mismatch_count);
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
        convolution_1D_int(in_arr + y*WIDTH, in_len[0], kernel[0], 8, in_dilation[0], test_out1 + y*WIDTH);
            for (int x = 0; x < width; x++) {
                printf("%d, ", test_out1[0*width + x]);
            }
            printf("\n");
    }

    for (int y = 1; y < HEIGHT - 1; y++) {
        convolution_1D_int(in_arr + y*WIDTH, in_len[0], kernel[1], 8, in_dilation[0], test_out2 + y*WIDTH);
            for (int x = 0; x < width; x++) {
                printf("%d, ", test_out2[1*width + x]);
            }
            printf("\n");
        // }
    }

    for (int y = 2; y < HEIGHT; y++) {
        convolution_1D_int(in_arr + y*WIDTH, in_len[0], kernel[2], 8, in_dilation[0], test_out3 + y*WIDTH);
            for (int x = 0; x < width; x++) {
                printf("%d, ", test_out3[2*width + x]);
            }
            printf("\n");
    }

    // uint16_t test_out[WIDTH * HEIGHT];
    for (int y = 1; y < height - 1; y++) {
        for (int x = 0; x < width; x++) {
            test_out[y*WIDTH+x] = test_out1[(y-1)*WIDTH+x] + test_out2[y*WIDTH+x] + test_out3[(y+1)*WIDTH+x];
        }
    }
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

    for (int y = 1; y < HEIGHT - 1; y++) {
        for (int x = 1; x < WIDTH - 1; x++) {
            int idx = y * WIDTH + x;
            float gx = gradientX[idx];
            float gy = gradientY[idx];
            gradientMagnitude[idx] = (unsigned char)sqrt(gx*gx + gy*gy);
            gradientDirection[idx] = (unsigned char)(atan2(gy, gx) * 180.0 / PI);
        }
    }
}

/* USER CODE END PUC */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(int argc, char **argv) {
  /* MCU Configuration--------------------------------------------------------*/

  /* Configure the system clock */
  /* Configure the system clock */
  
  /* USER CODE BEGIN SysInit */
  UART_InitType UART_init_config;
  UART_init_config.baudrate = 115200;
  UART_init_config.mode = UART_MODE_TX_RX;
  UART_init_config.stopbits = UART_STOPBITS_2;
  uart_init(UART0, &UART_init_config);
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */  
  /* USER CODE BEGIN Init */
  app_init();
  /* USER CODE END Init */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  // while (1) {
  //   app_main();
  //   return 0;
  // }
  
  // app_main();

  printf("running double threshold");
  benchmark_double_threshold();

//   printf("running non max suppresion");
//   benchmark_non_max_suppression();

  printf("running gaussian blur");
  compare_gaussian_blur();

  return 0;
  /* USER CODE END WHILE */
}

/*
 * Main function for secondary harts
 * 
 * Multi-threaded programs should provide their own implementation.
 */
void __attribute__((weak, noreturn)) __main(void) {
  while (1) {
   asm volatile ("wfi");
  }
}