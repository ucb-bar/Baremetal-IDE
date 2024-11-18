#ifndef LLVM_COV_H
#define LLVM_COV_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int __llvm_profile_runtime = 0;

// Get the size of the buffer needed for the profile data
uint64_t __llvm_profile_get_size_for_buffer(void);

// Write the profile data to the buffer
int __llvm_profile_write_buffer(char *Buffer);

// Initialize the profile runtime
char* llvm_profile_runtime_init(void);

// Finalize the profile runtime
void llvm_profile_runtime_end(char* buffer);

#endif
