#pragma once

#include "pipeline/objc3_typed_sema_to_lowering_contract_surface_core.h"

struct Objc3TypedSemaToLoweringContractSurfaceStageState {
  bool typed_diagnostics_hardening_key_ready = false;
  bool typed_recovery_determinism_key_ready = false;
  bool typed_conformance_matrix_key_ready = false;
  bool typed_conformance_corpus_key_ready = false;
  bool typed_performance_quality_guardrails_key_ready = false;
  bool typed_cross_lane_integration_key_ready = false;
  bool typed_docs_runbook_sync_key_ready = false;
  bool typed_release_candidate_replay_dry_run_key_ready = false;
  bool typed_advanced_core_shard1_key_ready = false;
  bool typed_advanced_edge_compatibility_shard1_key_ready = false;
  bool typed_advanced_diagnostics_shard1_key_ready = false;
  bool typed_advanced_conformance_shard1_key_ready = false;
  bool typed_advanced_integration_shard1_key_ready = false;
  bool typed_advanced_performance_shard1_key_ready = false;
  bool typed_advanced_core_shard2_key_ready = false;
  bool typed_advanced_edge_compatibility_shard2_key_ready = false;
  bool typed_advanced_diagnostics_shard2_key_ready = false;
  bool typed_advanced_conformance_shard2_key_ready = false;
  bool typed_advanced_integration_shard2_key_ready = false;
  bool typed_integration_closeout_signoff_key_ready = false;
};

inline Objc3TypedSemaToLoweringContractSurfaceStageState
PopulateObjc3TypedSemaToLoweringContractSurfaceStages(
    Objc3TypedSemaToLoweringContractSurface &surface) {
  Objc3TypedSemaToLoweringContractSurfaceStageState state;

  state.typed_diagnostics_hardening_key_ready =
      !surface.typed_diagnostics_hardening_key.empty();
  surface.typed_recovery_determinism_consistent =
      surface.typed_diagnostics_hardening_ready &&
      surface.parse_artifact_replay_key_deterministic &&
      surface.semantic_handoff_deterministic;
  surface.typed_recovery_determinism_ready =
      surface.typed_recovery_determinism_consistent &&
      !surface.typed_diagnostics_hardening_key.empty() &&
      !surface.typed_core_feature_edge_case_robustness_key.empty();
  surface.typed_recovery_determinism_key =
      BuildObjc3TypedSemaToLoweringRecoveryDeterminismKey(surface);
  state.typed_recovery_determinism_key_ready =
      !surface.typed_recovery_determinism_key.empty();
  surface.typed_conformance_matrix_consistent =
      surface.typed_recovery_determinism_ready &&
      surface.parse_artifact_replay_key_deterministic &&
      surface.semantic_handoff_deterministic &&
      surface.sema_parity_surface_deterministic;
  surface.typed_conformance_matrix_ready =
      surface.typed_conformance_matrix_consistent &&
      !surface.typed_recovery_determinism_key.empty() &&
      !surface.typed_diagnostics_hardening_key.empty();
  surface.typed_conformance_matrix_key =
      BuildObjc3TypedSemaToLoweringConformanceMatrixKey(surface);
  state.typed_conformance_matrix_key_ready =
      !surface.typed_conformance_matrix_key.empty();
  surface.typed_conformance_corpus_consistent =
      surface.typed_conformance_matrix_ready &&
      surface.parse_artifact_replay_key_deterministic &&
      surface.semantic_handoff_deterministic;
  surface.typed_conformance_corpus_ready =
      surface.typed_conformance_corpus_consistent &&
      !surface.typed_conformance_matrix_key.empty() &&
      !surface.typed_recovery_determinism_key.empty();
  surface.typed_conformance_corpus_key =
      BuildObjc3TypedSemaToLoweringConformanceCorpusKey(surface);
  state.typed_conformance_corpus_key_ready =
      !surface.typed_conformance_corpus_key.empty();
  surface.typed_performance_quality_guardrails_case_count =
      kObjc3TypedSemaToLoweringPerformanceQualityGuardrailsCaseCount;
  surface.typed_performance_quality_guardrails_passed_case_count =
      static_cast<std::size_t>(surface.typed_conformance_corpus_consistent) +
      static_cast<std::size_t>(surface.typed_conformance_corpus_ready) +
      static_cast<std::size_t>(surface.parse_artifact_replay_key_deterministic) +
      static_cast<std::size_t>(surface.semantic_handoff_deterministic);
  surface.typed_performance_quality_guardrails_failed_case_count =
      surface.typed_performance_quality_guardrails_case_count >=
              surface.typed_performance_quality_guardrails_passed_case_count
          ? (surface.typed_performance_quality_guardrails_case_count -
             surface.typed_performance_quality_guardrails_passed_case_count)
          : surface.typed_performance_quality_guardrails_case_count;
  const bool typed_performance_quality_guardrails_case_accounting_consistent =
      surface.typed_performance_quality_guardrails_case_count ==
          kObjc3TypedSemaToLoweringPerformanceQualityGuardrailsCaseCount &&
      surface.typed_performance_quality_guardrails_case_count > 0 &&
      surface.typed_performance_quality_guardrails_passed_case_count <=
          surface.typed_performance_quality_guardrails_case_count &&
      surface.typed_performance_quality_guardrails_failed_case_count ==
          (surface.typed_performance_quality_guardrails_case_count -
           surface.typed_performance_quality_guardrails_passed_case_count);
  const bool typed_performance_quality_guardrails_cases_passed =
      surface.typed_performance_quality_guardrails_passed_case_count ==
          surface.typed_performance_quality_guardrails_case_count &&
      surface.typed_performance_quality_guardrails_failed_case_count == 0;
  surface.typed_performance_quality_guardrails_consistent =
      typed_performance_quality_guardrails_case_accounting_consistent &&
      typed_performance_quality_guardrails_cases_passed &&
      state.typed_conformance_corpus_key_ready;
  surface.typed_performance_quality_guardrails_ready =
      surface.typed_performance_quality_guardrails_consistent &&
      !surface.typed_conformance_corpus_key.empty() &&
      !surface.typed_conformance_matrix_key.empty() &&
      !surface.typed_recovery_determinism_key.empty();
  surface.typed_performance_quality_guardrails_key =
      BuildObjc3TypedSemaToLoweringPerformanceQualityGuardrailsKey(surface);
  state.typed_performance_quality_guardrails_key_ready =
      !surface.typed_performance_quality_guardrails_key.empty();
  surface.typed_cross_lane_integration_consistent =
      surface.typed_performance_quality_guardrails_ready &&
      surface.parse_artifact_replay_key_deterministic &&
      surface.semantic_handoff_deterministic &&
      surface.typed_performance_quality_guardrails_case_count > 0;
  surface.typed_cross_lane_integration_ready =
      surface.typed_cross_lane_integration_consistent &&
      !surface.typed_performance_quality_guardrails_key.empty() &&
      !surface.typed_conformance_corpus_key.empty();
  surface.typed_cross_lane_integration_key =
      BuildObjc3TypedSemaToLoweringCrossLaneIntegrationKey(surface);
  state.typed_cross_lane_integration_key_ready =
      !surface.typed_cross_lane_integration_key.empty();
  surface.typed_docs_runbook_sync_consistent =
      surface.typed_cross_lane_integration_ready &&
      surface.parse_artifact_replay_key_deterministic &&
      surface.semantic_handoff_deterministic;
  surface.typed_docs_runbook_sync_ready =
      surface.typed_docs_runbook_sync_consistent &&
      !surface.typed_cross_lane_integration_key.empty() &&
      !surface.typed_performance_quality_guardrails_key.empty();
  surface.typed_docs_runbook_sync_key =
      BuildObjc3TypedSemaToLoweringDocsRunbookSyncKey(surface);
  state.typed_docs_runbook_sync_key_ready =
      !surface.typed_docs_runbook_sync_key.empty();
  surface.typed_release_candidate_replay_dry_run_consistent =
      surface.typed_docs_runbook_sync_ready &&
      surface.parse_artifact_replay_key_deterministic &&
      surface.semantic_handoff_deterministic;
  surface.typed_release_candidate_replay_dry_run_ready =
      surface.typed_release_candidate_replay_dry_run_consistent &&
      !surface.typed_docs_runbook_sync_key.empty() &&
      !surface.typed_cross_lane_integration_key.empty();
  surface.typed_release_candidate_replay_dry_run_key =
      BuildObjc3TypedSemaToLoweringReleaseCandidateReplayDryRunKey(surface);
  state.typed_release_candidate_replay_dry_run_key_ready =
      !surface.typed_release_candidate_replay_dry_run_key.empty();
  surface.typed_advanced_core_shard1_consistent =
      surface.typed_release_candidate_replay_dry_run_ready &&
      surface.parse_artifact_replay_key_deterministic &&
      surface.semantic_handoff_deterministic;
  surface.typed_advanced_core_shard1_ready =
      surface.typed_advanced_core_shard1_consistent &&
      !surface.typed_release_candidate_replay_dry_run_key.empty() &&
      !surface.typed_docs_runbook_sync_key.empty();
  surface.typed_advanced_core_shard1_key =
      BuildObjc3TypedSemaToLoweringAdvancedCoreShard1Key(surface);
  state.typed_advanced_core_shard1_key_ready =
      !surface.typed_advanced_core_shard1_key.empty();
  surface.typed_advanced_edge_compatibility_shard1_consistent =
      surface.typed_advanced_core_shard1_ready &&
      surface.parse_artifact_replay_key_deterministic &&
      surface.semantic_handoff_deterministic;
  surface.typed_advanced_edge_compatibility_shard1_ready =
      surface.typed_advanced_edge_compatibility_shard1_consistent &&
      !surface.typed_advanced_core_shard1_key.empty() &&
      !surface.typed_release_candidate_replay_dry_run_key.empty();
  surface.typed_advanced_edge_compatibility_shard1_key =
      BuildObjc3TypedSemaToLoweringAdvancedEdgeCompatibilityShard1Key(surface);
  state.typed_advanced_edge_compatibility_shard1_key_ready =
      !surface.typed_advanced_edge_compatibility_shard1_key.empty();
  surface.typed_advanced_diagnostics_shard1_consistent =
      surface.typed_advanced_edge_compatibility_shard1_ready &&
      surface.parse_artifact_replay_key_deterministic &&
      surface.semantic_handoff_deterministic;
  surface.typed_advanced_diagnostics_shard1_ready =
      surface.typed_advanced_diagnostics_shard1_consistent &&
      !surface.typed_advanced_edge_compatibility_shard1_key.empty() &&
      !surface.typed_release_candidate_replay_dry_run_key.empty();
  surface.typed_advanced_diagnostics_shard1_key =
      BuildObjc3TypedSemaToLoweringAdvancedDiagnosticsShard1Key(surface);
  state.typed_advanced_diagnostics_shard1_key_ready =
      !surface.typed_advanced_diagnostics_shard1_key.empty();
  surface.typed_advanced_conformance_shard1_consistent =
      surface.typed_advanced_diagnostics_shard1_ready &&
      surface.parse_artifact_replay_key_deterministic &&
      surface.semantic_handoff_deterministic;
  surface.typed_advanced_conformance_shard1_ready =
      surface.typed_advanced_conformance_shard1_consistent &&
      !surface.typed_advanced_diagnostics_shard1_key.empty() &&
      !surface.typed_release_candidate_replay_dry_run_key.empty();
  surface.typed_advanced_conformance_shard1_key =
      BuildObjc3TypedSemaToLoweringAdvancedConformanceShard1Key(surface);
  state.typed_advanced_conformance_shard1_key_ready =
      !surface.typed_advanced_conformance_shard1_key.empty();
  surface.typed_advanced_integration_shard1_consistent =
      surface.typed_advanced_conformance_shard1_ready &&
      surface.parse_artifact_replay_key_deterministic &&
      surface.semantic_handoff_deterministic;
  surface.typed_advanced_integration_shard1_ready =
      surface.typed_advanced_integration_shard1_consistent &&
      !surface.typed_advanced_conformance_shard1_key.empty() &&
      !surface.typed_release_candidate_replay_dry_run_key.empty();
  surface.typed_advanced_integration_shard1_key =
      BuildObjc3TypedSemaToLoweringAdvancedIntegrationShard1Key(surface);
  state.typed_advanced_integration_shard1_key_ready =
      !surface.typed_advanced_integration_shard1_key.empty();
  surface.typed_advanced_performance_shard1_consistent =
      surface.typed_advanced_integration_shard1_ready &&
      surface.parse_artifact_replay_key_deterministic &&
      surface.semantic_handoff_deterministic;
  surface.typed_advanced_performance_shard1_ready =
      surface.typed_advanced_performance_shard1_consistent &&
      !surface.typed_advanced_integration_shard1_key.empty() &&
      !surface.typed_release_candidate_replay_dry_run_key.empty();
  surface.typed_advanced_performance_shard1_key =
      BuildObjc3TypedSemaToLoweringAdvancedPerformanceShard1Key(surface);
  state.typed_advanced_performance_shard1_key_ready =
      !surface.typed_advanced_performance_shard1_key.empty();
  surface.typed_advanced_core_shard2_consistent =
      surface.typed_advanced_performance_shard1_ready &&
      surface.parse_artifact_replay_key_deterministic &&
      surface.semantic_handoff_deterministic;
  surface.typed_advanced_core_shard2_ready =
      surface.typed_advanced_core_shard2_consistent &&
      !surface.typed_advanced_performance_shard1_key.empty() &&
      !surface.typed_release_candidate_replay_dry_run_key.empty();
  surface.typed_advanced_core_shard2_key =
      BuildObjc3TypedSemaToLoweringAdvancedCoreShard2Key(surface);
  state.typed_advanced_core_shard2_key_ready =
      !surface.typed_advanced_core_shard2_key.empty();
  surface.typed_advanced_edge_compatibility_shard2_consistent =
      surface.typed_advanced_core_shard2_ready &&
      surface.parse_artifact_replay_key_deterministic &&
      surface.semantic_handoff_deterministic;
  surface.typed_advanced_edge_compatibility_shard2_ready =
      surface.typed_advanced_edge_compatibility_shard2_consistent &&
      !surface.typed_advanced_core_shard2_key.empty() &&
      !surface.typed_advanced_performance_shard1_key.empty();
  surface.typed_advanced_edge_compatibility_shard2_key =
      BuildObjc3TypedSemaToLoweringAdvancedEdgeCompatibilityShard2Key(surface);
  state.typed_advanced_edge_compatibility_shard2_key_ready =
      !surface.typed_advanced_edge_compatibility_shard2_key.empty();
  surface.typed_advanced_diagnostics_shard2_consistent =
      surface.typed_advanced_edge_compatibility_shard2_ready &&
      surface.parse_artifact_replay_key_deterministic &&
      surface.semantic_handoff_deterministic;
  surface.typed_advanced_diagnostics_shard2_ready =
      surface.typed_advanced_diagnostics_shard2_consistent &&
      !surface.typed_advanced_edge_compatibility_shard2_key.empty() &&
      !surface.typed_advanced_core_shard2_key.empty();
  surface.typed_advanced_diagnostics_shard2_key =
      BuildObjc3TypedSemaToLoweringAdvancedDiagnosticsShard2Key(surface);
  state.typed_advanced_diagnostics_shard2_key_ready =
      !surface.typed_advanced_diagnostics_shard2_key.empty();
  surface.typed_advanced_conformance_shard2_consistent =
      surface.typed_advanced_diagnostics_shard2_ready &&
      surface.parse_artifact_replay_key_deterministic &&
      surface.semantic_handoff_deterministic;
  surface.typed_advanced_conformance_shard2_ready =
      surface.typed_advanced_conformance_shard2_consistent &&
      !surface.typed_advanced_diagnostics_shard2_key.empty() &&
      !surface.typed_advanced_edge_compatibility_shard2_key.empty();
  surface.typed_advanced_conformance_shard2_key =
      BuildObjc3TypedSemaToLoweringAdvancedConformanceShard2Key(surface);
  state.typed_advanced_conformance_shard2_key_ready =
      !surface.typed_advanced_conformance_shard2_key.empty();
  surface.typed_advanced_integration_shard2_consistent =
      surface.typed_advanced_conformance_shard2_ready &&
      surface.parse_artifact_replay_key_deterministic &&
      surface.semantic_handoff_deterministic;
  surface.typed_advanced_integration_shard2_ready =
      surface.typed_advanced_integration_shard2_consistent &&
      !surface.typed_advanced_conformance_shard2_key.empty() &&
      !surface.typed_advanced_diagnostics_shard2_key.empty();
  surface.typed_advanced_integration_shard2_key =
      BuildObjc3TypedSemaToLoweringAdvancedIntegrationShard2Key(surface);
  state.typed_advanced_integration_shard2_key_ready =
      !surface.typed_advanced_integration_shard2_key.empty();
  surface.typed_integration_closeout_signoff_consistent =
      surface.typed_advanced_integration_shard2_ready &&
      surface.parse_artifact_replay_key_deterministic &&
      surface.semantic_handoff_deterministic;
  surface.typed_integration_closeout_signoff_ready =
      surface.typed_integration_closeout_signoff_consistent &&
      !surface.typed_advanced_integration_shard2_key.empty();
  surface.typed_integration_closeout_signoff_key =
      BuildObjc3TypedSemaToLoweringIntegrationCloseoutSignoffKey(surface);
  state.typed_integration_closeout_signoff_key_ready =
      !surface.typed_integration_closeout_signoff_key.empty();

  return state;
}
