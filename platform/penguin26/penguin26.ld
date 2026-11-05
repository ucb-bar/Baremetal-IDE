/**
  * @file penguin26.ld
  * @brief Linker script for the penguin26 tapeout chip (chipyard EE290SimConfig).
  *
  * Derived from glossy/glossy.ld with three changes:
  *   - DRAM is limited to 0x80000000-0x8FFFFFFF (256 MiB). The Atlas baremetal tests preload and
  *     check data at 0x90000000+ (`# @DRAM_BASE` in generators/atlas-npu/baremetal/assembly),
  *     so text/data/bss/heap/stack must all stay below 0x90000000; the linker errors out if
  *     they don't fit.
  *   - Default stack 128 KiB (riscv-tests' crt.S gives each hart 1 << STKSHIFT = 128 KiB, which
  *     the Saturn benchmarks were written against). glossy's default is 24 KiB.
  *   - Default heap 1 MiB (glossy: 128 KiB).
  * Real DRAM on this config is 64 GiB at 0x80000000 ([DTS] memory@80000000).
  * Both can be overridden per target with -Wl,--defsym=__stack_size=... / __heap_size=...
  */

OUTPUT_ARCH("riscv")

/* Program entry point */
ENTRY(_start)

MEMORY {
  DRAM     (rwx): ORIGIN = 0x80000000, LENGTH = 256M
}

SECTIONS {
  /* Default boot hart */
  PROVIDE(__boot_hart = 0);

  /* Default stack size */
  __stack_size = DEFINED(__stack_size) ? __stack_size : 128K;
  PROVIDE(__stack_size = __stack_size);

  /* Default heap size */
  __heap_size = DEFINED(__heap_size) ? __heap_size : 1M;
  PROVIDE(__heap_size = __heap_size);
  
  /* Section containing the program */
  .text : ALIGN(4) {
    PROVIDE(__text_start = .);
    *(.text.init)
    *(.text.trap_vector)
    *(.text.unlikely .text.unlikely.*)
    *(.text.startup .text.startup.*)
    *(.text .text.*)
    *(.gnu.linkonce.t.*)
    PROVIDE(__text_end = .);
  }> DRAM
  PROVIDE(__text_load_start = LOADADDR(.text));

  /* Section containing read-only data */
  .rodata : {
    . = ALIGN(4);
    *(.rodata .rodata.*)
    *(.gnu.linkonce.r.*)
    . = ALIGN(8);
    *(.srodata.cst16)
    *(.srodata.cst8)
    *(.srodata.cst4)
    *(.srodata.cst2)
    *(.srodata .srodata.*)
  }> DRAM

  /* Section for initialized data */
  .data : ALIGN(8) {
    PROVIDE(__data_start = .);
    *(.data)
    *(.gnu.linkonce.d.*)
  }> DRAM
  
  PROVIDE(__data_load_start = LOADADDR(.data));

  /* Section for initialized small data */
  .sdata : {
    PROVIDE(__global_pointer$ = . + 0x800);
    *(.sdata .sdata.*)
    *(.gnu.linkonce.s.*)
  }> DRAM

  /* Section for initialized thread-local small data */
  .tdata : {
    PROVIDE_HIDDEN(__tdata_start = .);
    *(.tdata .tdata.*)
    *(.gnu.linkonce.td.*)
    PROVIDE_HIDDEN(__tdata_end = .);
    PROVIDE(__data_end = .);
  }> DRAM
  PROVIDE(__tdata_size = SIZEOF(.tdata));

  /* Section for zero-initialized thread-local uninitialized data */
  .tbss (NOLOAD) : ALIGN(8) {
    PROVIDE_HIDDEN(__tbss_start = .);
    PROVIDE_HIDDEN(__tbss_offset = ABSOLUTE(__tbss_start - __tdata_start));
    *(.tbss .tbss.*)
    *(.gnu.linkonce.tb.*)
    *(.tcommon)
    PROVIDE_HIDDEN(__tbss_end = .);
  }> DRAM
  PROVIDE(__tbss_size = SIZEOF(.tbss));
  
  /* Section for uninitialized data */
  .bss : ALIGN (8) {
    PROVIDE_HIDDEN(__bss_start = .);
    *(.sbss .sbss.*)
    *(.gnu.linkonce.sb.*)
    *(.bss .bss.*)
    *(.gnu.linkonce.b.*)
    *(COMMON)
    PROVIDE_HIDDEN(__bss_end = .);
  }> DRAM

  . = ALIGN (8);
  PROVIDE(__end = .);

  /* Reserve heap space */
  .heap (NOLOAD) : ALIGN(8) {
    PROVIDE_HIDDEN(__heap_start = .);
    . += __heap_size;
    . = ALIGN(4K);
    PROVIDE_HIDDEN(__heap_end = .);
  }> DRAM

  /* Reserve stack space */
  .stack (NOLOAD) : ALIGN(16) {
    PROVIDE_HIDDEN(__stack_start = .);
    . += __stack_size;
    PROVIDE(__sp = .);
    PROVIDE_HIDDEN(__stack_end = .);
  }> DRAM

  PROVIDE(__stack_shift = LOG2CEIL(__stack_size));

  ASSERT(__stack_end <= 0x90000000, "penguin26.ld: program/heap/stack overlap the Atlas test data region at 0x90000000")
  
  /* C++ exception handling information is
   * not useful with our current runtime environment,
   * and it consumes flash space. Discard it until
   * we have something that can use it
   */
  /DISCARD/ : {
    *(.eh_frame .eh_frame.*)
  }
}
