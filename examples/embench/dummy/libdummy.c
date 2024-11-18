#include "support.h"

#define LOCAL_SCALE_FACTOR 1

int
verify_benchmark (int res __attribute ((unused)))
{
  return 1;
}

void
initialise_benchmark (void)
{
}

static int benchmark_body(int rpt);

void warm_caches(int heat) {
  int res = benchmark_body(heat);
  return;
}

int benchmark(void) {
  return benchmark_body(LOCAL_SCALE_FACTOR * CPU_MHZ);
}

static int __attribute__ ((noinline))
benchmark_body (int rpt)
{
  printf("hello world\n");
  return rpt;
}
