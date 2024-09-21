
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


# convert CHIP to lowercase
string(TOLOWER ${CHIP} CHIP)


if (NOT CHIP)
  message(STATUS "Chip not specified, using default configuration")
  set(CHIP "default")
endif()

include(${CMAKE_SOURCE_DIR}/platform/${CHIP}/${CHIP}.cmake)

message(STATUS "================ Glossy config ================")

message(STATUS " Building for CHIP: \"${CHIP}\"")
message(STATUS " Reading config from \"${CMAKE_SOURCE_DIR}/platform/${CHIP}/${CHIP}.cmake\"")


message(STATUS " Sys Clock: ${SYS_CLK_FREQ} Hz")
message(STATUS " MTIME Frequency: ${MTIME_FREQ} Hz")

if (TERMINAL_DEVICE_HTIF)
  message(STATUS " Terminal Device: HTIF")
  target_compile_definitions(chip-${CHIP}-config INTERFACE -D TERMINAL_DEVICE_HTIF)
elseif (TERMINAL_DEVICE_UART0)
  message(STATUS " Terminal Device: UART0")
  target_compile_definitions(chip-${CHIP}-config INTERFACE -D TERMINAL_DEVICE_UART0)
endif()

message(STATUS " Linker Script: ${LINKER_SCRIPT}")

message(STATUS "===============================================")


target_link_libraries(glossy PUBLIC chip-${CHIP}-config)


set_target_properties(glossy PROPERTIES
  ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/glossy"
)
