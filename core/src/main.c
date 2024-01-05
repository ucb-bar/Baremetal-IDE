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
  // I2C_InitTypeDef I2C_init_config;
  // //I2C_init_config.clock = ;
  // HAL_I2C_init(I2C0, &I2C_init_config);
  // HAL_I2C_masterTransmit(I2C0, )
  // set up GPIO registers
  // GPIO_InitTypeDef GPIO_init_config;
  // GPIO_init_config.mode = GPIO_MODE_OUTPUT;
  // GPIO_init_config.pull = GPIO_PULL_NONE;
  // GPIO_init_config.drive_strength = GPIO_DS_STRONG;
  // HAL_GPIO_init(GPIOA, &GPIO_init_config, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);

  // // set up UART registers
  UART_InitTypeDef UART_init_config;
  UART_init_config.baudrate = 115200;
  UART_init_config.mode = UART_MODE_TX_RX;
  UART_init_config.stopbits = UART_STOPBITS_2;
  HAL_UART_init(UART0, &UART_init_config);

  /* USER CODE END 2 */
  
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  uint8_t response[12];
  const char* string = "Hello World";
  int i;
  while (1) {
    //UART0->TXCTRL = 1;
    //UART0->RXCTRL = 1;

    HAL_UART_receive(UART0, response, 1, 0);
    //CLK_REF = 50 MHz
    if (response[0] == 48) { //(0 -> ASIIC = 48)
      PLL->PLLEN = 1;
      PLL->POWERGOOD_VNN = 1;
      printf("PLL_CLK enabled.\n");
    }
    // 400 MHz
    if (response[0] == 49) {
      PLL->RATIO= 50;
      PLL->FRACTION= 100000;
      PLL->MDIV_RATIO= 1;
      PLL->ZDIV0_RATIO= 2.5;
      PLL->ZDIV0_RATIO_P5= 5;
      PLL->ZDIV1_RATIO= 25;
      PLL->ZDIV1_RATIO_P5= 0;
      PLL->VCODIV_RATIO= 0;
      printf("Setting the config bits of PLL.\n");
    }
    // 500 MHz
    // if (response[0] == 49) {
    //   PLL->RATIO= 70;
    //   PLL->FRACTION= 0;
    //   PLL->MDIV_RATIO= 1;
    //   PLL->ZDIV0_RATIO= 2;
    //   PLL->ZDIV0_RATIO_P5= 0;
    //   PLL->ZDIV1_RATIO= 25;
    //   PLL->ZDIV1_RATIO_P5= 0;
    //   PLL->VCODIV_RATIO= 0;
    //   printf("Setting the config bits of PLL.\n");
    // }
    // 200 MHz
    // if (response[0] == 49) {
    //   PLL->RATIO= 60;
    //   PLL->FRACTION= 0;
    //   PLL->MDIV_RATIO= 1;
    //   PLL->ZDIV0_RATIO= 5;
    //   PLL->ZDIV0_RATIO_P5= 0;
    //   PLL->ZDIV1_RATIO= 25;
    //   PLL->ZDIV1_RATIO_P5= 0;
    //   PLL->VCODIV_RATIO= 0;
    //   printf("Setting the config bits of PLL.\n");
    // }
    if (response[0] == 50) {
      RCC->DEBUG_CLK_SEL = 2;
      RCC->DEBUG_CLK_DIV = 1000;
      printf("PLL_CLK0\n");
    }

    if (response[0] == 51) {
      printf("Setting internal clock select to PLL.");
      RCC->CLK_SEL = 2;
      UART0->DIV = (4*SYS_CLK_FREQ / UART_init_config.baudrate) - 1;
      for (i =0; i<=1000; i=i+1) {
        HAL_UART_transmit(UART0, (uint8_t*)string, strlen(string), 0);
      }
    }

    if (response[0] == 52) {
      printf("Setting internal clock select to external clock.");
      RCC->CLK_SEL = 0;
      UART0->DIV = (SYS_CLK_FREQ / UART_init_config.baudrate) - 1;
      for (i =0; i<=1000; i=i+1) {
        HAL_UART_transmit(UART0, (uint8_t*)string, strlen(string), 0);
      }
    }
  }



  /* USER CODE BEGIN 3 */

  /* USER CODE END 3 */
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
