add_library(objc3c_artifacts_identity STATIC
  identity/artifact_identity.cpp
  identity/artifact_identity.h
)
objc3c_apply_build_defaults(objc3c_artifacts_identity)

add_library(objc3c_artifacts_json STATIC
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
  json/program_manifest_json.cpp
  json/program_manifest_json.h
  json/program_manifest_records.cpp
  json/program_manifest_records.h
  json/runtime_metadata_manifest_json.cpp
  json/runtime_metadata_manifest_json.h
  json/runtime_metadata_manifest_records.cpp
  json/runtime_metadata_manifest_records.h
  json/semantic_type_manifest_json.cpp
  json/semantic_type_manifest_json.h
  json/semantic_type_manifest_records.cpp
  json/semantic_type_manifest_records.h
)
objc3c_apply_build_defaults(objc3c_artifacts_json)
objc3c_apply_llvm_direct_config(objc3c_artifacts_json)
target_link_libraries(objc3c_artifacts_json PUBLIC
  objc3c_ast
  objc3c_json
  objc3c_pipeline
  objc3c_support
)

add_library(objc3c_artifacts_evidence STATIC
  evidence/capability_status.h
  evidence/error_handling_replay_evidence.cpp
  evidence/error_handling_replay_evidence.h
  evidence/evidence_record.h
)
objc3c_apply_build_defaults(objc3c_artifacts_evidence)
objc3c_apply_llvm_direct_config(objc3c_artifacts_evidence)
target_link_libraries(objc3c_artifacts_evidence PUBLIC
  objc3c_json
  objc3c_pipeline
  objc3c_pipeline_results
)

add_library(objc3c_artifacts_interop STATIC
  interop/interop_bridge_artifacts.cpp
  interop/interop_bridge_artifacts.h
)
objc3c_apply_build_defaults(objc3c_artifacts_interop)
objc3c_apply_llvm_direct_config(objc3c_artifacts_interop)
target_link_libraries(objc3c_artifacts_interop PUBLIC
  objc3c_ast
  objc3c_json
  objc3c_pipeline
)

add_library(objc3c_artifacts_frontend_runtime STATIC
  objc3_frontend_runtime_capability_artifacts.cpp
  objc3_frontend_runtime_capability_artifacts.h
  objc3_frontend_runtime_import_artifacts.cpp
  objc3_frontend_runtime_import_artifacts.h
  objc3_runtime_state_publication_paths.cpp
  objc3_runtime_state_publication_paths.h
)
objc3c_apply_build_defaults(objc3c_artifacts_frontend_runtime)
objc3c_apply_llvm_direct_config(objc3c_artifacts_frontend_runtime)
target_link_libraries(objc3c_artifacts_frontend_runtime PUBLIC
  objc3c_json
  objc3c_pipeline
  objc3c_support
)

add_library(objc3c_artifacts_frontend_conformance STATIC
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
objc3c_apply_build_defaults(objc3c_artifacts_frontend_conformance)
objc3c_apply_llvm_direct_config(objc3c_artifacts_frontend_conformance)
target_link_libraries(objc3c_artifacts_frontend_conformance PUBLIC
  objc3c_artifacts_frontend_runtime
  objc3c_json
  objc3c_pipeline
  objc3c_support
)

add_library(objc3c_artifacts_reports STATIC
  reports/frontend_conformance_report_contracts.cpp
  reports/frontend_conformance_report_contracts.h
  reports/report_dto.h
)
objc3c_apply_build_defaults(objc3c_artifacts_reports)
objc3c_apply_llvm_direct_config(objc3c_artifacts_reports)
target_link_libraries(objc3c_artifacts_reports PUBLIC
  objc3c_artifacts_frontend_conformance
  objc3c_artifacts_frontend_runtime
  objc3c_json
  objc3c_pipeline_results
)

add_library(objc3c_artifacts STATIC
  objc3_frontend_artifacts.cpp
  objc3_frontend_artifacts.h
  objc3_frontend_runtime_ingest_binary_artifacts.cpp
  objc3_frontend_runtime_ingest_binary_artifacts.h
)
objc3c_apply_build_defaults(objc3c_artifacts)
objc3c_apply_llvm_direct_config(objc3c_artifacts)
target_link_libraries(objc3c_artifacts PUBLIC
  objc3c_artifacts_evidence
  objc3c_artifacts_frontend_conformance
  objc3c_artifacts_frontend_runtime
  objc3c_artifacts_identity
  objc3c_artifacts_interop
  objc3c_artifacts_json
  objc3c_artifacts_reports
  objc3c_pipeline
  objc3c_support
)
