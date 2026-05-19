set(OBJC3C_RUNTIME_ABI_PROCESS_SOURCES
  ../objc3_object_format.cpp
  ../objc3_object_format.h
  ../objc3_process.h
  ../objc3_process_boundary_tokens.cpp
  ../objc3_process_boundary_tokens.h
  ../objc3_process_execution.cpp
  ../objc3_process_internal.h
  ../objc3_process_json_helpers.cpp
  ../objc3_process_json_helpers.h
)
list(TRANSFORM OBJC3C_RUNTIME_ABI_PROCESS_SOURCES PREPEND "${CMAKE_CURRENT_LIST_DIR}/")

add_library(objc3c_runtime_abi_process STATIC
  ${OBJC3C_RUNTIME_ABI_PROCESS_SOURCES}
)

set(OBJC3C_RUNTIME_ABI_RELEASE_ARTIFACT_SOURCES
  ../objc3_advanced_feature_gate_artifact.cpp
  ../objc3_advanced_feature_gate_document.cpp
  ../objc3_conformance_claim_validation_artifact.cpp
  ../objc3_conformance_claim_validation_document.cpp
  ../objc3_conformance_claim_validation_inputs.cpp
  ../objc3_conformance_claim_validation_inputs.h
  ../objc3_conformance_dashboard_artifact_document.cpp
  ../objc3_conformance_profile_selection.cpp
  ../objc3_conformance_release_artifact_document.h
  ../objc3_conformance_report_publication_artifact.cpp
  ../objc3_conformance_report_publication_document.cpp
  ../objc3_dashboard_status_renderers.cpp
  ../objc3_dashboard_status_renderers.h
  ../objc3_metaprogramming_macro_host_cache_artifact.cpp
  ../objc3_metaprogramming_macro_host_cache_document.cpp
  ../objc3_metaprogramming_macro_host_cache_document.h
  ../objc3_release_candidate_matrix_artifact.cpp
  ../objc3_release_candidate_matrix_document.cpp
  ../objc3_release_evidence_operation_artifact.cpp
  ../objc3_release_evidence_operation_document.cpp
  ../objc3_retired_claim_sidecars.cpp
)
list(TRANSFORM OBJC3C_RUNTIME_ABI_RELEASE_ARTIFACT_SOURCES PREPEND "${CMAKE_CURRENT_LIST_DIR}/")

add_library(objc3c_runtime_abi_release_artifacts STATIC
  ${OBJC3C_RUNTIME_ABI_RELEASE_ARTIFACT_SOURCES}
)

set(OBJC3C_RUNTIME_ABI_CROSS_MODULE_SOURCES
  ../objc3_cross_module_imported_modules_document.cpp
  ../objc3_cross_module_imported_modules_document.h
  ../objc3_cross_module_imported_modules_document_record_header.cpp
  ../objc3_cross_module_imported_modules_document_record_header.h
  ../objc3_cross_module_imported_modules_document_record_interop_sections.cpp
  ../objc3_cross_module_imported_modules_document_record_interop_sections.h
  ../objc3_cross_module_imported_modules_document_record_runtime_sections.cpp
  ../objc3_cross_module_imported_modules_document_record_runtime_sections.h
  ../objc3_cross_module_imported_modules_document_record_storage_sections.cpp
  ../objc3_cross_module_imported_modules_document_record_storage_sections.h
  ../objc3_cross_module_imported_modules_document_records.cpp
  ../objc3_cross_module_imported_modules_document_records.h
  ../objc3_cross_module_runtime_link_plan_artifact.cpp
  ../objc3_cross_module_runtime_link_plan_document_artifacts.cpp
  ../objc3_cross_module_runtime_link_plan_document_artifacts.h
  ../objc3_cross_module_runtime_link_plan_document_counts.cpp
  ../objc3_cross_module_runtime_link_plan_document_counts.h
  ../objc3_cross_module_runtime_link_plan_document_counts_storage.cpp
  ../objc3_cross_module_runtime_link_plan_document_counts_storage.h
  ../objc3_cross_module_runtime_link_plan_document.cpp
  ../objc3_cross_module_runtime_link_plan_document.h
  ../objc3_cross_module_runtime_link_plan_document_header.cpp
  ../objc3_cross_module_runtime_link_plan_document_header.h
  ../objc3_cross_module_runtime_link_plan_document_header_modules.cpp
  ../objc3_cross_module_runtime_link_plan_document_header_modules.h
  ../objc3_cross_module_runtime_link_plan_document_header_runtime.cpp
  ../objc3_cross_module_runtime_link_plan_document_header_runtime.h
  ../objc3_cross_module_runtime_link_plan_inputs.cpp
  ../objc3_cross_module_runtime_link_plan_inputs.h
  ../objc3_cross_module_runtime_link_plan_ordering.cpp
  ../objc3_cross_module_runtime_link_plan_ordering.h
  ../objc3_cross_module_runtime_link_plan_sections.cpp
  ../objc3_cross_module_runtime_link_plan_sections.h
  ../objc3_cross_module_runtime_link_plan_sections_validation.cpp
  ../objc3_cross_module_runtime_link_plan_sections_validation.h
  ../objc3_cross_module_runtime_link_plan_sections_validation_replay.cpp
  ../objc3_cross_module_runtime_link_plan_sections_validation_replay.h
)
list(TRANSFORM OBJC3C_RUNTIME_ABI_CROSS_MODULE_SOURCES PREPEND "${CMAKE_CURRENT_LIST_DIR}/")

add_library(objc3c_runtime_abi_cross_module STATIC
  ${OBJC3C_RUNTIME_ABI_CROSS_MODULE_SOURCES}
)

set(OBJC3C_RUNTIME_ABI_METADATA_SOURCES
  ../objc3_runtime_artifact_contracts.cpp
  ../objc3_runtime_artifact_contracts.h
  ../objc3_runtime_artifact_contracts_translation_unit.cpp
  ../objc3_runtime_metadata_discovery_document.cpp
  ../objc3_runtime_metadata_discovery_document.h
  ../objc3_runtime_metadata_linker_retention_artifact.cpp
  ../objc3_runtime_registration_descriptor_artifact.cpp
  ../objc3_runtime_registration_descriptor_document.cpp
  ../objc3_runtime_registration_descriptor_document.h
  ../objc3_runtime_registration_manifest_artifact.cpp
  ../objc3_runtime_registration_manifest_document_bootstrap.cpp
  ../objc3_runtime_registration_manifest_document_bootstrap.h
  ../objc3_runtime_registration_manifest_document.cpp
  ../objc3_runtime_registration_manifest_document.h
  ../objc3_runtime_registration_manifest_document_header.cpp
  ../objc3_runtime_registration_manifest_document_header.h
  ../objc3_runtime_registration_manifest_sections.cpp
  ../objc3_runtime_registration_manifest_sections.h
)
list(TRANSFORM OBJC3C_RUNTIME_ABI_METADATA_SOURCES PREPEND "${CMAKE_CURRENT_LIST_DIR}/")

add_library(objc3c_runtime_abi_metadata STATIC
  ${OBJC3C_RUNTIME_ABI_METADATA_SOURCES}
)
