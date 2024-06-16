#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "arty.h"

#include "nn.h"


// extern char _binary_test_start[];
// extern char _binary_test_end[];


// http://elm-chan.org/junk/32bit/binclude.html
#define IMPORT_BIN(section, filename, symbol) asm (\
    ".section "#section"\n"                   /* Change section */\
    ".balign 4\n"                             /* Word alignment */\
    ".global "#symbol"\n"                     /* Export the object address */\
    ".global "#symbol"_start\n"               /* Export the object address */\
    #symbol"_start:\n"                         /* Define the object label */\
    #symbol":\n"                              /* Define the object label */\
    ".incbin \""filename"\"\n"                /* Import the file */\
    ".global "#symbol"_end\n"                 /* Export the object address */\
    #symbol"_end:\n"                          /* Define the object label */\
    ".balign 4\n"                             /* Word alignment */\
    ".section \".text\"\n")                   /* Restore section */

IMPORT_BIN(".rodata", "../hack_policy.bin", externdata);



#define N_OBS  795
#define N_ACS  14

#define FC1_SIZE 512
#define FC2_SIZE 256
#define FC3_SIZE 128



/* Declaration of symbols (any type can be used) */
extern uint8_t externdata[];
extern size_t externdata_start[];
extern size_t externdata_end[];




Model model;

void init(Model *model) {
  uint8_t *array_pointer = externdata;

  NN_initTensor(&model->input, 2, (size_t[]){ 1, N_OBS }, DTYPE_F32, (float *)model->input_data);

  NN_initTensor(&model->fc1_weight, 2, (size_t[]){ N_OBS, FC1_SIZE }, DTYPE_F32, (float *)(array_pointer));
  array_pointer += N_OBS * FC1_SIZE * sizeof(float);
  NN_initTensor(&model->fc1_bias, 2, (size_t[]){ 1, FC1_SIZE }, DTYPE_F32, (float *)(array_pointer));
  array_pointer += FC1_SIZE * sizeof(float);
  NN_initTensor(&model->fc1_out, 2, (size_t[]){ 1, FC1_SIZE }, DTYPE_F32, (float *)model->fc1_out_data);
  
  NN_initTensor(&model->fc2_weight, 2, (size_t[]){ FC1_SIZE, FC2_SIZE }, DTYPE_F32, (float *)(array_pointer));
  array_pointer += FC1_SIZE * FC2_SIZE * sizeof(float);
  NN_initTensor(&model->fc2_bias, 2, (size_t[]){ 1, FC2_SIZE }, DTYPE_F32, (float *)(array_pointer));
  array_pointer += FC2_SIZE * sizeof(float);
  NN_initTensor(&model->fc2_out, 2, (size_t[]){ 1, FC2_SIZE }, DTYPE_F32, (float *)model->fc2_out_data);
  
  NN_initTensor(&model->fc3_weight, 2, (size_t[]){ FC2_SIZE, FC3_SIZE }, DTYPE_F32, (float *)(array_pointer));
  array_pointer += FC2_SIZE * FC3_SIZE * sizeof(float);
  NN_initTensor(&model->fc3_bias, 2, (size_t[]){ 1, FC3_SIZE }, DTYPE_F32, (float *)(array_pointer));
  array_pointer += FC3_SIZE * sizeof(float);
  NN_initTensor(&model->fc3_out, 2, (size_t[]){ 1, FC3_SIZE }, DTYPE_F32, (float *)model->fc3_out_data);

  NN_initTensor(&model->fc4_weight, 2, (size_t[]){ FC3_SIZE, N_ACS }, DTYPE_F32, (float *)(array_pointer));
  array_pointer += FC3_SIZE * N_ACS * sizeof(float);
  NN_initTensor(&model->fc4_bias, 2, (size_t[]){ 1, N_ACS }, DTYPE_F32, (float *)(array_pointer));
  array_pointer += N_ACS * sizeof(float);
  NN_initTensor(&model->output, 2, (size_t[]){ 1, N_ACS }, DTYPE_F32, (float *)model->output_data);

  printf("fc4_bias: \n");
  NN_printf(&model->fc4_bias);
}

void forward(Model *model) {
  NN_linear_F32(&model->fc1_out, &model->input, &model->fc1_weight, &model->fc1_bias);
  NN_elu_F32(&model->fc1_out, &model->fc1_out, 1.0);
  NN_linear_F32(&model->fc2_out, &model->fc1_out, &model->fc2_weight, &model->fc2_bias);
  NN_elu_F32(&model->fc2_out, &model->fc2_out, 1.0);
  NN_linear_F32(&model->fc3_out, &model->fc2_out, &model->fc3_weight, &model->fc3_bias);
  NN_elu_F32(&model->fc3_out, &model->fc3_out, 1.0);
  NN_linear_F32(&model->output, &model->fc3_out, &model->fc4_weight, &model->fc4_bias);
}

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

  setvbuf(stdout, NULL, _IONBF, 0);

  init(&model);
}

void APP_main() {
  printf("loop\n");

  for (int i = 0; i < N_OBS; i++) {
    ((float *)model.input.data)[i] = 0.01;
  }
  
  forward(&model);

  NN_printf(&model.output);

  msleep(1000);
}


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
