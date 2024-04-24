set(INCLUDE 
  ${CMAKE_CURRENT_SOURCE_DIR}/inc/
)
set(SOURCES 
  ${CMAKE_CURRENT_SOURCE_DIR}/src/rv_core.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/rv_clint.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/rv_plic.c
)

add_library(rv STATIC ${SOURCES})
target_include_directories(rv PUBLIC ${INCLUDE})