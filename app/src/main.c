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

// #define FLOAT true // for FP Gemmini

#include "include/gemmini_testutils.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

uint8_t counter = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

#define CLOCK_GATE false 
#define BASE_ADDR 0x70000000L
#define PageSize 4096

#define REPEAT 4
#define NUM_EN_ARRAY 1

#define NO_BIAS 1
#define FULL_BIAS_WIDTH 1

#if FULL_BIAS_WIDTH
typedef acc_t ACC_T;
#else
typedef elem_t ACC_T;
#endif

#define NUM_INT 8
#define NUM_FP 5

#define MAT_DIM_I FLOAT ? (8*DIM) : (4*DIM)
#define MAT_DIM_J FLOAT ? (8*DIM) : (4*DIM)
#define MAT_DIM_K (16*DIM)
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN PUC */
ssize_t _write(int fd, const void *ptr, size_t len) {
  UART_transmit(UART0, (const uint8_t *)ptr, len, 100);
  return len;
}

void APP_init() {
  // set up UART registers
  UART_InitTypeDef UART_init_config;
  UART_init_config.baudrate = 115200;
  UART_init_config.mode = UART_MODE_TX_RX;
  UART_init_config.stopbits = UART_STOPBITS_2;
  UART_init(UART0, &UART_init_config);

  // GPIO_InitTypeDef GPIO_init_config;
  // GPIO_init_config.mode = GPIO_MODE_OUTPUT;
  // GPIO_init(GPIOA, &GPIO_init_config, GPIO_PIN_5);
  
  // GPIO_init_config.mode = GPIO_MODE_ALTERNATE_FUNCTION_0;
  // GPIO_init(GPIOA, &GPIO_init_config, GPIO_PIN_16 | GPIO_PIN_17);
}



void APP_main() {
    printf("hello\n");

  

    for(int i = 0; i < NUM_INT+NUM_FP; i++){
        int cfgid = i;
        bool acquired = false;
        while(!acquired) {
          acquired = rr_acquire_single(cfgid, i);
        }
        //if(acquired){
            //printf("int gemmini %d acquired to cfgid %d\n", i, cfgid);
            //break;
        //}
    }
    for(int i = 0; i < NUM_INT+NUM_FP; i++){
      rr_set_opc(XCUSTOM_ACC, i);
      //gemmini_flush(0);
      gemmini_clk_gate(0); // disable gemmini clock
    }
    for(int i = 0; i < NUM_INT+NUM_FP; i++)
      rr_release(i);
    printf("all gemmini clk gated\n");

    // this seems max tile 
    int tile_I = FLOAT ? 4 : 8;
    int tile_J = FLOAT ? 4 : 8;
    int tile_K = 16;
    int dim_I = DIM * tile_I;
    int dim_J = DIM * tile_J;
    int dim_K = DIM * tile_K;
    int tile_macs = dim_I * dim_J * dim_K;

    for(int i = 0; i < NUM_EN_ARRAY; i++){
        int cfgid = i;
        int acc_id = FLOAT ? NUM_INT + i : i;
        bool acquired = rr_acquire_single(cfgid, acc_id);
        if(acquired){
            printf("gemmini %d acquired to cfgid %d\n", acc_id, cfgid);
            //break;
        }
        rr_set_opc(XCUSTOM_ACC, cfgid); 
        gemmini_clk_gate(1); // enable clock
        gemmini_flush(0);
    }

    printf("Starting gemmini matmul\n");
    unsigned long start = read_cycles();

    for(int turn = 0; turn < REPEAT; turn++)
      for(int i = 0; i < NUM_EN_ARRAY; i++){
        rr_set_opc(XCUSTOM_ACC, i);
        tiled_matmul_small(dim_I, dim_J, dim_K,
            NULL, NULL, NULL, NULL,
            dim_K, dim_J, dim_J, dim_J,
            MVIN_SCALE_IDENTITY, MVIN_SCALE_IDENTITY, MVIN_SCALE_IDENTITY,
            tile_I, tile_J, tile_K,
            NO_ACTIVATION, ACC_SCALE_IDENTITY, false,
            false, false,
            false, !FULL_BIAS_WIDTH,
            turn == 0);
      }
    for (int i = 0; i < NUM_EN_ARRAY; i++)
      rr_fence(i);
    unsigned long end = read_cycles();
    printf("Cycles taken: %u\n", end-start);

    const uint64_t total_macs = tile_macs * REPEAT * NUM_EN_ARRAY;
    const uint64_t ideal_cycles = total_macs / (DIM * DIM * NUM_EN_ARRAY);
    const uint64_t utilization = 100 * ideal_cycles / (end-start);
    printf("Total macs: %llu\n", total_macs);
    printf("Ideal cycles: %llu\n", ideal_cycles);
    printf("Utilization: %llu%%\n", utilization);

    for (int i = 0; i < NUM_EN_ARRAY; i++)
      rr_release(i);


  // sleep(10);
}
/* USER CODE END PUC */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(int argc, char **argv) {
  /* MCU Configuration--------------------------------------------------------*/

  /* Configure the system clock */
  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */  
  /* USER CODE BEGIN Init */
  APP_init();
  /* USER CODE END Init */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1) {
    APP_main();
  }
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