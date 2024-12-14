#################################
# RISCV Toolchain
#################################
option(RISCV                "Build for RISC-V"                  ON)

set(CMAKE_SYSTEM_NAME       "Generic" CACHE STRING "")
set(CMAKE_SYSTEM_PROCESSOR  "riscv"   CACHE STRING "")

set(TOOLCHAIN_PREFIX        "riscv64-unknown-elf-")

set(MYRISCV                 "/scratch/iansseijelly/riscv-llvm-install")

set(CMAKE_AR                "llvm-ar")
set(CMAKE_ASM_COMPILER      "${MYRISCV}/bin/clang")
set(CMAKE_C_COMPILER        "${MYRISCV}/bin/clang")
set(CMAKE_CXX_COMPILER      "${MYRISCV}/bin/clang++")
set(CMAKE_LINKER            "${MYRISCV}/bin/lld")
set(CMAKE_OBJCOPY           "llvm-objcopy")
set(CMAKE_OBJDUMP           "llvm-objdump")
set(CMAKE_SIZE              "llvm-size")

set(CMAKE_FIND_ROOT_PATH    "${MYRISCV}/")
set(CMAKE_INCLUDE_PATH      "${MYRISCV}/include/")
set(CMAKE_LIBRARY_PATH      "${MYRISCV}/lib/")
set(CMAKE_PROGRAM_PATH      "${MYRISCV}/bin/")

# tell me the cc compiler cmake found
message(STATUS "CC: ${CMAKE_C_COMPILER}")

set(CMAKE_CROSSCOMPILING TRUE)

# prevent the toolchain from searching for programs in the host environment
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

