cmake_minimum_required(VERSION 3.20)
project(objc3c_native LANGUAGES CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(Objc3CompilerOptions)
include(Objc3Module)
include(Objc3Platform)
include(Objc3Sanitizers)
include(Objc3SourceSizeGate)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
if (NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

option(OBJC3C_ENABLE_LLVM_DIRECT_OBJECT_EMISSION
  "Enable llvm-direct object emission backend wiring for .objc3 IR compilation."
  ON
)
option(OBJC3C_ENABLE_WARNING_PARITY
  "Apply the warning-level parity flags used by scripts/build_objc3c_native.ps1."
  ON
)
option(OBJC3C_ENABLE_REPRODUCIBLE_BUILD
  "Apply deterministic compile/link options required by clean-room bootstrap rebuild gates."
  ON
)
option(OBJC3C_ENFORCE_SOURCE_SIZE_GATE
  "Fail CMake configure when native source files exceed the hard-cutover line budget."
  ON
)

set(OBJC3C_LLVM_ROOT "" CACHE PATH "Authoritative LLVM root propagated from the PowerShell wrapper.")
set(OBJC3C_LLVM_INCLUDE_DIR "" CACHE PATH "LLVM include directory resolved by the PowerShell wrapper.")
set(OBJC3C_LIBCLANG_LIBRARY "" CACHE FILEPATH "Path to libclang/clang import library resolved by the PowerShell wrapper.")
set(OBJC3C_RUNTIME_OUTPUT_DIR "" CACHE PATH "Canonical final runtime output directory for objc3c-native executables.")
set(OBJC3C_LIBRARY_OUTPUT_DIR "" CACHE PATH "Canonical final archive output directory for objc3_runtime.lib.")
set(OBJC3C_MAX_NATIVE_SOURCE_LINES "1500" CACHE STRING "Maximum line count for native source files.")
set(OBJC3C_MAX_NATIVE_HEADER_LINES "1000" CACHE STRING "Maximum line count for native model and public headers.")

if (OBJC3C_ENFORCE_SOURCE_SIZE_GATE)
  objc3c_enforce_source_size(
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    ${OBJC3C_MAX_NATIVE_SOURCE_LINES}
    ${OBJC3C_MAX_NATIVE_HEADER_LINES}
  )
endif()
if (NOT OBJC3C_LLVM_INCLUDE_DIR AND OBJC3C_LLVM_ROOT)
  set(OBJC3C_LLVM_INCLUDE_DIR "${OBJC3C_LLVM_ROOT}/include")
endif()

add_library(objc3c_build_defaults INTERFACE)
target_include_directories(objc3c_build_defaults INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}/src
)
if (OBJC3C_LLVM_INCLUDE_DIR)
  target_include_directories(objc3c_build_defaults INTERFACE
    ${OBJC3C_LLVM_INCLUDE_DIR}
  )
endif()
if (OBJC3C_ENABLE_WARNING_PARITY)
  if (MSVC)
    target_compile_options(objc3c_build_defaults INTERFACE
      /W4
    )
  else()
    target_compile_options(objc3c_build_defaults INTERFACE
      -Wall
      -Wextra
      -pedantic
    )
  endif()
endif()
if (OBJC3C_ENABLE_REPRODUCIBLE_BUILD)
  if (MSVC)
    target_compile_options(objc3c_build_defaults INTERFACE
      /Brepro
    )
    target_link_options(objc3c_build_defaults INTERFACE
      /Brepro
    )
  else()
    target_compile_options(objc3c_build_defaults INTERFACE
      "-ffile-prefix-map=${CMAKE_SOURCE_DIR}=."
      "-fdebug-prefix-map=${CMAKE_SOURCE_DIR}=."
    )
  endif()
endif()

add_library(objc3c_llvm_direct_config INTERFACE)
if (OBJC3C_ENABLE_LLVM_DIRECT_OBJECT_EMISSION)
  target_compile_definitions(objc3c_llvm_direct_config INTERFACE
    OBJC3C_ENABLE_LLVM_DIRECT_OBJECT_EMISSION=1
  )
endif()

if (OBJC3C_LIBCLANG_LIBRARY)
  add_library(objc3c_libclang UNKNOWN IMPORTED)
  set_target_properties(objc3c_libclang PROPERTIES
    IMPORTED_LOCATION "${OBJC3C_LIBCLANG_LIBRARY}"
  )
endif()

function(objc3c_apply_build_defaults target_name)
  target_link_libraries(${target_name} PUBLIC
    objc3c_build_defaults
  )
  objc3c_apply_sanitizers(${target_name})
endfunction()

function(objc3c_apply_llvm_direct_config target_name)
  target_link_libraries(${target_name} PUBLIC
    objc3c_llvm_direct_config
  )
endfunction()

function(objc3c_apply_runtime_output target_name)
  if (OBJC3C_RUNTIME_OUTPUT_DIR)
    set_target_properties(${target_name} PROPERTIES
      RUNTIME_OUTPUT_DIRECTORY "${OBJC3C_RUNTIME_OUTPUT_DIR}"
    )
  endif()
endfunction()

function(objc3c_apply_archive_output target_name)
  if (OBJC3C_LIBRARY_OUTPUT_DIR)
    set_target_properties(${target_name} PROPERTIES
      ARCHIVE_OUTPUT_DIRECTORY "${OBJC3C_LIBRARY_OUTPUT_DIR}"
    )
  endif()
endfunction()

add_subdirectory(src)
