#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "fe310.h"
#include "nn.h"

// load the weight data block from the model.bin file
INCLUDE_FILE(".rodata", "../model.bin", weights);
extern uint8_t weights_data[];
extern size_t weights_start[];
extern size_t weights_end[];


// Tensors can be defined either globally or locally
Tensor A;
Tensor B;
Tensor C;
Tensor D;

/**
 * Initialize the required tensors for the model
 */
void init(Tensor *A, Tensor *B, Tensor *C, Tensor *D) {
  NN_initTensor(A, 2, (size_t[]){3, 3}, DTYPE_F32, (float *)malloc(9 * sizeof(float)));
  NN_initTensor(B, 2, (size_t[]){3, 3}, DTYPE_F32, (float *)(weights_data + 3 * sizeof(float)));
  NN_initTensor(C, 2, (size_t[]){3, 3}, DTYPE_F32, (float *)malloc(9 * sizeof(float)));
  NN_initTensor(D, 1, (size_t[]){3}, DTYPE_F32, (float *)(weights_data + 0 * sizeof(float)));
}

/**
 * Deinitialize the tensors used for the model
 */
void deinit(Tensor *A, Tensor *B, Tensor *C, Tensor *D) {
  NN_deinitTensor(A);
  // NN_deinitTensor(B);
  NN_deinitTensor(C);
  // NN_deinitTensor(D);
}

/**
 * Forward pass of the model
 */
void forward(Tensor *C, Tensor *A, Tensor *B, Tensor *D) {
  NN_linear_F32(C, A, B, D);
}


void APP_init();
void APP_main();

uint8_t counter = 0;

ssize_t _write(int fd, const void *ptr, size_t len) {
  UART_transmit(UART0, (const uint8_t *)ptr, len, 100);
  return -1;
}


void APP_init() {
  // set up UART registers
  UART_InitType UART_init_config;
  UART_init_config.baudrate = 115200;
  UART_init_config.mode = UART_MODE_TX_RX;
  UART_init_config.stopbits = UART_STOPBITS_2;
  UART_init(UART0, &UART_init_config);

  GPIO_InitType GPIO_init_config;
  GPIO_init_config.mode = GPIO_MODE_OUTPUT;
  GPIO_init(GPIOA, &GPIO_init_config, GPIO_PIN_5);
  
  GPIO_init_config.mode = GPIO_MODE_ALTERNATE_FUNCTION_0;
  GPIO_init(GPIOA, &GPIO_init_config, GPIO_PIN_16 | GPIO_PIN_17);

}

void APP_main() {
  

  init(&A, &B, &C, &D);

  // load the input data to the tensor
  float input_data[] = {
    1., 2., 3.,
    1., 2., 3.,
    1., 2., 3.,
  };
  memcpy(A.data, input_data, 9 * sizeof(float));
  
  forward(&C, &A, &B, &D);

  printf("A:\n");
  NN_printf(&A);

  printf("B:\n");
  NN_printf(&B);
  
  printf("C:\n");
  NN_printf(&C);
  
  printf("D:\n");
  NN_printf(&D);
  
  deinit(&A, &B, &C, &D);
}

int main(void) {
  APP_init();
  while (1) {
    APP_main();
  }
  return 0;
}
