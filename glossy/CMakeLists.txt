
add_library(glossy-tmp STATIC
  src/startup/crt0.S
  src/startup/init.S
  src/startup/secondary.c
  
  src/trap/trap.S
  src/trap/trap.c

  src/sys/close.c
  src/sys/exit.c
  src/sys/fstat.c
  src/sys/getpid.c
  src/sys/isatty.c
  src/sys/kill.c
  src/sys/lseek.c
  src/sys/read.c
  src/sys/sbrk.c
  src/sys/time.c
  src/sys/write.c
  )


target_include_directories(glossy-tmp PUBLIC ${CMAKE_SOURCE_DIR}/glossy/include)
target_compile_definitions(glossy-tmp PUBLIC -D RISCV)


target_compile_definitions(glossy-tmp PUBLIC -D CLINT_BASE=0x02000000)
target_compile_definitions(glossy-tmp PUBLIC -D MTIME_FREQUENCY=10000000)

target_compile_definitions(glossy-tmp PUBLIC -D DEVICE_TERMINAL_HTIF)
target_link_libraries(glossy-tmp PUBLIC htif)
target_link_libraries(glossy-tmp PUBLIC rocket-chip-clint)
