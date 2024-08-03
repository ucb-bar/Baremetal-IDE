
#include "pthread.h"


typedef struct {
  volatile uint32_t flags[N_HARTS];
  pthread_t *threads[N_HARTS];
  void *(*start_routines[N_HARTS])(void *);
  void *args[N_HARTS];
} shared_data_t;

static volatile shared_data_t shared_data;


int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(* start_routine)(void *), void *arg) {
  // find available hart id
  ssize_t hid = -1;
  for (size_t i = 1; i < N_HARTS; i += 1) {
    if (shared_data.flags[i] == 0) {
      hid = i;
      break;
    }
  }
  
  if (hid == -1) {
    return -1;
  }

  shared_data.threads[hid] = thread;
  shared_data.start_routines[hid] = start_routine;
  shared_data.args[hid] = arg;
  
  // set the flag at last
  shared_data.flags[hid] = 1;

  *thread = hid;

  return 0;
}

int pthread_join(pthread_t th, void **thread_return) {
  while (shared_data.flags[th] == 1) {
    asm volatile ("nop");
  }

  return 0;
}

pthread_t pthread_self(void) {
  size_t mhartid = READ_CSR("mhartid");
  return mhartid;
}

int pthread_equal(pthread_t t1, pthread_t t2) {
  return t1 == t2;
}

void __main(void) {
  volatile size_t mhartid = READ_CSR("mhartid");

  while (1) {
    while (shared_data.flags[mhartid] == 0) {
      asm volatile ("nop");
    }
    
    shared_data.start_routines[mhartid](shared_data.args[mhartid]);
    // hello1();
    
    shared_data.flags[mhartid] = 0;
  }
}
