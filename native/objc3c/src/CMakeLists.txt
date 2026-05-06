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
set(OBJC3_RUNTIME_LIBRARY_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/runtime)
set(OBJC3_RUNTIME_LIBRARY_PUBLIC_HEADER ${OBJC3_RUNTIME_LIBRARY_SOURCE_DIR}/public/objc3_runtime_api.h)
set(OBJC3_RUNTIME_LIBRARY_KIND STATIC)
set(OBJC3_RUNTIME_LIBRARY_ARCHIVE_BASENAME objc3_runtime)
# M251-D001 freeze: reserve the canonical in-tree native runtime library
# target/header/source-root surface before M251-D002 lands the actual library
# skeleton and M251-D003 wires the driver/link path to consume it.

# Dependency-direction authority is defined in `ARCHITECTURE.md` and checked
# by `scripts/check_objc3c_dependency_boundaries.py`. The hard-cutover graph uses
# concrete stage targets below while module extraction continues.

add_executable(objc3c-native
  main.cpp
)
objc3c_apply_build_defaults(objc3c-native)
objc3c_apply_llvm_direct_config(objc3c-native)
objc3c_apply_runtime_output(objc3c-native)

# Placeholder interface targets so boundary rules can map to concrete target
# names before extraction work lands.
add_library(objc3c_lex STATIC
  lex/objc3_lexer.cpp
)
objc3c_apply_build_defaults(objc3c_lex)
objc3c_apply_llvm_direct_config(objc3c_lex)

add_library(objc3c_diag STATIC
  diag/objc3_diag_utils.cpp
)
objc3c_apply_build_defaults(objc3c_diag)
objc3c_apply_llvm_direct_config(objc3c_diag)

target_link_libraries(objc3c_lex PUBLIC
  objc3c_config
  objc3c_diag
)

add_library(objc3c_parse STATIC
  parse/objc3_ast_builder.cpp
  parse/objc3_ast_builder_contract.cpp
  parse/objc3_diagnostic_grammar_hooks_core_feature.cpp
  parse/objc3_diagnostic_source_precision_scaffold.cpp
  parse/objc3_message_send_profiles.cpp
  parse/objc3_parse_support.cpp
  parse/objc3_parser.cpp
)
objc3c_apply_build_defaults(objc3c_parse)
objc3c_apply_llvm_direct_config(objc3c_parse)
target_link_libraries(objc3c_parse PUBLIC
  objc3c_lex
  objc3c_diag
)

add_library(objc3c_sema STATIC
  sema/objc3_parser_sema_handoff_contract.h
  sema/objc3_parser_sema_handoff_scaffold.h
  sema/objc3_sema_diagnostics_bus.cpp
  sema/objc3_sema_pass_block_equivalence.cpp
  sema/objc3_sema_pass_flow_scaffold.cpp
  sema/objc3_sema_pass_manager.cpp
  sema/objc3_semantic_passes.cpp
  sema/objc3_type_form_scaffold.cpp
  sema/objc3_static_analysis.cpp
  sema/objc3_pure_contract.cpp
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

add_subdirectory(lower)

add_library(objc3c_ir STATIC
  ir/objc3_ir_emitter.cpp
)
objc3c_apply_build_defaults(objc3c_ir)
objc3c_apply_llvm_direct_config(objc3c_ir)
target_link_libraries(objc3c_ir PUBLIC
  objc3c_lower
  objc3c_sema_type_system
)

add_library(objc3c_json STATIC
  io/json/json_error.cpp
  io/json/json_parser.cpp
  io/json/json_schema.cpp
  io/json/json_value.cpp
  io/json/json_writer.cpp
  io/objc3_json.cpp
)
objc3c_apply_build_defaults(objc3c_json)
objc3c_apply_llvm_direct_config(objc3c_json)

add_library(objc3c_pipeline STATIC
  pipeline/objc3_frontend_pipeline.cpp
  pipeline/objc3_runtime_import_surface.cpp
  pipeline/objc3_typed_sema_to_lowering_contract_surface_core.h
  pipeline/objc3_typed_sema_to_lowering_contract_surface.h
  pipeline/objc3_ir_emission_completeness_scaffold.cpp
  pipeline/objc3_lowering_pipeline_pass_graph_core_feature_surface.cpp
  pipeline/objc3_lowering_pipeline_pass_graph_scaffold.cpp
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
  artifacts/json/artifact_json_model.cpp
  artifacts/json/artifact_json_writer.cpp
  artifacts/json/artifact_schema_registry.cpp
  artifacts/objc3_frontend_artifacts.cpp
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
  io/objc3_diagnostics_artifacts.cpp
  io/objc3_file_io.cpp
  io/objc3_manifest_artifacts.cpp
)
objc3c_apply_build_defaults(objc3c_io)
objc3c_apply_llvm_direct_config(objc3c_io)

add_library(objc3_runtime STATIC
  runtime/blocks/block_capture_storage.cpp
  runtime/blocks/block_capture_storage.h
  runtime/blocks/block_record.cpp
  runtime/blocks/block_record.h
  runtime/classes/category_attachment.cpp
  runtime/classes/category_attachment.h
  runtime/classes/class_graph.cpp
  runtime/classes/class_graph.h
  runtime/classes/metaclass_graph.cpp
  runtime/classes/metaclass_graph.h
  runtime/classes/protocol_conformance.cpp
  runtime/classes/protocol_conformance.h
  runtime/concurrency/actor.cpp
  runtime/concurrency/actor.h
  runtime/concurrency/executor.cpp
  runtime/concurrency/executor.h
  runtime/concurrency/task.cpp
  runtime/concurrency/task.h
  runtime/dispatch/builtin_methods.cpp
  runtime/dispatch/builtin_methods.h
  runtime/dispatch/dispatch_api.cpp
  runtime/dispatch/dispatch_api.h
  runtime/dispatch/dispatch_errors.cpp
  runtime/dispatch/dispatch_errors.h
  runtime/dispatch/method_cache.cpp
  runtime/dispatch/method_cache.h
  runtime/dispatch/method_lookup.cpp
  runtime/dispatch/method_lookup.h
  runtime/errors/catch_filter.cpp
  runtime/errors/catch_filter.h
  runtime/errors/runtime_error.cpp
  runtime/errors/runtime_error.h
  runtime/images/image_descriptor.cpp
  runtime/images/image_descriptor.h
  runtime/images/multi_image_ordering.cpp
  runtime/images/multi_image_ordering.h
  runtime/images/registration.cpp
  runtime/images/registration.h
  runtime/memory/arc.cpp
  runtime/memory/arc.h
  runtime/memory/autorelease_pool.cpp
  runtime/memory/autorelease_pool.h
  runtime/objc3_runtime.cpp
  runtime/public/objc3_runtime_api.h
  runtime/public/objc3_runtime_result.h
  runtime/selectors/keypath_table.cpp
  runtime/selectors/keypath_table.h
  runtime/selectors/selector_table.cpp
  runtime/selectors/selector_table.h
  runtime/state/runtime_reset.cpp
  runtime/state/runtime_reset.h
  runtime/state/runtime_state.cpp
  runtime/state/runtime_state.h
  runtime/storage/instance_storage.cpp
  runtime/storage/instance_storage.h
  runtime/storage/ivar_layout.cpp
  runtime/storage/ivar_layout.h
  runtime/storage/property_accessors.cpp
  runtime/storage/property_accessors.h
  runtime/storage/weak_slots.cpp
  runtime/storage/weak_slots.h
  support/selectors/selector_normalization.cpp
  support/selectors/selector_normalization.h
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
  io/objc3_process.cpp
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
  diagnostics/modes/objc3_removed_mode_options.cpp
)
objc3c_apply_build_defaults(objc3c_diagnostics_modes)
objc3c_apply_llvm_direct_config(objc3c_diagnostics_modes)

add_library(objc3c_frontend STATIC
  libobjc3c_frontend/c_api.cpp
  libobjc3c_frontend/frontend_anchor.cpp
  libobjc3c_frontend/objc3_cli_frontend.cpp
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
  driver/objc3_cli_options.cpp
  driver/objc3_driver_main.cpp
  driver/objc3_driver_shell.cpp
  driver/objc3_frontend_options.cpp
  driver/objc3_llvm_capability_routing.cpp
  driver/objc3_objc3_conformance_path.cpp
  driver/objc3_objc3_path.cpp
  driver/objc3_objectivec_path.cpp
  driver/objc3_compilation_driver.cpp
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
  tools/objc3c_frontend_c_api_runner.cpp
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
