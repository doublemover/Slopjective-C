include_guard(GLOBAL)

function(objc3c_configure_linux_runtime runtime_source runtime_build robin_source)
  if(NOT CMAKE_SYSTEM_NAME STREQUAL "Linux" OR NOT CMAKE_SIZEOF_VOID_P EQUAL 8 OR
     NOT CMAKE_SYSTEM_PROCESSOR MATCHES "^(x86_64|amd64|AMD64)$")
    message(FATAL_ERROR "linux-x86_64-libobjc2 requires a native Linux x86-64 target")
  endif()
  if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR
     NOT CMAKE_CXX_COMPILER_VERSION VERSION_EQUAL "23.1.3")
    message(FATAL_ERROR "Linux native runtime requires the pinned Clang 23.1.3 client")
  endif()
  foreach(_path IN ITEMS runtime_source runtime_build robin_source)
    if(NOT ${_path} OR NOT IS_DIRECTORY "${${_path}}")
      message(FATAL_ERROR "Pinned Linux runtime path is unavailable: ${_path}")
    endif()
  endforeach()
  find_package(Python3 REQUIRED COMPONENTS Interpreter)
  get_filename_component(_compiler_bin "${CMAKE_CXX_COMPILER}" DIRECTORY)
  execute_process(
    COMMAND "${Python3_EXECUTABLE}" "${_OBJC3_NATIVE_RUNTIME_REPO_ROOT}/scripts/objc3c_toolchain_runtime/verify_runtime_profile.py"
      --profile linux-x86_64 --source "${runtime_source}"
      --robin-map-source "${robin_source}" --bin-dir "${_compiler_bin}"
    RESULT_VARIABLE _verified OUTPUT_VARIABLE _output ERROR_VARIABLE _error)
  if(NOT _verified EQUAL 0)
    message(FATAL_ERROR "Pinned Linux runtime verification failed: ${_output}${_error}")
  endif()
  unset(_objc3_linux_objc CACHE)
  unset(_objc3_linux_objc)
  find_library(_objc3_linux_objc NAMES objc PATHS "${runtime_build}" "${runtime_build}/lib"
    NO_DEFAULT_PATH REQUIRED)
  if(NOT EXISTS "${runtime_build}/objc/objc-config.h" AND
     NOT EXISTS "${runtime_build}/include/objc/objc-config.h")
    message(FATAL_ERROR "Pinned Linux runtime generated headers are missing")
  endif()
  if(EXISTS "${runtime_build}/objc/objc-config.h")
    set(_generated_include "${runtime_build}/objc")
  else()
    set(_generated_include "${runtime_build}/include/objc")
  endif()
  set(OBJC3_NATIVE_RUNTIME_GENERATED_INCLUDE_DIR "${_generated_include}" CACHE PATH "Verified native runtime generated headers" FORCE)
  file(REAL_PATH "${_objc3_linux_objc}" _runtime_library)
  add_library(objc3_runtime_libobjc2 SHARED IMPORTED GLOBAL)
  set_target_properties(objc3_runtime_libobjc2 PROPERTIES
    IMPORTED_LOCATION "${_runtime_library}"
    INTERFACE_INCLUDE_DIRECTORIES "${runtime_source};${_generated_include}")
  add_library(objc3_runtime::objc ALIAS objc3_runtime_libobjc2)
  add_library(objc3_native_runtime_profile INTERFACE)
  target_link_libraries(objc3_native_runtime_profile INTERFACE objc3_runtime::objc)
  target_compile_options(objc3_native_runtime_profile INTERFACE
    "$<$<OR:$<COMPILE_LANGUAGE:OBJC>,$<COMPILE_LANGUAGE:OBJCXX>>:-fobjc-runtime=gnustep-2.2>")
  target_link_options(objc3_native_runtime_profile INTERFACE -fuse-ld=lld)
  set(OBJC3_NATIVE_RUNTIME_PROFILE linux-x86_64-libobjc2 CACHE STRING "Native Objective-C runtime profile" FORCE)
  set(OBJC3_NATIVE_RUNTIME_LIBOBJC2_DLL "${_runtime_library}" CACHE FILEPATH "Pinned native runtime library" FORCE)
  set(OBJC3_NATIVE_RUNTIME_LIBOBJC2_IMPORT_LIBRARY "${_runtime_library}" CACHE FILEPATH "Pinned native runtime link input" FORCE)
  set(OBJC3_LIBOBJC2_SOURCE_DIR "${runtime_source}" PARENT_SCOPE)
  set(OBJC3_LIBOBJC2_BUILD_DIR "${runtime_build}" PARENT_SCOPE)
  set(OBJC3_ROBIN_MAP_SOURCE_DIR "${robin_source}" PARENT_SCOPE)
endfunction()
