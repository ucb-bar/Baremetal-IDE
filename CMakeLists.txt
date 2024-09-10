########################################################################################################################
# file:  CMakeLists.txt
#
# usage: 
#   Edit "VARIABLES"-section to suit project requirements.
#   Build instructions:
#     cmake . -S ./ -B ./build/ -D CMAKE_BUILD_TYPE=Debug
#     cmake --build ./build/ --target all
#   Cleaning:
#     cmake --build ./build/ --target clean
########################################################################################################################
cmake_minimum_required(VERSION 3.10)

project(chipyard-tests LANGUAGES ASM C CXX)


#################################
# RISCV Toolchain
#################################

set(CMAKE_SYSTEM_NAME         "Generic" CACHE STRING "")
set(CMAKE_SYSTEM_PROCESSOR    "riscv"   CACHE STRING "")

set(TOOLCHAIN_PREFIX    "riscv64-unknown-elf-")

set(CMAKE_AR            "${TOOLCHAIN_PREFIX}ar")
set(CMAKE_ASM_COMPILER  "${TOOLCHAIN_PREFIX}gcc")
set(CMAKE_C_COMPILER    "${TOOLCHAIN_PREFIX}gcc")
set(CMAKE_CXX_COMPILER  "${TOOLCHAIN_PREFIX}g++")
set(CMAKE_LINKER        "${TOOLCHAIN_PREFIX}ld")
set(CMAKE_OBJCOPY       "${TOOLCHAIN_PREFIX}objcopy")
set(CMAKE_OBJDUMP       "${TOOLCHAIN_PREFIX}objdump")
set(CMAKE_SIZE          "${TOOLCHAIN_PREFIX}size")


set(CMAKE_EXECUTABLE_SUFFIX            ".riscv")


#################################
# Flags
#################################

# CPU architecture
set(ARCH                "rv64imafd")
set(ABI                 "lp64d")
set(CMODEL              "medany")
set(ARCH_FLAGS          -march=${ARCH} -mabi=${ABI} -mcmodel=${CMODEL})

# spec
set(SPECS               "nosys.specs")
set(SPEC_FLAGS          -specs=${SPECS})

# linker script
set(LINKER_SCRIPT       "${CMAKE_SOURCE_DIR}/bsp/spike/spike.ld")

add_compile_options(-std=gnu99)
add_compile_options(-O2 -Wall -Wextra)
add_compile_options(-ffunction-sections -fdata-sections -fno-common -fno-builtin-printf -fno-pie)
add_compile_options(-Wall -Wextra -Warray-bounds -Wno-unused-parameter -Wcast-qual)
add_compile_options(${ARCH_FLAGS})
add_compile_options(${SPEC_FLAGS})

add_link_options(-static)
add_link_options(-O2 -Wall -Wextra)
add_link_options(-ffunction-sections -fdata-sections -fno-common -fno-builtin-printf -fno-pie)
add_link_options(-Wall -Wextra -Warray-bounds -Wno-unused-parameter -Wcast-qual)
add_link_options(${ARCH_FLAGS})
add_link_options(${SPEC_FLAGS})
add_link_options(-nostartfiles)
add_link_options(-T ${LINKER_SCRIPT})


#################################
# Build
#################################

add_executable(app
    app/src/main.c
    bsp/spike/boot/startup.S
    bsp/spike/spike.c
    driver/htif/htif.c
    driver/rv/trap.c
    lib/syscall/exit.c
    lib/syscall/read.c
    lib/syscall/write.c
    lib/syscall/close.c
    lib/syscall/lseek.c
    lib/syscall/fstat.c
    lib/syscall/isatty.c
    lib/syscall/sbrk.c
    lib/syscall/getpid.c
    lib/syscall/kill.c)

target_include_directories(app PRIVATE app/inc)
target_include_directories(app PRIVATE bsp/spike)
target_include_directories(app PRIVATE driver/htif)
target_include_directories(app PRIVATE driver/rv)
target_include_directories(app PRIVATE lib/syscall)
target_compile_definitions(app PRIVATE -DCHIP=spike)
target_compile_definitions(app PRIVATE -DDEVICE_TERMINAL_HTIF)
