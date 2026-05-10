#pragma once

#include "pipeline/objc3_typed_sema_to_lowering_contract_surface_foundation.h"
#include "pipeline/objc3_typed_sema_to_lowering_contract_surface_stages.h"

inline void FinalizeObjc3TypedSemaToLoweringContractSurfaceReadiness(
    Objc3TypedSemaToLoweringContractSurface &surface,
    const Objc3TypedSemaToLoweringContractSurfaceFoundationState &foundation,
    const Objc3TypedSemaToLoweringContractSurfaceStageState &stages) {
  surface.typed_core_feature_consistent =
      foundation.typed_core_feature_consistent &&
      surface.typed_core_feature_expansion_consistent &&
      foundation.typed_core_feature_expansion_key_ready &&
      surface.typed_core_feature_edge_case_compatibility_ready &&
      foundation.typed_core_feature_edge_case_compatibility_key_ready &&
      surface.typed_core_feature_edge_case_expansion_consistent &&
      surface.typed_core_feature_edge_case_robustness_ready &&
      foundation.typed_core_feature_edge_case_robustness_key_ready &&
      surface.typed_diagnostics_hardening_consistent &&
      surface.typed_diagnostics_hardening_ready &&
      stages.typed_diagnostics_hardening_key_ready &&
      surface.typed_recovery_determinism_consistent &&
      surface.typed_recovery_determinism_ready &&
      stages.typed_recovery_determinism_key_ready &&
      surface.typed_conformance_matrix_consistent &&
      surface.typed_conformance_matrix_ready &&
      stages.typed_conformance_matrix_key_ready &&
      surface.typed_conformance_corpus_consistent &&
      surface.typed_conformance_corpus_ready &&
      stages.typed_conformance_corpus_key_ready &&
      surface.typed_performance_quality_guardrails_consistent &&
      surface.typed_performance_quality_guardrails_ready &&
      stages.typed_performance_quality_guardrails_key_ready &&
      surface.typed_cross_lane_integration_consistent &&
      surface.typed_cross_lane_integration_ready &&
      stages.typed_cross_lane_integration_key_ready &&
      surface.typed_docs_runbook_sync_consistent &&
      surface.typed_docs_runbook_sync_ready &&
      stages.typed_docs_runbook_sync_key_ready &&
      surface.typed_release_candidate_replay_dry_run_consistent &&
      surface.typed_release_candidate_replay_dry_run_ready &&
      stages.typed_release_candidate_replay_dry_run_key_ready &&
      surface.typed_advanced_core_shard1_consistent &&
      surface.typed_advanced_core_shard1_ready &&
      stages.typed_advanced_core_shard1_key_ready &&
      surface.typed_advanced_edge_compatibility_shard1_consistent &&
      surface.typed_advanced_edge_compatibility_shard1_ready &&
      stages.typed_advanced_edge_compatibility_shard1_key_ready &&
      surface.typed_advanced_diagnostics_shard1_consistent &&
      surface.typed_advanced_diagnostics_shard1_ready &&
      stages.typed_advanced_diagnostics_shard1_key_ready &&
      surface.typed_advanced_conformance_shard1_consistent &&
      surface.typed_advanced_conformance_shard1_ready &&
      stages.typed_advanced_conformance_shard1_key_ready &&
      surface.typed_advanced_integration_shard1_consistent &&
      surface.typed_advanced_integration_shard1_ready &&
      stages.typed_advanced_integration_shard1_key_ready &&
      surface.typed_advanced_performance_shard1_consistent &&
      surface.typed_advanced_performance_shard1_ready &&
      stages.typed_advanced_performance_shard1_key_ready &&
      surface.typed_advanced_core_shard2_consistent &&
      surface.typed_advanced_core_shard2_ready &&
      stages.typed_advanced_core_shard2_key_ready &&
      surface.typed_advanced_edge_compatibility_shard2_consistent &&
      surface.typed_advanced_edge_compatibility_shard2_ready &&
      stages.typed_advanced_edge_compatibility_shard2_key_ready &&
      surface.typed_advanced_diagnostics_shard2_consistent &&
      surface.typed_advanced_diagnostics_shard2_ready &&
      stages.typed_advanced_diagnostics_shard2_key_ready &&
      surface.typed_advanced_conformance_shard2_consistent &&
      surface.typed_advanced_conformance_shard2_ready &&
      stages.typed_advanced_conformance_shard2_key_ready &&
      surface.typed_advanced_integration_shard2_consistent &&
      surface.typed_advanced_integration_shard2_ready &&
      stages.typed_advanced_integration_shard2_key_ready &&
      surface.typed_integration_closeout_signoff_consistent &&
      surface.typed_integration_closeout_signoff_ready &&
      stages.typed_integration_closeout_signoff_key_ready;

  surface.ready_for_lowering = surface.typed_core_feature_consistent;
  surface.typed_handoff_key = BuildObjc3TypedSemaToLoweringContractHandoffKey(surface);
  surface.typed_handoff_key_deterministic =
      surface.semantic_handoff_deterministic &&
      surface.runtime_dispatch_contract_consistent &&
      surface.lowering_boundary_ready &&
      !surface.typed_handoff_key.empty();
  surface.typed_core_feature_consistent =
      surface.typed_core_feature_consistent &&
      surface.typed_handoff_key_deterministic;
  surface.typed_core_feature_key = BuildObjc3TypedSemaToLoweringCoreFeatureKey(surface);
  surface.ready_for_lowering = surface.typed_core_feature_consistent;
}
