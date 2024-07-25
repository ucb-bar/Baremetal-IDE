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

#define FLOAT false
#define NUM_INT 8
#define NUM_FP 5

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

  UART0->DIV = 139;
}



void APP_main() {
  uint64_t mhartid = READ_CSR("mhartid");



    for(int i = 0; i < NUM_INT; i++){
        int cfgid = i;
        bool acquired = rr_acquire_single(cfgid, i);
        if(acquired){
            printf("int gemmini %d acquired to cfgid %d\n", i, cfgid);
            
        }
    }
    
  printf("Hello world from hart %d: %d\n", mhartid, counter);

    for(int i = 0; i < NUM_INT; i++){
      rr_set_opc(XCUSTOM_ACC, i);
      gemmini_flush(0);
    }
    for(int i = 0; i < NUM_INT; i++)
      rr_release(i);
    printf("all int gemmini flushed\n");


    for(int i = 0; i < NUM_FP; i++){
        int cfgid = i;
        bool acquired = rr_acquire_single(cfgid, i+NUM_INT);
        if(acquired){
            printf("fp gemmini %d acquired to cfgid %d\n", i+NUM_INT, cfgid);
            //break;
        }
    }
    for(int i = 0; i < NUM_FP; i++){
      rr_set_opc(XCUSTOM_ACC, i);
      gemmini_flush(0);
    }
    for(int i = 0; i < NUM_FP; i++)
      rr_release(i);
    printf("all fp gemmini flushed\n");




  usleep(200000);
  // sleep(1);
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