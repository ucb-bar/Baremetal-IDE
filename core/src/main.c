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
//#include "kalman.h"
#define CHUNK_SIZE 16384

#define TEST_SIZE 134217728

#define INTIAL_SEED 0xa121a74464a7132f


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
  //uint8_t counter = 0;
  uint64_t byte_feedback[256] = {

  0x0000000000000000, 0xc70000000000000b, 0x8e0000000000000d, 0x4900000000000006,

  0x1c00000000000001, 0xdb0000000000000a, 0x920000000000000c, 0x5500000000000007,

  0x3800000000000002, 0xff00000000000009, 0xb60000000000000f, 0x7100000000000004,

  0x2400000000000003, 0xe300000000000008, 0xaa0000000000000e, 0x6d00000000000005,

  0x7000000000000004, 0xb70000000000000f, 0xfe00000000000009, 0x3900000000000002,

  0x6c00000000000005, 0xab0000000000000e, 0xe200000000000008, 0x2500000000000003,

  0x4800000000000006, 0x8f0000000000000d, 0xc60000000000000b, 0x0100000000000000,

  0x5400000000000007, 0x930000000000000c, 0xda0000000000000a, 0x1d00000000000001,

  0xe000000000000008, 0x2700000000000003, 0x6e00000000000005, 0xa90000000000000e,

  0xfc00000000000009, 0x3b00000000000002, 0x7200000000000004, 0xb50000000000000f,

  0xd80000000000000a, 0x1f00000000000001, 0x5600000000000007, 0x910000000000000c,

  0xc40000000000000b, 0x0300000000000000, 0x4a00000000000006, 0x8d0000000000000d,

  0x900000000000000c, 0x5700000000000007, 0x1e00000000000001, 0xd90000000000000a,

  0x8c0000000000000d, 0x4b00000000000006, 0x0200000000000000, 0xc50000000000000b,

  0xa80000000000000e, 0x6f00000000000005, 0x2600000000000003, 0xe100000000000008,

  0xb40000000000000f, 0x7300000000000004, 0x3a00000000000002, 0xfd00000000000009,

  0xc00000000000000b, 0x0700000000000000, 0x4e00000000000006, 0x890000000000000d,

  0xdc0000000000000a, 0x1b00000000000001, 0x5200000000000007, 0x950000000000000c,

  0xf800000000000009, 0x3f00000000000002, 0x7600000000000004, 0xb10000000000000f,

  0xe400000000000008, 0x2300000000000003, 0x6a00000000000005, 0xad0000000000000e,

  0xb00000000000000f, 0x7700000000000004, 0x3e00000000000002, 0xf900000000000009,

  0xac0000000000000e, 0x6b00000000000005, 0x2200000000000003, 0xe500000000000008,

  0x880000000000000d, 0x4f00000000000006, 0x0600000000000000, 0xc10000000000000b,

  0x940000000000000c, 0x5300000000000007, 0x1a00000000000001, 0xdd0000000000000a,

  0x2000000000000003, 0xe700000000000008, 0xae0000000000000e, 0x6900000000000005,

  0x3c00000000000002, 0xfb00000000000009, 0xb20000000000000f, 0x7500000000000004,

  0x1800000000000001, 0xdf0000000000000a, 0x960000000000000c, 0x5100000000000007,

  0x0400000000000000, 0xc30000000000000b, 0x8a0000000000000d, 0x4d00000000000006,

  0x5000000000000007, 0x970000000000000c, 0xde0000000000000a, 0x1900000000000001,

  0x4c00000000000006, 0x8b0000000000000d, 0xc20000000000000b, 0x0500000000000000,

  0x6800000000000005, 0xaf0000000000000e, 0xe600000000000008, 0x2100000000000003,

  0x7400000000000004, 0xb30000000000000f, 0xfa00000000000009, 0x3d00000000000002,

  0x800000000000000d, 0x4700000000000006, 0x0e00000000000000, 0xc90000000000000b,

  0x9c0000000000000c, 0x5b00000000000007, 0x1200000000000001, 0xd50000000000000a,

  0xb80000000000000f, 0x7f00000000000004, 0x3600000000000002, 0xf100000000000009,

  0xa40000000000000e, 0x6300000000000005, 0x2a00000000000003, 0xed00000000000008,

  0xf000000000000009, 0x3700000000000002, 0x7e00000000000004, 0xb90000000000000f,

  0xec00000000000008, 0x2b00000000000003, 0x6200000000000005, 0xa50000000000000e,

  0xc80000000000000b, 0x0f00000000000000, 0x4600000000000006, 0x810000000000000d,

  0xd40000000000000a, 0x1300000000000001, 0x5a00000000000007, 0x9d0000000000000c,

  0x6000000000000005, 0xa70000000000000e, 0xee00000000000008, 0x2900000000000003,

  0x7c00000000000004, 0xbb0000000000000f, 0xf200000000000009, 0x3500000000000002,

  0x5800000000000007, 0x9f0000000000000c, 0xd60000000000000a, 0x1100000000000001,

  0x4400000000000006, 0x830000000000000d, 0xca0000000000000b, 0x0d00000000000000,

  0x1000000000000001, 0xd70000000000000a, 0x9e0000000000000c, 0x5900000000000007,

  0x0c00000000000000, 0xcb0000000000000b, 0x820000000000000d, 0x4500000000000006,

  0x2800000000000003, 0xef00000000000008, 0xa60000000000000e, 0x6100000000000005,

  0x3400000000000002, 0xf300000000000009, 0xba0000000000000f, 0x7d00000000000004,

  0x4000000000000006, 0x870000000000000d, 0xce0000000000000b, 0x0900000000000000,

  0x5c00000000000007, 0x9b0000000000000c, 0xd20000000000000a, 0x1500000000000001,

  0x7800000000000004, 0xbf0000000000000f, 0xf600000000000009, 0x3100000000000002,

  0x6400000000000005, 0xa30000000000000e, 0xea00000000000008, 0x2d00000000000003,

  0x3000000000000002, 0xf700000000000009, 0xbe0000000000000f, 0x7900000000000004,

  0x2c00000000000003, 0xeb00000000000008, 0xa20000000000000e, 0x6500000000000005,

  0x0800000000000000, 0xcf0000000000000b, 0x860000000000000d, 0x4100000000000006,

  0x1400000000000001, 0xd30000000000000a, 0x9a0000000000000c, 0x5d00000000000007,

  0xa00000000000000e, 0x6700000000000005, 0x2e00000000000003, 0xe900000000000008,

  0xbc0000000000000f, 0x7b00000000000004, 0x3200000000000002, 0xf500000000000009,

  0x980000000000000c, 0x5f00000000000007, 0x1600000000000001, 0xd10000000000000a,

  0x840000000000000d, 0x4300000000000006, 0x0a00000000000000, 0xcd0000000000000b,

  0xd00000000000000a, 0x1700000000000001, 0x5e00000000000007, 0x990000000000000c,

  0xcc0000000000000b, 0x0b00000000000000, 0x4200000000000006, 0x850000000000000d,

  0xe800000000000008, 0x2f00000000000003, 0x6600000000000005, 0xa10000000000000e,

  0xf400000000000009, 0x3300000000000002, 0x7a00000000000004, 0xbd0000000000000f,

  };
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
  //const char* string = "H";
  const char* string0 = "PLL enabled\n";
  const char* string1 = "PLL configured\n";
  const char* string2 = "PLLCLK0 selected\n";
  const char* string3 = "Core clock debug\n";
  const char* string4 = "PLLCLK0 clock debug\n";
  const char* string5 = "Running DRAM test\n";
  int i;
  int j;
  while (1) {
    HAL_UART_receive(UART0, response, 1, 0);
    //HAL_UART_transmit(UART0, response, 1, 0);
    if (response[0] == 48) {
      PLL->PLLEN = 1;
      PLL->POWERGOOD_VNN = 1;
      HAL_UART_transmit(UART0, string0, 64, 0);
      //printf("PLL enabled.");
    } 
    if (response[0] == 49) {
      PLL->RATIO= 60;
      PLL->FRACTION= 0;
      PLL->MDIV_RATIO= 1;
      PLL->ZDIV0_RATIO= 5;
      PLL->ZDIV0_RATIO_P5= 0;
      PLL->ZDIV1_RATIO= 25;
      PLL->ZDIV1_RATIO_P5= 0;
      PLL->VCODIV_RATIO= 0;
      HAL_UART_transmit(UART0, string1, 64, 0);
      //printf("PLL configured.");
    }
    if (response[0] == 50) {
      RCC->CLK_SEL = 2;
      
      RCC->FBUS_CLK_DIV = 1;
      //UART0->DIV = (2*SYS_CLK_FREQ / UART_init_config.baudrate) - 1;
      HAL_UART_transmit(UART0, string2, 64, 0);
      //printf("PLLCLK0 select.");
    }
    if (response[0] == 51) {
      RCC->DEBUG_CLK_SEL = 3;
      RCC->DEBUG_CLK_DIV = 1000;
      HAL_UART_transmit(UART0, string3, 64, 0);
      //printf("Core clock debug.");
    } 
    if (response[0] == 52) {
      RCC->DEBUG_CLK_SEL = 2;
      RCC->DEBUG_CLK_DIV = 1000;
      HAL_UART_transmit(UART0, string4, 64, 0);
      //printf("PLLCLK0 clock debug.");
    } 
    if (response[0] == 53) {
      HAL_UART_transmit(UART0, string5, 64, 0);
      int err_cnt = 0;
      uint64_t cur_val = INTIAL_SEED;
      uint64_t* test_array = (uint64_t*) DRAM_BASE;
      size_t  first_err_location = -1;
      for (i = 0; i < TEST_SIZE/CHUNK_SIZE; i++) {
        uint64_t cstart_val = cur_val;
        for (j = 0; j < CHUNK_SIZE; j++) {
          test_array[i*CHUNK_SIZE + j] = cur_val;
          cur_val = (cur_val >> 8) ^ byte_feedback[cur_val & 0xff];
        }
        cur_val = cstart_val;
        for (j = 0; j < CHUNK_SIZE; j++) {
          if (cur_val != test_array[i*CHUNK_SIZE + j]) {
            printf("Failed at element %d\n, expected %ld but got %d", i*CHUNK_SIZE + j, cur_val, test_array[i*CHUNK_SIZE + j]);
            err_cnt += 1;
            if (first_err_location == -1)
              first_err_location = i*CHUNK_SIZE + j + DRAM_BASE;
          }
          cur_val = (cur_val >> 8) ^ byte_feedback[cur_val & 0xff];
        }
        printf("Tested chunk: %d\r\n", i);
      }
      while (1) {
        printf("Test Complete, encountered %d errors\r\n", err_cnt);
        if (err_cnt > 0) {
          printf("First error encountered at %lx\r\n", first_err_location);
        }
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
