#pragma once

#include "lower/model/lowered_owner_contracts.h"

#include <cstddef>
#include <string>

struct Objc3TypedSemaToLoweringContractSurface {
  bool semantic_integration_surface_built = false;
  bool semantic_type_metadata_handoff_deterministic = false;
  bool sema_parity_surface_ready = false;
  bool sema_parity_surface_deterministic = false;
  bool executable_metadata_lowering_handoff_ready = false;
  bool executable_metadata_lowering_handoff_deterministic = false;
  bool executable_metadata_typed_lowering_handoff_ready = false;
  bool executable_metadata_typed_lowering_handoff_deterministic = false;
  bool protocol_category_handoff_deterministic = false;
  bool class_protocol_category_linking_handoff_deterministic = false;
  bool selector_normalization_handoff_deterministic = false;
  bool property_attribute_handoff_deterministic = false;
  bool object_pointer_type_handoff_deterministic = false;
  bool symbol_graph_handoff_deterministic = false;
  bool scope_resolution_handoff_deterministic = false;
  bool semantic_handoff_consistent = false;
  bool semantic_handoff_deterministic = false;
  bool runtime_dispatch_contract_consistent = false;
  bool typed_handoff_key_deterministic = false;
  bool typed_core_feature_consistent = false;
  bool typed_core_feature_expansion_consistent = false;
  bool compatibility_handoff_consistent = false;
  bool language_version_pragma_coordinate_order_consistent = false;
  bool parse_artifact_replay_key_deterministic = false;
  bool parse_artifact_edge_case_robustness_consistent = false;
  bool typed_core_feature_edge_case_compatibility_ready = false;
  bool typed_core_feature_edge_case_expansion_consistent = false;
  bool typed_core_feature_edge_case_robustness_ready = false;
  bool typed_diagnostics_hardening_consistent = false;
  bool typed_diagnostics_hardening_ready = false;
  bool typed_recovery_determinism_consistent = false;
  bool typed_recovery_determinism_ready = false;
  bool typed_conformance_matrix_consistent = false;
  bool typed_conformance_matrix_ready = false;
  bool typed_conformance_corpus_consistent = false;
  bool typed_conformance_corpus_ready = false;
  bool typed_performance_quality_guardrails_consistent = false;
  bool typed_performance_quality_guardrails_ready = false;
  bool typed_cross_lane_integration_consistent = false;
  bool typed_cross_lane_integration_ready = false;
  bool typed_docs_runbook_sync_consistent = false;
  bool typed_docs_runbook_sync_ready = false;
  bool typed_release_candidate_replay_dry_run_consistent = false;
  bool typed_release_candidate_replay_dry_run_ready = false;
  bool typed_advanced_core_shard1_consistent = false;
  bool typed_advanced_core_shard1_ready = false;
  bool typed_advanced_edge_compatibility_shard1_consistent = false;
  bool typed_advanced_edge_compatibility_shard1_ready = false;
  bool typed_advanced_diagnostics_shard1_consistent = false;
  bool typed_advanced_diagnostics_shard1_ready = false;
  bool typed_advanced_conformance_shard1_consistent = false;
  bool typed_advanced_conformance_shard1_ready = false;
  bool typed_advanced_integration_shard1_consistent = false;
  bool typed_advanced_integration_shard1_ready = false;
  bool typed_advanced_performance_shard1_consistent = false;
  bool typed_advanced_performance_shard1_ready = false;
  bool typed_advanced_core_shard2_consistent = false;
  bool typed_advanced_core_shard2_ready = false;
  bool typed_advanced_edge_compatibility_shard2_consistent = false;
  bool typed_advanced_edge_compatibility_shard2_ready = false;
  bool typed_advanced_diagnostics_shard2_consistent = false;
  bool typed_advanced_diagnostics_shard2_ready = false;
  bool typed_advanced_conformance_shard2_consistent = false;
  bool typed_advanced_conformance_shard2_ready = false;
  bool typed_advanced_integration_shard2_consistent = false;
  bool typed_advanced_integration_shard2_ready = false;
  bool typed_integration_closeout_signoff_consistent = false;
  bool typed_integration_closeout_signoff_ready = false;
  bool lowering_boundary_ready = false;
  bool ready_for_lowering = false;
  std::size_t typed_core_feature_case_count = 0;
  std::size_t typed_core_feature_passed_case_count = 0;
  std::size_t typed_core_feature_failed_case_count = 0;
  std::size_t typed_core_feature_expansion_case_count = 0;
  std::size_t typed_core_feature_expansion_passed_case_count = 0;
  std::size_t typed_core_feature_expansion_failed_case_count = 0;
  std::size_t typed_performance_quality_guardrails_case_count = 0;
  std::size_t typed_performance_quality_guardrails_passed_case_count = 0;
  std::size_t typed_performance_quality_guardrails_failed_case_count = 0;
  std::string typed_handoff_key;
  std::string executable_metadata_lowering_handoff_key;
  std::string executable_metadata_typed_lowering_handoff_key;
  std::string typed_core_feature_key;
  std::string typed_core_feature_expansion_key;
  std::string compatibility_handoff_key;
  std::string parse_artifact_edge_robustness_key;
  std::string typed_core_feature_edge_case_compatibility_key;
  std::string typed_core_feature_edge_case_robustness_key;
  std::string typed_diagnostics_hardening_key;
  std::string typed_recovery_determinism_key;
  std::string typed_conformance_matrix_key;
  std::string typed_conformance_corpus_key;
  std::string typed_performance_quality_guardrails_key;
  std::string typed_cross_lane_integration_key;
  std::string typed_docs_runbook_sync_key;
  std::string typed_release_candidate_replay_dry_run_key;
  std::string typed_advanced_core_shard1_key;
  std::string typed_advanced_edge_compatibility_shard1_key;
  std::string typed_advanced_diagnostics_shard1_key;
  std::string typed_advanced_conformance_shard1_key;
  std::string typed_advanced_integration_shard1_key;
  std::string typed_advanced_performance_shard1_key;
  std::string typed_advanced_core_shard2_key;
  std::string typed_advanced_edge_compatibility_shard2_key;
  std::string typed_advanced_diagnostics_shard2_key;
  std::string typed_advanced_conformance_shard2_key;
  std::string typed_advanced_integration_shard2_key;
  std::string typed_integration_closeout_signoff_key;
  std::string lowering_boundary_replay_key;
  std::string failure_reason;
};

inline Objc3LoweringOwnerContractRecord
Objc3TypedSemaToLoweringOwnerContractRecord(
    const Objc3TypedSemaToLoweringContractSurface &surface) {
  const bool producer_ready =
      surface.semantic_integration_surface_built &&
      surface.semantic_type_metadata_handoff_deterministic &&
      surface.semantic_handoff_consistent &&
      surface.semantic_handoff_deterministic;
  const bool consumer_ready =
      surface.lowering_boundary_ready && surface.ready_for_lowering &&
      surface.runtime_dispatch_contract_consistent;
  const bool replay_key_deterministic =
      surface.typed_handoff_key_deterministic &&
      !surface.typed_handoff_key.empty() &&
      !surface.lowering_boundary_replay_key.empty();
  const bool artifact_publication_ready =
      surface.executable_metadata_lowering_handoff_ready &&
      surface.executable_metadata_lowering_handoff_deterministic &&
      surface.executable_metadata_typed_lowering_handoff_ready &&
      surface.executable_metadata_typed_lowering_handoff_deterministic &&
      !surface.executable_metadata_typed_lowering_handoff_key.empty();
  const bool fail_closed =
      surface.typed_cross_lane_integration_consistent &&
      surface.typed_cross_lane_integration_ready &&
      surface.typed_integration_closeout_signoff_consistent &&
      surface.typed_integration_closeout_signoff_ready &&
      surface.failure_reason.empty();

  return BuildObjc3LoweringOwnerContractRecord(
      kObjc3LoweringOwnerTypedSemaHandoffContractId,
      kObjc3LoweringOwnerTypedSemaHandoff, surface.typed_handoff_key,
      surface.lowering_boundary_replay_key,
      surface.executable_metadata_typed_lowering_handoff_key, producer_ready,
      consumer_ready, replay_key_deterministic, artifact_publication_ready,
      fail_closed, surface.failure_reason);
}
