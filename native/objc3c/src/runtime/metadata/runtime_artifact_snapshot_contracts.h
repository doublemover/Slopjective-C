#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct objc3_runtime_metaprogramming_expansion_host_boundary_snapshot {
  uint64_t property_runtime_ready;
  uint64_t macro_host_execution_ready;
  uint64_t macro_host_process_launch_ready;
  uint64_t runtime_package_loader_ready;
  uint64_t deterministic;
  const char *runtime_support_library_archive_relative_path;
  const char *property_behavior_runtime_model;
  const char *macro_expansion_host_model;
  const char *packaging_model;
  const char *fail_closed_model;
} objc3_runtime_metaprogramming_expansion_host_boundary_snapshot;
int objc3_runtime_copy_metaprogramming_expansion_host_boundary_snapshot_for_testing(
    objc3_runtime_metaprogramming_expansion_host_boundary_snapshot *snapshot);

typedef struct objc3_runtime_metaprogramming_macro_host_process_cache_integration_snapshot {
  uint64_t property_runtime_ready;
  uint64_t macro_host_execution_ready;
  uint64_t macro_host_process_launch_ready;
  uint64_t runtime_package_loader_ready;
  uint64_t deterministic;
  const char *host_executable_relative_path;
  const char *cache_root_relative_path;
  const char *host_model;
  const char *toolchain_model;
  const char *cache_model;
  const char *fail_closed_model;
} objc3_runtime_metaprogramming_macro_host_process_cache_integration_snapshot;
int objc3_runtime_copy_metaprogramming_macro_host_process_cache_integration_snapshot_for_testing(
    objc3_runtime_metaprogramming_macro_host_process_cache_integration_snapshot *snapshot);

typedef struct objc3_runtime_interop_bridge_packaging_toolchain_snapshot {
  uint64_t packaging_topology_ready;
  uint64_t operator_visible_evidence_ready;
  uint64_t header_generation_ready;
  uint64_t module_generation_ready;
  uint64_t bridge_generation_ready;
  uint64_t deterministic;
  const char *runtime_support_library_archive_relative_path;
  const char *registration_manifest_model;
  const char *cross_module_link_plan_model;
  const char *operator_visible_evidence_model;
  const char *fail_closed_model;
} objc3_runtime_interop_bridge_packaging_toolchain_snapshot;
int objc3_runtime_copy_interop_bridge_packaging_toolchain_snapshot_for_testing(
    objc3_runtime_interop_bridge_packaging_toolchain_snapshot *snapshot);

typedef struct objc3_runtime_release_candidate_claim_snapshot {
  uint64_t claim_bundle_ready;
  uint64_t deterministic;
  const char *selected_profile;
  const char *claimed_profile_ids_csv;
  const char *targeted_profile_ids_csv;
  const char *conformance_publication_contract_id;
  const char *conformance_claim_operations_contract_id;
  const char *selection_model;
  const char *failure_model;
  const char *advanced_feature_ops_contract_id;
  const char *advanced_feature_reporting_contract_id;
  const char *advanced_feature_release_evidence_contract_id;
  const char *release_evidence_operation_contract_id;
  const char *dashboard_status_publication_contract_id;
  const char *release_candidate_matrix_contract_id;
  const char *dashboard_schema_path;
  const char *gate_script_path;
  const char *runbook_reference_path;
  const char *release_bundle_model;
  const char *deprecated_path_shutdown_model;
} objc3_runtime_release_candidate_claim_snapshot;
int objc3_runtime_copy_release_candidate_claim_snapshot_for_testing(
    objc3_runtime_release_candidate_claim_snapshot *snapshot);

typedef struct objc3_runtime_release_candidate_evidence_state_snapshot {
  uint64_t validation_artifact_ready;
  uint64_t release_evidence_operation_ready;
  uint64_t dashboard_status_ready;
  uint64_t advanced_feature_gate_ready;
  uint64_t release_candidate_matrix_ready;
  uint64_t deprecated_paths_shutdown;
  uint64_t deterministic;
  const char *validation_artifact_name;
  const char *release_evidence_operation_artifact_name;
  const char *dashboard_status_artifact_name;
  const char *advanced_feature_gate_artifact_name;
  const char *release_candidate_matrix_artifact_name;
  const char *validation_model;
  const char *release_bundle_model;
  const char *deprecated_path_shutdown_model;
} objc3_runtime_release_candidate_evidence_state_snapshot;
int objc3_runtime_copy_release_candidate_evidence_state_for_testing(
    objc3_runtime_release_candidate_evidence_state_snapshot *snapshot);

typedef struct objc3_runtime_interop_bridge_generation_snapshot {
  uint64_t runtime_generation_ready;
  uint64_t cross_module_packaging_ready;
  uint64_t header_generation_ready;
  uint64_t module_generation_ready;
  uint64_t bridge_generation_ready;
  uint64_t deterministic;
  const char *header_artifact_relative_path;
  const char *module_artifact_relative_path;
  const char *bridge_artifact_relative_path;
  const char *generation_model;
  const char *packaging_model;
  const char *fail_closed_model;
} objc3_runtime_interop_bridge_generation_snapshot;
int objc3_runtime_copy_interop_bridge_generation_snapshot_for_testing(
    objc3_runtime_interop_bridge_generation_snapshot *snapshot);

#ifdef __cplusplus
}
#endif
