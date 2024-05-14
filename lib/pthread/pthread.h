
#include <sys/types.h>
#include <rv.h>

#define N_HARTS 4

int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(* start_routine)(void *), void *arg);

int pthread_join(pthread_t th, void **thread_return);

pthread_t pthread_self(void);

int pthread_equal(pthread_t t1, pthread_t t2);
