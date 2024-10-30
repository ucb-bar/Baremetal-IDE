
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


message(STATUS "================ Glossy config ================")

message(STATUS " Building for CHIP: \"${CHIP}\"")
message(STATUS " Reading config from \"${CMAKE_SOURCE_DIR}/platform/${CHIP}/CMakeLists.txt\"")

if (TERMINAL_DEVICE_HTIF)
  message(STATUS " Terminal Device: HTIF")
elseif (TERMINAL_DEVICE_UART0)
  message(STATUS " Terminal Device: UART0")
endif()

message(STATUS " Linker Script: ${LINKER_SCRIPT}")

message(STATUS "===============================================")

target_link_libraries(glossy PUBLIC chip-config)


set_target_properties(glossy PROPERTIES
  ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/glossy"
)
