# One warning policy for integrated and standalone native implementation builds.
option(OBJC3C_SOURCE_COVERAGE "Isolated native implementation source coverage" OFF)
if(OBJC3C_SOURCE_COVERAGE)
  if(NOT CMAKE_SYSTEM_NAME STREQUAL "Linux" OR NOT CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    message(FATAL_ERROR "Native source coverage requires the pinned Linux Clang compiler")
  endif()
  set(OBJC3C_SOURCE_COVERAGE_FLAGS
    -fprofile-instr-generate -fcoverage-mapping -fprofile-update=atomic)
endif()
function(objc3c_configure_build_warnings target)
  target_compile_options(${target} INTERFACE
    "$<BUILD_INTERFACE:-Wall;-Wextra;-pedantic;-ffile-prefix-map=${CMAKE_SOURCE_DIR}=.;-fdebug-prefix-map=${CMAKE_SOURCE_DIR}=.>")
  if(OBJC3C_SOURCE_COVERAGE)
    target_compile_options(${target} INTERFACE ${OBJC3C_SOURCE_COVERAGE_FLAGS}
      "-fcoverage-prefix-map=${CMAKE_SOURCE_DIR}=${CMAKE_SOURCE_DIR}")
    target_link_options(${target} INTERFACE -fprofile-instr-generate "LINKER:--build-id=sha1")
  endif()
endfunction()
