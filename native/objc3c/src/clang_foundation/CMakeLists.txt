if (MSVC)
  # Clang's generated option identifiers require standard macro expansion.
  target_compile_options(objc3c_build_defaults INTERFACE /Zc:preprocessor /Zc:__cplusplus)
endif()

add_library(objc3c_clang_foundation STATIC
  compile_session.cpp
  driver_native.cpp
  sdk_identity.cpp
  source_identity.cpp
  ast_contract_consumer.cpp
  interface_contract.cpp
  module_names.cpp
  interface_codec.cpp
  interface_emitter.cpp
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
target_link_libraries(objc3c_clang_foundation PRIVATE OpenSSL::Crypto
  clangTooling clangFrontend clangCodeGen clangIndex)
llvm_map_components_to_libnames(_objc3_native_codegen
  nativecodegen "${LLVM_NATIVE_ARCH}AsmParser")
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

# CMake's C++20 module scanning must use this SDK, including on fresh hosts.
# Build the upstream tool against the same qualified Clang/LLVM libraries.
set(_scan_source "${_objc3_repo}/out/toolchains/objc3-development/src/clang/tools/clang-scan-deps")
set(_tablegen "${_objc3_repo}/out/toolchains/objc3-development/build-linux-x86_64/bin/llvm-tblgen")
if(NOT EXISTS "${_tablegen}")
  message(FATAL_ERROR "The sealed foundation build must provide llvm-tblgen")
endif()
add_custom_command(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/Opts.inc"
  COMMAND "${_tablegen}" -gen-opt-parser-defs
    -I "${_objc3_repo}/out/toolchains/objc3-development/src/llvm/include"
    "${_scan_source}/Opts.td" -o "${CMAKE_CURRENT_BINARY_DIR}/Opts.inc"
  DEPENDS "${_scan_source}/Opts.td" "${_tablegen}"
  VERBATIM)
add_executable(clang-scan-deps "${_scan_source}/ClangScanDeps.cpp"
  scan_deps_main.cpp "${CMAKE_CURRENT_BINARY_DIR}/Opts.inc")
target_include_directories(clang-scan-deps SYSTEM PRIVATE
  "${CMAKE_CURRENT_BINARY_DIR}" ${LLVM_INCLUDE_DIRS} ${CLANG_INCLUDE_DIRS})
target_compile_features(clang-scan-deps PRIVATE cxx_std_20)
target_link_libraries(clang-scan-deps PRIVATE clangAST clangBasic clangDependencyScanning
  clangDriver clangFrontend clangLex clangSerialization clangTooling ${_objc3_native_codegen})
install(TARGETS clang-scan-deps RUNTIME DESTINATION bin)

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
foreach(_feature_fixture explicit_capture_lists typed_keypaths native_c_async native_isolation task_spawning)
  install(DIRECTORY "${_objc3_repo}/tests/clang_foundation/${_feature_fixture}"
    DESTINATION share/objc3c/examples/linux-features
    FILES_MATCHING PATTERN "*.h" PATTERN "*.m" PATTERN "*.mm"
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
      -c "${_runtime_lifetime_source}" -o "${_runtime_lifetime_object}"
    DEPENDS "${_runtime_lifetime_source}"
      "${_objc3_repo}/native/objc3c/tests/test_assertion_policy.h"
      "${CMAKE_CURRENT_SOURCE_DIR}/../runtime/public/objc3_task.hpp"
      "${CMAKE_CURRENT_SOURCE_DIR}/../runtime/public/objc3_runtime_services.h"
      "${OBJC3C_FOUNDATION_DRIVER}" VERBATIM)
  set_source_files_properties("${_runtime_lifetime_object}"
    PROPERTIES EXTERNAL_OBJECT TRUE GENERATED TRUE)
  add_executable(objc3_runtime_lifetime_test "${_runtime_lifetime_object}")
  set_target_properties(objc3_runtime_lifetime_test PROPERTIES LINKER_LANGUAGE CXX)
  target_link_libraries(objc3_runtime_lifetime_test PRIVATE objc3_runtime_services)
  add_test(NAME objc3_runtime_lifetime COMMAND objc3_runtime_lifetime_test)
  set_tests_properties(objc3_runtime_lifetime PROPERTIES TIMEOUT 20)
  add_executable(objc3_opaque_storage_transaction_test ../../tests/opaque_storage_transaction_test.cpp)
  target_link_libraries(objc3_opaque_storage_transaction_test PRIVATE objc3_runtime_services)
  target_link_options(objc3_opaque_storage_transaction_test PRIVATE "LINKER:--wrap=_Znwm")
  add_test(NAME objc3_opaque_storage_transaction COMMAND objc3_opaque_storage_transaction_test)
  set_tests_properties(objc3_opaque_storage_transaction PROPERTIES TIMEOUT 20)
  add_executable(objc3_assertion_control_test ../../tests/assertion_control_test.cpp)
  add_test(NAME objc3_assertion_control COMMAND objc3_assertion_control_test)
  set_tests_properties(objc3_assertion_control PROPERTIES WILL_FAIL TRUE)
  add_executable(objc3_async_error_bridge_test
    ../../../../tests/clang_foundation/structured_runtime/error_bridge.cpp)
  target_link_libraries(objc3_async_error_bridge_test PRIVATE objc3_runtime_services)
  target_link_options(objc3_async_error_bridge_test PRIVATE
    "LINKER:--wrap=_ZnwmRKSt9nothrow_t"
    "LINKER:--wrap=_ZnwmSt11align_val_tRKSt9nothrow_t")
  add_test(NAME objc3_async_error_bridge COMMAND objc3_async_error_bridge_test)
  add_executable(objc3_keypath_service_test ../../tests/keypath_service_test.cpp)
  target_link_libraries(objc3_keypath_service_test PRIVATE objc3_runtime_services)
  add_test(NAME objc3_keypath_service COMMAND objc3_keypath_service_test)
  add_executable(objc3_keypath_native_ast_test
    ../../../../tests/clang_foundation/keypath_native_ast_test.cpp)
  target_compile_definitions(objc3_keypath_native_ast_test PRIVATE
    OBJC3_KEYPATH_TEST_REPO_ROOT="${_objc3_repo}")
  target_include_directories(objc3_keypath_native_ast_test SYSTEM PRIVATE
    ${LLVM_INCLUDE_DIRS} ${CLANG_INCLUDE_DIRS})
  target_link_libraries(objc3_keypath_native_ast_test PRIVATE
    clangAST clangBasic clangFrontend clangSerialization clangTooling)
  add_test(NAME objc3_keypath_native_ast COMMAND objc3_keypath_native_ast_test)
  add_executable(objc3_stdlib_map_count_test ../../tests/stdlib_map_count_test.cpp)
  target_link_libraries(objc3_stdlib_map_count_test PRIVATE objc3_runtime_services)
  add_test(NAME objc3_stdlib_map_count COMMAND objc3_stdlib_map_count_test)
  add_executable(objc3_sdk_identity_test ../../../../tests/clang_foundation/sdk_identity_test.cpp)
  target_include_directories(objc3_sdk_identity_test SYSTEM PRIVATE ${LLVM_INCLUDE_DIRS} ${CLANG_INCLUDE_DIRS})
  target_link_libraries(objc3_sdk_identity_test PRIVATE objc3c_clang_foundation)
  add_test(NAME objc3_sdk_identity COMMAND objc3_sdk_identity_test)
  add_executable(objc3_carrier_service_test ../../tests/carrier_service_test.cpp)
  target_link_libraries(objc3_carrier_service_test PRIVATE objc3_runtime_services)
  add_test(NAME objc3_carrier_service COMMAND objc3_carrier_service_test)
  add_executable(objc3_interface_contract_test
    ../../../../tests/clang_foundation/interface_contract_test.cpp)
  target_link_libraries(objc3_interface_contract_test PRIVATE objc3c_clang_foundation)
  add_test(NAME objc3_interface_contract COMMAND objc3_interface_contract_test)
  add_executable(objc3_interface_codec_test
    ../../../../tests/clang_foundation/interface_codec_test.cpp)
  target_include_directories(objc3_interface_codec_test SYSTEM PRIVATE
    ${LLVM_INCLUDE_DIRS} ${CLANG_INCLUDE_DIRS})
  target_link_libraries(objc3_interface_codec_test PRIVATE objc3c_clang_foundation)
  add_test(NAME objc3_interface_codec COMMAND objc3_interface_codec_test)
  add_executable(objc3_interface_import_test
    ../../../../tests/clang_foundation/interface_import_test.cpp)
  target_include_directories(objc3_interface_import_test SYSTEM PRIVATE
    ${LLVM_INCLUDE_DIRS} ${CLANG_INCLUDE_DIRS})
  target_link_libraries(objc3_interface_import_test PRIVATE objc3c_clang_foundation)
  target_compile_definitions(objc3_interface_import_test PRIVATE
    OBJC3_INTERFACE_TEST_PACKAGE_FIXTURES="${_objc3_repo}/tests/clang_foundation/native_async_packages")
  add_test(NAME objc3_interface_import COMMAND objc3_interface_import_test)
  add_executable(objc3_interface_import_api_test
    ../../../../tests/clang_foundation/interface_import_api_test.cpp)
  target_include_directories(objc3_interface_import_api_test SYSTEM PRIVATE
    ${LLVM_INCLUDE_DIRS} ${CLANG_INCLUDE_DIRS})
  target_link_libraries(objc3_interface_import_api_test PRIVATE objc3c_frontend_v2)
  add_test(NAME objc3_interface_import_api COMMAND objc3_interface_import_api_test)
  add_executable(objc3_interface_module_resolution_test
    ../../../../tests/clang_foundation/interface_module_resolution_test.cpp)
  target_link_libraries(objc3_interface_module_resolution_test PRIVATE objc3c_frontend_v2)
  add_test(NAME objc3_interface_module_resolution COMMAND objc3_interface_module_resolution_test)
  add_executable(objc3_interface_module_provenance_test
    ../../../../tests/clang_foundation/interface_module_provenance_test.cpp)
  target_include_directories(objc3_interface_module_provenance_test SYSTEM PRIVATE
    ${LLVM_INCLUDE_DIRS} ${CLANG_INCLUDE_DIRS})
  target_link_libraries(objc3_interface_module_provenance_test PRIVATE
    objc3c_clang_foundation clangTooling clangFrontend clangAST)
  add_test(NAME objc3_interface_module_provenance COMMAND objc3_interface_module_provenance_test)

  add_executable(objc3_interface_source_import_bridge_test
    ../../../../tests/clang_foundation/interface_source_import_bridge_test.cpp)
  target_include_directories(objc3_interface_source_import_bridge_test SYSTEM PRIVATE
    ${LLVM_INCLUDE_DIRS} ${CLANG_INCLUDE_DIRS})
  target_link_libraries(objc3_interface_source_import_bridge_test PRIVATE
    objc3c_clang_foundation clangTooling clangFrontend clangAST)
  add_test(NAME objc3_interface_source_import_bridge
    COMMAND objc3_interface_source_import_bridge_test)
  add_executable(objc3_compile_session_test
    ../../../../tests/clang_foundation/compile_session_test.cpp)
  target_link_libraries(objc3_compile_session_test PRIVATE objc3c_frontend_v2)
  add_test(NAME objc3_compile_session COMMAND objc3_compile_session_test)
  add_executable(objc3_ownership_projection_test
    ../../../../tests/clang_foundation/ownership_projection_test.cpp)
  target_include_directories(objc3_ownership_projection_test SYSTEM PRIVATE
    ${CLANG_INCLUDE_DIRS})
  target_link_libraries(objc3_ownership_projection_test PRIVATE objc3c_clang_foundation clangTooling)
  add_test(NAME objc3_ownership_projection COMMAND objc3_ownership_projection_test)
  add_executable(objc3_module_method_names_test
    ../../../../tests/clang_foundation/module_method_names_test.cpp)
  target_include_directories(objc3_module_method_names_test SYSTEM PRIVATE
    "${_objc3_repo}/out/toolchains/objc3-development/src/lldb/source" ${LLVM_INCLUDE_DIRS})
  target_link_libraries(objc3_module_method_names_test PRIVATE LLVMSupport)
  add_test(NAME objc3_module_method_names COMMAND objc3_module_method_names_test)

  add_executable(objc3_interface_module_names_test
    ../../../../tests/clang_foundation/interface_module_names_test.cpp)
  target_include_directories(objc3_interface_module_names_test SYSTEM PRIVATE
    ${LLVM_INCLUDE_DIRS} ${CLANG_INCLUDE_DIRS})
  target_link_libraries(objc3_interface_module_names_test PRIVATE objc3c_frontend_v2)
  add_test(NAME objc3_interface_module_names COMMAND objc3_interface_module_names_test)
  add_executable(objc3_borrowed_pointers_test
    ../../../../tests/clang_foundation/borrowed_pointers_test.cpp)
  target_include_directories(objc3_borrowed_pointers_test SYSTEM PRIVATE
    ${LLVM_INCLUDE_DIRS} ${CLANG_INCLUDE_DIRS})
  target_link_libraries(objc3_borrowed_pointers_test PRIVATE objc3c_clang_foundation)
  add_test(NAME objc3_borrowed_pointers COMMAND objc3_borrowed_pointers_test)
  add_executable(objc3_interface_spi_test
    ../../../../tests/clang_foundation/interface_spi_test.cpp)
  target_include_directories(objc3_interface_spi_test SYSTEM PRIVATE
    ${LLVM_INCLUDE_DIRS} ${CLANG_INCLUDE_DIRS})
  target_link_libraries(objc3_interface_spi_test PRIVATE objc3c_frontend_v2)
  add_test(NAME objc3_interface_spi COMMAND objc3_interface_spi_test)
  add_executable(objc3_interface_availability_test
    ../../../../tests/clang_foundation/interface_availability_test.cpp)
  target_include_directories(objc3_interface_availability_test SYSTEM PRIVATE
    ${LLVM_INCLUDE_DIRS} ${CLANG_INCLUDE_DIRS})
  target_link_libraries(objc3_interface_availability_test PRIVATE objc3c_clang_foundation)
  add_test(NAME objc3_interface_availability COMMAND objc3_interface_availability_test)
  add_executable(objc3_interface_exports_test
    ../../../../tests/clang_foundation/interface_exports_test.cpp)
  target_include_directories(objc3_interface_exports_test SYSTEM PRIVATE
    ${LLVM_INCLUDE_DIRS} ${CLANG_INCLUDE_DIRS})
  target_link_libraries(objc3_interface_exports_test PRIVATE objc3c_clang_foundation)
  add_test(NAME objc3_interface_exports COMMAND objc3_interface_exports_test)
  add_executable(objc3_interface_emitter_test
    ../../../../tests/clang_foundation/interface_emitter_test.cpp)
  target_include_directories(objc3_interface_emitter_test SYSTEM PRIVATE
    ${LLVM_INCLUDE_DIRS} ${CLANG_INCLUDE_DIRS})
  target_link_libraries(objc3_interface_emitter_test PRIVATE objc3c_clang_foundation clangTooling)
  target_compile_definitions(objc3_interface_emitter_test PRIVATE
    OBJC3_INTERFACE_TEST_PACKAGE_FIXTURES="${_objc3_repo}/tests/clang_foundation/native_async_packages"
    OBJC3_INTERFACE_TEST_STDLIB_HEADERS="${_objc3_repo}/stdlib/include/objc3"
    OBJC3_INTERFACE_TEST_RESOURCE_DIR="${OBJC3C_FOUNDATION_RESOURCE_DIR}")
  add_test(NAME objc3_interface_emitter COMMAND objc3_interface_emitter_test)
  add_executable(objc3_isolation_contracts_test
    ../../../../tests/clang_foundation/isolation_contracts_test.cpp)
  target_include_directories(objc3_isolation_contracts_test SYSTEM PRIVATE
    ${LLVM_INCLUDE_DIRS} ${CLANG_INCLUDE_DIRS})
  target_compile_definitions(objc3_isolation_contracts_test PRIVATE
    OBJC3_ISOLATION_RUNTIME_HEADERS="${CMAKE_CURRENT_SOURCE_DIR}/../runtime/public")
  target_link_libraries(objc3_isolation_contracts_test PRIVATE
    objc3c_clang_foundation clangTooling)
  add_test(NAME objc3_isolation_contracts COMMAND objc3_isolation_contracts_test)
  add_executable(objc3_explicit_capture_lists_test
    ../../../../tests/clang_foundation/explicit_capture_lists_test.cpp)
  target_include_directories(objc3_explicit_capture_lists_test SYSTEM PRIVATE
    ${LLVM_INCLUDE_DIRS} ${CLANG_INCLUDE_DIRS})
  target_link_libraries(objc3_explicit_capture_lists_test PRIVATE objc3c_clang_foundation)
  add_test(NAME objc3_explicit_capture_lists COMMAND objc3_explicit_capture_lists_test)
  add_executable(objc3_interface_artifact_io_test
    ../../../../tests/clang_foundation/interface_artifact_io_test.cpp)
  target_include_directories(objc3_interface_artifact_io_test SYSTEM PRIVATE
    ${LLVM_INCLUDE_DIRS} ${CLANG_INCLUDE_DIRS})
  target_link_libraries(objc3_interface_artifact_io_test PRIVATE objc3c_clang_foundation)
  add_test(NAME objc3_interface_artifact_io COMMAND objc3_interface_artifact_io_test)
  add_executable(objc3_task_services_test
    ../../../../tests/tooling/fixtures/clang_foundation/services/task_services.cpp
    ../../../../tests/clang_foundation/structured_runtime/actor_scheduling.cpp
    ../runtime/concurrency/service_group.cpp)
  # Only this test translation unit enables deterministic fault hooks; the
  # production service library keeps them out of its ABI and implementation.
  target_compile_definitions(objc3_task_services_test PRIVATE OBJC3_RUNTIME_SERVICE_TESTING)
  target_link_libraries(objc3_task_services_test PRIVATE objc3_runtime_services)
  add_test(NAME objc3_task_services COMMAND objc3_task_services_test)
  set_tests_properties(objc3_task_services PROPERTIES TIMEOUT 30)
  add_executable(objc3_structured_runtime_test
    ../../../../tests/clang_foundation/structured_runtime/worker.cpp)
  target_link_libraries(objc3_structured_runtime_test PRIVATE objc3_runtime_services)
  add_test(NAME objc3_structured_runtime COMMAND objc3_structured_runtime_test)
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
        -c "${_source}" -o "${_object}"
      DEPENDS "${_objc3_repo}/native/objc3c/tests/test_assertion_policy.h" "${_source}"
        "${_objc3_repo}/tests/clang_foundation/task_coroutines/provider.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/../runtime/public/objc3_task.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/../runtime/public/objc3_runtime_services.h"
        "${OBJC3C_FOUNDATION_DRIVER}"
      VERBATIM)
    set_source_files_properties("${_object}" PROPERTIES EXTERNAL_OBJECT TRUE GENERATED TRUE)
    list(APPEND _task_coroutine_objects "${_object}")
  endforeach()
  add_executable(objc3_task_coroutines_test ${_task_coroutine_objects})
  set_target_properties(objc3_task_coroutines_test PROPERTIES LINKER_LANGUAGE CXX)
  target_link_libraries(objc3_task_coroutines_test PRIVATE objc3_runtime_services)
  add_test(NAME objc3_task_coroutines COMMAND objc3_task_coroutines_test)
  set_tests_properties(objc3_task_coroutines PROPERTIES TIMEOUT 45)
  set(_task_group_await_source
    "${_objc3_repo}/tests/clang_foundation/task_coroutines/group_await.cpp")
  set(_task_group_await_object "${CMAKE_CURRENT_BINARY_DIR}/task-group-await.o")
  add_custom_command(OUTPUT "${_task_group_await_object}"
    COMMAND "${OBJC3C_FOUNDATION_DRIVER}" --no-default-config -x c++ -std=c++20
      -O2 -g -UNDEBUG -include "${_objc3_repo}/native/objc3c/tests/test_assertion_policy.h" -fcoro-aligned-allocation
      "-I${CMAKE_CURRENT_SOURCE_DIR}/../runtime/public"
      -c "${_task_group_await_source}" -o "${_task_group_await_object}"
    DEPENDS "${_objc3_repo}/native/objc3c/tests/test_assertion_policy.h" "${_task_group_await_source}"
      "${CMAKE_CURRENT_SOURCE_DIR}/../runtime/public/objc3_task.hpp"
      "${CMAKE_CURRENT_SOURCE_DIR}/../runtime/public/objc3_runtime_services.h"
      "${OBJC3C_FOUNDATION_DRIVER}"
    VERBATIM)
  set_source_files_properties("${_task_group_await_object}"
    PROPERTIES EXTERNAL_OBJECT TRUE GENERATED TRUE)
  add_executable(objc3_task_group_await_test "${_task_group_await_object}")
  set_target_properties(objc3_task_group_await_test PROPERTIES LINKER_LANGUAGE CXX)
  target_link_libraries(objc3_task_group_await_test PRIVATE objc3_runtime_services)
  target_link_options(objc3_task_group_await_test PRIVATE
    "LINKER:--wrap=objc3_task_group_scope_enter")
  add_test(NAME objc3_task_group_await COMMAND objc3_task_group_await_test)
  set_tests_properties(objc3_task_group_await PROPERTIES TIMEOUT 45)
  add_executable(objc3_generic_collection_service_test
    ../../tests/generic_collection_service_test.cpp)
  target_link_libraries(objc3_generic_collection_service_test PRIVATE objc3_runtime_services)
  add_test(NAME objc3_generic_collection_service COMMAND objc3_generic_collection_service_test)
  add_executable(objc3_generic_associative_service_test
    ../../tests/generic_associative_service_test.cpp)
  target_link_libraries(objc3_generic_associative_service_test PRIVATE objc3_runtime_services)
  add_test(NAME objc3_generic_associative_service COMMAND objc3_generic_associative_service_test)
  add_executable(objc3_text_scalar_boundary_test ../../tests/text_scalar_boundary_test.cpp)
  target_link_libraries(objc3_text_scalar_boundary_test PRIVATE objc3_runtime_services)
  add_test(NAME objc3_text_scalar_boundary COMMAND objc3_text_scalar_boundary_test)
  add_executable(objc3_text_file_cli ../../tests/text_file_cli.cpp)
  target_link_libraries(objc3_text_file_cli PRIVATE objc3_runtime_services)
  find_package(Python3 COMPONENTS Interpreter REQUIRED)
  add_test(NAME objc3_text_file_cli
    COMMAND "${Python3_EXECUTABLE}" -B
      "${CMAKE_CURRENT_SOURCE_DIR}/../../../../tests/clang_foundation/text_file_cli_test.py"
      "$<TARGET_FILE:objc3_text_file_cli>")
  set_tests_properties(objc3_text_file_cli PROPERTIES TIMEOUT 30)

endif()

# Keep the SDK qualification plan available before test executables exist.
# CTest's JSON omits commands for missing binaries, so record target paths at
# generation time and cross-check this inventory before serial qualification.
if(BUILD_TESTING)
  get_property(_frontend_tests DIRECTORY PROPERTY TESTS)
  set(_frontend_test_rows "")
  foreach(_test IN LISTS _frontend_tests)
    if(_test STREQUAL "objc3_text_file_cli")
      set(_target "${_test}")
    else()
      set(_target "${_test}_test")
    endif()
    if(NOT TARGET "${_target}")
      message(FATAL_ERROR "Frontend CTest ${_test} has no owned target ${_target}")
    endif()
    # CTest inventory is the single owner of assertion policy: new targets
    # cannot silently miss a separately maintained list in optimized builds.
    target_compile_options(${_target} PRIVATE -UNDEBUG
      "SHELL:-include '${_objc3_repo}/native/objc3c/tests/test_assertion_policy.h'")
    if(_frontend_test_rows)
      string(APPEND _frontend_test_rows ",\n")
    endif()
    string(APPEND _frontend_test_rows
      "    {\"name\":\"${_test}\",\"target\":\"${_target}\",\"executable\":\"$<TARGET_FILE:${_target}>\"}")
  endforeach()
  file(GENERATE OUTPUT "${CMAKE_BINARY_DIR}/frontend-test-targets.json"
    CONTENT "{\n  \"tests\": [\n${_frontend_test_rows}\n  ]\n}\n")
endif()
