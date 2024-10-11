/**
 * @file secondary.c
 */


/*
 * Main function for secondary harts
 * 
 * Multi-threaded programs should provide their own implementation.
 */
__attribute__((weak, noreturn)) void __main(void) {
  while (1) {
    __asm__ __volatile__ ("wfi");
  }
}
