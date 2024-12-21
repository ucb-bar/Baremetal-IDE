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
void gaussian_blur_opt(unsigned char* src, uint16_t* dst) {
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
        test_out2[(HEIGHT - 1) * WIDTH + i] = 0;
    }
    uint16_t test_out3[WIDTH * HEIGHT];
    for (int i = 0; i < 8; i++) {
        test_out3[0*WIDTH + i] = 0;
        test_out3[1*WIDTH + i] = 0;
    }

    for (int y = 0; y < HEIGHT - 2; y++) {
        puts("\r\nStarting test");
        reg_write8(RESET_ADDR, 1);
        reg_write8(INPUT_TYPE_ADDR, 0);

        uint32_t in_len[1] = {WIDTH};
        uint16_t in_dilation[1] = {1};
        uint16_t in_kernel[16] = {0x2D00, 0x3000, 0x2D00, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // {1/16, 2/16, 1/16, 0, 0, 0, 0, 0} in FP16              

        puts("\r\nSetting values of MMIO registers");
        set_conv_params(16, 1, ((uint64_t*) in_kernel));                  
        write_conv_dma(0, WIDTH, src + y*WIDTH);


        puts("\r\nStarting Convolution");
        asm volatile ("fence");
        start_conv();

        asm volatile ("fence");

        puts("\r\nWaiting for convolution to complete");
        
        printf("\r\nInput (INT8): ");
        for (int i = 0; i < 16; i++) {
            printf("%d ", (src + y*WIDTH)[i]);
        }

        read_conv_dma(0, WIDTH, ((uint64_t*) (test_out1 + y*WIDTH)));
    }

    for (int y = 1; y < HEIGHT - 1; y++) {
        puts("\r\nStarting test");
        reg_write8(RESET_ADDR, 1);
        reg_write8(INPUT_TYPE_ADDR, 0);

        uint32_t in_len[1] = {WIDTH};
        uint16_t in_dilation[1] = {1};
        uint16_t in_kernel[16] = {0x3000, 0x3400, 0x3000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // {2/16, 4/16, 2/16, 0, 0, 0, 0, 0} in FP16              

        puts("\r\nSetting values of MMIO registers");
        set_conv_params(16, 1, ((uint64_t*) in_kernel));                  
        write_conv_dma(0, WIDTH, src + y*WIDTH);

        uint64_t cpu_start_cycles = READ_CSR("mcycle");
        puts("\r\nStarting Convolution");
        asm volatile ("fence");
        start_conv();

        uint64_t cpu_end_cycles = READ_CSR("mcycle");

        asm volatile ("fence");

        puts("\r\nWaiting for convolution to complete");
        
        printf("\r\nInput (INT8): ");
        for (int i = 0; i < 16; i++) {
            printf("%d ", (src + y*WIDTH)[i]);
        }

        read_conv_dma(0, WIDTH, ((uint64_t*) (test_out3 + y*WIDTH)));
    }

    for (int y = 2; y < HEIGHT; y++) {
        puts("\r\nStarting test");
        reg_write8(RESET_ADDR, 1);
        reg_write8(INPUT_TYPE_ADDR, 0);

        uint32_t in_len[1] = {WIDTH};
        uint16_t in_dilation[1] = {1};
        uint16_t in_kernel[16] = {0x2D00, 0x3000, 0x2D00, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // {1/16, 2/16, 1/16, 0, 0, 0, 0, 0} in FP16              

        puts("\r\nSetting values of MMIO registers");
        set_conv_params(16, 1, ((uint64_t*) in_kernel));                  
        write_conv_dma(0, WIDTH, src + y*WIDTH);

        puts("\r\nStarting Convolution");
        asm volatile ("fence");
        start_conv();

        asm volatile ("fence");

        puts("\r\nWaiting for convolution to complete");
        
        printf("\r\nInput (INT8): ");
        for (int i = 0; i < 16; i++) {
            printf("%d ", (src + y*WIDTH)[i]);
        }

        read_conv_dma(0, WIDTH, ((uint64_t*) (test_out3 + y*WIDTH)));
    }

    for (int y = 1; y < HEIGHT - 1; y++) {
        for (int x = 0; x < WIDTH; x++) {
            dst[y*WIDTH+x] = f16_add(f16_add(test_out1[(y-1)*WIDTH+x], test_out2[y*WIDTH+x]), test_out3[(y+1)*WIDTH+x]);
        }
    }
}

void compare_gaussian_blur() {
    uint8_t test_image[WIDTH * HEIGHT];
    uint8_t naive_result[WIDTH * HEIGHT];
    uint16_t opt_result[WIDTH * HEIGHT];
    
    // Initialize test image with some pattern
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            test_image[y*WIDTH + x] = (x + y) % 256;  // Create a gradient pattern
        }
    }
    
    // Run both implementations
    gaussian_blur(test_image, naive_result);  // Naive implementation
    gaussian_blur_opt(test_image, opt_result); // Optimized implementation
    for (int i = 0; i < 16; i++) {
        ref_out[i] = f16_from_int((int32_t) (in_arr[i] * 2));
    }
    
    f16_from_int
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

void app_main() {

    puts("\r\nStarting test");
    reg_write8(RESET_ADDR, 1);

    int8_t   in_arr[16]     = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
    uint32_t in_len[1]      = {16};
    uint16_t in_dilation[1] = {1};
    uint16_t in_kernel[8]   = {0x4000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000}; // {2, 0, 0, 0, 0, 0, 0, 0} in FP16              

    puts("\r\nSetting values of MMIO registers");
    set_conv_params(16, 1, ((uint64_t*) in_kernel));                  
    write_conv_dma(0, 16, in_arr);

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

    uint16_t test_out[32];
    read_conv_dma(0, 16, ((uint64_t*) test_out));
    printf("\r\nTest Output (FP16 binary): ");
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            printf("0x%"PRIx64" ", test_out[i*4 + j]);
        }
    }

    uint16_t ref_out[32];
    for (int i = 0; i < 16; i++) {
        ref_out[i] = f16_from_int((int32_t) (in_arr[i] * 2));
    }
    printf("\r\nReference Output (FP16 binary): ");
    for (int i = 0; i < 16; i++) {
        printf("%#x ", ref_out[i]);
    }
    printf("\r\n");

    if (memcmp(test_out, ref_out, 16) == 0) {
        printf("\r\n[TEST PASSED]: Test Output matches Reference Output.");
        printf("\r\nmcycle: %llu", cpu_end_cycles - cpu_start_cycles);
    } else {
        printf("\r\n[TEST FAILED]: Test Output does not match Reference Output.");
        printf("\r\nmcycle: %llu", cpu_end_cycles - cpu_start_cycles);
    }
    printf("\r\n\r\n");
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
  
  app_main();

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