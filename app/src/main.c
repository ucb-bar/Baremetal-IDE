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
#include <math.h>

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
#define ANGLE_ENCODER_PIN_A             0
#define ANGLE_ENCODER_PIN_B             1
#define MOTOR_ENCODER_PIN_A             2
#define MOTOR_ENCODER_PIN_B             3

#define SIZE                            10


volatile int angleStepCount = 0;
volatile uint8_t angleLastEncoded = 0;
volatile int motorPosition = 0;  
volatile uint8_t motorLastEncoded = 0;
int counter = 0; 
const int size = 10; 
unsigned long timeBuffer[SIZE]; 
long motorPositionBuffer[SIZE]; 
long angleStepCountBuffer[SIZE];
int bufferIndex = 0;
float angularSpeed; 
const float encoderCPR = 8192; 
const float gearRatio = 1;
volatile float ang = 0.0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */


/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN PUC */


void app_init() {
  GPIO_InitType gpio_init_config;
  gpio_init_config.mode = GPIO_MODE_INPUT;
  gpio_init_config.pull = GPIO_PULL_NONE;
  gpio_init_config.drive_strength = GPIO_DS_STRONG;

  gpio_init(GPIOC, &gpio_init_config, GPIO_PIN_0);
  gpio_init(GPIOC, &gpio_init_config, GPIO_PIN_2);
  gpio_init(GPIOC, &gpio_init_config, GPIO_PIN_3);
  gpio_init(GPIOC, &gpio_init_config, GPIO_PIN_4);
  for (int i = 0; i < size; i ++) {
      //timeBuffer[i] = *CLINT_MTIME; CLINT->MTIME
      motorPositionBuffer[i] = 0;
      angleStepCountBuffer[i] = 0;
  }
}

uint8_t readGPIO(int pin) { 
    if (pin == 0) { 
        if(gpio_read_pin(GPIOC, GPIO_PIN_0)==1) {
            return 1;
        } else {
            return 0;
        }
    }
    if (pin == 1) { 
        if(gpio_read_pin(GPIOC, GPIO_PIN_4)==1) {
            return 1;
        } else {
            return 0;
        }
    }
    if (pin == 2) { 
        if(gpio_read_pin(GPIOC, GPIO_PIN_2)==1) {
            return 1;
        } else {
            return 0;
        }
    }
    if (pin == 3) { 
        if(gpio_read_pin(GPIOC, GPIO_PIN_3)==1) {
            return 1;
        } else {
            return 0;
        }
    }

    return 0; 
}

void readAngleEncoder () { 
    volatile uint8_t pinA = readGPIO(ANGLE_ENCODER_PIN_A);
    //printf("Angle pinA %d \r\n", pinA);
    //fflush(stdout);
    volatile uint8_t pinB = readGPIO(ANGLE_ENCODER_PIN_B);
    //printf("Angle pinB %d \r\n", pinB);
    //fflush(stdout);
    uint8_t encoded = (pinA << 1) | pinB;
    uint8_t sum = (angleLastEncoded << 2) | encoded;

    if (sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) {
        angleStepCount++;
    }
    if (sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) {
        angleStepCount--;
    }
    // if (angleLastEncoded[0] != pinA || angleLastEncoded[1] != pinB) { 
    //     if (angleLastEncoded[1] == pinA){
    //         if ()
    //     }
    // }
    angleLastEncoded = encoded;
}

void readMotorEncoder() {
    volatile uint8_t pinA = readGPIO(MOTOR_ENCODER_PIN_A);
    //printf("Motor pinA %d \r\n", pinA);
    //fflush(stdout);

   volatile uint8_t pinB = readGPIO(MOTOR_ENCODER_PIN_B);
    //printf("Motor pinB %d \r\n", pinB);
    //fflush(stdout);

    uint8_t encoded = (pinA << 1) | pinB;
    uint8_t sum = (motorLastEncoded << 2) | encoded;

    if (sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) {
        motorPosition++;
    }
    if (sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) {
        motorPosition--;
    }
    motorLastEncoded = encoded;
}

float calculateAngle(int stepCount){ 
    float rawAngle = (stepCount / encoderCPR) * 360.0 / gearRatio;
    rawAngle = fmod(rawAngle, 360.0); // need to confirm this 
    if (rawAngle < 0) rawAngle += 360.0; 
    // Adjust the angle so that the downward position is 180 degrees
    float adjustedAngle = rawAngle + 180.0;
    if (adjustedAngle >= 360.0) adjustedAngle -= 360.0;
    // Convert to -180 to 180 range
    if (adjustedAngle > 180.0) adjustedAngle -= 360.0;
    
    return adjustedAngle;
}

void app_main() {
  

  while(1) {
    readMotorEncoder();
    readAngleEncoder();
    counter++;
    if(counter==10000) {
        ang = calculateAngle(angleStepCount);
        printf("motor encoder is %d \r\n", motorPosition);
        printf("angle encoder is %d \r\n", angleStepCount);
        printf("angle is %7.4f \r\n", ang);
        counter = 0;
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
  /* USER CODE BEGIN SysInit */
  // Initialize UART0 for Serial Monitor
  UART_InitType UART0_init_config;
  UART0_init_config.baudrate = 115200;
  UART0_init_config.mode = UART_MODE_TX_RX;
  UART0_init_config.stopbits = UART_STOPBITS_2;
  uart_init(UART0, &UART0_init_config);

  // Initialize UART1 for Motor Control
  UART_InitType UART1_init_config;
  UART1_init_config.baudrate = 115200;
  UART1_init_config.mode = UART_MODE_TX_RX;
  UART1_init_config.stopbits = UART_STOPBITS_2;
  uart_init(UART1, &UART1_init_config);
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
    uint64_t mhartid = READ_CSR("mhartid");
        while (1) {
            asm volatile ("wfi");
        }
}