if (USE_FREERTOS)

  include(FetchContent)

  FetchContent_Declare( freertos_kernel
    GIT_REPOSITORY https://github.com/FreeRTOS/FreeRTOS-Kernel.git
    GIT_TAG        V11.1.0
  )

  add_library(freertos_config INTERFACE)

  target_include_directories(freertos_config SYSTEM
  INTERFACE
      include
  )

  target_compile_definitions(freertos_config
    INTERFACE
      projCOVERAGE_TEST=0
  )

  set( FREERTOS_HEAP "4" CACHE STRING "" FORCE)
  # Select the native compile PORT
  set( FREERTOS_PORT "GCC_RISC_V" CACHE STRING "" FORCE)

  FetchContent_MakeAvailable(freertos_kernel)

  add_executable(freertos_demo src/main.c src/main_blinky.c src/riscv-virt.c)

  target_include_directories(freertos_demo PRIVATE
    ${freertos_kernel_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/config  # For FreeRTOSConfig.h
  )

  target_link_libraries(freertos_demo
    PUBLIC freertos_kernel freertos_config
    PUBLIC chip-config
    PUBLIC l_trace_encoder
  )

  target_link_libraries(freertos_demo PRIVATE 
    -L${CMAKE_BINARY_DIR}/glossy -Wl,--whole-archive glossy -Wl,--no-whole-archive
  )
  
endif()
