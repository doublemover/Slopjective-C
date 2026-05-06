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

add_library(objc3c_config INTERFACE)
target_link_libraries(objc3c_config INTERFACE
  objc3c_build_defaults
)

add_library(objc3c_pipeline_results INTERFACE)
target_link_libraries(objc3c_pipeline_results INTERFACE
  objc3c_build_defaults
)

add_library(objc3c_sema_model INTERFACE)
target_link_libraries(objc3c_sema_model INTERFACE
  objc3c_build_defaults
)

add_library(objc3c_lower_model INTERFACE)
target_link_libraries(objc3c_lower_model INTERFACE
  objc3c_build_defaults
)

add_library(objc3c_runtime_metadata INTERFACE)
target_link_libraries(objc3c_runtime_metadata INTERFACE
  objc3c_build_defaults
)

add_library(objc3c_artifacts_evidence INTERFACE)
target_link_libraries(objc3c_artifacts_evidence INTERFACE
  objc3c_build_defaults
  objc3c_pipeline_results
)

add_library(objc3c_artifacts_reports INTERFACE)
target_link_libraries(objc3c_artifacts_reports INTERFACE
  objc3c_build_defaults
  objc3c_pipeline_results
)

set(OBJC3_RUNTIME_LIBRARY_TARGET objc3_runtime)
set(OBJC3_RUNTIME_LIBRARY_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/runtime)
set(OBJC3_RUNTIME_LIBRARY_PUBLIC_HEADER ${OBJC3_RUNTIME_LIBRARY_SOURCE_DIR}/public/objc3_runtime_api.h)
set(OBJC3_RUNTIME_LIBRARY_KIND STATIC)
set(OBJC3_RUNTIME_LIBRARY_ARCHIVE_BASENAME objc3_runtime)
# M251-D001 freeze: reserve the canonical in-tree native runtime library
# target/header/source-root surface before M251-D002 lands the actual library
# skeleton and M251-D003 wires the driver/link path to consume it.

# Dependency-direction authority is defined in `src/ARCHITECTURE.md` and checked
# by `scripts/check_objc3c_dependency_boundaries.py`. The hard-cutover graph uses
# concrete stage targets below while module extraction continues.

add_executable(objc3c-native
  src/main.cpp
)
objc3c_apply_build_defaults(objc3c-native)
objc3c_apply_llvm_direct_config(objc3c-native)
objc3c_apply_runtime_output(objc3c-native)

# Placeholder interface targets so boundary rules can map to concrete target
# names before extraction work lands.
add_library(objc3c_lex STATIC
  src/lex/objc3_lexer.cpp
)
objc3c_apply_build_defaults(objc3c_lex)
objc3c_apply_llvm_direct_config(objc3c_lex)

add_library(objc3c_diag STATIC
  src/diag/objc3_diag_utils.cpp
)
objc3c_apply_build_defaults(objc3c_diag)
objc3c_apply_llvm_direct_config(objc3c_diag)

target_link_libraries(objc3c_lex PUBLIC
  objc3c_config
  objc3c_diag
)

add_library(objc3c_parse STATIC
  src/parse/objc3_ast_builder.cpp
  src/parse/objc3_ast_builder_contract.cpp
  src/parse/objc3_diagnostic_grammar_hooks_core_feature.cpp
  src/parse/objc3_diagnostic_source_precision_scaffold.cpp
  src/parse/objc3_message_send_profiles.cpp
  src/parse/objc3_parse_support.cpp
  src/parse/objc3_parser.cpp
)
objc3c_apply_build_defaults(objc3c_parse)
objc3c_apply_llvm_direct_config(objc3c_parse)
target_link_libraries(objc3c_parse PUBLIC
  objc3c_lex
  objc3c_diag
)

add_library(objc3c_sema STATIC
  src/sema/objc3_parser_sema_handoff_contract.h
  src/sema/objc3_parser_sema_handoff_scaffold.h
  src/sema/objc3_sema_diagnostics_bus.cpp
  src/sema/objc3_sema_pass_block_equivalence.cpp
  src/sema/objc3_sema_pass_flow_scaffold.cpp
  src/sema/objc3_sema_pass_manager.cpp
  src/sema/objc3_semantic_passes.cpp
  src/sema/objc3_type_form_scaffold.cpp
  src/sema/objc3_static_analysis.cpp
  src/sema/objc3_pure_contract.cpp
)
objc3c_apply_build_defaults(objc3c_sema)
objc3c_apply_llvm_direct_config(objc3c_sema)
target_link_libraries(objc3c_sema PUBLIC
  objc3c_parse
  objc3c_diag
  objc3c_sema_model
)
# M141-B001 sema/type-system surface target for downstream topology hardening.
add_library(objc3c_sema_type_system INTERFACE)
target_link_libraries(objc3c_sema_type_system INTERFACE
  objc3c_sema
)

add_library(objc3c_lower STATIC
  src/lower/objc3_lowering_contract.cpp
)
objc3c_apply_build_defaults(objc3c_lower)
objc3c_apply_llvm_direct_config(objc3c_lower)
target_link_libraries(objc3c_lower PUBLIC
  objc3c_lower_model
  objc3c_sema_type_system
)

add_library(objc3c_ir STATIC
  src/ir/objc3_ir_emitter.cpp
)
objc3c_apply_build_defaults(objc3c_ir)
objc3c_apply_llvm_direct_config(objc3c_ir)
target_link_libraries(objc3c_ir PUBLIC
  objc3c_lower
  objc3c_sema_type_system
)

add_library(objc3c_json STATIC
  src/io/json/json_error.cpp
  src/io/json/json_parser.cpp
  src/io/json/json_schema.cpp
  src/io/json/json_value.cpp
  src/io/json/json_writer.cpp
  src/io/objc3_json.cpp
)
objc3c_apply_build_defaults(objc3c_json)
objc3c_apply_llvm_direct_config(objc3c_json)

add_library(objc3c_pipeline STATIC
  src/pipeline/objc3_frontend_pipeline.cpp
  src/pipeline/objc3_runtime_import_surface.cpp
  src/pipeline/objc3_typed_sema_to_lowering_contract_surface_core.h
  src/pipeline/objc3_typed_sema_to_lowering_contract_surface.h
  src/pipeline/objc3_ir_emission_completeness_scaffold.cpp
  src/pipeline/objc3_lowering_pipeline_pass_graph_core_feature_surface.cpp
  src/pipeline/objc3_lowering_pipeline_pass_graph_scaffold.cpp
)
objc3c_apply_build_defaults(objc3c_pipeline)
objc3c_apply_llvm_direct_config(objc3c_pipeline)
target_link_libraries(objc3c_pipeline PUBLIC
  objc3c_parse
  objc3c_sema
  objc3c_lower
  objc3c_ir
  objc3c_diag
  objc3c_json
  objc3c_pipeline_results
)

add_library(objc3c_artifacts STATIC
  src/artifacts/json/artifact_json_model.cpp
  src/artifacts/json/artifact_json_writer.cpp
  src/artifacts/json/artifact_schema_registry.cpp
  src/artifacts/objc3_frontend_artifacts.cpp
)
objc3c_apply_build_defaults(objc3c_artifacts)
objc3c_apply_llvm_direct_config(objc3c_artifacts)
target_link_libraries(objc3c_artifacts PUBLIC
  objc3c_artifacts_evidence
  objc3c_artifacts_reports
  objc3c_pipeline
  objc3c_json
)

add_library(objc3c_io STATIC
  src/io/objc3_diagnostics_artifacts.cpp
  src/io/objc3_file_io.cpp
  src/io/objc3_manifest_artifacts.cpp
)
objc3c_apply_build_defaults(objc3c_io)
objc3c_apply_llvm_direct_config(objc3c_io)

add_library(objc3_runtime STATIC
  src/runtime/blocks/block_capture_storage.cpp
  src/runtime/blocks/block_capture_storage.h
  src/runtime/blocks/block_record.cpp
  src/runtime/blocks/block_record.h
  src/runtime/classes/category_attachment.cpp
  src/runtime/classes/category_attachment.h
  src/runtime/classes/class_graph.cpp
  src/runtime/classes/class_graph.h
  src/runtime/classes/metaclass_graph.cpp
  src/runtime/classes/metaclass_graph.h
  src/runtime/classes/protocol_conformance.cpp
  src/runtime/classes/protocol_conformance.h
  src/runtime/concurrency/actor.cpp
  src/runtime/concurrency/actor.h
  src/runtime/concurrency/executor.cpp
  src/runtime/concurrency/executor.h
  src/runtime/concurrency/task.cpp
  src/runtime/concurrency/task.h
  src/runtime/dispatch/builtin_methods.cpp
  src/runtime/dispatch/builtin_methods.h
  src/runtime/dispatch/dispatch_api.cpp
  src/runtime/dispatch/dispatch_api.h
  src/runtime/dispatch/dispatch_errors.cpp
  src/runtime/dispatch/dispatch_errors.h
  src/runtime/dispatch/method_cache.cpp
  src/runtime/dispatch/method_cache.h
  src/runtime/dispatch/method_lookup.cpp
  src/runtime/dispatch/method_lookup.h
  src/runtime/errors/catch_filter.cpp
  src/runtime/errors/catch_filter.h
  src/runtime/errors/runtime_error.cpp
  src/runtime/errors/runtime_error.h
  src/runtime/images/image_descriptor.cpp
  src/runtime/images/image_descriptor.h
  src/runtime/images/multi_image_ordering.cpp
  src/runtime/images/multi_image_ordering.h
  src/runtime/images/registration.cpp
  src/runtime/images/registration.h
  src/runtime/memory/arc.cpp
  src/runtime/memory/arc.h
  src/runtime/memory/autorelease_pool.cpp
  src/runtime/memory/autorelease_pool.h
  src/runtime/objc3_runtime.cpp
  src/runtime/public/objc3_runtime_api.h
  src/runtime/public/objc3_runtime_result.h
  src/runtime/selectors/keypath_table.cpp
  src/runtime/selectors/keypath_table.h
  src/runtime/selectors/selector_table.cpp
  src/runtime/selectors/selector_table.h
  src/runtime/state/runtime_reset.cpp
  src/runtime/state/runtime_reset.h
  src/runtime/state/runtime_state.cpp
  src/runtime/state/runtime_state.h
  src/runtime/storage/instance_storage.cpp
  src/runtime/storage/instance_storage.h
  src/runtime/storage/ivar_layout.cpp
  src/runtime/storage/ivar_layout.h
  src/runtime/storage/property_accessors.cpp
  src/runtime/storage/property_accessors.h
  src/runtime/storage/weak_slots.cpp
  src/runtime/storage/weak_slots.h
  src/support/selectors/selector_normalization.cpp
  src/support/selectors/selector_normalization.h
)
objc3c_apply_build_defaults(objc3_runtime)
target_link_libraries(objc3_runtime PUBLIC
  objc3c_runtime_metadata
)
set_target_properties(objc3_runtime PROPERTIES
  OUTPUT_NAME ${OBJC3_RUNTIME_LIBRARY_ARCHIVE_BASENAME}
)
objc3c_apply_archive_output(objc3_runtime)
add_library(objc3c::runtime ALIAS objc3_runtime)

add_library(objc3c_runtime_abi STATIC
  src/io/objc3_process.cpp
)
objc3c_apply_build_defaults(objc3c_runtime_abi)
objc3c_apply_llvm_direct_config(objc3c_runtime_abi)
target_link_libraries(objc3c_runtime_abi PUBLIC
  objc3c_json
)
target_link_libraries(objc3c_io PUBLIC
  objc3c_diag
  objc3c_json
  objc3c_runtime_abi
)

add_library(objc3c_diagnostics_modes STATIC
  src/diagnostics/modes/objc3_removed_mode_options.cpp
)
objc3c_apply_build_defaults(objc3c_diagnostics_modes)
objc3c_apply_llvm_direct_config(objc3c_diagnostics_modes)

add_library(objc3c_frontend STATIC
  src/libobjc3c_frontend/c_api.cpp
  src/libobjc3c_frontend/frontend_anchor.cpp
  src/libobjc3c_frontend/objc3_cli_frontend.cpp
)
objc3c_apply_build_defaults(objc3c_frontend)
objc3c_apply_llvm_direct_config(objc3c_frontend)
target_link_libraries(objc3c_frontend PUBLIC
  objc3c_artifacts
  objc3c_pipeline
  objc3c_runtime_abi
  objc3c_json
)

add_library(objc3c_driver STATIC
  src/driver/objc3_cli_options.cpp
  src/driver/objc3_driver_main.cpp
  src/driver/objc3_driver_shell.cpp
  src/driver/objc3_frontend_options.cpp
  src/driver/objc3_llvm_capability_routing.cpp
  src/driver/objc3_objc3_conformance_path.cpp
  src/driver/objc3_objc3_path.cpp
  src/driver/objc3_objectivec_path.cpp
  src/driver/objc3_compilation_driver.cpp
)
objc3c_apply_build_defaults(objc3c_driver)
objc3c_apply_llvm_direct_config(objc3c_driver)
target_link_libraries(objc3c_driver PUBLIC
  objc3c_config
  objc3c_diag
  objc3c_diagnostics_modes
  objc3c_frontend
  objc3c_io
)
if (TARGET objc3c_libclang)
  target_link_libraries(objc3c_driver PUBLIC
    objc3c_libclang
  )
endif()

target_link_libraries(objc3c-native PRIVATE
  objc3c_driver
)

add_executable(objc3c_tools_frontend_c_api_runner
  src/tools/objc3c_frontend_c_api_runner.cpp
)
objc3c_apply_build_defaults(objc3c_tools_frontend_c_api_runner)
objc3c_apply_llvm_direct_config(objc3c_tools_frontend_c_api_runner)
objc3c_apply_runtime_output(objc3c_tools_frontend_c_api_runner)
set_target_properties(objc3c_tools_frontend_c_api_runner PROPERTIES
  OUTPUT_NAME objc3c-frontend-c-api-runner
)
target_link_libraries(objc3c_tools_frontend_c_api_runner PRIVATE
  objc3c_frontend
  objc3c_driver
  objc3c_diagnostics_modes
  objc3c_json
)
