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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "mte.h"
#include "mte_addti.h"
#include "mte_add.h"
#include "mte_basic_load_store.h"
#include "mte_csrs.h"
#include "mte_dbg.h"
#include "mte_irt.h"

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
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(int argc, char **argv) {
  /* USER CODE BEGIN 1 */
  uint8_t counter = 0;
  
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/
  
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  /* USER CODE BEGIN 2 */

  // set up UART registers
  UART_InitTypeDef UART_init_config;
  UART_init_config.baudrate = 115200;
  UART_init_config.mode = UART_MODE_TX_RX;
  UART_init_config.stopbits = UART_STOPBITS_2;
  HAL_UART_init(UART0, &UART_init_config);

    // CLEAR_BITS(UART0->RXCTRL, UART_RXCTRL_RXEN_MSK);
    // CLEAR_BITS(UART0->TXCTRL, UART_TXCTRL_TXEN_MSK);

    // SET_BITS(UART0->RXCTRL, UART_RXCTRL_RXEN_MSK);
    // SET_BITS(UART0->TXCTRL, UART_TXCTRL_TXEN_MSK);

    // CLEAR_BITS(UART0->TXCTRL, UART_TXCTRL_NSTOP_MSK);
    // CLEAR_BITS(UART0->TXCTRL, UART_STOPBITS_2);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  uint64_t mhartid = READ_CSR("mhartid");


  for (int counter = 0; counter < 100; counter++) {
    printf("Hello world from hart %d\n", mhartid);
  }

  printf("\n\n\n", mhartid);
 
  while (1) {
  }
}

/*
 * Main function for secondary harts
 * 
 * Multi-threaded programs should provide their own implementation.
 */
void __attribute__((weak, noreturn)) __main(void) {

  if (READ_CSR("mhartid") == 1) {
      printf("Hart 1 does not have MTE!");
      while(1) {
      }
  }

  printf("Running MTE add test...\n");
  run_mte_add();
  printf("Running MTE addti test...\n");
  run_mte_addti();
  printf("Running MTE basic load store test...\n");
  run_mte_basic_load_store();
  printf("Running MTE CSR test...\n");
  run_mte_csrs();
  printf("Running MTE Debug test...\n");
  run_mte_dbg();
  printf("Running MTE IRT test...\n");
  run_mte_irt();


  while (1) {
  }

}
