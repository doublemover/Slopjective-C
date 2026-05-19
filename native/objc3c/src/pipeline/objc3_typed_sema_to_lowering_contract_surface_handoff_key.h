#pragma once

#include <sstream>
#include <string>

#include "pipeline/objc3_frontend_types.h"

inline std::string BuildObjc3TypedSemaToLoweringContractHandoffKey(
    const Objc3TypedSemaToLoweringContractSurface &surface) {
  std::ostringstream key;
  key << "typed-sema-lowering:v1:"
      << "semantic_surface=" << (surface.semantic_integration_surface_built ? "true" : "false")
      << ";type_metadata=" << (surface.semantic_type_metadata_handoff_deterministic ? "true" : "false")
      << ";sema_parity=" << (surface.sema_parity_surface_ready ? "true" : "false")
      << ";sema_parity_deterministic=" << (surface.sema_parity_surface_deterministic ? "true" : "false")
      << ";metadata_graph_lowering_handoff_ready="
      << (surface.executable_metadata_lowering_handoff_ready ? "true" : "false")
      << ";metadata_graph_lowering_handoff_deterministic="
      << (surface.executable_metadata_lowering_handoff_deterministic ? "true"
                                                                     : "false")
      << ";metadata_graph_typed_lowering_handoff_ready="
      << (surface.executable_metadata_typed_lowering_handoff_ready ? "true"
                                                                   : "false")
      << ";metadata_graph_typed_lowering_handoff_deterministic="
      << (surface.executable_metadata_typed_lowering_handoff_deterministic
              ? "true"
              : "false")
      << ";protocol_category=" << (surface.protocol_category_handoff_deterministic ? "true" : "false")
      << ";class_protocol_category_linking="
      << (surface.class_protocol_category_linking_handoff_deterministic ? "true" : "false")
      << ";selector_normalization="
      << (surface.selector_normalization_handoff_deterministic ? "true" : "false")
      << ";property_attribute="
      << (surface.property_attribute_handoff_deterministic ? "true" : "false")
      << ";object_pointer=" << (surface.object_pointer_type_handoff_deterministic ? "true" : "false")
      << ";symbol_graph=" << (surface.symbol_graph_handoff_deterministic ? "true" : "false")
      << ";scope_resolution=" << (surface.scope_resolution_handoff_deterministic ? "true" : "false")
      << ";semantic_handoff_consistent=" << (surface.semantic_handoff_consistent ? "true" : "false")
      << ";semantic_handoff_deterministic=" << (surface.semantic_handoff_deterministic ? "true" : "false")
      << ";runtime_dispatch=" << (surface.runtime_dispatch_contract_consistent ? "true" : "false")
      << ";metadata_graph_lowering_handoff_key="
      << surface.executable_metadata_lowering_handoff_key
      << ";metadata_graph_typed_lowering_handoff_key="
      << surface.executable_metadata_typed_lowering_handoff_key
      << ";core_feature_passed_case_count=" << surface.typed_core_feature_passed_case_count
      << ";core_feature_failed_case_count=" << surface.typed_core_feature_failed_case_count
      << ";core_feature_consistent=" << (surface.typed_core_feature_consistent ? "true" : "false")
      << ";core_feature_expansion_passed_case_count=" << surface.typed_core_feature_expansion_passed_case_count
      << ";core_feature_expansion_failed_case_count=" << surface.typed_core_feature_expansion_failed_case_count
      << ";core_feature_expansion_consistent="
      << (surface.typed_core_feature_expansion_consistent ? "true" : "false")
      << ";compatibility_handoff_consistent="
      << (surface.compatibility_handoff_consistent ? "true" : "false")
      << ";language_version_pragma_coordinate_order_consistent="
      << (surface.language_version_pragma_coordinate_order_consistent ? "true" : "false")
      << ";parse_artifact_replay_key_deterministic="
      << (surface.parse_artifact_replay_key_deterministic ? "true" : "false")
      << ";parse_artifact_edge_case_robustness_consistent="
      << (surface.parse_artifact_edge_case_robustness_consistent ? "true" : "false")
      << ";core_feature_edge_case_compatibility_ready="
      << (surface.typed_core_feature_edge_case_compatibility_ready ? "true" : "false")
      << ";core_feature_edge_case_expansion_consistent="
      << (surface.typed_core_feature_edge_case_expansion_consistent ? "true" : "false")
      << ";core_feature_edge_case_robustness_ready="
      << (surface.typed_core_feature_edge_case_robustness_ready ? "true" : "false")
      << ";typed_diagnostics_hardening_consistent="
      << (surface.typed_diagnostics_hardening_consistent ? "true" : "false")
      << ";typed_diagnostics_hardening_ready="
      << (surface.typed_diagnostics_hardening_ready ? "true" : "false")
      << ";typed_recovery_determinism_consistent="
      << (surface.typed_recovery_determinism_consistent ? "true" : "false")
      << ";typed_recovery_determinism_ready="
      << (surface.typed_recovery_determinism_ready ? "true" : "false")
      << ";typed_conformance_matrix_consistent="
      << (surface.typed_conformance_matrix_consistent ? "true" : "false")
      << ";typed_conformance_matrix_ready="
      << (surface.typed_conformance_matrix_ready ? "true" : "false")
      << ";typed_conformance_corpus_consistent="
      << (surface.typed_conformance_corpus_consistent ? "true" : "false")
      << ";typed_conformance_corpus_ready="
      << (surface.typed_conformance_corpus_ready ? "true" : "false")
      << ";typed_performance_quality_guardrails_consistent="
      << (surface.typed_performance_quality_guardrails_consistent ? "true" : "false")
      << ";typed_performance_quality_guardrails_ready="
      << (surface.typed_performance_quality_guardrails_ready ? "true" : "false")
      << ";typed_cross_lane_integration_consistent="
      << (surface.typed_cross_lane_integration_consistent ? "true" : "false")
      << ";typed_cross_lane_integration_ready="
      << (surface.typed_cross_lane_integration_ready ? "true" : "false")
      << ";typed_docs_runbook_sync_consistent="
      << (surface.typed_docs_runbook_sync_consistent ? "true" : "false")
      << ";typed_docs_runbook_sync_ready="
      << (surface.typed_docs_runbook_sync_ready ? "true" : "false")
      << ";typed_release_candidate_replay_dry_run_consistent="
      << (surface.typed_release_candidate_replay_dry_run_consistent ? "true" : "false")
      << ";typed_release_candidate_replay_dry_run_ready="
      << (surface.typed_release_candidate_replay_dry_run_ready ? "true" : "false")
      << ";typed_advanced_core_shard1_consistent="
      << (surface.typed_advanced_core_shard1_consistent ? "true" : "false")
      << ";typed_advanced_core_shard1_ready="
      << (surface.typed_advanced_core_shard1_ready ? "true" : "false")
      << ";typed_advanced_edge_compatibility_shard1_consistent="
      << (surface.typed_advanced_edge_compatibility_shard1_consistent ? "true" : "false")
      << ";typed_advanced_edge_compatibility_shard1_ready="
      << (surface.typed_advanced_edge_compatibility_shard1_ready ? "true" : "false")
      << ";typed_advanced_diagnostics_shard1_consistent="
      << (surface.typed_advanced_diagnostics_shard1_consistent ? "true" : "false")
      << ";typed_advanced_diagnostics_shard1_ready="
      << (surface.typed_advanced_diagnostics_shard1_ready ? "true" : "false")
      << ";typed_advanced_conformance_shard1_consistent="
      << (surface.typed_advanced_conformance_shard1_consistent ? "true" : "false")
      << ";typed_advanced_conformance_shard1_ready="
      << (surface.typed_advanced_conformance_shard1_ready ? "true" : "false")
      << ";typed_advanced_integration_shard1_consistent="
      << (surface.typed_advanced_integration_shard1_consistent ? "true" : "false")
      << ";typed_advanced_integration_shard1_ready="
      << (surface.typed_advanced_integration_shard1_ready ? "true" : "false")
      << ";typed_advanced_performance_shard1_consistent="
      << (surface.typed_advanced_performance_shard1_consistent ? "true" : "false")
      << ";typed_advanced_performance_shard1_ready="
      << (surface.typed_advanced_performance_shard1_ready ? "true" : "false")
      << ";typed_advanced_core_shard2_consistent="
      << (surface.typed_advanced_core_shard2_consistent ? "true" : "false")
      << ";typed_advanced_core_shard2_ready="
      << (surface.typed_advanced_core_shard2_ready ? "true" : "false")
      << ";typed_advanced_edge_compatibility_shard2_consistent="
      << (surface.typed_advanced_edge_compatibility_shard2_consistent ? "true" : "false")
      << ";typed_advanced_edge_compatibility_shard2_ready="
      << (surface.typed_advanced_edge_compatibility_shard2_ready ? "true" : "false")
      << ";typed_advanced_diagnostics_shard2_consistent="
      << (surface.typed_advanced_diagnostics_shard2_consistent ? "true" : "false")
      << ";typed_advanced_diagnostics_shard2_ready="
      << (surface.typed_advanced_diagnostics_shard2_ready ? "true" : "false")
      << ";typed_advanced_conformance_shard2_consistent="
      << (surface.typed_advanced_conformance_shard2_consistent ? "true" : "false")
      << ";typed_advanced_conformance_shard2_ready="
      << (surface.typed_advanced_conformance_shard2_ready ? "true" : "false")
      << ";typed_advanced_integration_shard2_consistent="
      << (surface.typed_advanced_integration_shard2_consistent ? "true" : "false")
      << ";typed_advanced_integration_shard2_ready="
      << (surface.typed_advanced_integration_shard2_ready ? "true" : "false")
      << ";typed_integration_closeout_signoff_consistent="
      << (surface.typed_integration_closeout_signoff_consistent ? "true" : "false")
      << ";typed_integration_closeout_signoff_ready="
      << (surface.typed_integration_closeout_signoff_ready ? "true" : "false")
      << ";lowering_boundary=" << (surface.lowering_boundary_ready ? "true" : "false")
      << ";ready_for_lowering=" << (surface.ready_for_lowering ? "true" : "false");
  return key.str();
}
