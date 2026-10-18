if (NOT OBJC3C_CLANG_FOUNDATION_ROOT)
  message(FATAL_ERROR "A verified patched Clang SDK root is required")
endif()
file(REAL_PATH "${OBJC3C_CLANG_FOUNDATION_ROOT}" _objc3_sdk)
set(LLVM_DIR "${_objc3_sdk}/lib/cmake/llvm")
set(Clang_DIR "${_objc3_sdk}/lib/cmake/clang")
find_package(LLVM 23.1.3 EXACT REQUIRED CONFIG PATHS "${LLVM_DIR}" NO_DEFAULT_PATH)
find_package(Clang REQUIRED CONFIG PATHS "${Clang_DIR}" NO_DEFAULT_PATH)
file(REAL_PATH "${LLVM_DIR}" _objc3_llvm_config)
file(REAL_PATH "${Clang_DIR}" _objc3_clang_config)
cmake_path(IS_PREFIX _objc3_sdk "${_objc3_llvm_config}" NORMALIZE _objc3_llvm_within_sdk)
cmake_path(IS_PREFIX _objc3_sdk "${_objc3_clang_config}" NORMALIZE _objc3_clang_within_sdk)
if (NOT _objc3_llvm_within_sdk OR NOT _objc3_clang_within_sdk)
  message(FATAL_ERROR "LLVM and Clang must resolve from the same SDK root")
endif()
get_filename_component(_objc3_repo "${CMAKE_CURRENT_SOURCE_DIR}/../.." ABSOLUTE)
find_package(Python3 REQUIRED COMPONENTS Interpreter)
execute_process(
  COMMAND "${Python3_EXECUTABLE}" -B -m scripts.objc3c_toolchain verify-development
  WORKING_DIRECTORY "${_objc3_repo}"
  RESULT_VARIABLE _objc3_verify_exit
  OUTPUT_VARIABLE _objc3_source_identity
  ERROR_VARIABLE _objc3_verify_error)
if (NOT _objc3_verify_exit EQUAL 0)
  message(FATAL_ERROR "Clang source seal rejected: ${_objc3_verify_error}")
endif()
string(JSON OBJC3C_FOUNDATION_PATCH_DIGEST GET "${_objc3_source_identity}" patch_digest)
unset(_objc3_identity_header)
unset(_objc3_identity_header CACHE)
find_file(_objc3_identity_header ObjC3Foundation.h
  PATHS "${_objc3_sdk}/include/clang/Basic"
        "${_objc3_sdk}/tools/clang/include/clang/Basic"
  NO_DEFAULT_PATH REQUIRED)
file(READ "${_objc3_identity_header}" _objc3_compiled_identity)
if (NOT _objc3_compiled_identity MATCHES "0d261d1ca552c95a8f007e061c787ac7132fbcbc" OR
    NOT _objc3_compiled_identity MATCHES "${OBJC3C_FOUNDATION_PATCH_DIGEST}")
  message(FATAL_ERROR "Clang headers do not match the pinned source and patch stack")
endif()
unset(OBJC3C_FOUNDATION_DRIVER)
unset(OBJC3C_FOUNDATION_DRIVER CACHE)
find_program(OBJC3C_FOUNDATION_DRIVER NAMES clang
  PATHS "${_objc3_sdk}/bin" NO_DEFAULT_PATH REQUIRED)
file(TO_CMAKE_PATH "${OBJC3C_FOUNDATION_DRIVER}" OBJC3C_FOUNDATION_DRIVER)
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS
  "${OBJC3C_FOUNDATION_DRIVER}")
file(SHA256 "${OBJC3C_FOUNDATION_DRIVER}" OBJC3C_FOUNDATION_DRIVER_SHA256)
execute_process(
  COMMAND "${OBJC3C_FOUNDATION_DRIVER}" --version
  RESULT_VARIABLE _objc3_driver_exit
  OUTPUT_VARIABLE _objc3_driver_identity
  ERROR_VARIABLE _objc3_driver_error)
if (NOT _objc3_driver_exit EQUAL 0 OR
    NOT _objc3_driver_identity MATCHES "0d261d1ca552c95a8f007e061c787ac7132fbcbc" OR
    NOT _objc3_driver_identity MATCHES "ObjC3-patches=${OBJC3C_FOUNDATION_PATCH_DIGEST}")
  message(FATAL_ERROR "Clang executable does not match the sealed source and SDK headers: ${_objc3_driver_error}")
endif()
set(OBJC3C_FOUNDATION_RESOURCE_DIR "${_objc3_sdk}/lib/clang/23")
if (NOT EXISTS "${OBJC3C_FOUNDATION_RESOURCE_DIR}/include/stddef.h")
  message(FATAL_ERROR "Matching Clang resource headers are missing")
endif()
file(GLOB_RECURSE _objc3_resource_files CONFIGURE_DEPENDS LIST_DIRECTORIES FALSE
  "${OBJC3C_FOUNDATION_RESOURCE_DIR}/*")
list(SORT _objc3_resource_files)
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS
  ${_objc3_resource_files})
set(_objc3_resource_identity_input "")
foreach(_objc3_resource_file IN LISTS _objc3_resource_files)
  file(RELATIVE_PATH _objc3_resource_relative
    "${OBJC3C_FOUNDATION_RESOURCE_DIR}" "${_objc3_resource_file}")
  file(TO_CMAKE_PATH "${_objc3_resource_relative}" _objc3_resource_relative)
  file(SHA256 "${_objc3_resource_file}" _objc3_resource_sha256)
  string(LENGTH "${_objc3_resource_relative}" _objc3_resource_path_length)
  string(APPEND _objc3_resource_identity_input
    "${_objc3_resource_path_length}:${_objc3_resource_relative}${_objc3_resource_sha256}\n")
endforeach()
string(SHA256 OBJC3C_FOUNDATION_RESOURCE_SHA256
  "${_objc3_resource_identity_input}")
message(STATUS "ObjC3 pinned Clang patch digest: ${OBJC3C_FOUNDATION_PATCH_DIGEST}")

# Reconfigure when a frontend input changes, but embed its identity in one tiny
# translation unit so an edit does not rebuild every frontend source file.
file(GLOB_RECURSE _objc3_frontend_sources CONFIGURE_DEPENDS
  "${_objc3_repo}/native/objc3c/src/clang_foundation/*.cpp"
  "${_objc3_repo}/native/objc3c/src/clang_foundation/*.h")
file(GLOB _objc3_frontend_cmake CONFIGURE_DEPENDS "${_objc3_repo}/native/objc3c/cmake/*.cmake")
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS
  ${_objc3_frontend_sources} ${_objc3_frontend_cmake}
  "${_objc3_repo}/scripts/objc3c_toolchain/facade.py"
  "${_objc3_repo}/native/objc3c/CMakeLists.txt"
  "${_objc3_repo}/native/objc3c/src/clang_foundation/CMakeLists.txt")
execute_process(COMMAND "${Python3_EXECUTABLE}" -B -m scripts.objc3c_toolchain facade-identity
  WORKING_DIRECTORY "${_objc3_repo}" RESULT_VARIABLE _objc3_frontend_identity_exit
  OUTPUT_VARIABLE _objc3_frontend_identity ERROR_VARIABLE _objc3_frontend_identity_error)
if(NOT _objc3_frontend_identity_exit EQUAL 0)
  message(FATAL_ERROR "Cannot identify native frontend inputs: ${_objc3_frontend_identity_error}")
endif()
string(JSON OBJC3C_FRONTEND_INPUT_SHA256 GET "${_objc3_frontend_identity}" sha256)
