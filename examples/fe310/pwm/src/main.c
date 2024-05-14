#include <stdio.h>
#include <stdint.h>
#include <time.h>

#include "fe310.h"

void APP_init();
void APP_main();

uint8_t counter = 0;


void APP_init() {
}

void APP_main() {
  
  usleep(200000);
  // sleep(1);
}

int main(void) {
  APP_init();
  while (1) {
    APP_main();
  }
  return 0;
}
