/* Copyright (c) 2022 UC Berkeley */

OUTPUT_ARCH("riscv")

ENTRY(_start)

MEMORY {
  BOOTROM (rx):   ORIGIN = 0x00010000, LENGTH = 128K
  DRAM (rwx):     ORIGIN = 0x80000000, LENGTH = 16384M
}

SECTIONS {
  /* Default boot hart */
  PROVIDE (__boot_hart = 0);
  
  __stack_size = DEFINED(__stack_size) ? __stack_size : 24K;
  PROVIDE(__stack_size = __stack_size);

  __heap_size = DEFINED(__heap_size) ? __heap_size : 128K;
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

  .text : {
    *(.text.init)
    *(.text.trap_vector)
    *(.text.unlikely .text.unlikely.*)
    *(.text.startup .text.startup.*)
    *(.text .text.*)
    
    KEEP (*(.init))
    KEEP (*(.fini))
    
    *(.gnu.linkonce.t.*)
  }> DRAM
  PROVIDE (_etext = .);

  .rodata : {
    *(.rodata .rodata.*)
    *(.gnu.linkonce.r.*)
    . = ALIGN(8);
    *(.srodata.cst16)
    *(.srodata.cst8)
    *(.srodata.cst4)
    *(.srodata.cst2)
    *(.srodata .srodata.*)
  }> DRAM

  .preinit_array : {
    PROVIDE_HIDDEN (__preinit_array_start = .);
    KEEP (*(.preinit_array))
    PROVIDE_HIDDEN (__preinit_array_end = .);
  }> DRAM

  .init_array : {
    PROVIDE_HIDDEN (__init_array_start = .);
    KEEP (*(SORT_BY_INIT_PRIORITY(.init_array.*) SORT_BY_INIT_PRIORITY(.ctors.*)))
    KEEP (*(.init_array EXCLUDE_FILE (*crtbegin.o *crtbegin?.o *crtend.o *crtend?.o ) .ctors))
    PROVIDE_HIDDEN (__init_array_end = .);
  }> DRAM

  .fini_array : {
    PROVIDE_HIDDEN (__fini_array_start = .);
    KEEP (*(SORT_BY_INIT_PRIORITY(.fini_array.*) SORT_BY_INIT_PRIORITY(.dtors.*)))
    KEEP (*(.fini_array EXCLUDE_FILE (*crtbegin.o *crtbegin?.o *crtend.o *crtend?.o ) .dtors))
    PROVIDE_HIDDEN (__fini_array_end = .);
  }> DRAM

  .ctors : {
    KEEP (*crtbegin.o(.ctors))
    KEEP (*crtbegin?.o(.ctors))
    KEEP (*(EXCLUDE_FILE (*crtend.o *crtend?.o) .ctors))
    KEEP (*(SORT(.ctors.*)))
    KEEP (*(.ctors))
  }> DRAM

  .dtors : {
    KEEP (*crtbegin.o(.dtors))
    KEEP (*crtbegin?.o(.dtors))
    KEEP (*(EXCLUDE_FILE (*crtend.o *crtend?.o) .dtors))
    KEEP (*(SORT(.dtors.*)))
    KEEP (*(.dtors))
  }> DRAM

  /* Host-Target Interface */
  .htif ALIGN(0x40) : {
    *(.htif)
  }> DRAM

  .data : ALIGN (8) {
    *(.data)
    *(.gnu.linkonce.d.*)
  }> DRAM

  .sdata : {
    PROVIDE (__global_pointer$ = . + 0x800);
    *(.sdata .sdata.*)
    *(.gnu.linkonce.s.*)
  }> DRAM

  /* Static Thread Local Storage template */
  .tdata : {
    PROVIDE_HIDDEN (__tdata_start = .);
    *(.tdata .tdata.*)
    *(.gnu.linkonce.td.*)
    PROVIDE_HIDDEN (__tdata_end = .);
  }> DRAM
  PROVIDE (__tdata_size = SIZEOF (.tdata));

  PROVIDE (_edata = .);

  .tbss (NOLOAD) : {
    PROVIDE_HIDDEN (__tbss_start = .);
    PROVIDE_HIDDEN (__tbss_offset = ABSOLUTE (__tbss_start - __tdata_start));
    *(.tbss .tbss.*)
    *(.gnu.linkonce.tb.*)
    *(.tcommon)
    PROVIDE_HIDDEN (__tbss_end = .);
  }> DRAM
  PROVIDE (__tbss_size = SIZEOF (.tbss));

  .bss : ALIGN (8) {
    PROVIDE_HIDDEN (__bss_start = .);
    *(.sbss .sbss.*)
    *(.gnu.linkonce.sb.*)
    *(.bss .bss.*)
    *(.gnu.linkonce.b.*)
    *(COMMON)
    PROVIDE_HIDDEN (__bss_end = .);
  }> DRAM

  . = ALIGN (8);
  PROVIDE (_end = .);
  PROVIDE(__end = .);

  /* Reserve heap space */
  .heap (NOLOAD) : ALIGN(8) {
    . += __heap_size;
    . = ALIGN(4K);
  }> DRAM

  PROVIDE(__heap_start = ADDR(.heap));
  PROVIDE(__heap_end = ADDR(.heap) + SIZEOF(.heap));
  
  .stack (NOLOAD) : ALIGN(16) {
    . += __stack_size * 5; /* Hart 0 */
    PROVIDE(_sp = .);
  }> DRAM

  PROVIDE(__stack_start = ADDR(.stack));
  PROVIDE(__stack_end = ADDR(.stack) + SIZEOF(.stack));

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
