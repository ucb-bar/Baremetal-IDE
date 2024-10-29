#include <stdio.h>
#include <stdint.h>
#include <pthread.h>
#include <rv.h>

#include "marchid.h"

static void sleep(int seconds) {
  for (int i = 0; i < seconds * 1000000; i++) {
    asm volatile ("nop");
  }
}

void hello1() {
  size_t mhartid = READ_CSR("mhartid");
  const char* march = get_machine_architecture(READ_CSR("marchid"));
    
  sleep(mhartid);
  
  printf("Hello world from core %lu, a %s\n", mhartid, march);
}


int main(void) {
  pthread_t threads[N_HARTS];
  
  ssize_t error;

  sleep(1);

  for (size_t i = 0; i < N_HARTS; i += 1) {
    error = pthread_create(&threads[i], NULL, hello1, NULL);
    if (error != 0) {
      printf("Error: %ld\n", error);
    }
  }

  pthread_join(threads[0], NULL);
  pthread_join(threads[1], NULL);
  pthread_join(threads[2], NULL);
  pthread_join(threads[3], NULL);

  printf("exit\n");

  return 0;
}
