#include "llvm_cov.h"

char* llvm_profile_runtime_init(void) {
  printf("llvm_profile_runtime_init\n");
  char *buffer = (char *)malloc(__llvm_profile_get_size_for_buffer());
  return buffer;
}

void llvm_profile_runtime_end(char* buffer) {
  __llvm_profile_write_buffer(buffer);
  free(buffer);
}
