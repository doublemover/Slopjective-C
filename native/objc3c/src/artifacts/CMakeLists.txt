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

add_library(objc3c_artifacts STATIC
  evidence/error_handling_replay_evidence.cpp
  identity/artifact_identity.cpp
  interop/interop_bridge_artifacts.cpp
  json/artifact_json_model.cpp
  json/artifact_json_publication_contract.cpp
  json/artifact_json_writer.cpp
  json/artifact_schema_contract_table.cpp
  json/artifact_schema_registry.cpp
  json/program_manifest_json.cpp
  json/runtime_metadata_manifest_json.cpp
  json/semantic_type_manifest_json.cpp
  objc3_frontend_artifact_sanity.cpp
  objc3_frontend_conformance_artifacts.cpp
  objc3_frontend_artifacts.cpp
  objc3_frontend_feature_claim_artifacts.cpp
  objc3_frontend_runtime_import_artifacts.cpp
  objc3_runtime_state_publication_paths.cpp
)
objc3c_apply_build_defaults(objc3c_artifacts)
objc3c_apply_llvm_direct_config(objc3c_artifacts)
target_link_libraries(objc3c_artifacts PUBLIC
  objc3c_artifacts_evidence
  objc3c_artifacts_reports
  objc3c_pipeline
  objc3c_json
  objc3c_support
)
