include_guard(GLOBAL)

set(_OBJC3_NATIVE_RUNTIME_MODULE_DIR "${CMAKE_CURRENT_LIST_DIR}")
set(_OBJC3_NATIVE_RUNTIME_REPO_ROOT "${CMAKE_CURRENT_LIST_DIR}/../../..")
set(OBJC3_NATIVE_RUNTIME_PROFILE "" CACHE STRING
    "Qualified native Objective-C runtime profile (currently windows-x86_64-libobjc2).")
set(OBJC3_LIBOBJC2_SOURCE_DIR "" CACHE PATH
    "Local source checkout for the pinned libobjc2 v2.3 headers.")
set(OBJC3_LIBOBJC2_BUILD_DIR "" CACHE PATH
    "Local libobjc2 v2.3 build output containing objc.dll, objc.lib, and generated objc/objc-config.h.")
set(OBJC3_ROBIN_MAP_SOURCE_DIR "" CACHE PATH
    "Local robin-map v1.4.0 source checkout used by the pinned Windows runtime build.")

function(objc3c_configure_native_runtime)
  cmake_parse_arguments(ARG "" "PROFILE;SOURCE_DIR;BUILD_DIR;ROBIN_MAP_SOURCE_DIR" "" ${ARGN})
  if (ARG_UNPARSED_ARGUMENTS)
    message(FATAL_ERROR "objc3c_configure_native_runtime: unknown arguments: ${ARG_UNPARSED_ARGUMENTS}")
  endif()

  if (ARG_PROFILE)
    set(_profile "${ARG_PROFILE}")
  else()
    set(_profile "${OBJC3_NATIVE_RUNTIME_PROFILE}")
  endif()
  if (NOT _profile STREQUAL "windows-x86_64-libobjc2")
    message(FATAL_ERROR
      "Native runtime profile '${_profile}' is not qualified for CMake integration. "
      "Only windows-x86_64-libobjc2 currently has linked runtime/client fixture evidence; "
      "Apple and Linux remain unverified.")
  endif()
  if (NOT WIN32 OR NOT CMAKE_SIZEOF_VOID_P EQUAL 8)
    message(FATAL_ERROR "windows-x86_64-libobjc2 requires a Windows x64 CMake target.")
  endif()
  if (NOT CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    message(FATAL_ERROR
      "The native adapter requires the pinned Clang 22.1.6 client driver (GNU or clang-cl frontend).")
  endif()
  if (NOT CMAKE_CXX_COMPILER_VERSION VERSION_EQUAL "22.1.6")
    message(FATAL_ERROR
      "The qualified client compiler is Clang 22.1.6; found ${CMAKE_CXX_COMPILER_VERSION}.")
  endif()
  if (NOT CMAKE_CXX_COMPILER_FRONTEND_VARIANT MATCHES "^(GNU|MSVC)$")
    message(FATAL_ERROR
      "Unsupported Clang frontend '${CMAKE_CXX_COMPILER_FRONTEND_VARIANT}'.")
  endif()

  if (ARG_SOURCE_DIR)
    set(_runtime_source "${ARG_SOURCE_DIR}")
  else()
    set(_runtime_source "${OBJC3_LIBOBJC2_SOURCE_DIR}")
  endif()
  if (ARG_BUILD_DIR)
    set(_runtime_build "${ARG_BUILD_DIR}")
  else()
    set(_runtime_build "${OBJC3_LIBOBJC2_BUILD_DIR}")
  endif()
  if (ARG_ROBIN_MAP_SOURCE_DIR)
    set(_robin_map_source "${ARG_ROBIN_MAP_SOURCE_DIR}")
  else()
    set(_robin_map_source "${OBJC3_ROBIN_MAP_SOURCE_DIR}")
  endif()
  foreach(_required_path IN ITEMS _runtime_source _runtime_build _robin_map_source)
    if (NOT ${_required_path})
      message(FATAL_ERROR
        "${_required_path} is required; set the exact local pinned source/build path. "
        "This module does not download or configure dependencies.")
    endif()
  endforeach()
  file(REAL_PATH "${_runtime_source}" _runtime_source)
  file(REAL_PATH "${_runtime_build}" _runtime_build)
  file(REAL_PATH "${_robin_map_source}" _robin_map_source)
  set(_runtime_dll "${_runtime_build}/objc.dll")
  set(_runtime_import "${_runtime_build}/objc.lib")
  set(_runtime_generated_include "${_runtime_build}/objc")
  foreach(_required_file IN ITEMS
      "${_runtime_source}/objc/runtime.h"
      "${_runtime_source}/objc/objc-arc.h"
      "${_runtime_generated_include}/objc-config.h"
      "${_runtime_dll}"
      "${_runtime_import}")
    if (NOT EXISTS "${_required_file}")
      message(FATAL_ERROR "Pinned libobjc2 artifact/header unavailable: ${_required_file}")
    endif()
  endforeach()

  find_package(Python3 REQUIRED COMPONENTS Interpreter)
  get_filename_component(_compiler_bin "${CMAKE_CXX_COMPILER}" DIRECTORY)
  set(_profile_verifier "${_OBJC3_NATIVE_RUNTIME_REPO_ROOT}/scripts/objc3c_toolchain_runtime/verify_runtime_profile.py")
  execute_process(
    COMMAND "${Python3_EXECUTABLE}" "${_profile_verifier}"
      --profile windows-x86_64
      --source "${_runtime_source}"
      --robin-map-source "${_robin_map_source}"
      --bin-dir "${_compiler_bin}"
    RESULT_VARIABLE _verify_result
    OUTPUT_VARIABLE _verify_output
    ERROR_VARIABLE _verify_error
    WORKING_DIRECTORY "${_OBJC3_NATIVE_RUNTIME_REPO_ROOT}")
  if (NOT _verify_result EQUAL 0)
    message(FATAL_ERROR
      "Pinned native runtime verification failed; no fallback will be used.\n"
      "${_verify_output}${_verify_error}")
  endif()
  message(STATUS "${_verify_output}")
  find_program(_llvm_lib NAMES llvm-lib.exe llvm-lib
    HINTS "${_compiler_bin}" NO_DEFAULT_PATH REQUIRED)
  find_program(_gnu_clangxx NAMES clang++.exe
    HINTS "${_compiler_bin}" NO_DEFAULT_PATH REQUIRED)
  set(OBJC3_NATIVE_RUNTIME_LLVM_LIB "${_llvm_lib}" CACHE FILEPATH
      "Pinned LLVM librarian used to create the native adapter archive." FORCE)
  set(OBJC3_NATIVE_RUNTIME_ADAPTER_COMPILER "${_gnu_clangxx}" CACHE FILEPATH
      "Pinned GNU-style Clang++ driver used to compile the Objective-C runtime adapter." FORCE)

  if (NOT TARGET objc3_runtime_libobjc2)
    add_library(objc3_runtime_libobjc2 SHARED IMPORTED GLOBAL)
    set_target_properties(objc3_runtime_libobjc2 PROPERTIES
      IMPORTED_LOCATION "${_runtime_dll}"
      IMPORTED_IMPLIB "${_runtime_import}"
      INTERFACE_INCLUDE_DIRECTORIES "${_runtime_source};${_runtime_generated_include}")
    add_library(objc3_runtime::objc ALIAS objc3_runtime_libobjc2)
  endif()

  if (NOT TARGET objc3_native_runtime_profile)
    add_library(objc3_native_runtime_profile INTERFACE)
    target_link_libraries(objc3_native_runtime_profile INTERFACE objc3_runtime::objc)
    if (CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
      target_compile_options(objc3_native_runtime_profile INTERFACE
        "$<$<COMPILE_LANGUAGE:OBJC>:/clang:-fobjc-runtime=gnustep-2.0>"
        "$<$<COMPILE_LANGUAGE:OBJCXX>:/clang:-fobjc-runtime=gnustep-2.0>"
        "$<$<COMPILE_LANGUAGE:OBJC>:/clang:-fms-runtime-lib=dll>"
        "$<$<COMPILE_LANGUAGE:OBJCXX>:/clang:-fms-runtime-lib=dll>")
    else()
      target_compile_options(objc3_native_runtime_profile INTERFACE
        "$<$<COMPILE_LANGUAGE:OBJC>:-fobjc-runtime=gnustep-2.0>"
        "$<$<COMPILE_LANGUAGE:OBJCXX>:-fobjc-runtime=gnustep-2.0>"
        "$<$<COMPILE_LANGUAGE:OBJC>:-fms-runtime-lib=dll>"
        "$<$<COMPILE_LANGUAGE:OBJCXX>:-fms-runtime-lib=dll>")
      target_link_options(objc3_native_runtime_profile INTERFACE -fuse-ld=lld)
    endif()
  endif()

  set(OBJC3_NATIVE_RUNTIME_PROFILE windows-x86_64-libobjc2 CACHE STRING
      "Qualified native Objective-C runtime profile." FORCE)
  set(OBJC3_NATIVE_RUNTIME_LIBOBJC2_SOURCE_DIR "${_runtime_source}" CACHE PATH
      "Local pinned libobjc2 source checkout." FORCE)
  set(OBJC3_NATIVE_RUNTIME_LIBOBJC2_BUILD_DIR "${_runtime_build}" CACHE PATH
      "Local pinned libobjc2 build output." FORCE)
  set(OBJC3_NATIVE_RUNTIME_LIBOBJC2_DLL "${_runtime_dll}" CACHE FILEPATH
      "Verified pinned libobjc2 DLL." FORCE)
  set(OBJC3_NATIVE_RUNTIME_LIBOBJC2_IMPORT_LIBRARY "${_runtime_import}" CACHE FILEPATH
      "Verified pinned libobjc2 import library." FORCE)
  set(OBJC3_LIBOBJC2_SOURCE_DIR "${_runtime_source}" PARENT_SCOPE)
  set(OBJC3_LIBOBJC2_BUILD_DIR "${_runtime_build}" PARENT_SCOPE)
  set(OBJC3_ROBIN_MAP_SOURCE_DIR "${_robin_map_source}" PARENT_SCOPE)
endfunction()

function(objc3c_apply_native_runtime_profile target_name)
  if (NOT TARGET objc3_native_runtime_profile)
    message(FATAL_ERROR "Call objc3c_configure_native_runtime() before applying the runtime profile.")
  endif()
  if (NOT TARGET "${target_name}")
    message(FATAL_ERROR "objc3c_apply_native_runtime_profile: unknown target '${target_name}'.")
  endif()
  target_link_libraries("${target_name}" PUBLIC objc3_native_runtime_profile)
  set_property(TARGET "${target_name}" PROPERTY MSVC_RUNTIME_LIBRARY MultiThreadedDLL)
endfunction()

function(objc3c_add_native_object_adapter)
  cmake_parse_arguments(ARG "" "TARGET" "" ${ARGN})
  if (ARG_UNPARSED_ARGUMENTS OR NOT ARG_TARGET)
    message(FATAL_ERROR "Usage: objc3c_add_native_object_adapter(TARGET <target-name>)")
  endif()
  if (NOT TARGET objc3_runtime::objc)
    message(FATAL_ERROR "Call objc3c_configure_native_runtime() before adding the adapter.")
  endif()
  if (TARGET "${ARG_TARGET}")
    message(FATAL_ERROR "Native adapter target already exists: ${ARG_TARGET}")
  endif()
  set(_adapter_source
      "${_OBJC3_NATIVE_RUNTIME_REPO_ROOT}/native/objc3c/src/runtime/platform/objc3_native_object_adapter.mm")
  set(_adapter_header
      "${_OBJC3_NATIVE_RUNTIME_REPO_ROOT}/native/objc3c/src/runtime/platform/objc3_native_object_adapter.h")
  file(GLOB _runtime_headers CONFIGURE_DEPENDS "${OBJC3_LIBOBJC2_SOURCE_DIR}/objc/*.h")
  set(_adapter_object "${CMAKE_CURRENT_BINARY_DIR}/${ARG_TARGET}.obj")
  set(_adapter_archive "${CMAKE_CURRENT_BINARY_DIR}/${ARG_TARGET}.lib")
  set(_compile_flags
    --target=x86_64-pc-windows-msvc
    # This branch calls libobjc2's C API and contains no ObjC declarations.
    # Compile it as C++ so its archive does not emit an empty ObjC module.
    -x c++
    -fms-runtime-lib=dll
    -DOBJC3_NATIVE_RUNTIME_GNUSTEP=1
    "-I${OBJC3_LIBOBJC2_SOURCE_DIR}"
    "-I${OBJC3_LIBOBJC2_BUILD_DIR}/objc"
    "-I${_OBJC3_NATIVE_RUNTIME_REPO_ROOT}/native/objc3c/src")
  set(_compile_command
    "${OBJC3_NATIVE_RUNTIME_ADAPTER_COMPILER}" ${_compile_flags}
    -c "${_adapter_source}" -o "${_adapter_object}")
  add_custom_command(OUTPUT "${_adapter_archive}"
    COMMAND ${_compile_command}
    COMMAND "${OBJC3_NATIVE_RUNTIME_LLVM_LIB}" "/OUT:${_adapter_archive}" "${_adapter_object}"
    DEPENDS "${_adapter_source}" "${_adapter_header}"
      "${OBJC3_LIBOBJC2_BUILD_DIR}/objc/objc-config.h" ${_runtime_headers}
    COMMENT "Building the MRC native Objective-C object adapter (${CMAKE_CXX_COMPILER_FRONTEND_VARIANT} frontend)"
    COMMAND_EXPAND_LISTS
    VERBATIM)
  add_custom_target("${ARG_TARGET}_archive" DEPENDS "${_adapter_archive}")
  add_library("${ARG_TARGET}" STATIC IMPORTED GLOBAL)
  set_target_properties("${ARG_TARGET}" PROPERTIES
    IMPORTED_LOCATION "${_adapter_archive}"
    INTERFACE_INCLUDE_DIRECTORIES
      "${_OBJC3_NATIVE_RUNTIME_REPO_ROOT}/native/objc3c/src"
    INTERFACE_LINK_LIBRARIES "objc3_native_runtime_profile")
  add_dependencies("${ARG_TARGET}" "${ARG_TARGET}_archive")
  set(OBJC3_NATIVE_OBJECT_ADAPTER_LIBRARY "${_adapter_archive}" CACHE FILEPATH
      "Built native object adapter archive." FORCE)
endfunction()

function(objc3c_install_native_runtime)
  cmake_parse_arguments(ARG "" "TARGET;DESTINATION" "" ${ARGN})
  if (ARG_UNPARSED_ARGUMENTS OR NOT ARG_TARGET OR NOT ARG_DESTINATION)
    message(FATAL_ERROR
      "Usage: objc3c_install_native_runtime(TARGET <adapter-target> DESTINATION <prefix-relative-dir>)")
  endif()
  if (NOT TARGET "${ARG_TARGET}" OR
      NOT OBJC3_NATIVE_RUNTIME_LIBOBJC2_DLL OR
      NOT OBJC3_NATIVE_RUNTIME_LIBOBJC2_IMPORT_LIBRARY)
    message(FATAL_ERROR "Configure the verified runtime and adapter before installing it.")
  endif()
  install(FILES "${OBJC3_NATIVE_OBJECT_ADAPTER_LIBRARY}"
    DESTINATION "${ARG_DESTINATION}/lib"
    COMPONENT Runtime)
  install(FILES
    "${OBJC3_NATIVE_RUNTIME_LIBOBJC2_DLL}"
    DESTINATION "${ARG_DESTINATION}/bin"
    COMPONENT Runtime)
  install(FILES
    "${OBJC3_NATIVE_RUNTIME_LIBOBJC2_IMPORT_LIBRARY}"
    DESTINATION "${ARG_DESTINATION}/lib"
    COMPONENT Runtime)
  install(FILES
    "${_OBJC3_NATIVE_RUNTIME_REPO_ROOT}/native/objc3c/src/runtime/platform/objc3_native_object_adapter.h"
    DESTINATION "${ARG_DESTINATION}/include/runtime/platform"
    COMPONENT Runtime)
  install(DIRECTORY "${OBJC3_LIBOBJC2_SOURCE_DIR}/objc/"
    DESTINATION "${ARG_DESTINATION}/include/objc"
    COMPONENT Runtime
    FILES_MATCHING PATTERN "*.h")
  install(FILES "${OBJC3_LIBOBJC2_BUILD_DIR}/objc/objc-config.h"
    DESTINATION "${ARG_DESTINATION}/include/objc"
    COMPONENT Runtime)
  install(FILES
    "${_OBJC3_NATIVE_RUNTIME_REPO_ROOT}/scripts/objc3c_toolchain_runtime/runtime-profile.json"
    DESTINATION "${ARG_DESTINATION}/share/objc3c/runtime"
    COMPONENT Runtime)
  install(FILES "${OBJC3_LIBOBJC2_SOURCE_DIR}/COPYING"
    DESTINATION "${ARG_DESTINATION}/share/licenses/libobjc2"
    COMPONENT Runtime)
endfunction()
