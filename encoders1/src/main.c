#include <math.h>
#include "main.h"
#include "chip_config.h"


#define ANGLE_ENCODER_PIN_A             0
#define ANGLE_ENCODER_PIN_B             1
#define MOTOR_ENCODER_PIN_A             2
#define MOTOR_ENCODER_PIN_B             3

#define SIZE                            10


int angleStepCount = 0;
uint8_t angleLastEncoded = 0;
int motorPosition = 0;  
uint8_t motorLastEncoded = 0; 
const int size = 10; 
unsigned long timeBuffer[SIZE]; 
long motorPositionBuffer[SIZE]; 
long angleStepCountBuffer[SIZE];
int bufferIndex = 0;
float angularSpeed; 
const float encoderCPR = 8192; 
const float gearRatio = 1; 


void setup() { 
    GPIO_InitType gpio_init_config;
    gpio_init_config.mode = GPIO_MODE_INPUT;
    gpio_init_config.pull = GPIO_PULL_NONE;
    gpio_init_config.drive_strength = GPIO_DS_STRONG;

    gpio_init(GPIOA, &gpio_init_config, GPIO_PIN_0);
    gpio_init(GPIOA, &gpio_init_config, GPIO_PIN_1);
    gpio_init(GPIOA, &gpio_init_config, GPIO_PIN_2);
    gpio_init(GPIOA, &gpio_init_config, GPIO_PIN_3);
    for (int i = 0; i < size; i ++) {
        //timeBuffer[i] = *CLINT_MTIME; CLINT->MTIME
        motorPositionBuffer[i] = 0;
        angleStepCountBuffer[i] = 0;
    }
}

uint8_t readGPIO(int pin) { 
    if (pin == 0) { 
        if(gpio_read_pin(GPIOA, GPIO_PIN_0)==1) {
            return 1;
        } else {
            return 0;
        }
    }
    if (pin == 1) { 
        if(gpio_read_pin(GPIOA, GPIO_PIN_1)==1) {
            return 1;
        } else {
            return 0;
        }
    }
    if (pin == 2) { 
        if(gpio_read_pin(GPIOA, GPIO_PIN_2)==1) {
            return 1;
        } else {
            return 0;
        }
    }
    if (pin == 3) { 
        if(gpio_read_pin(GPIOA, GPIO_PIN_3)==1) {
            return 1;
        } else {
            return 0;
        }
    }

    return 0; 
}

void readAngleEncoder () { 
    uint8_t MSB = readGPIO(ANGLE_ENCODER_PIN_A);
    //printf("msb %d \r\n", MSB);
    //fflush(stdout);
    uint8_t LSB = readGPIO(ANGLE_ENCODER_PIN_B);
    //printf("lsb %d \r\n", LSB);
    //fflush(stdout);
    uint8_t encoded = (MSB << 1) | LSB;
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
    uint8_t MSB = readGPIO(MOTOR_ENCODER_PIN_A);
    //printf("msb %d \r\n", MSB);\
    //fflush(stdout);

    uint8_t LSB = readGPIO(MOTOR_ENCODER_PIN_B);
    //printf("lsb %d \r\n", LSB);
    //fflush(stdout);

    uint8_t encoded = (MSB << 1) | LSB;
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

float pd_controller(float curr_theta, float curr_x, float curr_dtheta, float curr_dx) {
    const float kp_theta = -0.012; 
    // const float kd_theta = -0.015;
    ///const float kd_theta = -0.0012;
    const float kd_theta = -0.0012;
    const float kp_x = 0.000;
    // const float kd_x = -0.01;
    const float kd_x = -0.0080;

    float p_term_theta = kp_theta * (-curr_theta); 
    float d_term_theta = kd_theta * curr_dtheta;

    float p_term_x = kp_x * (-curr_x); 
    float d_term_x = kd_x * curr_dx;

    float control_output_theta = p_term_theta - d_term_theta;
    float control_output_x = p_term_x - d_term_x;

    return control_output_theta + control_output_x;
}
void app_init() {
    UART_InitType UART_init_config;
    UART_init_config.baudrate = 115200;
    UART_init_config.mode = UART_MODE_TX_RX;
    UART_init_config.stopbits = UART_STOPBITS_2;
    uart_init(UART0, &UART_init_config);
}

int main(int argc, char **argv) { 
    // setup();
    app_init();
      
    // const uint8_t obs_dim = 4;
    // const uint8_t control_dim = 1;
    // float control_output; 
    // int oldestIndex; 
    // float deltaTime; 
    while (1) {
        // unsigned long currTime = *CLINT_MTIME; 
                
        // bufferIndex = (bufferIndex + 1) % size; 
        // timeBuffer[bufferIndex] = *CLINT_MTIME;
        // motorPositionBuffer[bufferIndex]  = motorPosition;
        // angleStepCountBuffer[bufferIndex] = angleStepCount; 

        // oldestIndex = (bufferIndex + 1) % size; 
        // deltaTime = (currTime - timeBuffer[oldestIndex]);
        // motorSpeed = (motorPosition - motorPositionBuffer[oldestIndex]) / deltaTime;
        // angularSpeed = (angleStepCount - angleStepCountBuffer[oldestIndex] * (360.0 / encoderCPR / gearRatio / deltaTime));

        // control_output = pd_controller(angleLastEncoded, motorLastEncoded, angularSpeed, motorSpeed);
        // uart_transmit(UART0, &control_output, control_dim * sizeof(float), 200);
        //readMotorEncoder(); 
        //readAngleEncoder(); 
        

        printf("motor encoder is %d \r\n", motorPosition);
        // fflush(stdout); 
        printf("angle encoder is %d \r\n", angleStepCount);
        // fflush(stdout); 
        msleep(1000); 
    }

}