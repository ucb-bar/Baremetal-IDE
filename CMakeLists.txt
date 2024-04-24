########################################################################################################################
# file:  CMakeLists.txt
#
# usage: 
#   Edit "VARIABLES"-section to suit project requirements.
#   Build instructions:
#     cmake -D CMAKE_TOOLCHAIN_FILE=./riscv-gcc.cmake -S ./ -B ./build/ -G "Unix Makefiles" -D CMAKE_BUILD_TYPE=Debug
#     cmake --build ./build/ --target all
########################################################################################################################
cmake_minimum_required(VERSION 3.15)

######################## CONSTANTS #############################################

set(ARCH                      "rv64imafdc")
set(ABI                       "lp64d")
set(CODEMODEL                 "medany")
set(ARCHFLAGS                 "-march=${ARCH} -mabi=${ABI}")
# set(ARCHFLAGS                 "-march=${ARCH} -mabi=${ABI} -mcmodel=${CODEMODEL} -fno-pie")

set(libgloss_specs            "htif_nano.specs")


################################################################################

######################## VARIABLES #############################################
set(PROJECT_NAME        "firmware")
set(LINKER_SCRIPT       "htif.ld")

set(PROJECT_SOURCES
    app/src/main.c
)

set(PROJECT_INCLUDES
    app/inc
    drivers/core/inc
)

################################################################################

######################## PROJECT SETUP #########################################
message(STATUS "Using toolchain file:" ${CMAKE_TOOLCHAIN_FILE})

project(${PROJECT_NAME})
enable_language(ASM)

# configure_file(src/TutorialConfig.h.in TutorialConfig.h)


# add_executable(standalone ${PROJECT_SOURCES})
add_executable(spike ${PROJECT_SOURCES})



add_subdirectory(drivers)
target_link_libraries(spike PUBLIC rv)
target_link_libraries(spike PUBLIC rocket-chip-blocks)

# add_subdirectory(bsp/examplechip)

# target_link_libraries(spike PUBLIC examplechip)

# add_subdirectory(drivers)
# target_link_libraries(spike PUBLIC UART)
# target_link_libraries(standalone PUBLIC UART)

# target_include_directories(standalone PUBLIC
#   ${PROJECT_INCLUDES})
target_include_directories(spike PUBLIC
  ${PROJECT_INCLUDES})


# set_target_properties(standalone PROPERTIES RUNTIME_OUTPUT_DIRECTORY "../dist/")
set_target_properties(spike PROPERTIES RUNTIME_OUTPUT_DIRECTORY "../dist/")

set(CMAKE_EXECUTABLE_SUFFIX ".elf")
set(CMAKE_STATIC_LIBRARY_SUFFIX ".a")
set(CMAKE_C_FLAGS "-std=gnu99 -O2 -fno-common -fno-builtin-printf -Wall ${ARCHFLAGS} -specs=${libgloss_specs}")
set(CMAKE_EXE_LINKER_FLAGS "-static") # -specs=${libgloss_specs}")

################################################################################
