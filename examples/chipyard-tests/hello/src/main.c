#include <stdio.h>
#include <stdint.h>

#include "rv.h"
#include "marchid.h"

int main(void) {
  uint64_t marchid = READ_CSR("marchid");
  const char* march = getMachineArchitecture(marchid);
  printf("Hello world from core 0, a %s\n", march);
  return 0;
}
