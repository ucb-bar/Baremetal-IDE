/**
  * @file linker.ld
  * @brief Linker script to place sections and symbol values. Should be used together
  * with other linker script that defines memory regions FLASH and RAM.
  * It references following symbols, which must be defined in code:
  *   Reset_Handler : Entry of reset handler
  * 
  * It defines following symbols, which code can use without definition:
  *   __boot_hart
  *   __stack_size
  *   __heap_size
  *   __text_start
  *   __text_load_start
  *   __text_end
  *   __data_start
  *   __data_load_start
  *   __preinit_array_start
  *   __preinit_array_end
  *   __init_array_start
  *   __init_array_end
  *   __fini_array_start
  *   __fini_array_end
  *   __tdata_start
  *   __tdata_end
  *   __tdata_size
  *   __data_end
  *   __tbss_start
  *   __tbss_end
  *   __tbss_size
  *   __bss_start
  *   __bss_end
  *   __end
  *   __heap_start
  *   __heap_end
  *   __stack_start
  *   __stack_end
  *   __stack_shift
  *
  * Copyright (c) 2022 UC Berkeley
  *
  * @date 2022
  */

OUTPUT_ARCH("riscv")

/* Entry point */
ENTRY(_start)

MEMORY {
  LIBIFROM (rwx): ORIGIN = 0x00020000, LENGTH = 64K
  LIBIFRAM (rwx): ORIGIN = 0x00030000, LENGTH = 64K
  SCRATCH  (rwx): ORIGIN = 0x08000000, LENGTH = 16K
  FLASH    (rwx): ORIGIN = 0x20000000, LENGTH = 128M
}

SECTIONS {
  /* Default boot hart */
  PROVIDE(__boot_hart = 0);
  
  /* Default stack size */
  __stack_size = DEFINED(__stack_size) ? __stack_size : 1K;
  PROVIDE(__stack_size = __stack_size);

  /* Default heap size */
  __heap_size = DEFINED(__heap_size) ? __heap_size : 1K;
  PROVIDE(__heap_size = __heap_size);
  
  /*
  .bootrom (NOLOAD) : {
    *(.bootrom.start)
    . = ORIGIN(BOOTROM) + 0x40;
    *(.bootrom.hang)
    . = ORIGIN(BOOTROM) + 0x80;
    *(.bootrom.hang80)
  }> BOOTROM
  */

  /* Section containing the program */
  .text : ALIGN(4) {
    PROVIDE(__text_start = .);
    *(.text.init)
    *(.text.trap_vector)
    *(.text.unlikely .text.unlikely.*)
    *(.text.startup .text.startup.*)
    *(.text .text.*)
    
    KEEP(*(.init))
    KEEP(*(.fini))

    /* .ctors - Section for constructors */
    KEEP(*crtbegin.o(.ctors))
    KEEP(*crtbegin?.o(.ctors))
    KEEP(*(EXCLUDE_FILE(*crtend.o *crtend?.o) .ctors))
    KEEP(*(SORT(.ctors.*)))
    KEEP(*(.ctors))

    /* .dtors - Section for destructors */
    KEEP(*crtbegin.o(.dtors))
    KEEP(*crtbegin?.o(.dtors))
    KEEP(*(EXCLUDE_FILE(*crtend.o *crtend?.o) .dtors))
    KEEP(*(SORT(.dtors.*)))
    KEEP(*(.dtors))
    
    *(.gnu.linkonce.t.*)
    PROVIDE(__text_end = .);
  }> FLASH
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
  }> FLASH

  /* Section for initialized data */
  .data : ALIGN(8) {
    PROVIDE(__data_start = .);
    *(.data)
    *(.gnu.linkonce.d.*)

    . = ALIGN(8);
    /* .preinit_array - Section for initialization routines */
    PROVIDE_HIDDEN(__preinit_array_start = .);
    KEEP(*(.preinit_array))
    PROVIDE_HIDDEN(__preinit_array_end = .);

    . = ALIGN(8);
    /* .init_array - Section for finalization routines */
    PROVIDE_HIDDEN(__init_array_start = .);
    KEEP(*(SORT_BY_INIT_PRIORITY(.init_array.*) SORT_BY_INIT_PRIORITY(.ctors.*)))
    KEEP(*(.init_array EXCLUDE_FILE(*crtbegin.o *crtbegin?.o *crtend.o *crtend?.o ) .ctors))
    PROVIDE_HIDDEN(__init_array_end = .);

    . = ALIGN(8);
    /* .fini_array - Section for termination routines */
    PROVIDE_HIDDEN(__fini_array_start = .);
    KEEP(*(SORT_BY_INIT_PRIORITY(.fini_array.*) SORT_BY_INIT_PRIORITY(.dtors.*)))
    KEEP(*(.fini_array EXCLUDE_FILE(*crtbegin.o *crtbegin?.o *crtend.o *crtend?.o ) .dtors))
    PROVIDE_HIDDEN(__fini_array_end = .);
  }> SCRATCH AT> FLASH
  
  PROVIDE(__data_load_start = LOADADDR(.data));

  /* Section for initialized small data */
  .sdata : {
    PROVIDE(__global_pointer$ = . + 0x800);
    *(.sdata .sdata.*)
    *(.gnu.linkonce.s.*)
  }> SCRATCH AT> FLASH

  /* Section for initialized thread-local small data */
  .tdata : {
    PROVIDE_HIDDEN(__tdata_start = .);
    *(.tdata .tdata.*)
    *(.gnu.linkonce.td.*)
    PROVIDE_HIDDEN(__tdata_end = .);
    PROVIDE(__data_end = .);
  }> SCRATCH AT> FLASH

  PROVIDE(__tdata_size = SIZEOF(.tdata));

  /* Section for zero-initialized thread-local uninitialized data */
  .tbss (NOLOAD) : ALIGN(8) {
    PROVIDE_HIDDEN(__tbss_start = .);
    PROVIDE_HIDDEN(__tbss_offset = ABSOLUTE(__tbss_start - __tdata_start));
    *(.tbss .tbss.*)
    *(.gnu.linkonce.tb.*)
    *(.tcommon)
    PROVIDE_HIDDEN(__tbss_end = .);
  }> SCRATCH
  
  PROVIDE(__tbss_size = SIZEOF(.tbss));

  /* Section for zero-initialized uninitialized data */
  .sbss (NOLOAD) : ALIGN(8) {
    PROVIDE_HIDDEN(__bss_start = .);
    *(.sbss .sbss.*)
    *(.gnu.linkonce.sb.*)
  }> SCRATCH
  
  /* Section for uninitialized data */
  .bss (NOLOAD) : ALIGN(8) {
    *(.bss .bss.*)
    *(.gnu.linkonce.b.*)
    *(COMMON)
    PROVIDE_HIDDEN(__bss_end = .);
  }> SCRATCH

  . = ALIGN(8);
  PROVIDE(__end = .);

  /* Reserve heap space */
  .heap (NOLOAD) : ALIGN(8) {
    PROVIDE_HIDDEN(__heap_start = .);
    . += __heap_size;
    . = ALIGN(4K);
    PROVIDE_HIDDEN(__heap_end = .);
  }> SCRATCH
  
  /* Reserve stack space */
  .stack (NOLOAD) : ALIGN(16) {
    PROVIDE_HIDDEN(__stack_start = .);
    . += __stack_size * 5; /* Hart 0 */
    PROVIDE(_sp = .);
    PROVIDE_HIDDEN(__stack_end = .);
  }> SCRATCH

  PROVIDE(__stack_shift = LOG2CEIL(__stack_size));

  /* C++ exception handling information is
   * not useful with our current runtime environment,
   * and it consumes flash space. Discard it until
   * we have something that can use it
   */
  /DISCARD/ : {
    *(.eh_frame .eh_frame.*)
  }
}
