add_library(objc3c_json STATIC
  json/json_equivalence.cpp
  json/json_equivalence_nodes.cpp
  json/json_error.cpp
  json/json_array_writer.cpp
  json/json_object_writer.cpp
  json/json_parser.cpp
  json/json_pointer.cpp
  json/json_pointer_tokens.cpp
  json/json_schema.cpp
  json/json_schema_array_cardinality_validation.cpp
  json/json_schema_array_contains_validation.cpp
  json/json_schema_array_items_validation.cpp
  json/json_schema_array_uniqueness_validation.cpp
  json/json_schema_array_validation.cpp
  json/json_schema_all_of_validation.cpp
  json/json_schema_annotation_contract_validation.cpp
  json/json_schema_any_of_validation.cpp
  json/json_schema_applicator_contract_validation.cpp
  json/json_schema_composition_contract_validation.cpp
  json/json_schema_composition_validation.cpp
  json/json_schema_enum_contract_validation.cpp
  json/json_schema_errors.cpp
  json/json_schema_node_preflight_validation.cpp
  json/json_schema_numeric_contract_validation.cpp
  json/json_schema_numeric_constraint_validation.cpp
  json/json_schema_object_additional_properties_validation.cpp
  json/json_schema_object_properties_validation.cpp
  json/json_schema_object_required_validation.cpp
  json/json_schema_object_validation.cpp
  json/json_schema_path_helpers.cpp
  json/json_schema_required_contract_validation.cpp
  json/json_schema_scalar_validation.cpp
  json/json_schema_string_constraint_validation.cpp
  json/json_schema_subschema.cpp
  json/json_schema_validation.cpp
  json/json_schema_type.cpp
  json/json_schema_type_contract_validation.cpp
  json/json_schema_type_names.cpp
  json/json_schema_value_keyword_validation.cpp
  json/json_string_escape.cpp
  json/json_string_array_writer.cpp
  json/json_value.cpp
  json/json_value_container_writer.cpp
  json/json_value_writer.cpp
  json/json_writer.cpp
  objc3_json.cpp
)
objc3c_apply_build_defaults(objc3c_json)
objc3c_apply_llvm_direct_config(objc3c_json)

add_library(objc3c_io STATIC
  objc3_artifact_paths.cpp
  objc3_artifact_writers.cpp
  objc3_conformance_artifact_paths.cpp
  objc3_conformance_artifact_writers.cpp
  objc3_diagnostics_artifact_document.cpp
  objc3_diagnostics_artifacts.cpp
  objc3_file_io.cpp
  objc3_file_payload_io.cpp
  objc3_line_join.cpp
  objc3_runtime_artifact_paths.cpp
  objc3_runtime_artifact_writers.cpp
)
objc3c_apply_build_defaults(objc3c_io)
objc3c_apply_llvm_direct_config(objc3c_io)

add_library(objc3c_runtime_abi_process STATIC
  objc3_object_format.cpp
  objc3_object_format.h
  objc3_process.h
  objc3_process_boundary_tokens.cpp
  objc3_process_boundary_tokens.h
  objc3_process_execution.cpp
  objc3_process_internal.h
  objc3_process_json_helpers.cpp
  objc3_process_json_helpers.h
)
objc3c_apply_build_defaults(objc3c_runtime_abi_process)
objc3c_apply_llvm_direct_config(objc3c_runtime_abi_process)
target_link_libraries(objc3c_runtime_abi_process PUBLIC
  objc3c_json
  objc3c_support
)

add_library(objc3c_runtime_abi_release_artifacts STATIC
  objc3_advanced_feature_gate_artifact.cpp
  objc3_advanced_feature_gate_document.cpp
  objc3_conformance_claim_validation_artifact.cpp
  objc3_conformance_claim_validation_document.cpp
  objc3_conformance_claim_validation_inputs.cpp
  objc3_conformance_claim_validation_inputs.h
  objc3_conformance_dashboard_artifact_document.cpp
  objc3_conformance_profile_selection.cpp
  objc3_conformance_release_artifact_document.h
  objc3_conformance_report_publication_artifact.cpp
  objc3_conformance_report_publication_document.cpp
  objc3_dashboard_status_renderers.cpp
  objc3_dashboard_status_renderers.h
  objc3_metaprogramming_macro_host_cache_artifact.cpp
  objc3_metaprogramming_macro_host_cache_document.cpp
  objc3_metaprogramming_macro_host_cache_document.h
  objc3_release_candidate_matrix_artifact.cpp
  objc3_release_candidate_matrix_document.cpp
  objc3_release_evidence_operation_artifact.cpp
  objc3_release_evidence_operation_document.cpp
  objc3_retired_claim_sidecars.cpp
)
objc3c_apply_build_defaults(objc3c_runtime_abi_release_artifacts)
objc3c_apply_llvm_direct_config(objc3c_runtime_abi_release_artifacts)
target_link_libraries(objc3c_runtime_abi_release_artifacts PUBLIC
  objc3c_runtime_abi_process
)

add_library(objc3c_runtime_abi_cross_module STATIC
  objc3_cross_module_imported_modules_document.cpp
  objc3_cross_module_imported_modules_document.h
  objc3_cross_module_runtime_link_plan_artifact.cpp
  objc3_cross_module_runtime_link_plan_document.cpp
  objc3_cross_module_runtime_link_plan_document.h
  objc3_cross_module_runtime_link_plan_inputs.cpp
  objc3_cross_module_runtime_link_plan_inputs.h
  objc3_cross_module_runtime_link_plan_ordering.cpp
  objc3_cross_module_runtime_link_plan_ordering.h
  objc3_cross_module_runtime_link_plan_sections.cpp
  objc3_cross_module_runtime_link_plan_sections.h
)
objc3c_apply_build_defaults(objc3c_runtime_abi_cross_module)
objc3c_apply_llvm_direct_config(objc3c_runtime_abi_cross_module)
target_link_libraries(objc3c_runtime_abi_cross_module PUBLIC
  objc3c_runtime_abi_process
)

add_library(objc3c_runtime_abi_metadata STATIC
  objc3_runtime_artifact_contracts.cpp
  objc3_runtime_artifact_contracts.h
  objc3_runtime_metadata_discovery_document.cpp
  objc3_runtime_metadata_discovery_document.h
  objc3_runtime_metadata_linker_retention_artifact.cpp
  objc3_runtime_registration_descriptor_artifact.cpp
  objc3_runtime_registration_descriptor_document.cpp
  objc3_runtime_registration_descriptor_document.h
  objc3_runtime_registration_manifest_artifact.cpp
  objc3_runtime_registration_manifest_document.cpp
  objc3_runtime_registration_manifest_document.h
  objc3_runtime_registration_manifest_sections.cpp
  objc3_runtime_registration_manifest_sections.h
)
objc3c_apply_build_defaults(objc3c_runtime_abi_metadata)
objc3c_apply_llvm_direct_config(objc3c_runtime_abi_metadata)
target_link_libraries(objc3c_runtime_abi_metadata PUBLIC
  objc3c_runtime_abi_process
)
target_link_libraries(objc3c_runtime_abi_release_artifacts PUBLIC
  objc3c_runtime_abi_metadata
)

add_library(objc3c_runtime_abi INTERFACE)
target_link_libraries(objc3c_runtime_abi INTERFACE
  objc3c_runtime_abi_cross_module
  objc3c_runtime_abi_metadata
  objc3c_runtime_abi_process
  objc3c_runtime_abi_release_artifacts
)
target_link_libraries(objc3c_io PUBLIC
  objc3c_diag
  objc3c_json
  objc3c_runtime_abi
  objc3c_support
)
