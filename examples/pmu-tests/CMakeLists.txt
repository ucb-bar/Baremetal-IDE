set (TESTS
  pmu-test-load
  pmu-test-store
)

foreach(test ${TESTS})
  add_executable(${test} src/${test}.c)
  target_link_libraries(${test} PRIVATE 
    -L${CMAKE_BINARY_DIR}/glossy -Wl,--whole-archive glossy -Wl,--no-whole-archive
  )
endforeach()