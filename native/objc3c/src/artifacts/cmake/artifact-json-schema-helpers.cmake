add_library(objc3c_artifacts_identity STATIC)
target_sources(objc3c_artifacts_identity PRIVATE
  identity/artifact_identity.cpp
  identity/artifact_identity.h
)

add_library(objc3c_artifacts_json STATIC)
target_sources(objc3c_artifacts_json PRIVATE
  json/artifact_json_model.cpp
  json/artifact_json_model.h
  json/artifact_json_publication_contract.cpp
  json/artifact_json_publication_contract.h
  json/artifact_json_writer.cpp
  json/artifact_json_writer.h
  json/artifact_record_array_json.h
  json/artifact_schema_contract_requirements.cpp
  json/artifact_schema_contract_table.cpp
  json/artifact_schema_contract_table.h
  json/artifact_schema_registry.cpp
  json/artifact_schema_registry.h
  json/artifact_schema_registry_summary.cpp
  json/capability_support_schema_records.cpp
  json/capability_support_schema_records.h
  json/program_manifest_json.cpp
  json/program_manifest_json.h
  json/program_manifest_records.cpp
  json/program_manifest_records.h
  json/runtime_metadata_manifest_json.cpp
  json/runtime_metadata_manifest_json.h
  json/runtime_metadata_manifest_records.cpp
  json/runtime_metadata_manifest_records.h
  json/release_readiness_schema_records.cpp
  json/release_readiness_schema_records.h
  json/semantic_type_manifest_json.cpp
  json/semantic_type_manifest_json.h
  json/semantic_type_manifest_records.cpp
  json/semantic_type_manifest_records.h
)

add_library(objc3c_artifacts_evidence STATIC)
target_sources(objc3c_artifacts_evidence PRIVATE
  evidence/capability_status.h
  evidence/error_handling_replay_evidence.cpp
  evidence/error_handling_replay_evidence.h
  evidence/evidence_record.h
)

add_library(objc3c_artifacts_interop STATIC)
target_sources(objc3c_artifacts_interop PRIVATE
  interop/interop_bridge_artifact_json.cpp
  interop/interop_bridge_artifacts.cpp
  interop/interop_bridge_artifacts.h
)

add_library(objc3c_artifacts_frontend_runtime STATIC)
target_sources(objc3c_artifacts_frontend_runtime PRIVATE
  objc3_frontend_runtime_capability_artifacts.cpp
  objc3_frontend_runtime_capability_artifacts.h
  objc3_frontend_runtime_import_artifact_payload_json.cpp
  objc3_frontend_runtime_import_artifact_reuse.cpp
  objc3_frontend_runtime_import_actor_mailbox_artifacts.cpp
  objc3_frontend_runtime_import_cross_module_orchestration.cpp
  objc3_frontend_runtime_import_cross_module_semantic_preservation.cpp
  objc3_frontend_runtime_import_dispatch_preservation_artifacts.cpp
  objc3_frontend_runtime_import_imported_semantic_rules.cpp
  objc3_frontend_runtime_import_artifacts.cpp
  objc3_frontend_runtime_import_artifacts.h
  objc3_frontend_runtime_import_metadata_record_json.cpp
  objc3_frontend_runtime_import_preservation_summary_json.cpp
  objc3_frontend_runtime_import_reuse_record_set.cpp
  objc3_frontend_runtime_import_serialized_lowering.cpp
  objc3_runtime_import_preservation_artifact_builders.cpp
  objc3_runtime_import_preservation_artifact_builders.h
  objc3_runtime_state_publication_paths.cpp
  objc3_runtime_state_publication_paths.h
)

add_library(objc3c_artifacts_frontend_conformance STATIC)
target_sources(objc3c_artifacts_frontend_conformance PRIVATE
  objc3_frontend_artifact_diagnostics.cpp
  objc3_frontend_artifact_diagnostics.h
  objc3_frontend_artifact_sanity.cpp
  objc3_frontend_artifact_sanity.h
  objc3_frontend_conformance_artifacts.cpp
  objc3_frontend_conformance_artifacts.h
  objc3_frontend_conformance_publication.cpp
  objc3_frontend_conformance_publication.h
  objc3_frontend_feature_claim_artifacts.cpp
  objc3_frontend_feature_claim_artifacts.h
  objc3_frontend_feature_claim_truth_artifacts.cpp
  objc3_frontend_feature_claim_truth_artifacts.h
)

add_library(objc3c_artifacts_reports STATIC)
target_sources(objc3c_artifacts_reports PRIVATE
  reports/frontend_conformance_report_contracts.cpp
  reports/frontend_conformance_report_contracts.h
  reports/frontend_conformance_report_contracts_json_helpers.h
  reports/frontend_conformance_report_lowering.cpp
  reports/frontend_conformance_report_semantics.cpp
  reports/frontend_conformance_report_tooling.cpp
  reports/report_dto.h
)
