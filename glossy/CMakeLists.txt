

add_library(glossy OBJECT
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


target_include_directories(glossy PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_compile_definitions(glossy PUBLIC -D CLINT_BASE=0x02000000)
target_compile_definitions(glossy PUBLIC -D MTIME_FREQUENCY=10000000)

target_compile_definitions(glossy PUBLIC -D DEVICE_TERMINAL_HTIF)

target_link_libraries(glossy PUBLIC htif)
target_link_libraries(glossy PUBLIC clint)


