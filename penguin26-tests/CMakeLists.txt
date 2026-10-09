# penguin26 tapeout chip only (CHIP=penguin26)
if (NOT CHIP STREQUAL "penguin26")
  return()
endif()

add_subdirectory(hello_world)
add_subdirectory(atlas_example)
add_subdirectory(atlas-tests)
