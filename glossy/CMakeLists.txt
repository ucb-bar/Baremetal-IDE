
add_library(glossy STATIC
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
  src/sys/open.c
  src/sys/read.c
  src/sys/sbrk.c
  src/sys/time.c
  src/sys/write.c
)

target_include_directories(glossy PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)


# convert CHIP to uppercase
string(TOUPPER ${CHIP} CHIP)


if (CHIP STREQUAL "BEARLYML")
  include(${CMAKE_SOURCE_DIR}/platforms/bearlyml/bearlyml.cmake)
  message(STATUS "Chip UART: ${UART_BASE}")
  target_link_libraries(glossy PUBLIC chip-bearlyml)
else()
  if (CHIP)
    message(WARNING "Chip ${CHIP} not supported, using default baremetal configuration")
  else()
    message(WARNING "Chip not specified, using default baremetal configuration")
  endif()
  target_compile_definitions(glossy PUBLIC -D TERMINAL_DEVICE_HTIF)

  target_compile_definitions(glossy PUBLIC -D CLINT_BASE=0x02000000)
  target_compile_definitions(glossy PUBLIC -D MTIME_FREQUENCY=10000000)
  
  target_link_libraries(glossy PUBLIC htif)
  target_link_libraries(glossy PUBLIC clint)
endif()


set_target_properties(glossy PROPERTIES
  ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/glossy"
)
