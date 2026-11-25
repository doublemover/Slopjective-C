include("${CMAKE_CURRENT_LIST_DIR}/../../cmake/FrontendTests.cmake")

if (MSVC)
  # Clang's generated option identifiers require standard macro expansion.
  target_compile_options(objc3c_build_defaults INTERFACE /Zc:preprocessor /Zc:__cplusplus)
endif()

add_library(objc3c_clang_foundation STATIC
  async_c_exports.cpp
  compile_session.cpp
  meta_package.cpp
  meta_program.cpp
  meta_compilation.cpp
  driver_native.cpp
  sdk_identity.cpp
  source_identity.cpp
  ast_contract_consumer.cpp
  interface_contract.cpp
  module_names.cpp
  interface_codec.cpp
  interface_emitter.cpp
  interface_emission_templates.cpp
  interface_emission_features.cpp
  interface_emission_identity.cpp
  interface_emission_dependencies.cpp
  runtime_interface_types.cpp
  interface_artifact_io.cpp
  interface_validation.cpp
  interface_import.cpp
  interface_module_resolver.cpp
  interface_source_import_bridge.cpp)
set_source_files_properties(source_identity.cpp PROPERTIES
  COMPILE_DEFINITIONS "OBJC3C_FRONTEND_INPUT_SHA256=\"${OBJC3C_FRONTEND_INPUT_SHA256}\"")
target_include_directories(objc3c_clang_foundation SYSTEM PRIVATE
  ${LLVM_INCLUDE_DIRS} ${CLANG_INCLUDE_DIRS})
target_link_libraries(objc3c_clang_foundation PUBLIC objc3c_build_defaults)
set(_runtime_services_header "${CMAKE_CURRENT_SOURCE_DIR}/../runtime/public/objc3_runtime_services.h")
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${_runtime_services_header}")
file(SHA256 "${_runtime_services_header}" _runtime_services_header_sha256)
file(READ "${_runtime_services_header}" _runtime_services_header_source)
set(_runtime_completion_header "${CMAKE_CURRENT_SOURCE_DIR}/../runtime/public/objc3_async_c.h")
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${_runtime_completion_header}")
file(SHA256 "${_runtime_completion_header}" _runtime_completion_header_sha256)
file(READ "${_runtime_completion_header}" _runtime_completion_header_source)
set(_runtime_task_header "${CMAKE_CURRENT_SOURCE_DIR}/../runtime/public/objc3_task.hpp")
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${_runtime_task_header}")
file(SHA256 "${_runtime_task_header}" _runtime_task_header_sha256)
file(READ "${_runtime_task_header}" _runtime_task_header_source)
set(_runtime_keypath_header "${CMAKE_CURRENT_SOURCE_DIR}/../runtime/public/objc3_keypath.h")
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${_runtime_keypath_header}")
file(READ "${_runtime_keypath_header}" _runtime_keypath_header_source)
# SDK module imports are header imports, not unpublished facade dependencies.
# Bind the allowed names to the exact public headers in the library inventory.
set(_stdlib_inventory "${_objc3_repo}/stdlib/module_inventory.json")
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${_stdlib_inventory}")
file(READ "${_stdlib_inventory}" _stdlib_inventory_json)
string(JSON _stdlib_module_count LENGTH "${_stdlib_inventory_json}" canonical_modules)
math(EXPR _stdlib_last_module "${_stdlib_module_count} - 1")
set(_stdlib_header_contracts "")
foreach(_stdlib_index RANGE 0 ${_stdlib_last_module})
  string(JSON _stdlib_module GET "${_stdlib_inventory_json}" canonical_modules ${_stdlib_index} module)
  string(JSON _stdlib_header GET "${_stdlib_inventory_json}" canonical_modules ${_stdlib_index} header)
  set(_stdlib_path "${_objc3_repo}/stdlib/${_stdlib_header}")
  set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${_stdlib_path}")
  file(SHA256 "${_stdlib_path}" _stdlib_header_sha256)
  string(APPEND _stdlib_header_contracts "  {\"${_stdlib_module}\", \"${_stdlib_header_sha256}\"},\n")
endforeach()
configure_file(runtime_interface_header.h.in runtime_interface_header.h @ONLY)
target_include_directories(objc3c_clang_foundation PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")
find_package(OpenSSL 3.5 REQUIRED COMPONENTS Crypto)
target_link_libraries(objc3c_clang_foundation PRIVATE OpenSSL::Crypto)
objc3c_link_clang_components(objc3c_clang_foundation
  clangTooling clangFrontend clangCodeGen clangIndex)
if(LLVM_LINK_LLVM_DYLIB)
  # Match Clang's shared LLVM dependency. Mixing component archives with that
  # dylib can duplicate LLVM globals and command-line option registrations.
  set(_objc3_native_codegen LLVM)
else()
  set(_objc3_codegen_components)
  foreach(_objc3_backend IN LISTS LLVM_TARGETS_TO_BUILD)
    if(NOT TARGET LLVM${_objc3_backend}CodeGen)
      message(FATAL_ERROR "The pinned LLVM SDK omits its configured ${_objc3_backend} code generator")
    endif()
    list(APPEND _objc3_codegen_components "${_objc3_backend}CodeGen")
    if(TARGET LLVM${_objc3_backend}AsmParser)
      list(APPEND _objc3_codegen_components "${_objc3_backend}AsmParser")
    endif()
  endforeach()
  if(NOT _objc3_codegen_components)
    message(FATAL_ERROR "The pinned LLVM SDK exports no configured code-generation backends")
  endif()
  llvm_map_components_to_libnames(_objc3_native_codegen ${_objc3_codegen_components})
endif()
target_link_libraries(objc3c_clang_foundation PRIVATE ${_objc3_native_codegen})
set_property(SOURCE compile_session.cpp driver_native.cpp APPEND PROPERTY COMPILE_DEFINITIONS
  "OBJC3C_FOUNDATION_DRIVER=\"${OBJC3C_FOUNDATION_DRIVER}\""
  "OBJC3C_FOUNDATION_DRIVER_SHA256=\"${OBJC3C_FOUNDATION_DRIVER_SHA256}\""
  "OBJC3C_FOUNDATION_PATCH_DIGEST=\"${OBJC3C_FOUNDATION_PATCH_DIGEST}\""
  "OBJC3C_FOUNDATION_RESOURCE_SHA256=\"${OBJC3C_FOUNDATION_RESOURCE_SHA256}\""
  "OBJC3C_FOUNDATION_RESOURCE_DIR=\"${OBJC3C_FOUNDATION_RESOURCE_DIR}\"")

add_library(objc3c_frontend_v2 STATIC frontend_api.cpp)
target_link_libraries(objc3c_frontend_v2 PUBLIC objc3c_clang_foundation)
add_executable(objc3c driver_main.cpp)
target_include_directories(objc3c SYSTEM PRIVATE
  ${LLVM_INCLUDE_DIRS} ${CLANG_INCLUDE_DIRS})
target_link_libraries(objc3c PRIVATE objc3c_clang_foundation)

# The compiler-profile qualifier executes paired expected-detection/clean
# outcomes while parsing source through this in-process frontend consumer.
if(OBJC3C_COMPILER_INSTRUMENTATION_PROFILE)
  add_executable(objc3_compiler_process_control EXCLUDE_FROM_ALL
    "${_objc3_repo}/tests/clang_foundation/compiler_instrumentation_process.cpp")
  target_include_directories(objc3_compiler_process_control SYSTEM PRIVATE
    ${LLVM_INCLUDE_DIRS} ${CLANG_INCLUDE_DIRS})
  target_link_libraries(objc3_compiler_process_control PRIVATE
    objc3c_build_defaults clangTooling clangFrontend ${_objc3_native_codegen})
endif()

# CMake's C++20 module scanning must use this SDK, including on fresh hosts.
# Build the upstream tool against the same qualified Clang/LLVM libraries.
option(OBJC3C_BUILD_SCAN_DEPS "Build the development clang-scan-deps tool" ON)
if(OBJC3C_BUILD_SCAN_DEPS)
set(_scan_source "${_objc3_repo}/out/toolchains/objc3-development/src/clang/tools/clang-scan-deps")
include("${_objc3_repo}/native/objc3c/cmake/Objc3LinuxHost.cmake")
set(_tablegen "${_objc3_repo}/out/toolchains/objc3-development/build-${OBJC3_LINUX_NATIVE_HOST}/bin/llvm-tblgen")
set(OBJC3C_TABLEGEN "" CACHE FILEPATH "Explicit same-version LLVM TableGen for frontend development")
if(OBJC3C_TABLEGEN)
  if(OBJC3C_COMPILER_INSTRUMENTATION_PROFILE)
    message(FATAL_ERROR "Compiler profiles require their own foundation TableGen")
  endif()
  file(REAL_PATH "${OBJC3C_TABLEGEN}" _tablegen)
elseif(OBJC3C_COMPILER_INSTRUMENTATION_PROFILE)
  string(JSON _profile_build GET "${_objc3_profile}" paths build)
  set(_tablegen "${_profile_build}/bin/llvm-tblgen")
endif()
if(NOT EXISTS "${_tablegen}")
  message(FATAL_ERROR "The sealed foundation build must provide llvm-tblgen")
endif()
execute_process(COMMAND "${_tablegen}" --version
  RESULT_VARIABLE _tablegen_exit OUTPUT_VARIABLE _tablegen_version ERROR_VARIABLE _tablegen_error)
string(FIND "${_tablegen_version}" "LLVM version ${LLVM_PACKAGE_VERSION}\n" _tablegen_version_at)
if(NOT _tablegen_exit EQUAL 0 OR _tablegen_version_at EQUAL -1)
  message(FATAL_ERROR "Frontend TableGen must match LLVM ${LLVM_PACKAGE_VERSION}: ${_tablegen_error}")
endif()
file(SHA256 "${_tablegen}" _tablegen_sha256)
message(STATUS "Frontend TableGen: ${_tablegen} (SHA-256 ${_tablegen_sha256})")
add_custom_command(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/Opts.inc"
  COMMAND "${_tablegen}" -gen-opt-parser-defs
    -I "${_objc3_repo}/out/toolchains/objc3-development/src/llvm/include"
    "${_scan_source}/Opts.td" -o "${CMAKE_CURRENT_BINARY_DIR}/Opts.inc"
  DEPENDS "${_scan_source}/Opts.td" "${_tablegen}"
  VERBATIM)
set(_objc3_scan_deps_target clang-scan-deps)
# A sealed Clang build tree exports its own clang-scan-deps target. Keep the
# product scanner distinct while retaining the installed executable name.
if(TARGET clang-scan-deps)
  set(_objc3_scan_deps_target objc3c-clang-scan-deps)
endif()
add_executable(${_objc3_scan_deps_target} "${_scan_source}/ClangScanDeps.cpp"
  scan_deps_main.cpp "${CMAKE_CURRENT_BINARY_DIR}/Opts.inc")
set_target_properties(${_objc3_scan_deps_target} PROPERTIES OUTPUT_NAME clang-scan-deps)
target_include_directories(${_objc3_scan_deps_target} SYSTEM PRIVATE
  "${CMAKE_CURRENT_BINARY_DIR}" ${LLVM_INCLUDE_DIRS} ${CLANG_INCLUDE_DIRS})
target_compile_features(${_objc3_scan_deps_target} PRIVATE cxx_std_20)
target_link_libraries(${_objc3_scan_deps_target} PRIVATE objc3c_build_defaults clangAST clangBasic clangDependencyScanning
  clangDriver clangFrontend clangLex clangSerialization clangTooling ${_objc3_native_codegen})
install(TARGETS ${_objc3_scan_deps_target} RUNTIME DESTINATION bin)
endif()

# The runtime can also be built independently of the compiler SDK.
add_subdirectory(../runtime "${CMAKE_CURRENT_BINARY_DIR}/runtime")

include(GNUInstallDirs)
install(TARGETS objc3c objc3c_frontend_v2 objc3c_clang_foundation objc3_runtime_services
  EXPORT objc3cTargets
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(TARGETS objc3c_build_defaults EXPORT objc3cTargets)
set_target_properties(objc3c_frontend_v2 PROPERTIES EXPORT_NAME Frontend)
set_target_properties(objc3_runtime_services PROPERTIES EXPORT_NAME RuntimeServices)
install(FILES frontend_api.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/objc3c)
install(DIRECTORY ../runtime/public/
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/objc3c/runtime
  FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp")
install(DIRECTORY "${_objc3_repo}/stdlib/include/objc3"
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(FILES "${_objc3_repo}/stdlib/module_inventory.json"
  DESTINATION share/objc3c/stdlib)
foreach(_feature_fixture explicit_capture_lists typed_keypaths native_c_async native_isolation task_spawning strict_async_profiles nonzero_nil_results isolated_properties native_frame_exceptions catch_binding_ownership runtime_scopes)
  install(DIRECTORY "${_objc3_repo}/tests/clang_foundation/${_feature_fixture}"
    DESTINATION share/objc3c/examples/linux-features
    FILES_MATCHING PATTERN "*.h" PATTERN "*.c" PATTERN "*.m" PATTERN "*.mm"
      PATTERN "*.cpp" PATTERN "README.md")
endforeach()
target_include_directories(objc3c_frontend_v2 INTERFACE "$<INSTALL_INTERFACE:include>")
install(EXPORT objc3cTargets NAMESPACE objc3c:: DESTINATION lib/cmake/objc3c)
include(CMakePackageConfigHelpers)
configure_package_config_file("${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/objc3cConfig.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/objc3cConfig.cmake" INSTALL_DESTINATION lib/cmake/objc3c)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/objc3cConfig.cmake" DESTINATION lib/cmake/objc3c)

if (BUILD_TESTING)
  set(_runtime_lifetime_source "${_objc3_repo}/native/objc3c/tests/runtime_lifetime_test.cpp")
  set(_runtime_lifetime_object "${CMAKE_CURRENT_BINARY_DIR}/runtime-lifetime.o")
  add_custom_command(OUTPUT "${_runtime_lifetime_object}"
    COMMAND "${OBJC3C_FOUNDATION_DRIVER}" --no-default-config -x c++ -std=c++20
      -O2 -g -UNDEBUG -include "${_objc3_repo}/native/objc3c/tests/test_assertion_policy.h"
      "-I${_objc3_repo}/native/objc3c/src"
      ${OBJC3C_COMPILER_INSTRUMENTATION_FLAGS}
      ${OBJC3C_SOURCE_COVERAGE_FLAGS}
      -c "${_runtime_lifetime_source}" -o "${_runtime_lifetime_object}"
    DEPENDS "${_runtime_lifetime_source}"
      "${_objc3_repo}/native/objc3c/tests/test_assertion_policy.h"
      "${CMAKE_CURRENT_SOURCE_DIR}/../runtime/public/objc3_task.hpp"
      "${CMAKE_CURRENT_SOURCE_DIR}/../runtime/public/objc3_runtime_services.h"
      "${OBJC3C_FOUNDATION_DRIVER}" VERBATIM)
  set_source_files_properties("${_runtime_lifetime_object}"
    PROPERTIES EXTERNAL_OBJECT TRUE GENERATED TRUE)
  objc3c_register_custom_object("${_runtime_lifetime_source}" "${_runtime_lifetime_object}")
  add_executable(objc3_runtime_lifetime_test "${_runtime_lifetime_object}")
  set_target_properties(objc3_runtime_lifetime_test PROPERTIES LINKER_LANGUAGE CXX)
  target_link_libraries(objc3_runtime_lifetime_test PRIVATE objc3_runtime_services)
  objc3c_add_frontend_test(NAME objc3_runtime_lifetime COMMAND objc3_runtime_lifetime_test)
  set_tests_properties(objc3_runtime_lifetime PROPERTIES TIMEOUT 20)
  add_executable(objc3_opaque_storage_transaction_test ../../tests/opaque_storage_transaction_test.cpp)
  target_link_libraries(objc3_opaque_storage_transaction_test PRIVATE objc3_runtime_services)
  target_link_options(objc3_opaque_storage_transaction_test PRIVATE "LINKER:--wrap=_Znwm")
  objc3c_add_frontend_test(NAME objc3_opaque_storage_transaction COMMAND objc3_opaque_storage_transaction_test)
  add_executable(objc3_assertion_control_test ../../tests/assertion_control_test.cpp)
  objc3c_add_frontend_test(NAME objc3_assertion_control COMMAND objc3_assertion_control_test)
  set_tests_properties(objc3_assertion_control PROPERTIES WILL_FAIL TRUE)
  add_executable(objc3_async_error_bridge_test
    ../../../../tests/clang_foundation/structured_runtime/error_bridge.cpp)
  target_link_libraries(objc3_async_error_bridge_test PRIVATE objc3_runtime_services)
  target_link_options(objc3_async_error_bridge_test PRIVATE
    "LINKER:--wrap=_ZnwmRKSt9nothrow_t"
    "LINKER:--wrap=_ZnwmSt11align_val_tRKSt9nothrow_t")
  objc3c_add_frontend_test(NAME objc3_async_error_bridge COMMAND objc3_async_error_bridge_test)
  add_executable(objc3_keypath_service_test ../../tests/keypath_service_test.cpp)
  target_link_libraries(objc3_keypath_service_test PRIVATE objc3_runtime_services)
  objc3c_add_frontend_test(NAME objc3_keypath_service COMMAND objc3_keypath_service_test)
  add_executable(objc3_aarch64_native_abi_test
    ../../../../tests/clang_foundation/aarch64_native_abi_test.cpp)
  target_include_directories(objc3_aarch64_native_abi_test SYSTEM PRIVATE
    ${LLVM_INCLUDE_DIRS} ${CLANG_INCLUDE_DIRS})
  target_link_libraries(objc3_aarch64_native_abi_test PRIVATE
    objc3c_clang_foundation objc3c_build_defaults)
  objc3c_add_frontend_test(NAME objc3_aarch64_native_abi COMMAND objc3_aarch64_native_abi_test)
  add_executable(objc3_keypath_native_ast_test
    ../../../../tests/clang_foundation/keypath_native_ast_test.cpp)
  target_compile_definitions(objc3_keypath_native_ast_test PRIVATE
    OBJC3_KEYPATH_TEST_REPO_ROOT="${_objc3_repo}")
  target_include_directories(objc3_keypath_native_ast_test SYSTEM PRIVATE
    ${LLVM_INCLUDE_DIRS} ${CLANG_INCLUDE_DIRS})
  target_link_libraries(objc3_keypath_native_ast_test PRIVATE
    clangAST clangBasic clangFrontend clangSerialization clangTooling)
  objc3c_add_frontend_test(NAME objc3_keypath_native_ast COMMAND objc3_keypath_native_ast_test)
  add_executable(objc3_stdlib_map_count_test ../../tests/stdlib_map_count_test.cpp
    ../runtime/stdlib/collections_service.cpp)
  target_compile_definitions(objc3_stdlib_map_count_test PRIVATE OBJC3_RUNTIME_I32_INDEX_TESTING)
  target_link_options(objc3_stdlib_map_count_test PRIVATE "LINKER:--wrap=_Znwm")
  target_link_libraries(objc3_stdlib_map_count_test PRIVATE objc3_runtime_services)
  objc3c_add_frontend_test(NAME objc3_stdlib_map_count COMMAND objc3_stdlib_map_count_test)
  add_executable(objc3_sdk_identity_test ../../../../tests/clang_foundation/sdk_identity_test.cpp)
  target_include_directories(objc3_sdk_identity_test SYSTEM PRIVATE ${LLVM_INCLUDE_DIRS} ${CLANG_INCLUDE_DIRS})
  target_link_libraries(objc3_sdk_identity_test PRIVATE objc3c_clang_foundation)
  objc3c_add_frontend_test(NAME objc3_sdk_identity COMMAND objc3_sdk_identity_test)
  add_executable(objc3_carrier_service_test ../../tests/carrier_service_test.cpp)
  target_link_libraries(objc3_carrier_service_test PRIVATE objc3_runtime_services)
  objc3c_add_frontend_test(NAME objc3_carrier_service COMMAND objc3_carrier_service_test)
  add_executable(objc3_interface_contract_test
    ../../../../tests/clang_foundation/interface_contract_test.cpp)
  target_link_libraries(objc3_interface_contract_test PRIVATE objc3c_clang_foundation)
  objc3c_add_frontend_test(NAME objc3_interface_contract COMMAND objc3_interface_contract_test)
  add_executable(objc3_interface_codec_test
    ../../../../tests/clang_foundation/interface_codec_test.cpp)
  target_include_directories(objc3_interface_codec_test SYSTEM PRIVATE
    ${LLVM_INCLUDE_DIRS} ${CLANG_INCLUDE_DIRS})
  target_link_libraries(objc3_interface_codec_test PRIVATE objc3c_clang_foundation)
  objc3c_add_frontend_test(NAME objc3_interface_codec COMMAND objc3_interface_codec_test)

  add_executable(objc3_codec_production_semantics_test
    ../../../../tests/clang_foundation/codec_production_semantics_test.cpp
    ../../../fuzz/CodecTarget.cpp)
  target_include_directories(objc3_codec_production_semantics_test SYSTEM PRIVATE
    ${LLVM_INCLUDE_DIRS} ${CLANG_INCLUDE_DIRS})
  target_include_directories(objc3_codec_production_semantics_test PRIVATE ../../../fuzz)
  target_compile_definitions(objc3_codec_production_semantics_test PRIVATE
    OBJC3_CODEC_TEST_RESOURCE_DIR="${OBJC3C_FOUNDATION_RESOURCE_DIR}")
  target_link_libraries(objc3_codec_production_semantics_test PRIVATE objc3c_clang_foundation)
  objc3c_link_clang_components(objc3_codec_production_semantics_test clangTooling)
  objc3c_add_frontend_test(NAME objc3_codec_production_semantics COMMAND objc3_codec_production_semantics_test)
  add_executable(objc3_interface_import_test
    ../../../../tests/clang_foundation/interface_import_test.cpp)
  target_include_directories(objc3_interface_import_test SYSTEM PRIVATE
    ${LLVM_INCLUDE_DIRS} ${CLANG_INCLUDE_DIRS})
  target_link_libraries(objc3_interface_import_test PRIVATE objc3c_clang_foundation)
  target_compile_definitions(objc3_interface_import_test PRIVATE
    OBJC3_INTERFACE_TEST_PACKAGE_FIXTURES="${_objc3_repo}/tests/clang_foundation/native_async_packages")
  objc3c_add_frontend_test(NAME objc3_interface_import COMMAND objc3_interface_import_test)
  add_executable(objc3_interface_import_api_test
    ../../../../tests/clang_foundation/interface_import_api_test.cpp)
  target_include_directories(objc3_interface_import_api_test SYSTEM PRIVATE
    ${LLVM_INCLUDE_DIRS} ${CLANG_INCLUDE_DIRS})
  target_link_libraries(objc3_interface_import_api_test PRIVATE objc3c_frontend_v2)
  objc3c_add_frontend_test(NAME objc3_interface_import_api COMMAND objc3_interface_import_api_test)
  add_executable(objc3_interface_module_resolution_test
    ../../../../tests/clang_foundation/interface_module_resolution_test.cpp)
  target_link_libraries(objc3_interface_module_resolution_test PRIVATE objc3c_frontend_v2)
  objc3c_add_frontend_test(NAME objc3_interface_module_resolution COMMAND objc3_interface_module_resolution_test)
  add_executable(objc3_interface_module_provenance_test
    ../../../../tests/clang_foundation/interface_module_provenance_test.cpp)
  target_include_directories(objc3_interface_module_provenance_test SYSTEM PRIVATE
    ${LLVM_INCLUDE_DIRS} ${CLANG_INCLUDE_DIRS})
  target_link_libraries(objc3_interface_module_provenance_test PRIVATE objc3c_clang_foundation)
  objc3c_link_clang_components(objc3_interface_module_provenance_test clangTooling clangFrontend clangAST)
  objc3c_add_frontend_test(NAME objc3_interface_module_provenance COMMAND objc3_interface_module_provenance_test)

  add_executable(objc3_interface_source_import_bridge_test
    ../../../../tests/clang_foundation/interface_source_import_bridge_test.cpp)
  target_include_directories(objc3_interface_source_import_bridge_test SYSTEM PRIVATE
    ${LLVM_INCLUDE_DIRS} ${CLANG_INCLUDE_DIRS})
  target_link_libraries(objc3_interface_source_import_bridge_test PRIVATE objc3c_clang_foundation)
  objc3c_link_clang_components(objc3_interface_source_import_bridge_test clangTooling clangFrontend clangAST)
  objc3c_add_frontend_test(NAME objc3_interface_source_import_bridge
    COMMAND objc3_interface_source_import_bridge_test)
  add_executable(objc3_compile_session_test
    ../../../../tests/clang_foundation/compile_session_test.cpp)
  target_link_libraries(objc3_compile_session_test PRIVATE objc3c_frontend_v2)
  objc3c_add_frontend_test(NAME objc3_compile_session COMMAND objc3_compile_session_test)
  add_executable(objc3_ownership_projection_test
    ../../../../tests/clang_foundation/ownership_projection_test.cpp)
  target_include_directories(objc3_ownership_projection_test SYSTEM PRIVATE
    ${CLANG_INCLUDE_DIRS})
  target_link_libraries(objc3_ownership_projection_test PRIVATE objc3c_clang_foundation)
  objc3c_link_clang_components(objc3_ownership_projection_test clangTooling)
  objc3c_add_frontend_test(NAME objc3_ownership_projection COMMAND objc3_ownership_projection_test)
  add_executable(objc3_module_method_names_test
    ../../../../tests/clang_foundation/module_method_names_test.cpp)
  # The installed SDK has no private LLDB source tree. Read the exact maintained
  # header from the checksum-locked patch, keeping the test on production bytes.
  find_package(Python3 COMPONENTS Interpreter REQUIRED)
  file(GLOB _objc3_debugger_patch_inputs "${_objc3_repo}/toolchains/llvm/patches/*.patch")
  set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS
    ${_objc3_debugger_patch_inputs} "${_objc3_repo}/toolchains/llvm/manifest.json"
    "${_objc3_repo}/scripts/objc3c_toolchain/debugger_test_header.py")
  execute_process(COMMAND "${Python3_EXECUTABLE}" -B -m scripts.objc3c_toolchain.debugger_test_header
    --repo "${_objc3_repo}" --output "${CMAKE_CURRENT_BINARY_DIR}/lldb-test-headers"
    WORKING_DIRECTORY "${_objc3_repo}" COMMAND_ERROR_IS_FATAL ANY)
  target_include_directories(objc3_module_method_names_test SYSTEM PRIVATE
    "${CMAKE_CURRENT_BINARY_DIR}/lldb-test-headers" ${LLVM_INCLUDE_DIRS})
  target_link_libraries(objc3_module_method_names_test PRIVATE LLVMSupport)
  objc3c_add_frontend_test(NAME objc3_module_method_names COMMAND objc3_module_method_names_test)

  add_executable(objc3_interface_module_names_test
    ../../../../tests/clang_foundation/interface_module_names_test.cpp)
  target_include_directories(objc3_interface_module_names_test SYSTEM PRIVATE
    ${LLVM_INCLUDE_DIRS} ${CLANG_INCLUDE_DIRS})
  target_link_libraries(objc3_interface_module_names_test PRIVATE objc3c_frontend_v2)
  objc3c_add_frontend_test(NAME objc3_interface_module_names COMMAND objc3_interface_module_names_test)
  add_executable(objc3_c_inline_provider_test
    ../../../../tests/clang_foundation/c_inline_provider_test.cpp)
  target_include_directories(objc3_c_inline_provider_test SYSTEM PRIVATE
    ${LLVM_INCLUDE_DIRS} ${CLANG_INCLUDE_DIRS})
  target_link_libraries(objc3_c_inline_provider_test PRIVATE objc3c_clang_foundation)
  if(OBJC3_LIBOBJC2_BUILD_DIR)
    set(_c_inline_runtime_paths "${OBJC3_LIBOBJC2_BUILD_DIR}/lib" "${OBJC3_LIBOBJC2_BUILD_DIR}")
  else()
    set(_c_inline_runtime_paths "${OBJC3C_CLANG_FOUNDATION_ROOT}/lib")
  endif()
  find_library(_c_inline_objc_runtime NAMES objc
    PATHS ${_c_inline_runtime_paths} NO_DEFAULT_PATH NO_CACHE REQUIRED)
  set(_c_inline_link_flags "${CMAKE_CXX_FLAGS} ${CMAKE_EXE_LINKER_FLAGS}")
  if(OBJC3C_COMPILER_INSTRUMENTATION_PROFILE)
    get_target_property(_profile_link_options objc3c_build_defaults INTERFACE_LINK_OPTIONS)
    list(JOIN _profile_link_options " " _profile_link_flags)
    string(APPEND _c_inline_link_flags " ${_profile_link_flags}")
  endif()
  string(REPLACE "\\" "\\\\" _c_inline_link_flags "${_c_inline_link_flags}")
  string(REPLACE "\"" "\\\"" _c_inline_link_flags "${_c_inline_link_flags}")
  string(REPLACE "\n" "\\n" _c_inline_link_flags "${_c_inline_link_flags}")
  target_compile_definitions(objc3_interface_import_test PRIVATE
    OBJC3_INTERFACE_TEST_OBJC_LIBRARY="${_c_inline_objc_runtime}")
  target_compile_definitions(objc3_c_inline_provider_test PRIVATE
    OBJC3_C_INLINE_TEST_LINKER="${CMAKE_CXX_COMPILER}"
    OBJC3_C_INLINE_TEST_OBJC_LIBRARY="${_c_inline_objc_runtime}"
    OBJC3_C_INLINE_TEST_RUNTIME_LIBRARY="$<TARGET_FILE:objc3_runtime_services>"
    OBJC3_C_INLINE_TEST_LINK_FLAGS="${_c_inline_link_flags}")
  add_dependencies(objc3_c_inline_provider_test objc3_runtime_services)
  objc3c_add_frontend_test(NAME objc3_c_inline_provider COMMAND objc3_c_inline_provider_test)
  add_executable(objc3_standard_library_identity_test
    ../../../../tests/clang_foundation/standard_library_identity_test.cpp)
  target_include_directories(objc3_standard_library_identity_test SYSTEM PRIVATE
    ${LLVM_INCLUDE_DIRS} ${CLANG_INCLUDE_DIRS})
  target_link_libraries(objc3_standard_library_identity_test PRIVATE objc3c_clang_foundation)
  objc3c_link_clang_components(objc3_standard_library_identity_test clangTooling)
  objc3c_add_frontend_test(NAME objc3_standard_library_identity COMMAND objc3_standard_library_identity_test)
  add_executable(objc3_standard_library_provider_test
    ../../../../tests/clang_foundation/standard_library_provider_test.cpp)
  add_executable(objc3_standard_library_customization_provider_test
    ../../../../tests/clang_foundation/standard_library_customization_provider_test.cpp)
  add_executable(objc3_native_deduced_group_provider_test
    ../../../../tests/clang_foundation/native_deduced_group_provider_test.cpp)
  add_executable(objc3_native_emission_policy_provider_test
    ../../../../tests/clang_foundation/native_emission_policy_provider_test.cpp)
  add_executable(objc3_native_standard_owned_payload_provider_test
    ../../../../tests/clang_foundation/native_standard_owned_payload_provider_test.cpp)
  add_executable(objc3_native_mrc_record_payload_provider_test
    ../../../../tests/clang_foundation/native_mrc_record_payload_provider_test.cpp)
  add_executable(objc3_native_storage_lifetime_provider_test
    ../../../../tests/clang_foundation/native_storage_lifetime_provider_test.cpp)
  add_executable(objc3_native_diagnostic_contract_provider_test
    ../../../../tests/clang_foundation/native_diagnostic_contract_provider_test.cpp)
  add_executable(objc3_native_union_template_provider_test
    ../../../../tests/clang_foundation/native_union_template_provider_test.cpp)
  add_executable(objc3_native_abi_tag_provider_test
    ../../../../tests/clang_foundation/native_abi_tag_provider_test.cpp)
  add_executable(objc3_native_indirect_family_provider_test
    ../../../../tests/clang_foundation/native_indirect_family_provider_test.cpp)
  add_executable(objc3_native_typestate_provider_test
    ../../../../tests/clang_foundation/native_typestate_provider_test.cpp)
  add_executable(objc3_native_friend_template_provider_test
    ../../../../tests/clang_foundation/native_friend_template_provider_test.cpp)
  add_executable(objc3_native_friend_class_provider_test
    ../../../../tests/clang_foundation/native_friend_class_provider_test.cpp)
  add_executable(objc3_native_control_flow_provider_test
    ../../../../tests/clang_foundation/native_control_flow_provider_test.cpp)
  add_executable(objc3_native_assembly_profile_provider_test
    ../../../../tests/clang_foundation/native_assembly_profile_provider_test.cpp)
  add_executable(objc3_native_c_overload_provider_test
    ../../../../tests/clang_foundation/native_c_overload_provider_test.cpp)
  add_executable(objc3_native_actor_receiver_provider_test
    ../../../../tests/clang_foundation/native_actor_receiver_provider_test.cpp)
  add_executable(objc3_native_function_policy_provider_test
    ../../../../tests/clang_foundation/native_function_policy_provider_test.cpp)
  add_executable(objc3_native_boundary_resource_provider_test
    ../../../../tests/clang_foundation/native_boundary_resource_provider_test.cpp)
  add_executable(objc3_native_function_effect_provider_test
    ../../../../tests/clang_foundation/native_function_effect_provider_test.cpp)
  add_executable(objc3_native_block_binding_provider_test
    ../../../../tests/clang_foundation/native_block_binding_provider_test.cpp)
  add_executable(objc3_native_deduction_guide_provider_test
    ../../../../tests/clang_foundation/native_deduction_guide_provider_test.cpp)
  add_executable(objc3_native_explicit_specifier_provider_test
    ../../../../tests/clang_foundation/native_explicit_specifier_provider_test.cpp)
  add_executable(objc3_native_object_policy_provider_test
    ../../../../tests/clang_foundation/native_object_policy_provider_test.cpp)
  add_executable(objc3_native_availability_provider_test
    ../../../../tests/clang_foundation/native_availability_provider_test.cpp)
  add_executable(objc3_native_object_expression_provider_test
    ../../../../tests/clang_foundation/native_object_expression_provider_test.cpp)
  add_executable(objc3_native_external_binding_provider_test
    ../../../../tests/clang_foundation/native_external_binding_provider_test.cpp)
  add_executable(objc3_native_lifetime_annotation_provider_test
    ../../../../tests/clang_foundation/native_lifetime_annotation_provider_test.cpp)
  foreach(_standard_library_test IN ITEMS objc3_native_deduction_guide_provider_test objc3_native_explicit_specifier_provider_test objc3_native_object_policy_provider_test objc3_native_availability_provider_test objc3_native_object_expression_provider_test objc3_native_external_binding_provider_test objc3_native_block_binding_provider_test objc3_native_lifetime_annotation_provider_test objc3_native_function_effect_provider_test objc3_native_boundary_resource_provider_test objc3_native_function_policy_provider_test objc3_native_actor_receiver_provider_test objc3_native_c_overload_provider_test objc3_native_assembly_profile_provider_test objc3_native_control_flow_provider_test objc3_standard_library_provider_test objc3_native_typestate_provider_test objc3_native_friend_template_provider_test objc3_native_friend_class_provider_test
      objc3_standard_library_customization_provider_test objc3_native_indirect_family_provider_test objc3_native_abi_tag_provider_test objc3_native_union_template_provider_test objc3_native_diagnostic_contract_provider_test objc3_native_storage_lifetime_provider_test objc3_native_emission_policy_provider_test objc3_native_deduced_group_provider_test objc3_native_mrc_record_payload_provider_test objc3_native_standard_owned_payload_provider_test)
    target_include_directories(${_standard_library_test} SYSTEM PRIVATE
      ${LLVM_INCLUDE_DIRS} ${CLANG_INCLUDE_DIRS})
    target_link_libraries(${_standard_library_test} PRIVATE objc3c_clang_foundation)
    target_compile_definitions(${_standard_library_test} PRIVATE
      OBJC3_STANDARD_LIBRARY_TEST_LINKER="${CMAKE_CXX_COMPILER}"
      OBJC3_STANDARD_LIBRARY_TEST_OBJC_LIBRARY="${_c_inline_objc_runtime}"
      OBJC3_STANDARD_LIBRARY_TEST_RESOURCE_DIR="${OBJC3C_FOUNDATION_RESOURCE_DIR}"
      OBJC3_STANDARD_LIBRARY_TEST_LINK_FLAGS="${_c_inline_link_flags}")
  endforeach()
  target_compile_definitions(objc3_native_block_binding_provider_test PRIVATE
    OBJC3_BLOCK_BINDING_RUNTIME_HEADERS="${CMAKE_CURRENT_SOURCE_DIR}/../runtime/public"
    OBJC3_BLOCK_BINDING_RUNTIME_LIBRARY="$<TARGET_FILE:objc3_runtime_services>")
  add_dependencies(objc3_native_block_binding_provider_test objc3_runtime_services)
  target_compile_definitions(objc3_native_boundary_resource_provider_test PRIVATE
    OBJC3_BOUNDARY_RUNTIME_HEADERS="${CMAKE_CURRENT_SOURCE_DIR}/../runtime/public"
    OBJC3_BOUNDARY_RUNTIME_LIBRARY="$<TARGET_FILE:objc3_runtime_services>")
  add_dependencies(objc3_native_boundary_resource_provider_test objc3_runtime_services)
  target_compile_definitions(objc3_native_actor_receiver_provider_test PRIVATE
    OBJC3_ACTOR_BINDING_RUNTIME_HEADERS="${CMAKE_CURRENT_SOURCE_DIR}/../runtime/public"
    OBJC3_ACTOR_BINDING_RUNTIME_LIBRARY="$<TARGET_FILE:objc3_runtime_services>")
  add_dependencies(objc3_native_actor_receiver_provider_test objc3_runtime_services)
  target_compile_definitions(objc3_native_deduced_group_provider_test PRIVATE
    OBJC3_GROUP_TEST_FIXTURES="${_objc3_repo}/tests/clang_foundation/task_spawning"
    OBJC3_GROUP_TEST_STDLIB="${_objc3_repo}/stdlib/include"
    OBJC3_GROUP_TEST_RUNTIME_HEADERS="${CMAKE_CURRENT_SOURCE_DIR}/../runtime/public"
    OBJC3_GROUP_TEST_RUNTIME_LIBRARY="$<TARGET_FILE:objc3_runtime_services>")
  add_dependencies(objc3_native_deduced_group_provider_test objc3_runtime_services)
  target_compile_definitions(objc3_native_standard_owned_payload_provider_test PRIVATE
    OBJC3_STANDARD_OWNED_RUNTIME_LIBRARY="$<TARGET_FILE:objc3_runtime_services>"
    OBJC3_STANDARD_OWNED_RUNTIME_HEADERS="${CMAKE_CURRENT_SOURCE_DIR}/../runtime/public")
  add_dependencies(objc3_native_standard_owned_payload_provider_test objc3_runtime_services)
  foreach(_owned_standard IN ITEMS c++20 c++23)
    if(_owned_standard STREQUAL "c++20")
      set(_owned_optimization -O0)
    else()
      set(_owned_optimization -O2)
    endif()
    foreach(_owned_mode IN ITEMS 1 2)
      string(REPLACE "++" "xx" _owned_label "${_owned_standard}")
      set(_owned_name "objc3_native_standard_owned_${_owned_label}_${_owned_mode}")
      objc3c_add_frontend_test(NAME "${_owned_name}"
        COMMAND objc3_native_standard_owned_payload_provider_test
          "${_owned_standard}" "${_owned_mode}" "${_owned_optimization}")
      set_tests_properties("${_owned_name}" PROPERTIES
        LABELS "native;standard-library;ownership;extended")
    endforeach()
  endforeach()
  target_compile_definitions(objc3_native_mrc_record_payload_provider_test PRIVATE
    OBJC3_MRC_TEST_RUNTIME_LIBRARY="$<TARGET_FILE:objc3_runtime_services>"
    OBJC3_MRC_TEST_RUNTIME_HEADERS="${CMAKE_CURRENT_SOURCE_DIR}/../runtime/public")
  add_dependencies(objc3_native_mrc_record_payload_provider_test objc3_runtime_services)
  objc3c_add_frontend_test(NAME objc3_native_deduced_group_provider
    COMMAND objc3_native_deduced_group_provider_test)
  set_tests_properties(objc3_native_deduced_group_provider PROPERTIES
    LABELS "native;task-group;extended")
  objc3c_add_frontend_test(NAME objc3_native_emission_policy_provider
    COMMAND objc3_native_emission_policy_provider_test)
  set_tests_properties(objc3_native_emission_policy_provider PROPERTIES
    LABELS "native;emission-policy;extended")
  objc3c_add_frontend_test(NAME objc3_native_mrc_record_payload_provider
    COMMAND objc3_native_mrc_record_payload_provider_test)
  set_tests_properties(objc3_native_mrc_record_payload_provider PROPERTIES
    LABELS "native;mrc;carrier;extended")
  objc3c_add_frontend_test(NAME objc3_native_storage_lifetime_provider
    COMMAND objc3_native_storage_lifetime_provider_test)
  set_tests_properties(objc3_native_storage_lifetime_provider PROPERTIES
    LABELS "native;lifetime;extended")
  objc3c_add_frontend_test(NAME objc3_native_diagnostic_contract_provider
    COMMAND objc3_native_diagnostic_contract_provider_test)
  set_tests_properties(objc3_native_diagnostic_contract_provider PROPERTIES
    LABELS "native;publication;extended")
  objc3c_add_frontend_test(NAME objc3_native_union_template_provider
    COMMAND objc3_native_union_template_provider_test)
  set_tests_properties(objc3_native_union_template_provider PROPERTIES
    LABELS "native;publication;extended")
  objc3c_add_frontend_test(NAME objc3_native_abi_tag_provider
    COMMAND objc3_native_abi_tag_provider_test)
  set_tests_properties(objc3_native_abi_tag_provider PROPERTIES
    LABELS "native;publication;extended")
  objc3c_add_frontend_test(NAME objc3_native_indirect_family_provider
    COMMAND objc3_native_indirect_family_provider_test)
  set_tests_properties(objc3_native_indirect_family_provider PROPERTIES
    LABELS "native;ownership;extended")
  objc3c_add_frontend_test(NAME objc3_native_typestate_provider
    COMMAND objc3_native_typestate_provider_test)
  set_tests_properties(objc3_native_typestate_provider PROPERTIES
    LABELS "native;publication;extended")
  objc3c_add_frontend_test(NAME objc3_native_friend_template_provider
    COMMAND objc3_native_friend_template_provider_test)
  set_tests_properties(objc3_native_friend_template_provider PROPERTIES
    LABELS "native;publication;extended")
  foreach(_block_cxx IN ITEMS 20 23)
    foreach(_block_mode IN ITEMS arc mrc)
      if(_block_mode STREQUAL "arc")
        set(_block_ownership 2)
      else()
        set(_block_ownership 1)
      endif()
      set(_block_test "objc3_native_block_binding_provider_cxx${_block_cxx}_${_block_mode}")
      objc3c_add_frontend_test(NAME "${_block_test}"
        COMMAND objc3_native_block_binding_provider_test "c++${_block_cxx}" "${_block_ownership}")
      set_tests_properties("${_block_test}" PROPERTIES
        LABELS "native;ownership;concurrency;extended")
    endforeach()
  endforeach()
  objc3c_add_frontend_test(NAME objc3_native_deduction_guide_provider
    COMMAND objc3_native_deduction_guide_provider_test)
  set_tests_properties(objc3_native_deduction_guide_provider PROPERTIES
    LABELS "native;publication;extended")
  objc3c_add_frontend_test(NAME objc3_native_explicit_specifier_provider
    COMMAND objc3_native_explicit_specifier_provider_test)
  set_tests_properties(objc3_native_explicit_specifier_provider PROPERTIES
    LABELS "native;publication;extended")
  objc3c_add_frontend_test(NAME objc3_native_object_policy_provider
    COMMAND objc3_native_object_policy_provider_test)
  set_tests_properties(objc3_native_object_policy_provider PROPERTIES
    LABELS "native;publication;interop;ownership;extended")
  objc3c_add_frontend_test(NAME objc3_native_availability_provider
    COMMAND objc3_native_availability_provider_test)
  set_tests_properties(objc3_native_availability_provider PROPERTIES
    LABELS "native;publication;interop;ownership;extended")
  objc3c_add_frontend_test(NAME objc3_native_object_expression_provider
    COMMAND objc3_native_object_expression_provider_test)
  set_tests_properties(objc3_native_object_expression_provider PROPERTIES
    LABELS "native;publication;interop;ownership;extended")
  objc3c_add_frontend_test(NAME objc3_native_external_binding_provider
    COMMAND objc3_native_external_binding_provider_test)
  set_tests_properties(objc3_native_external_binding_provider PROPERTIES
    LABELS "native;publication;interop;extended")
  objc3c_add_frontend_test(NAME objc3_native_lifetime_annotation_provider
    COMMAND objc3_native_lifetime_annotation_provider_test)
  set_tests_properties(objc3_native_lifetime_annotation_provider PROPERTIES
    LABELS "native;publication;ownership;extended")
  objc3c_add_frontend_test(NAME objc3_native_function_effect_provider
    COMMAND objc3_native_function_effect_provider_test)
  set_tests_properties(objc3_native_function_effect_provider PROPERTIES
    LABELS "native;publication;extended")
  foreach(_boundary_standard IN ITEMS 20 23)
    foreach(_boundary_mode IN ITEMS 1 2)
      set(_boundary_test "objc3_native_boundary_resource_provider_cxx${_boundary_standard}_${_boundary_mode}")
      objc3c_add_frontend_test(NAME "${_boundary_test}"
        COMMAND objc3_native_boundary_resource_provider_test "c++${_boundary_standard}" "${_boundary_mode}")
      set_tests_properties("${_boundary_test}" PROPERTIES
        LABELS "native;ownership;isolation;extended")
    endforeach()
  endforeach()
  objc3c_add_frontend_test(NAME objc3_native_function_policy_provider
    COMMAND objc3_native_function_policy_provider_test)
  objc3c_add_frontend_test(NAME objc3_native_actor_receiver_provider
    COMMAND objc3_native_actor_receiver_provider_test)
  objc3c_add_frontend_test(NAME objc3_native_c_overload_provider
    COMMAND objc3_native_c_overload_provider_test)
  objc3c_add_frontend_test(NAME objc3_native_assembly_profile_provider
    COMMAND objc3_native_assembly_profile_provider_test)
  objc3c_add_frontend_test(NAME objc3_native_control_flow_provider
    COMMAND objc3_native_control_flow_provider_test)
  objc3c_add_frontend_test(NAME objc3_native_friend_class_provider
    COMMAND objc3_native_friend_class_provider_test)
  set_tests_properties(objc3_native_friend_class_provider PROPERTIES
    LABELS "native;publication;extended")
  objc3c_add_frontend_test(NAME objc3_standard_library_customization_provider
    COMMAND objc3_standard_library_customization_provider_test)
  set_tests_properties(objc3_standard_library_customization_provider PROPERTIES
    LABELS "native;standard-library;extended")
  objc3c_add_frontend_test(NAME objc3_standard_library_provider COMMAND objc3_standard_library_provider_test)
  objc3c_add_frontend_test(NAME objc3_standard_library_no_rtti_exceptions
    COMMAND objc3_standard_library_provider_test -fno-exceptions -fno-rtti)
  set_tests_properties(objc3_standard_library_no_rtti_exceptions PROPERTIES
    LABELS "native;standard-library;extended")
  objc3c_add_frontend_test(NAME objc3_standard_library_assertions
    COMMAND objc3_standard_library_provider_test -D_GLIBCXX_ASSERTIONS=1)
  set_tests_properties(objc3_standard_library_assertions PROPERTIES
    LABELS "native;standard-library;extended")
  objc3c_add_frontend_test(NAME objc3_standard_library_libstdcxx_debug
    COMMAND objc3_standard_library_provider_test -stdlib=libstdc++ -D_GLIBCXX_DEBUG=1)
  set_tests_properties(objc3_standard_library_libstdcxx_debug PROPERTIES
    LABELS "native;standard-library;extended")
  # Keep synchronization independently scheduled; combining both package
  # inventories approaches the baseline timeout before compiler instrumentation.
  objc3c_add_frontend_test(NAME objc3_standard_library_synchronization
    COMMAND objc3_standard_library_provider_test --synchronization)
  objc3c_add_frontend_test(NAME objc3_standard_library_synchronization_no_rtti_exceptions
    COMMAND objc3_standard_library_provider_test --synchronization -fno-exceptions -fno-rtti)
  objc3c_add_frontend_test(NAME objc3_standard_library_synchronization_assertions
    COMMAND objc3_standard_library_provider_test --synchronization -D_GLIBCXX_ASSERTIONS=1)
  set_tests_properties(objc3_standard_library_synchronization
    objc3_standard_library_synchronization_no_rtti_exceptions
    objc3_standard_library_synchronization_assertions PROPERTIES
    LABELS "native;standard-library;extended")
  set(OBJC3_TEST_LIBCXX_ROOT "" CACHE PATH
    "Optional libc++ installation for native standard-library interoperability tests")
  if(OBJC3_TEST_LIBCXX_ROOT)
    if(NOT EXISTS "${OBJC3_TEST_LIBCXX_ROOT}/include/c++/v1/__config" OR
        NOT IS_DIRECTORY "${OBJC3_TEST_LIBCXX_ROOT}/lib")
      message(FATAL_ERROR "OBJC3_TEST_LIBCXX_ROOT must contain libc++ headers and libraries")
    endif()
    objc3c_add_frontend_test(NAME objc3_standard_library_libcxx
      COMMAND objc3_standard_library_provider_test
        -stdlib=libc++ -nostdinc++ -isystem "${OBJC3_TEST_LIBCXX_ROOT}/include/c++/v1"
        "-L${OBJC3_TEST_LIBCXX_ROOT}/lib" "-Wl,-rpath,${OBJC3_TEST_LIBCXX_ROOT}/lib")
    set_tests_properties(objc3_standard_library_libcxx PROPERTIES
      LABELS "native;standard-library;extended")
    objc3c_add_frontend_test(NAME objc3_standard_library_libcxx_synchronization
      COMMAND objc3_standard_library_provider_test --synchronization --thread-annotations
        -stdlib=libc++ -nostdinc++ -isystem "${OBJC3_TEST_LIBCXX_ROOT}/include/c++/v1"
        "-L${OBJC3_TEST_LIBCXX_ROOT}/lib" "-Wl,-rpath,${OBJC3_TEST_LIBCXX_ROOT}/lib")
    set_tests_properties(objc3_standard_library_libcxx_synchronization PROPERTIES
      LABELS "native;standard-library;extended")
    objc3c_add_frontend_test(NAME objc3_standard_library_libcxx_customizations
      COMMAND objc3_standard_library_customization_provider_test
        -stdlib=libc++ -nostdinc++ -isystem "${OBJC3_TEST_LIBCXX_ROOT}/include/c++/v1"
        "-L${OBJC3_TEST_LIBCXX_ROOT}/lib" "-Wl,-rpath,${OBJC3_TEST_LIBCXX_ROOT}/lib")
    set_tests_properties(objc3_standard_library_libcxx_customizations PROPERTIES
      LABELS "native;standard-library;extended")
  endif()
  add_executable(objc3_borrowed_pointers_test
    ../../../../tests/clang_foundation/borrowed_pointers_test.cpp)
  target_include_directories(objc3_borrowed_pointers_test SYSTEM PRIVATE
    ${LLVM_INCLUDE_DIRS} ${CLANG_INCLUDE_DIRS})
  target_link_libraries(objc3_borrowed_pointers_test PRIVATE objc3c_clang_foundation)
  objc3c_add_frontend_test(NAME objc3_borrowed_pointers COMMAND objc3_borrowed_pointers_test)
  add_executable(objc3_interface_spi_test
    ../../../../tests/clang_foundation/interface_spi_test.cpp)
  target_include_directories(objc3_interface_spi_test SYSTEM PRIVATE
    ${LLVM_INCLUDE_DIRS} ${CLANG_INCLUDE_DIRS})
  target_link_libraries(objc3_interface_spi_test PRIVATE objc3c_frontend_v2)
  objc3c_add_frontend_test(NAME objc3_interface_spi COMMAND objc3_interface_spi_test)
  add_executable(objc3_interface_availability_test
    ../../../../tests/clang_foundation/interface_availability_test.cpp)
  target_include_directories(objc3_interface_availability_test SYSTEM PRIVATE
    ${LLVM_INCLUDE_DIRS} ${CLANG_INCLUDE_DIRS})
  target_link_libraries(objc3_interface_availability_test PRIVATE objc3c_clang_foundation)
  objc3c_add_frontend_test(NAME objc3_interface_availability COMMAND objc3_interface_availability_test)
  add_executable(objc3_interface_exports_test
    ../../../../tests/clang_foundation/interface_exports_test.cpp)
  target_include_directories(objc3_interface_exports_test SYSTEM PRIVATE
    ${LLVM_INCLUDE_DIRS} ${CLANG_INCLUDE_DIRS})
  target_link_libraries(objc3_interface_exports_test PRIVATE objc3c_clang_foundation)
  objc3c_add_frontend_test(NAME objc3_interface_exports COMMAND objc3_interface_exports_test)
  add_executable(objc3_interface_emitter_test
    ../../../../tests/clang_foundation/interface_emitter_test.cpp)
  target_compile_options(objc3_interface_emitter_test PRIVATE -Werror=unused-function)
  target_include_directories(objc3_interface_emitter_test SYSTEM PRIVATE
    ${LLVM_INCLUDE_DIRS} ${CLANG_INCLUDE_DIRS})
  target_link_libraries(objc3_interface_emitter_test PRIVATE objc3c_clang_foundation)
  objc3c_link_clang_components(objc3_interface_emitter_test clangTooling)
  target_compile_definitions(objc3_interface_emitter_test PRIVATE
    OBJC3_INTERFACE_TEST_PACKAGE_FIXTURES="${_objc3_repo}/tests/clang_foundation/native_async_packages"
    OBJC3_INTERFACE_TEST_STDLIB_HEADERS="${_objc3_repo}/stdlib/include/objc3"
    OBJC3_INTERFACE_TEST_RESOURCE_DIR="${OBJC3C_FOUNDATION_RESOURCE_DIR}")
  objc3c_add_frontend_test(NAME objc3_interface_emitter COMMAND objc3_interface_emitter_test)
  if(OBJC3C_COMPILER_INSTRUMENTATION_PROFILE STREQUAL "compiler-source-coverage")
    set(_objc3_coverage_policy "${_objc3_repo}/toolchains/llvm/source-coverage.json")
    set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${_objc3_coverage_policy}")
    file(READ "${_objc3_coverage_policy}" _objc3_coverage_json)
    string(JSON _objc3_emitter_shards GET "${_objc3_coverage_json}" emitter_shards)
    if(_objc3_emitter_shards LESS 8 OR _objc3_emitter_shards GREATER 64)
      message(FATAL_ERROR "Emitter coverage requires 8..64 bounded shards")
    endif()
    math(EXPR _objc3_emitter_last "${_objc3_emitter_shards} - 1")
    foreach(_objc3_emitter_shard RANGE 0 ${_objc3_emitter_last})
      objc3c_add_frontend_test(NAME objc3_interface_emitter_coverage_${_objc3_emitter_shard}
        COMMAND objc3_interface_emitter_test coverage-shard-${_objc3_emitter_shard}-of-${_objc3_emitter_shards})
    endforeach()
  endif()
  objc3c_add_frontend_test(NAME objc3_standard_library_headers
    COMMAND objc3_interface_emitter_test standard-library-headers)
  objc3c_add_frontend_test(NAME objc3_standard_library_specializations
    COMMAND objc3_interface_emitter_test standard-library-specializations)
  set_tests_properties(objc3_standard_library_headers objc3_standard_library_specializations
    PROPERTIES LABELS "native;standard-library;extended")
  add_executable(objc3_isolation_contracts_test
    ../../../../tests/clang_foundation/isolation_contracts_test.cpp)
  target_include_directories(objc3_isolation_contracts_test SYSTEM PRIVATE
    ${LLVM_INCLUDE_DIRS} ${CLANG_INCLUDE_DIRS})
  target_compile_definitions(objc3_isolation_contracts_test PRIVATE
    OBJC3_ISOLATION_RUNTIME_HEADERS="${CMAKE_CURRENT_SOURCE_DIR}/../runtime/public")
  target_link_libraries(objc3_isolation_contracts_test PRIVATE objc3c_clang_foundation)
  objc3c_link_clang_components(objc3_isolation_contracts_test clangTooling)
  objc3c_add_frontend_test(NAME objc3_isolation_contracts COMMAND objc3_isolation_contracts_test)
  add_executable(objc3_native_global_provenance_test
    ../../../../tests/clang_foundation/native_global_provenance_test.cpp)
  target_include_directories(objc3_native_global_provenance_test SYSTEM PRIVATE
    ${LLVM_INCLUDE_DIRS} ${CLANG_INCLUDE_DIRS})
  target_compile_definitions(objc3_native_global_provenance_test PRIVATE
    OBJC3_GLOBAL_RUNTIME_HEADERS="${CMAKE_CURRENT_SOURCE_DIR}/../runtime/public")
  target_link_libraries(objc3_native_global_provenance_test PRIVATE objc3c_clang_foundation)
  foreach(_provenance_standard c17 cxx20 cxx23)
    string(REPLACE "cxx" "c++" _provenance_argument "${_provenance_standard}")
    foreach(_provenance_ownership arc mrc)
      set(_provenance_name "objc3_native_global_provenance_${_provenance_standard}_${_provenance_ownership}")
      objc3c_add_frontend_test(NAME "${_provenance_name}"
        COMMAND objc3_native_global_provenance_test "${_provenance_argument}" "${_provenance_ownership}")
    endforeach()
  endforeach()
  add_executable(objc3_explicit_capture_lists_test
    ../../../../tests/clang_foundation/explicit_capture_lists_test.cpp)
  target_include_directories(objc3_explicit_capture_lists_test SYSTEM PRIVATE
    ${LLVM_INCLUDE_DIRS} ${CLANG_INCLUDE_DIRS})
  target_link_libraries(objc3_explicit_capture_lists_test PRIVATE objc3c_clang_foundation)
  objc3c_add_frontend_test(NAME objc3_explicit_capture_lists COMMAND objc3_explicit_capture_lists_test)
  add_executable(objc3_interface_artifact_io_test
    ../../../../tests/clang_foundation/interface_artifact_io_test.cpp)
  target_include_directories(objc3_interface_artifact_io_test SYSTEM PRIVATE
    ${LLVM_INCLUDE_DIRS} ${CLANG_INCLUDE_DIRS})
  target_link_libraries(objc3_interface_artifact_io_test PRIVATE objc3c_clang_foundation)
  objc3c_add_frontend_test(NAME objc3_interface_artifact_io COMMAND objc3_interface_artifact_io_test)
  add_executable(objc3_task_services_test
    ../../../../tests/tooling/fixtures/clang_foundation/services/task_services.cpp
    ../../../../tests/clang_foundation/structured_runtime/actor_scheduling.cpp
    ../runtime/concurrency/service_group.cpp)
  # Only this test translation unit enables deterministic fault hooks; the
  # production service library keeps them out of its ABI and implementation.
  target_compile_definitions(objc3_task_services_test PRIVATE OBJC3_RUNTIME_SERVICE_TESTING)
  target_link_libraries(objc3_task_services_test PRIVATE objc3_runtime_services)
  objc3c_add_frontend_test(NAME objc3_task_services COMMAND objc3_task_services_test)
  set_tests_properties(objc3_task_services PROPERTIES TIMEOUT 30)
  add_executable(objc3_structured_runtime_test
    ../../../../tests/clang_foundation/structured_runtime/worker.cpp)
  target_link_libraries(objc3_structured_runtime_test PRIVATE objc3_runtime_services)
  objc3c_add_frontend_test(NAME objc3_structured_runtime COMMAND objc3_structured_runtime_test)
  set_tests_properties(objc3_structured_runtime PROPERTIES TIMEOUT 30)
  # Task headers require the product compiler's canonical type-identity builtin.
  # Keep the compiler implementation bootstrapped independently, while compiling
  # these actual language witnesses with the verified foundation driver.
  set(_task_coroutine_objects)
  foreach(_part provider client)
    set(_object "${CMAKE_CURRENT_BINARY_DIR}/task-coroutines-${_part}.o")
    set(_source "${_objc3_repo}/tests/clang_foundation/task_coroutines/${_part}.cpp")
    add_custom_command(OUTPUT "${_object}"
      COMMAND "${OBJC3C_FOUNDATION_DRIVER}" --no-default-config -x c++ -std=c++20
        -O2 -g -UNDEBUG -include "${_objc3_repo}/native/objc3c/tests/test_assertion_policy.h" -fcoro-aligned-allocation
        "-I${CMAKE_CURRENT_SOURCE_DIR}/../runtime/public"
        ${OBJC3C_COMPILER_INSTRUMENTATION_FLAGS}
        ${OBJC3C_SOURCE_COVERAGE_FLAGS}
        -c "${_source}" -o "${_object}"
      DEPENDS "${_objc3_repo}/native/objc3c/tests/test_assertion_policy.h" "${_source}"
        "${_objc3_repo}/tests/clang_foundation/task_coroutines/provider.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/../runtime/public/objc3_task.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/../runtime/public/objc3_runtime_services.h"
        "${OBJC3C_FOUNDATION_DRIVER}"
      VERBATIM)
    set_source_files_properties("${_object}" PROPERTIES EXTERNAL_OBJECT TRUE GENERATED TRUE)
    objc3c_register_custom_object("${_source}" "${_object}")
    list(APPEND _task_coroutine_objects "${_object}")
  endforeach()
  add_executable(objc3_task_coroutines_test ${_task_coroutine_objects})
  set_target_properties(objc3_task_coroutines_test PROPERTIES LINKER_LANGUAGE CXX)
  target_link_libraries(objc3_task_coroutines_test PRIVATE objc3_runtime_services)
  objc3c_add_frontend_test(NAME objc3_task_coroutines COMMAND objc3_task_coroutines_test)
  set_tests_properties(objc3_task_coroutines PROPERTIES TIMEOUT 45)
  set(_task_group_await_source
    "${_objc3_repo}/tests/clang_foundation/task_coroutines/group_await.cpp")
  set(_task_group_await_object "${CMAKE_CURRENT_BINARY_DIR}/task-group-await.o")
  add_custom_command(OUTPUT "${_task_group_await_object}"
    COMMAND "${OBJC3C_FOUNDATION_DRIVER}" --no-default-config -x c++ -std=c++20
      -O2 -g -UNDEBUG -include "${_objc3_repo}/native/objc3c/tests/test_assertion_policy.h" -fcoro-aligned-allocation
      "-I${CMAKE_CURRENT_SOURCE_DIR}/../runtime/public"
      ${OBJC3C_COMPILER_INSTRUMENTATION_FLAGS}
      ${OBJC3C_SOURCE_COVERAGE_FLAGS}
      -c "${_task_group_await_source}" -o "${_task_group_await_object}"
    DEPENDS "${_objc3_repo}/native/objc3c/tests/test_assertion_policy.h" "${_task_group_await_source}"
      "${CMAKE_CURRENT_SOURCE_DIR}/../runtime/public/objc3_task.hpp"
      "${CMAKE_CURRENT_SOURCE_DIR}/../runtime/public/objc3_runtime_services.h"
      "${OBJC3C_FOUNDATION_DRIVER}"
    VERBATIM)
  set_source_files_properties("${_task_group_await_object}"
    PROPERTIES EXTERNAL_OBJECT TRUE GENERATED TRUE)
  objc3c_register_custom_object("${_task_group_await_source}" "${_task_group_await_object}")
  add_executable(objc3_task_group_await_test "${_task_group_await_object}")
  set_target_properties(objc3_task_group_await_test PROPERTIES LINKER_LANGUAGE CXX)
  target_link_libraries(objc3_task_group_await_test PRIVATE objc3_runtime_services)
  target_link_options(objc3_task_group_await_test PRIVATE
    "LINKER:--wrap=objc3_task_group_scope_enter")
  objc3c_add_frontend_test(NAME objc3_task_group_await COMMAND objc3_task_group_await_test)
  set_tests_properties(objc3_task_group_await PROPERTIES TIMEOUT 45)
  include("${CMAKE_CURRENT_LIST_DIR}/../../cmake/GenericCollectionTests.cmake")
  objc3_add_generic_collection_tests()
  add_executable(objc3_text_scalar_boundary_test ../../tests/text_scalar_boundary_test.cpp
    ../runtime/stdlib/text_service.cpp)
  target_compile_definitions(objc3_text_scalar_boundary_test PRIVATE OBJC3_TEXT_CONSTRUCTION_TESTING)
  target_link_options(objc3_text_scalar_boundary_test PRIVATE "LINKER:--wrap=_Znwm")
  target_link_libraries(objc3_text_scalar_boundary_test PRIVATE objc3_runtime_services)
  objc3c_add_frontend_test(NAME objc3_text_scalar_boundary COMMAND objc3_text_scalar_boundary_test)
  add_executable(objc3_text_file_cli ../../tests/text_file_cli.cpp)
  target_link_libraries(objc3_text_file_cli PRIVATE objc3_runtime_services)
  find_package(Python3 COMPONENTS Interpreter REQUIRED)
  objc3c_add_frontend_test(NAME objc3_text_file_cli TARGET objc3_text_file_cli
    COMMAND "${Python3_EXECUTABLE}" -B
      "${CMAKE_CURRENT_SOURCE_DIR}/../../../../tests/clang_foundation/text_file_cli_test.py"
      "$<TARGET_FILE:objc3_text_file_cli>")

endif()

# Keep the SDK qualification plan available before test executables exist.
# CTest's JSON omits commands for missing binaries, so record target paths at
# generation time and cross-check this inventory before serial qualification.
if(BUILD_TESTING)
  add_executable(objc3_meta_compilation_test "${_objc3_repo}/tests/clang_foundation/meta_compilation_test.cpp")
  target_include_directories(objc3_meta_compilation_test SYSTEM PRIVATE ${LLVM_INCLUDE_DIRS} ${CLANG_INCLUDE_DIRS})
  target_link_libraries(objc3_meta_compilation_test PRIVATE objc3c_frontend_v2)
  target_compile_definitions(objc3_meta_compilation_test PRIVATE
    OBJC3_META_TEST_OBJC_LIBRARY="${_c_inline_objc_runtime}"
    OBJC3_META_TEST_RUNTIME_HEADERS="${CMAKE_CURRENT_SOURCE_DIR}/../runtime/public")
  objc3c_add_frontend_test(NAME objc3_meta_compilation COMMAND objc3_meta_compilation_test)
  objc3c_add_frontend_test(NAME objc3_meta_cli TARGET objc3c
    COMMAND "${Python3_EXECUTABLE}" -B "${_objc3_repo}/tests/clang_foundation/verify-meta-cli.py" "$<TARGET_FILE:objc3c>")
  add_executable(objc3_meta_program_test "${_objc3_repo}/tests/clang_foundation/meta_program_test.cpp")
  target_include_directories(objc3_meta_program_test SYSTEM PRIVATE ${LLVM_INCLUDE_DIRS} ${CLANG_INCLUDE_DIRS})
  target_link_libraries(objc3_meta_program_test PRIVATE objc3c_clang_foundation)
  objc3c_add_frontend_test(NAME objc3_meta_program COMMAND objc3_meta_program_test)
  add_executable(objc3_meta_package_test "${_objc3_repo}/tests/clang_foundation/meta_package_test.cpp")
  target_include_directories(objc3_meta_package_test SYSTEM PRIVATE ${LLVM_INCLUDE_DIRS} ${CLANG_INCLUDE_DIRS})
  target_link_libraries(objc3_meta_package_test PRIVATE objc3c_clang_foundation)
  objc3c_add_frontend_test(NAME objc3_meta_package COMMAND objc3_meta_package_test)
  add_executable(objc3_native_weak_policy_test "${_objc3_repo}/tests/clang_foundation/native_weak_policy_test.cpp")
  target_include_directories(objc3_native_weak_policy_test SYSTEM PRIVATE ${LLVM_INCLUDE_DIRS} ${CLANG_INCLUDE_DIRS})
  target_link_libraries(objc3_native_weak_policy_test PRIVATE objc3c_clang_foundation)
  objc3c_add_frontend_test(NAME objc3_native_weak_policy COMMAND objc3_native_weak_policy_test)
  add_executable(objc3_native_transparent_union_test "${_objc3_repo}/tests/clang_foundation/native_transparent_union_test.cpp")
  target_include_directories(objc3_native_transparent_union_test SYSTEM PRIVATE ${LLVM_INCLUDE_DIRS} ${CLANG_INCLUDE_DIRS})
  target_link_libraries(objc3_native_transparent_union_test PRIVATE objc3c_clang_foundation)
  objc3c_add_frontend_test(NAME objc3_native_transparent_union COMMAND objc3_native_transparent_union_test)
  string(REPLACE ";" "," _objc3_test_codegen_backends "${LLVM_TARGETS_TO_BUILD}")
  add_executable(objc3_linux_codegen_targets_test "${_objc3_repo}/tests/clang_foundation/linux_codegen_targets_test.cpp")
  target_include_directories(objc3_linux_codegen_targets_test SYSTEM PRIVATE ${LLVM_INCLUDE_DIRS} ${CLANG_INCLUDE_DIRS})
  target_link_libraries(objc3_linux_codegen_targets_test PRIVATE objc3c_clang_foundation)
  target_compile_definitions(objc3_linux_codegen_targets_test PRIVATE OBJC3_TEST_CODEGEN_BACKENDS="${_objc3_test_codegen_backends}")
  objc3c_add_frontend_test(NAME objc3_linux_codegen_targets COMMAND objc3_linux_codegen_targets_test)
  add_executable(objc3_native_public_async_syntax_test "${_objc3_repo}/tests/clang_foundation/native_public_async_syntax_test.cpp")
  target_include_directories(objc3_native_public_async_syntax_test SYSTEM PRIVATE ${LLVM_INCLUDE_DIRS} ${CLANG_INCLUDE_DIRS})
  target_link_libraries(objc3_native_public_async_syntax_test PRIVATE objc3c_clang_foundation)
  target_compile_definitions(objc3_native_public_async_syntax_test PRIVATE
    OBJC3_PUBLIC_ASYNC_HEADERS="${CMAKE_CURRENT_SOURCE_DIR}/../runtime/public"
    OBJC3_PUBLIC_ASYNC_C_FIXTURES="${_objc3_repo}/tests/clang_foundation/async_c_completion"
    OBJC3_PUBLIC_ASYNC_LINKER="${CMAKE_CXX_COMPILER}"
    OBJC3_PUBLIC_ASYNC_RUNTIME="$<TARGET_FILE:objc3_runtime_services>"
    OBJC3_PUBLIC_ASYNC_OBJC="${_c_inline_objc_runtime}"
    OBJC3_PUBLIC_ASYNC_LINK_FLAGS="${_c_inline_link_flags}")
  add_dependencies(objc3_native_public_async_syntax_test objc3_runtime_services)
  objc3c_add_frontend_test(NAME objc3_native_public_async_syntax COMMAND objc3_native_public_async_syntax_test)
  add_executable(objc3_native_aliasing_policy_test "${_objc3_repo}/tests/clang_foundation/native_aliasing_policy_test.cpp")
  target_include_directories(objc3_native_aliasing_policy_test SYSTEM PRIVATE ${LLVM_INCLUDE_DIRS} ${CLANG_INCLUDE_DIRS})
  target_link_libraries(objc3_native_aliasing_policy_test PRIVATE objc3c_clang_foundation)
  objc3c_add_frontend_test(NAME objc3_native_aliasing_policy COMMAND objc3_native_aliasing_policy_test)
  add_executable(objc3_diagnostic_edits_test "${_objc3_repo}/tests/clang_foundation/diagnostic_edits_test.cpp")
  target_include_directories(objc3_diagnostic_edits_test SYSTEM PRIVATE ${LLVM_INCLUDE_DIRS} ${CLANG_INCLUDE_DIRS})
  target_link_libraries(objc3_diagnostic_edits_test PRIVATE objc3c_frontend_v2)
  objc3c_add_frontend_test(NAME objc3_diagnostic_edits COMMAND objc3_diagnostic_edits_test)
  objc3c_add_frontend_test(NAME objc3_diagnostic_cli TARGET objc3c
    COMMAND "${Python3_EXECUTABLE}" "${_objc3_repo}/tests/clang_foundation/diagnostic_cli_test.py" --driver "$<TARGET_FILE:objc3c>")
  add_executable(objc3_native_anonymous_factory_test "${_objc3_repo}/tests/clang_foundation/native_anonymous_factory_test.cpp")
  target_include_directories(objc3_native_anonymous_factory_test SYSTEM PRIVATE ${LLVM_INCLUDE_DIRS} ${CLANG_INCLUDE_DIRS})
  target_link_libraries(objc3_native_anonymous_factory_test PRIVATE objc3c_clang_foundation)
  objc3c_add_frontend_test(NAME objc3_native_anonymous_factory COMMAND objc3_native_anonymous_factory_test)
  objc3c_write_frontend_test_inventory()
  get_property(_custom_compile_rows GLOBAL PROPERTY OBJC3C_CUSTOM_COMPILE_ROWS)
  list(JOIN _custom_compile_rows ",\n" _custom_compile_json)
  file(GENERATE OUTPUT "${CMAKE_BINARY_DIR}/custom-compile-records.json"
    CONTENT "[\n${_custom_compile_json}\n]\n")
endif()

objc3c_verify_clang_linkage()
