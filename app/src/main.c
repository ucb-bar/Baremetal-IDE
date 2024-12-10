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
#define ANGLE_ENCODER_PIN_A 3
#define ANGLE_ENCODER_PIN_B 2
#define MOTOR_ENCODER_PIN_A 1
#define MOTOR_ENCODER_PIN_B 0

#define SIZE      10
#define MAX_SPEED 50

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
const float gearRatio = 1.0;
volatile float ang = 0.0;
uint64_t curr_time = 0;
uint64_t last_time = 0;
volatile float dt = 0.0;
volatile int last_x = 0;
volatile float last_theta = 0.0;
volatile float x_dot;
volatile float theta_dot;
volatile float target;
volatile float motor_speed = 0;
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
  for (int i = 0; i < size; i++) {
    // timeBuffer[i] = *CLINT_MTIME; CLINT->MTIME
    motorPositionBuffer[i] = 0;
    angleStepCountBuffer[i] = 0;
  }
  pwm_enable(PWM0_BASE);
  pwm_set_frequency(PWM0_BASE, 0, 444);
  pwm_get_frequency(PWM0_BASE, 0);
  //pwm_set_duty_cycle(PWM0_BASE, 0, 1, 1000, 0);
  pwm_set_duty_cycle(PWM0_BASE, 1, 50, 1000, 0);
  
}

uint8_t readGPIO(int pin) {
  if (pin == 0) {
    if (gpio_read_pin(GPIOC, GPIO_PIN_0) == 1) {
      return 1;
    } else {
      return 0;
    }
  }
  if (pin == 1) {
    if (gpio_read_pin(GPIOC, GPIO_PIN_4) == 1) {
      return 1;
    } else {
      return 0;
    }
  }
  if (pin == 2) {
    if (gpio_read_pin(GPIOC, GPIO_PIN_2) == 1) {
      return 1;
    } else {
      return 0;
    }
  }
  if (pin == 3) {
    if (gpio_read_pin(GPIOC, GPIO_PIN_3) == 1) {
      return 1;
    } else {
      return 0;
    }
  }
  return 0;
}

void readAngleEncoder() {
  volatile uint8_t pinA = readGPIO(ANGLE_ENCODER_PIN_A);
  // printf("Angle pinA %d \r\n", pinA);
  // fflush(stdout);
  volatile uint8_t pinB = readGPIO(ANGLE_ENCODER_PIN_B);
  // printf("Angle pinB %d \r\n", pinB);
  // fflush(stdout);
  uint8_t encoded = (pinA << 1) | pinB;
  uint8_t sum = (angleLastEncoded << 2) | encoded;

  if (sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) {
    angleStepCount++;
  }
  if (sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) {
    angleStepCount--;
  }
  angleLastEncoded = encoded;
}

void readMotorEncoder() {
  volatile uint8_t pinA = readGPIO(MOTOR_ENCODER_PIN_A);
  //printf("Motor pinA %d \r\n", pinA);
  // fflush(stdout);

  volatile uint8_t pinB = readGPIO(MOTOR_ENCODER_PIN_B);
  //printf("Motor pinB %d \r\n", pinB);
  // fflush(stdout);

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

float calculateAngle(int stepCount) {
  //printf("angle encoder is %d \r\n", stepCount);
  float rawAngle = ((float) stepCount / encoderCPR) * (float) (360.0 / gearRatio);
  //printf("raw angle is %9.4f \r\n", rawAngle);
  rawAngle = (float) fmod(rawAngle, 360.0);
  if (rawAngle < 0)
    rawAngle += 360.0;
  // Adjust the angle so that the downward position is 180 degrees
  float adjustedAngle = rawAngle + 180.0;
  if (adjustedAngle >= 360.0)
    adjustedAngle -= 360.0;
  // Convert to -180 to 180 range
  if (adjustedAngle > 180.0)
    adjustedAngle -= 360.0;

  return adjustedAngle;
}

void update_state() {
  curr_time = CLINT->MTIME;
  //printf("curr_time:%lld, last_time:%lld, dt:%lld, freq:%d \r\n", curr_time, last_time, (curr_time-last_time), SYS_CLK_FREQ);

  dt = (float) (curr_time - last_time) / MTIME_FREQ;
  //printf("dt:%7.6f \r\n", dt);
  x_dot = ((motorPosition - last_x) / (dt));
  theta_dot = ((ang - last_theta) / (dt));

  //printf("x:%f, theta:%f, dx:%f, dtheta:%f\n", motorPosition, ang, x_dot, theta_dot);
  last_x = motorPosition;
  last_theta = ang;
  last_time = curr_time;
}

float pd_controller(float curr_theta, float curr_x, float curr_dtheta,
                    float curr_dx) {
  const float kp_theta = -0.012;
  // const float kd_theta = -0.015;
  const float kd_theta = -0.0012;
  const float kp_x = 0.0024;
  // const float kd_x = -0.01;
  const float kd_x = 0.0008;

  float p_term_theta = kp_theta * (-curr_theta);
  float d_term_theta = kd_theta * curr_dtheta;

  float p_term_x = kp_x * (-curr_x);
  float d_term_x = kd_x * curr_dx;

  float control_output_theta = p_term_theta - d_term_theta;
  float control_output_x = p_term_x - d_term_x;

  return control_output_theta + control_output_x;
}

void set_motor(float speed) {
  speed = speed > MAX_SPEED ? MAX_SPEED : speed;
  speed = speed < -MAX_SPEED ? -MAX_SPEED : speed;
  //pwm_set_duty_cycle(PWM0_BASE, 0, speed, 0);
}

void app_main() {
  last_time = CLINT->MTIME;
  while (1) {
    readMotorEncoder();
    readAngleEncoder();
    counter++;
    ang = calculateAngle(angleStepCount);
    update_state();
    float fmotorPosition = (float) motorPosition;
    target = pd_controller(ang, fmotorPosition, theta_dot, x_dot);
    motor_speed = motor_speed + dt * target;

    if ((ang < 45 && ang > -45) &&
        (motorPosition < 23 && motorPosition > -23)) {
      //set_motor(motor_speed);
    } else {
      //set_motor(0);
    }

    if (counter == 10000) { //5000
      printf("motor encoder is %d \r\n", motorPosition);
      printf("angle encoder is %d \r\n", angleStepCount);
      printf("angle is %7.4f \r\n", ang);
      printf("pid target is %4.2f \r\n", target);
      printf("dt is %8.7f \r\n", dt);
      printf("motor speed is %7.4f \r\n", motor_speed);
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

  // Initialize PWM0 for Motor Control
  PWM_InitType PWM_init_config;
  PWM_init_config.pwmscale = 0;
  PWM_init_config.RESERVED = 0;
  PWM_init_config.pwmsticky = 0;
  PWM_init_config.pwmzerocmp = 0;
  PWM_init_config.pwmdeglitch = 0;
  PWM_init_config.RESERVED1 = 0;
  PWM_init_config.pwmenalways = 0;
  PWM_init_config.pwmenoneshot = 0;
  PWM_init_config.RESERVED2 = 0;
  PWM_init_config.pwmcmp0center = 0;
  PWM_init_config.pwmcmp1center = 0;
  PWM_init_config.pwmcmp2center = 0;
  PWM_init_config.pwmcmp3center = 0;
  PWM_init_config.RESERVED3 = 0;
  PWM_init_config.pwmcmp0gang = 0;
  PWM_init_config.pwmcmp1gang = 0;
  PWM_init_config.pwmcmp2gang = 0;
  PWM_init_config.pwmcmp3gang = 0;
  PWM_init_config.pwmcmp0ip = 0;
  PWM_init_config.pwmcmp1ip = 0;
  PWM_init_config.pwmcmp2ip = 0;
  PWM_init_config.pwmcmp3ip = 0;
  pwm_init(PWM0_BASE, &PWM_init_config);
  *((uint32_t*) (PWM0_BASE+0x08)) = 0;
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  /* USER CODE BEGIN Init */
  app_init();
  /* USER CODE END Init */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1) {
    app_main();
  }
  return 0;
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
    asm volatile("wfi");
  }
}