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

add_library(objc3_runtime_services STATIC
  ../runtime/services/error_service.cpp
  ../runtime/services/carrier_service.cpp
  ../runtime/concurrency/service_executor.cpp
  ../runtime/concurrency/service_task.cpp
  ../runtime/concurrency/service_group.cpp
  ../runtime/concurrency/service_actor.cpp
  ../runtime/concurrency/service_public.cpp
  ../runtime/stdlib/text_service.cpp
  ../runtime/stdlib/collections_service.cpp
  ../runtime/stdlib/generic_collection_service.cpp
  ../runtime/stdlib/generic_associative_service.cpp)
target_link_libraries(objc3_runtime_services PUBLIC objc3c_build_defaults)
target_include_directories(objc3_runtime_services PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../runtime/public>"
  "$<INSTALL_INTERFACE:include>")
find_package(Threads REQUIRED)
target_link_libraries(objc3_runtime_services PRIVATE Threads::Threads)

include(GNUInstallDirs)
install(TARGETS objc3c objc3c_frontend_v2 objc3c_clang_foundation objc3_runtime_services
  EXPORT objc3cTargets
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(TARGETS objc3c_build_defaults EXPORT objc3cTargets)
set_target_properties(objc3c_frontend_v2 PROPERTIES EXPORT_NAME Frontend)
set_target_properties(objc3_runtime_services PROPERTIES EXPORT_NAME RuntimeServices)
install(FILES frontend_api.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/objc3c)
install(FILES ../runtime/public/objc3_runtime_services.h
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/objc3c/runtime)
target_include_directories(objc3c_frontend_v2 INTERFACE "$<INSTALL_INTERFACE:include>")
install(EXPORT objc3cTargets NAMESPACE objc3c:: DESTINATION lib/cmake/objc3c)
include(CMakePackageConfigHelpers)
configure_package_config_file("${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/objc3cConfig.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/objc3cConfig.cmake" INSTALL_DESTINATION lib/cmake/objc3c)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/objc3cConfig.cmake" DESTINATION lib/cmake/objc3c)

if (BUILD_TESTING)
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
  add_executable(objc3_interface_import_test
    ../../../../tests/clang_foundation/interface_import_test.cpp)
  target_include_directories(objc3_interface_import_test SYSTEM PRIVATE
    ${LLVM_INCLUDE_DIRS} ${CLANG_INCLUDE_DIRS})
  target_link_libraries(objc3_interface_import_test PRIVATE objc3c_clang_foundation)
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
  add_test(NAME objc3_interface_emitter COMMAND objc3_interface_emitter_test)
  add_executable(objc3_interface_artifact_io_test
    ../../../../tests/clang_foundation/interface_artifact_io_test.cpp)
  target_include_directories(objc3_interface_artifact_io_test SYSTEM PRIVATE
    ${LLVM_INCLUDE_DIRS} ${CLANG_INCLUDE_DIRS})
  target_link_libraries(objc3_interface_artifact_io_test PRIVATE objc3c_clang_foundation)
  add_test(NAME objc3_interface_artifact_io COMMAND objc3_interface_artifact_io_test)
  add_executable(objc3_task_services_test
    ../../../../tests/tooling/fixtures/clang_foundation/services/task_services.cpp
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
  foreach(_objc3_test objc3_sdk_identity_test objc3_carrier_service_test objc3_interface_contract_test objc3_compile_session_test
      objc3_ownership_projection_test objc3_module_method_names_test objc3_interface_module_names_test objc3_interface_spi_test objc3_interface_availability_test objc3_interface_exports_test objc3_interface_emitter_test objc3_interface_artifact_io_test objc3_interface_import_test
      objc3_interface_import_api_test
      objc3_interface_module_resolution_test objc3_interface_module_provenance_test
      objc3_interface_source_import_bridge_test objc3_task_services_test objc3_structured_runtime_test
      objc3_generic_collection_service_test objc3_generic_associative_service_test
      objc3_text_scalar_boundary_test)
    if (MSVC)
      target_compile_options(${_objc3_test} PRIVATE /UNDEBUG)
    else()
      target_compile_options(${_objc3_test} PRIVATE -UNDEBUG)
    endif()
  endforeach()
endif()
