set (TESTS
  pmu-test-load
  pmu-test-store
  pmu-test-inhibit
  lbr-test
  ltrace-dma-test
  trig-sin
  sort
)

foreach(test ${TESTS})
  add_executable(${test} src/${test}.c)
  target_link_libraries(${test} 
  PUBLIC l_trace_encoder 
  PRIVATE -L${CMAKE_BINARY_DIR}/glossy -Wl,--whole-archive glossy -Wl,--no-whole-archive
  PUBLIC m
  )
endforeach()
