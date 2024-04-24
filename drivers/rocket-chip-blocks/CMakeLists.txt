set(INCLUDE 
  ${CMAKE_CURRENT_SOURCE_DIR}/gpio/
  ${CMAKE_CURRENT_SOURCE_DIR}/i2c/
  ${CMAKE_CURRENT_SOURCE_DIR}/pwm/
  ${CMAKE_CURRENT_SOURCE_DIR}/spi/
  ${CMAKE_CURRENT_SOURCE_DIR}/uart/
)
set(SOURCES 
  ${CMAKE_CURRENT_SOURCE_DIR}/gpio/hal_gpio.c
  ${CMAKE_CURRENT_SOURCE_DIR}/i2c/hal_i2c.c
  ${CMAKE_CURRENT_SOURCE_DIR}/pwm/hal_pwm.c
  ${CMAKE_CURRENT_SOURCE_DIR}/spi/hal_spi.c
  ${CMAKE_CURRENT_SOURCE_DIR}/uart/hal_uart.c
)


add_library(rocket-chip-blocks STATIC ${SOURCES})
target_include_directories(rocket-chip-blocks PUBLIC ${INCLUDE})
target_link_libraries(rocket-chip-blocks rv)