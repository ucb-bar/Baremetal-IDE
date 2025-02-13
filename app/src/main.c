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
#include "test_dma.h"
#include "chip_config.h"

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

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */


/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN PUC */


void app_init() {
  // torch::executor::runtime_init();
}



void app_main() {
  uint64_t mhartid = READ_CSR("mhartid");


  // Perform basic DMA tests
  if(test_basic() == 1) {
    printf("\n\n-----------------------------------\r\n");
    printf("TEST BASIC FAILED\r\n");
    printf("-----------------------------------\r\n");
    while(1);
  }
  printf("\n\n-----------------------------------\r\n");
  printf("TEST BASIC PASS\r\n");
  printf("-----------------------------------\r\n");

  // Perform stride DMA tests
  if(test_stride() == 1) {
    printf("\n\n-----------------------------------\r\n");
    printf("TEST STRIDE FAILED\r\n");
    printf("-----------------------------------\r\n");
    while(1);
  }

  printf("\n\n-----------------------------------\r\n");
  printf("TEST STRIDE PASS\r\n");
  printf("-----------------------------------\r\n");

  // Perform combined DMA tests
  if(test_combined() == 1) {
    printf("\n\n-----------------------------------\r\n");
    printf("TEST COMBINED FAILED\r\n");
    printf("-----------------------------------\r\n");
    while(1);
  }
  printf("\n\n-----------------------------------\r\n");
  printf("TEST COMBINED PASS\r\n");
  printf("-----------------------------------\r\n");


  // Perform width DMA tests
  if(test_width() == 1) {
    printf("\n\n-----------------------------------\r\n");
    printf("TEST WIDTH FAILED\r\n");
    printf("-----------------------------------\r\n");
    while(1);
  }
  printf("\n\n-----------------------------------\r\n");
  printf("TEST WIDTH PASS\r\n");
  printf("-----------------------------------\r\n");



  printf("\n\n-----------------------------------\r\n");
  printf("   ALL TESTS PASSED\r\n");
  printf("-----------------------------------\r\n");
  while(1);

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
  while (1) {
    app_main();
    return 0;
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