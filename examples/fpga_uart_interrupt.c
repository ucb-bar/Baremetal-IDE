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
const char* get_march(size_t marchid) {
  switch (marchid) {
  case 1:
    return "rocket";
  case 2:
    return "sonicboom";
  case 5:
    return "spike";
  default:
    return "unknown";
  }
}

void HAL_UART0_Callback() {
  volatile a = UART0->RXDATA;
  printf("UART irq: %c\n", a);
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(int argc, char **argv) {
  /* USER CODE BEGIN 1 */
  
	/* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/
  
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  /* USER CODE BEGIN 2 */

  GPIO_InitTypeDef GPIO_init_config;
  GPIO_init_config.mode = GPIO_MODE_OUTPUT;
  GPIO_init_config.pull = GPIO_PULL_NONE;
  GPIO_init_config.drive_strength = GPIO_DS_STRONG;
  HAL_GPIO_init(GPIOA, &GPIO_init_config, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);

  UART_InitTypeDef UART_init_config;
  UART_init_config.baudrate = 115200;
  UART_init_config.mode = UART_MODE_TX_RX;
  UART_init_config.stopbits = UART_STOPBITS_2;
  HAL_UART_init(UART0, &UART_init_config);

  HAL_GPIO_writePin(GPIOA, GPIO_PIN_2, 0);

  char str[128];
  uint8_t counter = 0;


  UART0->IE = UART_IE_RXWM_MSK;
  // CLEAR_BITS(UART0->TXCTRL, UART_TXCTRL_TXCNT_MSK);
  // SET_BITS(UART0->TXCTRL, 1 << UART_TXCTRL_TXCNT_POS);

  CLEAR_BITS(UART0->RXCTRL, UART_RXCTRL_RXCNT_MSK);
  SET_BITS(UART0->RXCTRL, 0 << UART_RXCTRL_RXCNT_POS);

  HAL_CORE_enableGlobalInterrupt();
  HAL_CORE_enableInterrupt(MachineSoftwareInterrupt);
  HAL_CORE_enableInterrupt(MachineExternalInterrupt);

  HAL_PLIC_enable(0, UART0_IRQn);
  HAL_PLIC_setPriority(UART0_IRQn, 1);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1) {

    printf("%d\tmie: %x   mip: %x   \n", counter, READ_CSR("mie"), READ_CSR("mip"));
    printf("\tPLIC en %x, pendings %x\n", PLIC->enables[0], PLIC->pendings[0]);
    printf("\tUARTie %x, UARTip %x\n", UART0->IE, UART0->IP);

    HAL_GPIO_writePin(GPIOA, GPIO_PIN_2, 1);
    HAL_delay(100);

    HAL_GPIO_writePin(GPIOA, GPIO_PIN_2, 0);
    HAL_delay(100);

    
    
    counter += 1;
		/* USER CODE END WHILE */
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
