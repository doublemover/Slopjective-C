#include "pipeline/readiness/objc3_typed_sema_lowering_readiness_private.h"

void PopulateObjc3TypedSemaLoweringReadinessAlignments(
    Objc3TypedSemaLoweringReadinessRecord &record,
    const Objc3ParseLoweringReadinessSurface &surface,
    const Objc3TypedSemaToLoweringContractSurface &typed_sema_to_lowering_contract_surface) {
  record.typed_edge_case_compatibility_alignment =
      surface.compatibility_handoff_consistent ==
          typed_sema_to_lowering_contract_surface.compatibility_handoff_consistent &&
      surface.language_version_pragma_coordinate_order_consistent ==
          typed_sema_to_lowering_contract_surface.language_version_pragma_coordinate_order_consistent &&
      surface.parse_artifact_replay_key_deterministic ==
          typed_sema_to_lowering_contract_surface.parse_artifact_replay_key_deterministic &&
      surface.parse_artifact_edge_case_robustness_consistent ==
          typed_sema_to_lowering_contract_surface.parse_artifact_edge_case_robustness_consistent &&
      surface.compatibility_handoff_key ==
          typed_sema_to_lowering_contract_surface.compatibility_handoff_key &&
      surface.parse_artifact_edge_robustness_key ==
          typed_sema_to_lowering_contract_surface.parse_artifact_edge_robustness_key;
  record.typed_edge_case_robustness_alignment =
      surface.typed_sema_edge_case_expansion_consistent ==
          typed_sema_to_lowering_contract_surface.typed_core_feature_edge_case_expansion_consistent &&
      surface.typed_sema_edge_case_robustness_ready ==
          typed_sema_to_lowering_contract_surface.typed_core_feature_edge_case_robustness_ready &&
      surface.typed_sema_edge_case_robustness_key ==
          typed_sema_to_lowering_contract_surface.typed_core_feature_edge_case_robustness_key;
  record.typed_diagnostics_hardening_alignment =
      surface.typed_sema_diagnostics_hardening_consistent ==
          typed_sema_to_lowering_contract_surface.typed_diagnostics_hardening_consistent &&
      surface.typed_sema_diagnostics_hardening_ready ==
          typed_sema_to_lowering_contract_surface.typed_diagnostics_hardening_ready &&
      surface.typed_sema_diagnostics_hardening_key ==
          typed_sema_to_lowering_contract_surface.typed_diagnostics_hardening_key;
  record.typed_recovery_determinism_alignment =
      surface.typed_sema_recovery_determinism_consistent ==
          typed_sema_to_lowering_contract_surface.typed_recovery_determinism_consistent &&
      surface.typed_sema_recovery_determinism_ready ==
          typed_sema_to_lowering_contract_surface.typed_recovery_determinism_ready &&
      surface.typed_sema_recovery_determinism_key ==
          typed_sema_to_lowering_contract_surface.typed_recovery_determinism_key;
  record.typed_conformance_matrix_alignment =
      surface.typed_sema_conformance_matrix_consistent ==
          typed_sema_to_lowering_contract_surface.typed_conformance_matrix_consistent &&
      surface.typed_sema_conformance_matrix_ready ==
          typed_sema_to_lowering_contract_surface.typed_conformance_matrix_ready &&
      surface.typed_sema_conformance_matrix_key ==
          typed_sema_to_lowering_contract_surface.typed_conformance_matrix_key;
  record.typed_conformance_corpus_alignment =
      surface.typed_sema_conformance_corpus_consistent ==
          typed_sema_to_lowering_contract_surface.typed_conformance_corpus_consistent &&
      surface.typed_sema_conformance_corpus_ready ==
          typed_sema_to_lowering_contract_surface.typed_conformance_corpus_ready &&
      surface.typed_sema_conformance_corpus_key ==
          typed_sema_to_lowering_contract_surface.typed_conformance_corpus_key;
  record.typed_performance_quality_guardrails_alignment =
      surface.typed_sema_performance_quality_guardrails_consistent ==
          typed_sema_to_lowering_contract_surface.typed_performance_quality_guardrails_consistent &&
      surface.typed_sema_performance_quality_guardrails_ready ==
          typed_sema_to_lowering_contract_surface.typed_performance_quality_guardrails_ready &&
      surface.typed_sema_performance_quality_guardrails_case_count ==
          typed_sema_to_lowering_contract_surface.typed_performance_quality_guardrails_case_count &&
      surface.typed_sema_performance_quality_guardrails_passed_case_count ==
          typed_sema_to_lowering_contract_surface.typed_performance_quality_guardrails_passed_case_count &&
      surface.typed_sema_performance_quality_guardrails_failed_case_count ==
          typed_sema_to_lowering_contract_surface.typed_performance_quality_guardrails_failed_case_count &&
      surface.typed_sema_performance_quality_guardrails_key ==
          typed_sema_to_lowering_contract_surface.typed_performance_quality_guardrails_key;
  record.typed_cross_lane_integration_alignment =
      surface.typed_sema_cross_lane_integration_consistent ==
          typed_sema_to_lowering_contract_surface.typed_cross_lane_integration_consistent &&
      surface.typed_sema_cross_lane_integration_ready ==
          typed_sema_to_lowering_contract_surface.typed_cross_lane_integration_ready &&
      surface.typed_sema_cross_lane_integration_key ==
          typed_sema_to_lowering_contract_surface.typed_cross_lane_integration_key;
  record.typed_docs_runbook_sync_alignment =
      surface.typed_sema_docs_runbook_sync_consistent ==
          typed_sema_to_lowering_contract_surface.typed_docs_runbook_sync_consistent &&
      surface.typed_sema_docs_runbook_sync_ready ==
          typed_sema_to_lowering_contract_surface.typed_docs_runbook_sync_ready &&
      surface.typed_sema_docs_runbook_sync_key ==
          typed_sema_to_lowering_contract_surface.typed_docs_runbook_sync_key;
  record.typed_release_candidate_replay_dry_run_alignment =
      surface.typed_sema_release_candidate_replay_dry_run_consistent ==
          typed_sema_to_lowering_contract_surface.typed_release_candidate_replay_dry_run_consistent &&
      surface.typed_sema_release_candidate_replay_dry_run_ready ==
          typed_sema_to_lowering_contract_surface.typed_release_candidate_replay_dry_run_ready &&
      surface.typed_sema_release_candidate_replay_dry_run_key ==
          typed_sema_to_lowering_contract_surface.typed_release_candidate_replay_dry_run_key;
  record.typed_advanced_core_shard1_alignment =
      surface.typed_sema_advanced_core_shard1_consistent ==
          typed_sema_to_lowering_contract_surface.typed_advanced_core_shard1_consistent &&
      surface.typed_sema_advanced_core_shard1_ready ==
          typed_sema_to_lowering_contract_surface.typed_advanced_core_shard1_ready &&
      surface.typed_sema_advanced_core_shard1_key ==
          typed_sema_to_lowering_contract_surface.typed_advanced_core_shard1_key;
  record.typed_advanced_edge_compatibility_shard1_alignment =
      surface.typed_sema_advanced_edge_compatibility_shard1_consistent ==
          typed_sema_to_lowering_contract_surface.typed_advanced_edge_compatibility_shard1_consistent &&
      surface.typed_sema_advanced_edge_compatibility_shard1_ready ==
          typed_sema_to_lowering_contract_surface.typed_advanced_edge_compatibility_shard1_ready &&
      surface.typed_sema_advanced_edge_compatibility_shard1_key ==
          typed_sema_to_lowering_contract_surface.typed_advanced_edge_compatibility_shard1_key;
  record.typed_advanced_diagnostics_shard1_alignment =
      surface.typed_sema_advanced_diagnostics_shard1_consistent ==
          typed_sema_to_lowering_contract_surface.typed_advanced_diagnostics_shard1_consistent &&
      surface.typed_sema_advanced_diagnostics_shard1_ready ==
          typed_sema_to_lowering_contract_surface.typed_advanced_diagnostics_shard1_ready &&
      surface.typed_sema_advanced_diagnostics_shard1_key ==
          typed_sema_to_lowering_contract_surface.typed_advanced_diagnostics_shard1_key;
  record.typed_advanced_conformance_shard1_alignment =
      surface.typed_sema_advanced_conformance_shard1_consistent ==
          typed_sema_to_lowering_contract_surface.typed_advanced_conformance_shard1_consistent &&
      surface.typed_sema_advanced_conformance_shard1_ready ==
          typed_sema_to_lowering_contract_surface.typed_advanced_conformance_shard1_ready &&
      surface.typed_sema_advanced_conformance_shard1_key ==
          typed_sema_to_lowering_contract_surface.typed_advanced_conformance_shard1_key;
  record.typed_advanced_integration_shard1_alignment =
      surface.typed_sema_advanced_integration_shard1_consistent ==
          typed_sema_to_lowering_contract_surface.typed_advanced_integration_shard1_consistent &&
      surface.typed_sema_advanced_integration_shard1_ready ==
          typed_sema_to_lowering_contract_surface.typed_advanced_integration_shard1_ready &&
      surface.typed_sema_advanced_integration_shard1_key ==
          typed_sema_to_lowering_contract_surface.typed_advanced_integration_shard1_key;
  record.typed_advanced_performance_shard1_alignment =
      surface.typed_sema_advanced_performance_shard1_consistent ==
          typed_sema_to_lowering_contract_surface.typed_advanced_performance_shard1_consistent &&
      surface.typed_sema_advanced_performance_shard1_ready ==
          typed_sema_to_lowering_contract_surface.typed_advanced_performance_shard1_ready &&
      surface.typed_sema_advanced_performance_shard1_key ==
          typed_sema_to_lowering_contract_surface.typed_advanced_performance_shard1_key;
  record.typed_advanced_core_shard2_alignment =
      surface.typed_sema_advanced_core_shard2_consistent ==
          typed_sema_to_lowering_contract_surface.typed_advanced_core_shard2_consistent &&
      surface.typed_sema_advanced_core_shard2_ready ==
          typed_sema_to_lowering_contract_surface.typed_advanced_core_shard2_ready &&
      surface.typed_sema_advanced_core_shard2_key ==
          typed_sema_to_lowering_contract_surface.typed_advanced_core_shard2_key;
  record.typed_advanced_edge_compatibility_shard2_alignment =
      surface.typed_sema_advanced_edge_compatibility_shard2_consistent ==
          typed_sema_to_lowering_contract_surface.typed_advanced_edge_compatibility_shard2_consistent &&
      surface.typed_sema_advanced_edge_compatibility_shard2_ready ==
          typed_sema_to_lowering_contract_surface.typed_advanced_edge_compatibility_shard2_ready &&
      surface.typed_sema_advanced_edge_compatibility_shard2_key ==
          typed_sema_to_lowering_contract_surface.typed_advanced_edge_compatibility_shard2_key;
  record.typed_advanced_diagnostics_shard2_alignment =
      surface.typed_sema_advanced_diagnostics_shard2_consistent ==
          typed_sema_to_lowering_contract_surface.typed_advanced_diagnostics_shard2_consistent &&
      surface.typed_sema_advanced_diagnostics_shard2_ready ==
          typed_sema_to_lowering_contract_surface.typed_advanced_diagnostics_shard2_ready &&
      surface.typed_sema_advanced_diagnostics_shard2_key ==
          typed_sema_to_lowering_contract_surface.typed_advanced_diagnostics_shard2_key;
  record.typed_advanced_conformance_shard2_alignment =
      surface.typed_sema_advanced_conformance_shard2_consistent ==
          typed_sema_to_lowering_contract_surface.typed_advanced_conformance_shard2_consistent &&
      surface.typed_sema_advanced_conformance_shard2_ready ==
          typed_sema_to_lowering_contract_surface.typed_advanced_conformance_shard2_ready &&
      surface.typed_sema_advanced_conformance_shard2_key ==
          typed_sema_to_lowering_contract_surface.typed_advanced_conformance_shard2_key;
  record.typed_advanced_integration_shard2_alignment =
      surface.typed_sema_advanced_integration_shard2_consistent ==
          typed_sema_to_lowering_contract_surface.typed_advanced_integration_shard2_consistent &&
      surface.typed_sema_advanced_integration_shard2_ready ==
          typed_sema_to_lowering_contract_surface.typed_advanced_integration_shard2_ready &&
      surface.typed_sema_advanced_integration_shard2_key ==
          typed_sema_to_lowering_contract_surface.typed_advanced_integration_shard2_key;
  record.typed_integration_closeout_signoff_alignment =
      surface.typed_sema_integration_closeout_signoff_consistent ==
          typed_sema_to_lowering_contract_surface.typed_integration_closeout_signoff_consistent &&
      surface.typed_sema_integration_closeout_signoff_ready ==
          typed_sema_to_lowering_contract_surface.typed_integration_closeout_signoff_ready &&
      surface.typed_sema_integration_closeout_signoff_key ==
          typed_sema_to_lowering_contract_surface.typed_integration_closeout_signoff_key;
}

bool IsObjc3TypedSemaCoreFeatureReady(
    const Objc3ParseLoweringReadinessSurface &surface,
    const Objc3TypedSemaLoweringReadinessRecord &record,
    bool typed_core_feature_expansion_ready) {
  return surface.typed_handoff_key_deterministic &&
         surface.typed_sema_core_feature_consistent &&
         typed_core_feature_expansion_ready &&
         surface.typed_sema_edge_case_compatibility_ready &&
         surface.typed_sema_edge_case_expansion_consistent &&
         surface.typed_sema_edge_case_robustness_ready &&
         surface.typed_sema_diagnostics_hardening_consistent &&
         surface.typed_sema_diagnostics_hardening_ready &&
         surface.typed_sema_recovery_determinism_consistent &&
         surface.typed_sema_recovery_determinism_ready &&
         surface.typed_sema_conformance_matrix_consistent &&
         surface.typed_sema_conformance_matrix_ready &&
         surface.typed_sema_conformance_corpus_consistent &&
         surface.typed_sema_conformance_corpus_ready &&
         surface.typed_sema_performance_quality_guardrails_consistent &&
         surface.typed_sema_performance_quality_guardrails_ready &&
         surface.typed_sema_cross_lane_integration_consistent &&
         surface.typed_sema_cross_lane_integration_ready &&
         surface.typed_sema_docs_runbook_sync_consistent &&
         surface.typed_sema_docs_runbook_sync_ready &&
         surface.typed_sema_release_candidate_replay_dry_run_consistent &&
         surface.typed_sema_release_candidate_replay_dry_run_ready &&
         surface.typed_sema_advanced_core_shard1_consistent &&
         surface.typed_sema_advanced_core_shard1_ready &&
         surface.typed_sema_advanced_edge_compatibility_shard1_consistent &&
         surface.typed_sema_advanced_edge_compatibility_shard1_ready &&
         surface.typed_sema_advanced_diagnostics_shard1_consistent &&
         surface.typed_sema_advanced_diagnostics_shard1_ready &&
         surface.typed_sema_advanced_conformance_shard1_consistent &&
         surface.typed_sema_advanced_conformance_shard1_ready &&
         surface.typed_sema_advanced_integration_shard1_consistent &&
         surface.typed_sema_advanced_integration_shard1_ready &&
         surface.typed_sema_advanced_performance_shard1_consistent &&
         surface.typed_sema_advanced_performance_shard1_ready &&
         surface.typed_sema_advanced_core_shard2_consistent &&
         surface.typed_sema_advanced_core_shard2_ready &&
         surface.typed_sema_advanced_edge_compatibility_shard2_consistent &&
         surface.typed_sema_advanced_edge_compatibility_shard2_ready &&
         surface.typed_sema_advanced_diagnostics_shard2_consistent &&
         surface.typed_sema_advanced_diagnostics_shard2_ready &&
         surface.typed_sema_advanced_conformance_shard2_consistent &&
         surface.typed_sema_advanced_conformance_shard2_ready &&
         surface.typed_sema_advanced_integration_shard2_consistent &&
         surface.typed_sema_advanced_integration_shard2_ready &&
         surface.typed_sema_integration_closeout_signoff_consistent &&
         surface.typed_sema_integration_closeout_signoff_ready &&
         record.typed_edge_case_compatibility_alignment &&
         record.typed_edge_case_robustness_alignment &&
         !surface.typed_sema_edge_case_compatibility_key.empty() &&
         !surface.typed_sema_edge_case_robustness_key.empty() &&
         record.typed_diagnostics_hardening_alignment &&
         !surface.typed_sema_diagnostics_hardening_key.empty() &&
         record.typed_recovery_determinism_alignment &&
         !surface.typed_sema_recovery_determinism_key.empty() &&
         record.typed_conformance_matrix_alignment &&
         !surface.typed_sema_conformance_matrix_key.empty() &&
         record.typed_conformance_corpus_alignment &&
         !surface.typed_sema_conformance_corpus_key.empty() &&
         record.typed_performance_quality_guardrails_alignment &&
         !surface.typed_sema_performance_quality_guardrails_key.empty() &&
         record.typed_cross_lane_integration_alignment &&
         !surface.typed_sema_cross_lane_integration_key.empty() &&
         record.typed_docs_runbook_sync_alignment &&
         !surface.typed_sema_docs_runbook_sync_key.empty() &&
         record.typed_release_candidate_replay_dry_run_alignment &&
         !surface.typed_sema_release_candidate_replay_dry_run_key.empty() &&
         record.typed_advanced_core_shard1_alignment &&
         !surface.typed_sema_advanced_core_shard1_key.empty() &&
         record.typed_advanced_edge_compatibility_shard1_alignment &&
         !surface.typed_sema_advanced_edge_compatibility_shard1_key.empty() &&
         record.typed_advanced_diagnostics_shard1_alignment &&
         !surface.typed_sema_advanced_diagnostics_shard1_key.empty() &&
         record.typed_advanced_conformance_shard1_alignment &&
         !surface.typed_sema_advanced_conformance_shard1_key.empty() &&
         record.typed_advanced_integration_shard1_alignment &&
         !surface.typed_sema_advanced_integration_shard1_key.empty() &&
         record.typed_advanced_performance_shard1_alignment &&
         !surface.typed_sema_advanced_performance_shard1_key.empty() &&
         record.typed_advanced_core_shard2_alignment &&
         !surface.typed_sema_advanced_core_shard2_key.empty() &&
         record.typed_advanced_edge_compatibility_shard2_alignment &&
         !surface.typed_sema_advanced_edge_compatibility_shard2_key.empty() &&
         record.typed_advanced_diagnostics_shard2_alignment &&
         !surface.typed_sema_advanced_diagnostics_shard2_key.empty() &&
         record.typed_advanced_conformance_shard2_alignment &&
         !surface.typed_sema_advanced_conformance_shard2_key.empty() &&
         record.typed_advanced_integration_shard2_alignment &&
         !surface.typed_sema_advanced_integration_shard2_key.empty() &&
         record.typed_integration_closeout_signoff_alignment &&
         !surface.typed_sema_integration_closeout_signoff_key.empty() &&
         !surface.typed_sema_core_feature_key.empty();
}

bool IsObjc3TypedSemaSemanticHandoffDeterministic(
    const Objc3ParseLoweringReadinessSurface &surface) {
  return surface.semantic_diagnostics_deterministic &&
         surface.semantic_type_metadata_deterministic &&
         surface.protocol_category_deterministic &&
         surface.class_protocol_category_linking_deterministic &&
         surface.selector_normalization_deterministic &&
         surface.property_attribute_deterministic &&
         surface.symbol_graph_deterministic &&
         surface.scope_resolution_deterministic &&
         surface.object_pointer_type_handoff_deterministic;
}
