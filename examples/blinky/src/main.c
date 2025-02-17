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
#include "gpio.h"

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

GPIO_Type *GPIOA = (GPIO_Type *)GPIOA_BASE;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */


/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN PUC */


void app_init() {
  GPIO_InitType gpio_init_config;
  gpio_init_config.mode = GPIO_MODE_OUTPUT;
  gpio_init_config.pull = GPIO_PULL_NONE;
  gpio_init_config.drive_strength = GPIO_DS_STRONG;

  gpio_init(GPIOA, &gpio_init_config, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);

  
  gpio_write_pin(GPIOA, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, 1);
}



void app_main() {
  // uint64_t mhartid = READ_CSR("mhartid");

  gpio_write_pin(GPIOA, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, 1);

  sleep(1);
  // printf("Hello world from hart %d: %d\n", mhartid, counter);

  
  gpio_write_pin(GPIOA, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, 0);
  sleep(1);
  // printf("Hello world from hart %d: %d\n", mhartid, counter);

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
  app_init();
  /* USER CODE END Init */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1) {
    app_main();
    // return 0;
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