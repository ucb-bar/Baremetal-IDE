# Optional tile PMU metadata, exported by the selected platform.
set(TILE_PMU_MANIFEST "" CACHE FILEPATH "The tile's Chipyard event list, <config>.<source>.pmu.json")
set(TILE_PMU_PLATFORM_JSON "" CACHE FILEPATH "Matching Chipyard device-tree .json")
set(TILE_PMU_SOFTWARE_DIR "" CACHE PATH "TilePMU ABI headers and platform header generator")

if(TILE_PMU_MANIFEST OR TILE_PMU_PLATFORM_JSON)
  foreach(input TILE_PMU_MANIFEST TILE_PMU_PLATFORM_JSON TILE_PMU_SOFTWARE_DIR)
    if(NOT ${input})
      message(FATAL_ERROR "${input} is required for tile PMU support")
    endif()
    # Relative user inputs are relative to the Baremetal-IDE source directory,
    # not to an assumed Chipyard checkout or the generated build directory.
    get_filename_component(${input} "${${input}}" ABSOLUTE BASE_DIR "${CMAKE_SOURCE_DIR}")
    if(NOT EXISTS "${${input}}")
      message(FATAL_ERROR "${input} does not exist: ${${input}}")
    endif()
  endforeach()
  foreach(component generate_platform_header.py pmu_mmio.h tile_pmu.h)
    if(NOT EXISTS "${TILE_PMU_SOFTWARE_DIR}/${component}")
      message(FATAL_ERROR "TILE_PMU_SOFTWARE_DIR is missing ${component}: ${TILE_PMU_SOFTWARE_DIR}")
    endif()
  endforeach()
  find_package(PythonInterp 3 REQUIRED)
  set(tile_pmu_header "${CMAKE_BINARY_DIR}/platform/tile_pmu_platform.h")
  add_custom_command(OUTPUT "${tile_pmu_header}"
    COMMAND "${PYTHON_EXECUTABLE}" "${TILE_PMU_SOFTWARE_DIR}/generate_platform_header.py"
      --manifest "${TILE_PMU_MANIFEST}" --platform "${TILE_PMU_PLATFORM_JSON}"
      --output "${tile_pmu_header}"
    DEPENDS "${TILE_PMU_MANIFEST}" "${TILE_PMU_PLATFORM_JSON}"
      "${TILE_PMU_SOFTWARE_DIR}/generate_platform_header.py"
    VERBATIM)
  add_custom_target(tile-pmu-platform DEPENDS "${tile_pmu_header}")
  add_dependencies(chip-config tile-pmu-platform)
  target_include_directories(chip-config PUBLIC
    "${CMAKE_BINARY_DIR}/platform" "${TILE_PMU_SOFTWARE_DIR}")
endif()
