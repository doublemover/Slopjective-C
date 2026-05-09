#pragma once

#include <string>

#include "pipeline/objc3_frontend_types.h"
#include "pipeline/readiness/objc3_final_readiness_gate_core_keys.h"
#include "pipeline/readiness/objc3_final_readiness_gate_advanced_keys.h"

inline Objc3FinalReadinessGateCoreFeatureImplementationSurface
BuildObjc3FinalReadinessGateCoreFeatureImplementationSurface(
    const Objc3FinalReadinessGateCoreFeatureScaffold &scaffold,
    const Objc3FrontendLongTailGrammarCoreFeatureSurface &lane_a_surface,
    const Objc3SemanticStabilityCoreFeatureImplementationSurface &lane_b_surface,
    const Objc3LoweringRuntimeStabilityCoreFeatureImplementationSurface &lane_c_surface,
    const Objc3ToolchainRuntimeGaOperationsCoreFeatureExpansionSurface &lane_d_surface) {
  Objc3FinalReadinessGateCoreFeatureImplementationSurface surface;
  surface.governance_contract_ready = scaffold.governance_contract_ready;
  surface.modular_split_ready = scaffold.modular_split_ready;
  surface.lane_a_core_feature_ready = lane_a_surface.core_feature_ready;
  surface.lane_b_core_feature_ready = lane_b_surface.core_feature_impl_ready;
  surface.lane_c_core_feature_ready = lane_c_surface.core_feature_impl_ready;
  surface.lane_d_core_feature_ready = lane_d_surface.core_feature_impl_ready;
  surface.governance_key = scaffold.governance_key;
  surface.modular_split_key = scaffold.modular_split_key;
  surface.lane_a_key = lane_a_surface.core_feature_key;
  surface.lane_b_key = lane_b_surface.core_feature_key;
  surface.lane_c_key = lane_c_surface.core_feature_key;
  surface.lane_d_key = lane_d_surface.core_feature_key;

  const bool upstream_lanes_ready =
      surface.lane_a_core_feature_ready &&
      surface.lane_b_core_feature_ready &&
      surface.lane_c_core_feature_ready &&
      surface.lane_d_core_feature_ready;
  const bool replay_keys_ready =
      !surface.governance_key.empty() &&
      !surface.modular_split_key.empty() &&
      !surface.lane_a_key.empty() &&
      !surface.lane_b_key.empty() &&
      !surface.lane_c_key.empty() &&
      !surface.lane_d_key.empty();

  surface.dependency_chain_ready =
      surface.governance_contract_ready &&
      surface.modular_split_ready &&
      upstream_lanes_ready &&
      replay_keys_ready;
  surface.core_feature_impl_ready = surface.dependency_chain_ready;
  const bool lane_expansion_consistent =
      lane_a_surface.expansion_ready &&
      lane_b_surface.expansion_ready &&
      lane_c_surface.expansion_ready &&
      lane_d_surface.core_feature_expansion_ready;
  const bool core_feature_expansion_consistent =
      surface.core_feature_impl_ready &&
      lane_expansion_consistent;
  const bool core_feature_expansion_ready =
      core_feature_expansion_consistent &&
      !surface.governance_key.empty() &&
      !surface.modular_split_key.empty();
  surface.core_feature_expansion_consistent = core_feature_expansion_consistent;
  surface.core_feature_expansion_ready = core_feature_expansion_ready;
  surface.core_feature_key =
      BuildObjc3FinalReadinessGateCoreFeatureImplementationKey(surface);
  surface.core_feature_expansion_key =
      "final-readiness-gate-core-feature-expansion:v1:"
      "dependency-chain-ready=" +
      std::string(surface.dependency_chain_ready ? "true" : "false") +
      ";lane-a-expansion-ready=" +
      std::string(lane_a_surface.expansion_ready ? "true" : "false") +
      ";lane-b-expansion-ready=" +
      std::string(lane_b_surface.expansion_ready ? "true" : "false") +
      ";lane-c-expansion-ready=" +
      std::string(lane_c_surface.expansion_ready ? "true" : "false") +
      ";lane-d-core-feature-expansion-ready=" +
      std::string(lane_d_surface.core_feature_expansion_ready ? "true" : "false") +
      ";core-feature-expansion-consistent=" +
      std::string(core_feature_expansion_consistent ? "true" : "false") +
      ";core-feature-expansion-ready=" +
      std::string(core_feature_expansion_ready ? "true" : "false");

  const bool lane_edge_case_compatibility_consistent =
      lane_a_surface.edge_case_compatibility_ready &&
      lane_b_surface.edge_case_compatibility_ready &&
      lane_c_surface.edge_case_compatibility_ready &&
      lane_d_surface.edge_case_compatibility_ready;
  const bool edge_case_compatibility_consistent =
      surface.core_feature_expansion_ready &&
      lane_edge_case_compatibility_consistent;
  const bool edge_case_compatibility_ready =
      edge_case_compatibility_consistent &&
      !surface.governance_key.empty() &&
      !surface.modular_split_key.empty() &&
      !surface.core_feature_expansion_key.empty();

  surface.core_feature_expansion_ready =
      surface.core_feature_expansion_ready &&
      !surface.core_feature_expansion_key.empty();
  surface.edge_case_compatibility_consistent =
      edge_case_compatibility_consistent;
  surface.edge_case_compatibility_ready =
      edge_case_compatibility_ready;
  surface.edge_case_compatibility_key =
      BuildObjc3FinalReadinessGateEdgeCaseCompatibilityKey(
          surface,
          lane_a_surface.edge_case_compatibility_ready,
          lane_b_surface.edge_case_compatibility_ready,
          lane_c_surface.edge_case_compatibility_ready,
          lane_d_surface.edge_case_compatibility_ready);
  surface.edge_case_compatibility_ready =
      surface.edge_case_compatibility_ready &&
      !surface.edge_case_compatibility_key.empty();

  const bool lane_edge_case_expansion_consistent =
      lane_a_surface.core_feature_ready &&
      lane_b_surface.core_feature_impl_ready &&
      lane_c_surface.core_feature_impl_ready &&
      lane_d_surface.edge_case_compatibility_ready;
  const bool edge_case_expansion_consistent =
      surface.edge_case_compatibility_ready &&
      lane_edge_case_expansion_consistent;
  const bool edge_case_robustness_ready =
      edge_case_expansion_consistent &&
      !surface.governance_key.empty() &&
      !surface.modular_split_key.empty() &&
      !surface.edge_case_compatibility_key.empty();
  surface.edge_case_expansion_consistent =
      edge_case_expansion_consistent;
  surface.edge_case_robustness_ready =
      edge_case_robustness_ready;
  surface.edge_case_robustness_key =
      BuildObjc3FinalReadinessGateEdgeCaseRobustnessKey(
          surface,
          lane_a_surface.core_feature_ready,
          lane_b_surface.core_feature_impl_ready,
          lane_c_surface.core_feature_impl_ready,
          lane_d_surface.edge_case_compatibility_ready);
  surface.edge_case_robustness_ready =
      surface.edge_case_robustness_ready &&
      !surface.edge_case_robustness_key.empty();
  const bool lane_diagnostics_hardening_consistent =
      lane_a_surface.core_feature_ready &&
      lane_b_surface.core_feature_impl_ready &&
      lane_c_surface.core_feature_impl_ready &&
      lane_d_surface.edge_case_robustness_ready;
  const bool diagnostics_hardening_consistent =
      surface.edge_case_robustness_ready &&
      lane_diagnostics_hardening_consistent;
  const bool diagnostics_hardening_ready =
      diagnostics_hardening_consistent &&
      !surface.governance_key.empty() &&
      !surface.modular_split_key.empty() &&
      !surface.edge_case_robustness_key.empty();
  surface.diagnostics_hardening_consistent =
      diagnostics_hardening_consistent;
  surface.diagnostics_hardening_ready =
      diagnostics_hardening_ready;
  surface.diagnostics_hardening_key =
      BuildObjc3FinalReadinessGateDiagnosticsHardeningKey(
          surface,
          lane_a_surface.core_feature_ready,
          lane_b_surface.core_feature_impl_ready,
          lane_c_surface.core_feature_impl_ready,
          lane_d_surface.edge_case_robustness_ready);
  surface.diagnostics_hardening_ready =
      surface.diagnostics_hardening_ready &&
      !surface.diagnostics_hardening_key.empty();
  const bool lane_recovery_determinism_consistent =
      lane_a_surface.core_feature_ready &&
      lane_b_surface.expansion_ready &&
      lane_c_surface.expansion_ready &&
      lane_d_surface.diagnostics_hardening_ready;
  const bool recovery_determinism_consistent =
      surface.diagnostics_hardening_ready &&
      lane_recovery_determinism_consistent;
  const bool recovery_determinism_ready =
      recovery_determinism_consistent &&
      !surface.governance_key.empty() &&
      !surface.modular_split_key.empty() &&
      !surface.diagnostics_hardening_key.empty();
  surface.recovery_determinism_consistent =
      recovery_determinism_consistent;
  surface.recovery_determinism_ready =
      recovery_determinism_ready;
  surface.recovery_determinism_key =
      BuildObjc3FinalReadinessGateRecoveryDeterminismKey(
          surface,
          lane_a_surface.core_feature_ready,
          lane_b_surface.expansion_ready,
          lane_c_surface.expansion_ready,
          lane_d_surface.diagnostics_hardening_ready);
  surface.recovery_determinism_ready =
      surface.recovery_determinism_ready &&
      !surface.recovery_determinism_key.empty();
  const bool lane_conformance_matrix_consistent =
      lane_a_surface.core_feature_ready &&
      lane_b_surface.expansion_ready &&
      lane_c_surface.expansion_ready &&
      lane_d_surface.diagnostics_hardening_ready;
  const bool conformance_matrix_consistent =
      surface.recovery_determinism_ready &&
      lane_conformance_matrix_consistent;
  const bool conformance_matrix_ready =
      conformance_matrix_consistent &&
      !surface.governance_key.empty() &&
      !surface.modular_split_key.empty() &&
      !surface.recovery_determinism_key.empty();
  surface.conformance_matrix_consistent =
      conformance_matrix_consistent;
  surface.conformance_matrix_ready =
      conformance_matrix_ready;
  surface.conformance_matrix_key =
      BuildObjc3FinalReadinessGateConformanceMatrixKey(
          surface,
          lane_a_surface.core_feature_ready,
          lane_b_surface.expansion_ready,
          lane_c_surface.expansion_ready,
          lane_d_surface.diagnostics_hardening_ready);
  surface.conformance_matrix_ready =
      surface.conformance_matrix_ready &&
      !surface.conformance_matrix_key.empty();
  const bool lane_conformance_corpus_consistent =
      lane_a_surface.core_feature_expansion_ready &&
      lane_b_surface.expansion_ready &&
      lane_c_surface.edge_case_compatibility_ready &&
      lane_d_surface.recovery_determinism_ready &&
      !lane_d_surface.recovery_determinism_key.empty();
  const bool conformance_corpus_consistent =
      surface.conformance_matrix_ready &&
      lane_conformance_corpus_consistent;
  const bool conformance_corpus_ready =
      conformance_corpus_consistent &&
      !surface.governance_key.empty() &&
      !surface.modular_split_key.empty() &&
      !surface.conformance_matrix_key.empty() &&
      !lane_d_surface.recovery_determinism_key.empty();
  surface.conformance_corpus_consistent =
      conformance_corpus_consistent;
  surface.conformance_corpus_ready =
      conformance_corpus_ready;
  surface.conformance_corpus_key =
      BuildObjc3FinalReadinessGateConformanceCorpusKey(
          surface,
          lane_a_surface.core_feature_expansion_ready,
          lane_b_surface.expansion_ready,
          lane_c_surface.edge_case_compatibility_ready,
          lane_d_surface.recovery_determinism_ready,
          !lane_d_surface.recovery_determinism_key.empty());
  surface.conformance_corpus_ready =
      surface.conformance_corpus_ready &&
      !surface.conformance_corpus_key.empty();
  const bool lane_performance_quality_guardrails_consistent =
      lane_a_surface.core_feature_expansion_ready &&
      lane_b_surface.edge_case_compatibility_ready &&
      lane_c_surface.edge_case_compatibility_ready &&
      lane_d_surface.conformance_matrix_ready &&
      !lane_d_surface.conformance_matrix_key.empty();
  const bool performance_quality_guardrails_consistent =
      surface.conformance_corpus_ready &&
      lane_performance_quality_guardrails_consistent;
  const bool performance_quality_guardrails_ready =
      performance_quality_guardrails_consistent &&
      !surface.governance_key.empty() &&
      !surface.modular_split_key.empty() &&
      !surface.conformance_corpus_key.empty() &&
      !lane_d_surface.conformance_matrix_key.empty();
  surface.performance_quality_guardrails_consistent =
      performance_quality_guardrails_consistent;
  surface.performance_quality_guardrails_ready =
      performance_quality_guardrails_ready;
  surface.performance_quality_guardrails_key =
      BuildObjc3FinalReadinessGatePerformanceQualityGuardrailsKey(
          surface,
          lane_a_surface.core_feature_expansion_ready,
          lane_b_surface.edge_case_compatibility_ready,
          lane_c_surface.edge_case_compatibility_ready,
          lane_d_surface.conformance_matrix_ready,
          !lane_d_surface.conformance_matrix_key.empty());
  surface.performance_quality_guardrails_ready =
      surface.performance_quality_guardrails_ready &&
      !surface.performance_quality_guardrails_key.empty();
  const bool lane_cross_lane_integration_consistent =
      lane_a_surface.core_feature_expansion_ready &&
      lane_b_surface.edge_case_compatibility_ready &&
      lane_c_surface.edge_case_robustness_ready &&
      lane_d_surface.conformance_matrix_ready &&
      !lane_d_surface.conformance_matrix_key.empty();
  const bool cross_lane_integration_consistent =
      surface.performance_quality_guardrails_ready &&
      lane_cross_lane_integration_consistent;
  const bool cross_lane_integration_ready =
      cross_lane_integration_consistent &&
      !surface.governance_key.empty() &&
      !surface.modular_split_key.empty() &&
      !surface.performance_quality_guardrails_key.empty() &&
      !lane_d_surface.conformance_matrix_key.empty();
  surface.cross_lane_integration_consistent =
      cross_lane_integration_consistent;
  surface.cross_lane_integration_ready =
      cross_lane_integration_ready;
  surface.cross_lane_integration_key =
      BuildObjc3FinalReadinessGateCrossLaneIntegrationKey(
          surface,
          lane_a_surface.core_feature_expansion_ready,
          lane_b_surface.edge_case_compatibility_ready,
          lane_c_surface.edge_case_robustness_ready,
          lane_d_surface.conformance_matrix_ready,
          !lane_d_surface.conformance_matrix_key.empty());
  surface.cross_lane_integration_ready =
      surface.cross_lane_integration_ready &&
      !surface.cross_lane_integration_key.empty();
  const bool lane_docs_runbook_sync_consistent =
      lane_a_surface.edge_case_compatibility_ready &&
      lane_b_surface.edge_case_robustness_ready &&
      lane_c_surface.edge_case_robustness_ready &&
      lane_d_surface.conformance_matrix_ready &&
      !lane_d_surface.conformance_matrix_key.empty();
  const bool docs_runbook_sync_consistent =
      surface.cross_lane_integration_ready &&
      lane_docs_runbook_sync_consistent;
  const bool docs_runbook_sync_ready =
      docs_runbook_sync_consistent &&
      !surface.governance_key.empty() &&
      !surface.modular_split_key.empty() &&
      !surface.cross_lane_integration_key.empty() &&
      !lane_d_surface.conformance_matrix_key.empty();
  surface.docs_runbook_sync_consistent =
      docs_runbook_sync_consistent;
  surface.docs_runbook_sync_ready =
      docs_runbook_sync_ready;
  surface.docs_runbook_sync_key =
      BuildObjc3FinalReadinessGateDocsRunbookSyncKey(
          surface,
          lane_a_surface.edge_case_compatibility_ready,
          lane_b_surface.edge_case_robustness_ready,
          lane_c_surface.edge_case_robustness_ready,
          lane_d_surface.conformance_matrix_ready,
          !lane_d_surface.conformance_matrix_key.empty());
  surface.docs_runbook_sync_ready =
      surface.docs_runbook_sync_ready &&
      !surface.docs_runbook_sync_key.empty();
  const bool lane_release_candidate_replay_dry_run_consistent =
      lane_a_surface.edge_case_compatibility_ready &&
      lane_b_surface.edge_case_robustness_ready &&
      lane_c_surface.diagnostics_hardening_ready &&
      lane_d_surface.core_feature_impl_ready &&
      !lane_d_surface.core_feature_key.empty();
  const bool release_candidate_replay_dry_run_consistent =
      surface.docs_runbook_sync_ready &&
      lane_release_candidate_replay_dry_run_consistent;
  const bool release_candidate_replay_dry_run_ready =
      release_candidate_replay_dry_run_consistent &&
      !surface.governance_key.empty() &&
      !surface.modular_split_key.empty() &&
      !surface.docs_runbook_sync_key.empty() &&
      !lane_d_surface.core_feature_key.empty();
  surface.release_candidate_replay_dry_run_consistent =
      release_candidate_replay_dry_run_consistent;
  surface.release_candidate_replay_dry_run_ready =
      release_candidate_replay_dry_run_ready;
  surface.release_candidate_replay_dry_run_key =
      BuildObjc3FinalReadinessGateReleaseCandidateReplayDryRunKey(
          surface,
          lane_a_surface.edge_case_compatibility_ready,
          lane_b_surface.edge_case_robustness_ready,
          lane_c_surface.diagnostics_hardening_ready,
          lane_d_surface.core_feature_impl_ready,
          !lane_d_surface.core_feature_key.empty());
  surface.release_candidate_replay_dry_run_ready =
      surface.release_candidate_replay_dry_run_ready &&
      !surface.release_candidate_replay_dry_run_key.empty();
  const bool lane_advanced_core_shard1_consistent =
      lane_a_surface.edge_case_robustness_ready &&
      lane_b_surface.diagnostics_hardening_ready &&
      lane_c_surface.diagnostics_hardening_ready &&
      lane_d_surface.core_feature_impl_ready &&
      !lane_d_surface.core_feature_key.empty();
  const bool advanced_core_shard1_consistent =
      surface.release_candidate_replay_dry_run_ready &&
      lane_advanced_core_shard1_consistent;
  const bool advanced_core_shard1_ready =
      advanced_core_shard1_consistent &&
      !surface.governance_key.empty() &&
      !surface.modular_split_key.empty() &&
      !surface.release_candidate_replay_dry_run_key.empty() &&
      !lane_d_surface.core_feature_key.empty();
  surface.advanced_core_shard1_consistent =
      advanced_core_shard1_consistent;
  surface.advanced_core_shard1_ready =
      advanced_core_shard1_ready;
  surface.advanced_core_shard1_key =
      BuildObjc3FinalReadinessGateAdvancedCoreShard1Key(
          surface,
          lane_a_surface.edge_case_robustness_ready,
          lane_b_surface.diagnostics_hardening_ready,
          lane_c_surface.diagnostics_hardening_ready,
          lane_d_surface.core_feature_impl_ready,
          !lane_d_surface.core_feature_key.empty());
  surface.advanced_core_shard1_ready =
      surface.advanced_core_shard1_ready &&
      !surface.advanced_core_shard1_key.empty();
  const bool lane_advanced_edge_compatibility_shard1_consistent =
      lane_a_surface.edge_case_robustness_ready &&
      lane_b_surface.diagnostics_hardening_ready &&
      lane_c_surface.recovery_determinism_ready &&
      lane_d_surface.core_feature_impl_ready &&
      !lane_d_surface.core_feature_key.empty();
  const bool advanced_edge_compatibility_shard1_consistent =
      surface.advanced_core_shard1_ready &&
      lane_advanced_edge_compatibility_shard1_consistent;
  const bool advanced_edge_compatibility_shard1_ready =
      advanced_edge_compatibility_shard1_consistent &&
      !surface.governance_key.empty() &&
      !surface.modular_split_key.empty() &&
      !surface.advanced_core_shard1_key.empty() &&
      !lane_d_surface.core_feature_key.empty();
  surface.advanced_edge_compatibility_shard1_consistent =
      advanced_edge_compatibility_shard1_consistent;
  surface.advanced_edge_compatibility_shard1_ready =
      advanced_edge_compatibility_shard1_ready;
  surface.advanced_edge_compatibility_shard1_key =
      BuildObjc3FinalReadinessGateAdvancedEdgeCompatibilityShard1Key(
          surface,
          lane_a_surface.edge_case_robustness_ready,
          lane_b_surface.diagnostics_hardening_ready,
          lane_c_surface.recovery_determinism_ready,
          lane_d_surface.core_feature_impl_ready,
          !lane_d_surface.core_feature_key.empty());
  surface.advanced_edge_compatibility_shard1_ready =
      surface.advanced_edge_compatibility_shard1_ready &&
      !surface.advanced_edge_compatibility_shard1_key.empty();
  const bool lane_advanced_diagnostics_shard1_consistent =
      lane_a_surface.edge_case_robustness_ready &&
      lane_b_surface.recovery_determinism_ready &&
      lane_c_surface.recovery_determinism_ready &&
      lane_d_surface.core_feature_impl_ready &&
      !lane_d_surface.core_feature_key.empty();
  const bool advanced_diagnostics_shard1_consistent =
      surface.advanced_edge_compatibility_shard1_ready &&
      lane_advanced_diagnostics_shard1_consistent;
  const bool advanced_diagnostics_shard1_ready =
      advanced_diagnostics_shard1_consistent &&
      !surface.governance_key.empty() &&
      !surface.modular_split_key.empty() &&
      !surface.advanced_edge_compatibility_shard1_key.empty() &&
      !lane_d_surface.core_feature_key.empty();
  surface.advanced_diagnostics_shard1_consistent =
      advanced_diagnostics_shard1_consistent;
  surface.advanced_diagnostics_shard1_ready =
      advanced_diagnostics_shard1_ready;
  surface.advanced_diagnostics_shard1_key =
      BuildObjc3FinalReadinessGateAdvancedDiagnosticsShard1Key(
          surface,
          lane_a_surface.edge_case_robustness_ready,
          lane_b_surface.recovery_determinism_ready,
          lane_c_surface.recovery_determinism_ready,
          lane_d_surface.core_feature_impl_ready,
          !lane_d_surface.core_feature_key.empty());
  surface.advanced_diagnostics_shard1_ready =
      surface.advanced_diagnostics_shard1_ready &&
      !surface.advanced_diagnostics_shard1_key.empty();
  const bool lane_advanced_conformance_shard1_consistent =
      lane_a_surface.diagnostics_hardening_ready &&
      lane_b_surface.recovery_determinism_ready &&
      lane_c_surface.conformance_matrix_ready &&
      lane_d_surface.core_feature_impl_ready &&
      !lane_d_surface.core_feature_key.empty();
  const bool advanced_conformance_shard1_consistent =
      surface.advanced_diagnostics_shard1_ready &&
      lane_advanced_conformance_shard1_consistent;
  const bool advanced_conformance_shard1_ready =
      advanced_conformance_shard1_consistent &&
      !surface.governance_key.empty() &&
      !surface.modular_split_key.empty() &&
      !surface.advanced_diagnostics_shard1_key.empty() &&
      !lane_d_surface.core_feature_key.empty();
  surface.advanced_conformance_shard1_consistent =
      advanced_conformance_shard1_consistent;
  surface.advanced_conformance_shard1_ready =
      advanced_conformance_shard1_ready;
  surface.advanced_conformance_shard1_key =
      BuildObjc3FinalReadinessGateAdvancedConformanceShard1Key(
          surface,
          lane_a_surface.diagnostics_hardening_ready,
          lane_b_surface.recovery_determinism_ready,
          lane_c_surface.conformance_matrix_ready,
          lane_d_surface.core_feature_impl_ready,
          !lane_d_surface.core_feature_key.empty());
  surface.advanced_conformance_shard1_ready =
      surface.advanced_conformance_shard1_ready &&
      !surface.advanced_conformance_shard1_key.empty();
  const bool lane_advanced_integration_shard1_consistent =
      lane_a_surface.diagnostics_hardening_ready &&
      lane_b_surface.recovery_determinism_ready &&
      lane_c_surface.conformance_matrix_ready &&
      lane_d_surface.core_feature_impl_ready &&
      !lane_d_surface.core_feature_key.empty();
  const bool advanced_integration_shard1_consistent =
      surface.advanced_conformance_shard1_ready &&
      lane_advanced_integration_shard1_consistent;
  const bool advanced_integration_shard1_ready =
      advanced_integration_shard1_consistent &&
      !surface.governance_key.empty() &&
      !surface.modular_split_key.empty() &&
      !surface.advanced_conformance_shard1_key.empty() &&
      !lane_d_surface.core_feature_key.empty();
  surface.advanced_integration_shard1_consistent =
      advanced_integration_shard1_consistent;
  surface.advanced_integration_shard1_ready =
      advanced_integration_shard1_ready;
  surface.advanced_integration_shard1_key =
      BuildObjc3FinalReadinessGateAdvancedIntegrationShard1Key(
          surface,
          lane_a_surface.diagnostics_hardening_ready,
          lane_b_surface.recovery_determinism_ready,
          lane_c_surface.conformance_matrix_ready,
          lane_d_surface.core_feature_impl_ready,
          !lane_d_surface.core_feature_key.empty());
  surface.advanced_integration_shard1_ready =
      surface.advanced_integration_shard1_ready &&
      !surface.advanced_integration_shard1_key.empty();
  const bool lane_advanced_performance_shard1_consistent =
      lane_a_surface.diagnostics_hardening_ready &&
      lane_b_surface.conformance_matrix_ready &&
      lane_c_surface.conformance_corpus_ready &&
      lane_d_surface.edge_case_robustness_ready &&
      !lane_d_surface.edge_case_robustness_key.empty();
  const bool advanced_performance_shard1_consistent =
      surface.advanced_integration_shard1_ready &&
      lane_advanced_performance_shard1_consistent;
  const bool advanced_performance_shard1_ready =
      advanced_performance_shard1_consistent &&
      !surface.governance_key.empty() &&
      !surface.modular_split_key.empty() &&
      !surface.advanced_integration_shard1_key.empty() &&
      !lane_d_surface.edge_case_robustness_key.empty();
  surface.advanced_performance_shard1_consistent =
      advanced_performance_shard1_consistent;
  surface.advanced_performance_shard1_ready =
      advanced_performance_shard1_ready;
  surface.advanced_performance_shard1_key =
      BuildObjc3FinalReadinessGateAdvancedPerformanceShard1Key(
          surface,
          lane_a_surface.diagnostics_hardening_ready,
          lane_b_surface.conformance_matrix_ready,
          lane_c_surface.conformance_corpus_ready,
          lane_d_surface.edge_case_robustness_ready,
          !lane_d_surface.edge_case_robustness_key.empty());
  surface.advanced_performance_shard1_ready =
      surface.advanced_performance_shard1_ready &&
      !surface.advanced_performance_shard1_key.empty();
  const bool lane_advanced_core_shard2_consistent =
      lane_a_surface.recovery_determinism_ready &&
      lane_b_surface.conformance_matrix_ready &&
      lane_c_surface.conformance_corpus_ready &&
      lane_d_surface.diagnostics_hardening_ready &&
      !lane_d_surface.diagnostics_hardening_key.empty();
  const bool advanced_core_shard2_consistent =
      surface.advanced_performance_shard1_ready &&
      lane_advanced_core_shard2_consistent;
  const bool advanced_core_shard2_ready =
      advanced_core_shard2_consistent &&
      !surface.governance_key.empty() &&
      !surface.modular_split_key.empty() &&
      !surface.advanced_performance_shard1_key.empty() &&
      !lane_d_surface.diagnostics_hardening_key.empty();
  surface.advanced_core_shard2_consistent = advanced_core_shard2_consistent;
  surface.advanced_core_shard2_ready = advanced_core_shard2_ready;
  surface.advanced_core_shard2_key =
      BuildObjc3FinalReadinessGateAdvancedCoreShard2Key(
          surface,
          lane_a_surface.recovery_determinism_ready,
          lane_b_surface.conformance_matrix_ready,
          lane_c_surface.conformance_corpus_ready,
          lane_d_surface.diagnostics_hardening_ready,
          !lane_d_surface.diagnostics_hardening_key.empty());
  surface.advanced_core_shard2_ready =
      surface.advanced_core_shard2_ready &&
      !surface.advanced_core_shard2_key.empty();
  const bool lane_advanced_core_shard3_consistent =
      lane_a_surface.conformance_corpus_ready &&
      lane_b_surface.cross_lane_integration_ready &&
      lane_c_surface.advanced_core_shard1_ready &&
      lane_d_surface.advanced_integration_shard1_ready &&
      !lane_d_surface.advanced_integration_shard1_key.empty();
  const bool advanced_core_shard3_consistent =
      surface.advanced_performance_shard2_ready &&
      lane_advanced_core_shard3_consistent;
  const bool advanced_core_shard3_ready =
      advanced_core_shard3_consistent &&
      !surface.governance_key.empty() &&
      !surface.modular_split_key.empty() &&
      !surface.advanced_performance_shard2_key.empty() &&
      !lane_d_surface.advanced_integration_shard1_key.empty();
  surface.advanced_core_shard3_consistent = advanced_core_shard3_consistent;
  surface.advanced_core_shard3_ready = advanced_core_shard3_ready;
  surface.advanced_core_shard3_key =
      BuildObjc3FinalReadinessGateAdvancedCoreShard3Key(
          surface,
          lane_a_surface.conformance_corpus_ready,
          lane_b_surface.cross_lane_integration_ready,
          lane_c_surface.advanced_core_shard1_ready,
          lane_d_surface.advanced_integration_shard1_ready,
          !lane_d_surface.advanced_integration_shard1_key.empty());
  surface.advanced_core_shard3_ready =
      surface.advanced_core_shard3_ready &&
      !surface.advanced_core_shard3_key.empty();
  const bool lane_advanced_edge_compatibility_shard3_consistent =
      lane_a_surface.conformance_corpus_ready &&
      lane_b_surface.docs_runbook_sync_ready &&
      lane_c_surface.advanced_core_shard1_ready &&
      lane_d_surface.advanced_performance_shard1_ready &&
      !lane_d_surface.advanced_performance_shard1_key.empty();
  const bool advanced_edge_compatibility_shard3_consistent =
      surface.advanced_core_shard3_ready &&
      lane_advanced_edge_compatibility_shard3_consistent;
  const bool advanced_edge_compatibility_shard3_ready =
      advanced_edge_compatibility_shard3_consistent &&
      !surface.governance_key.empty() &&
      !surface.modular_split_key.empty() &&
      !surface.advanced_core_shard3_key.empty() &&
      !lane_d_surface.advanced_performance_shard1_key.empty();
  surface.advanced_edge_compatibility_shard3_consistent =
      advanced_edge_compatibility_shard3_consistent;
  surface.advanced_edge_compatibility_shard3_ready =
      advanced_edge_compatibility_shard3_ready;
  surface.advanced_edge_compatibility_shard3_key =
      BuildObjc3FinalReadinessGateAdvancedEdgeCompatibilityShard3Key(
          surface,
          lane_a_surface.conformance_corpus_ready,
          lane_b_surface.docs_runbook_sync_ready,
          lane_c_surface.advanced_core_shard1_ready,
          lane_d_surface.advanced_performance_shard1_ready,
          !lane_d_surface.advanced_performance_shard1_key.empty());
  surface.advanced_edge_compatibility_shard3_ready =
      surface.advanced_edge_compatibility_shard3_ready &&
      !surface.advanced_edge_compatibility_shard3_key.empty();
  const bool lane_advanced_diagnostics_shard3_consistent =
      lane_a_surface.performance_quality_guardrails_ready &&
      lane_b_surface.docs_runbook_sync_ready &&
      lane_c_surface.advanced_edge_compatibility_shard1_ready &&
      lane_d_surface.advanced_core_shard2_ready &&
      !lane_d_surface.advanced_core_shard2_key.empty();
  const bool advanced_diagnostics_shard3_consistent =
      surface.advanced_edge_compatibility_shard3_ready &&
      lane_advanced_diagnostics_shard3_consistent;
  const bool advanced_diagnostics_shard3_ready =
      advanced_diagnostics_shard3_consistent &&
      !surface.governance_key.empty() &&
      !surface.modular_split_key.empty() &&
      !surface.advanced_edge_compatibility_shard3_key.empty() &&
      !lane_d_surface.advanced_core_shard2_key.empty();
  surface.advanced_diagnostics_shard3_consistent =
      advanced_diagnostics_shard3_consistent;
  surface.advanced_diagnostics_shard3_ready = advanced_diagnostics_shard3_ready;
  surface.advanced_diagnostics_shard3_key =
      BuildObjc3FinalReadinessGateAdvancedDiagnosticsShard3Key(
          surface,
          lane_a_surface.performance_quality_guardrails_ready,
          lane_b_surface.docs_runbook_sync_ready,
          lane_c_surface.advanced_edge_compatibility_shard1_ready,
          lane_d_surface.advanced_core_shard2_ready,
          !lane_d_surface.advanced_core_shard2_key.empty());
  surface.advanced_diagnostics_shard3_ready =
      surface.advanced_diagnostics_shard3_ready &&
      !surface.advanced_diagnostics_shard3_key.empty();
  const bool lane_advanced_conformance_shard3_consistent =
      lane_a_surface.performance_quality_guardrails_ready &&
      lane_b_surface.release_candidate_replay_dry_run_ready &&
      lane_c_surface.advanced_edge_compatibility_shard1_ready &&
      lane_d_surface.advanced_core_shard2_ready &&
      !lane_d_surface.advanced_core_shard2_key.empty();
  const bool advanced_conformance_shard3_consistent =
      surface.advanced_diagnostics_shard3_ready &&
      lane_advanced_conformance_shard3_consistent;
  const bool advanced_conformance_shard3_ready =
      advanced_conformance_shard3_consistent &&
      !surface.governance_key.empty() &&
      !surface.modular_split_key.empty() &&
      !surface.advanced_diagnostics_shard3_key.empty() &&
      !lane_d_surface.advanced_core_shard2_key.empty();
  surface.advanced_conformance_shard3_consistent =
      advanced_conformance_shard3_consistent;
  surface.advanced_conformance_shard3_ready = advanced_conformance_shard3_ready;
  surface.advanced_conformance_shard3_key =
      BuildObjc3FinalReadinessGateAdvancedConformanceShard3Key(
          surface,
          lane_a_surface.performance_quality_guardrails_ready,
          lane_b_surface.release_candidate_replay_dry_run_ready,
          lane_c_surface.advanced_edge_compatibility_shard1_ready,
          lane_d_surface.advanced_core_shard2_ready,
          !lane_d_surface.advanced_core_shard2_key.empty());
  surface.advanced_conformance_shard3_ready =
      surface.advanced_conformance_shard3_ready &&
      !surface.advanced_conformance_shard3_key.empty();
  const bool lane_advanced_integration_shard3_consistent =
      lane_a_surface.cross_lane_integration_ready &&
      lane_b_surface.release_candidate_replay_dry_run_ready &&
      lane_c_surface.advanced_diagnostics_shard1_ready &&
      lane_d_surface.advanced_edge_compatibility_shard2_ready &&
      !lane_d_surface.advanced_edge_compatibility_shard2_key.empty();
  const bool advanced_integration_shard3_consistent =
      surface.advanced_conformance_shard3_ready &&
      lane_advanced_integration_shard3_consistent;
  const bool advanced_integration_shard3_ready =
      advanced_integration_shard3_consistent &&
      !surface.governance_key.empty() &&
      !surface.modular_split_key.empty() &&
      !surface.advanced_conformance_shard3_key.empty() &&
      !lane_d_surface.advanced_edge_compatibility_shard2_key.empty();
  surface.advanced_integration_shard3_consistent =
      advanced_integration_shard3_consistent;
  surface.advanced_integration_shard3_ready = advanced_integration_shard3_ready;
  surface.advanced_integration_shard3_key =
      BuildObjc3FinalReadinessGateAdvancedIntegrationShard3Key(
          surface,
          lane_a_surface.cross_lane_integration_ready,
          lane_b_surface.release_candidate_replay_dry_run_ready,
          lane_c_surface.advanced_diagnostics_shard1_ready,
          lane_d_surface.advanced_edge_compatibility_shard2_ready,
          !lane_d_surface.advanced_edge_compatibility_shard2_key.empty());
  surface.advanced_integration_shard3_ready =
      surface.advanced_integration_shard3_ready &&
      !surface.advanced_integration_shard3_key.empty();
  const bool lane_advanced_performance_shard3_consistent =
      lane_a_surface.cross_lane_integration_ready &&
      lane_b_surface.advanced_core_shard1_ready &&
      lane_c_surface.advanced_diagnostics_shard1_ready &&
      lane_d_surface.advanced_diagnostics_shard2_ready &&
      !lane_d_surface.advanced_diagnostics_shard2_key.empty();
  const bool advanced_performance_shard3_consistent =
      surface.advanced_integration_shard3_ready &&
      lane_advanced_performance_shard3_consistent;
  const bool advanced_performance_shard3_ready =
      advanced_performance_shard3_consistent &&
      !surface.governance_key.empty() &&
      !surface.modular_split_key.empty() &&
      !surface.advanced_integration_shard3_key.empty() &&
      !lane_d_surface.advanced_diagnostics_shard2_key.empty();
  surface.advanced_performance_shard3_consistent =
      advanced_performance_shard3_consistent;
  surface.advanced_performance_shard3_ready = advanced_performance_shard3_ready;
  surface.advanced_performance_shard3_key =
      BuildObjc3FinalReadinessGateAdvancedPerformanceShard3Key(
          surface,
          lane_a_surface.cross_lane_integration_ready,
          lane_b_surface.advanced_core_shard1_ready,
          lane_c_surface.advanced_diagnostics_shard1_ready,
          lane_d_surface.advanced_diagnostics_shard2_ready,
          !lane_d_surface.advanced_diagnostics_shard2_key.empty());
  surface.advanced_performance_shard3_ready =
      surface.advanced_performance_shard3_ready &&
      !surface.advanced_performance_shard3_key.empty();
  const bool lane_advanced_core_shard4_consistent =
      lane_a_surface.cross_lane_integration_ready &&
      lane_b_surface.advanced_core_shard1_ready &&
      lane_c_surface.advanced_conformance_shard1_ready &&
      lane_d_surface.advanced_conformance_shard2_ready &&
      !lane_d_surface.advanced_conformance_shard2_key.empty();
  const bool advanced_core_shard4_consistent =
      surface.advanced_performance_shard3_ready &&
      lane_advanced_core_shard4_consistent;
  const bool advanced_core_shard4_ready =
      advanced_core_shard4_consistent &&
      !surface.governance_key.empty() &&
      !surface.modular_split_key.empty() &&
      !surface.advanced_performance_shard3_key.empty() &&
      !lane_d_surface.advanced_conformance_shard2_key.empty();
  surface.advanced_core_shard4_consistent = advanced_core_shard4_consistent;
  surface.advanced_core_shard4_ready = advanced_core_shard4_ready;
  surface.advanced_core_shard4_key =
      BuildObjc3FinalReadinessGateAdvancedCoreShard4Key(
          surface,
          lane_a_surface.cross_lane_integration_ready,
          lane_b_surface.advanced_core_shard1_ready,
          lane_c_surface.advanced_conformance_shard1_ready,
          lane_d_surface.advanced_conformance_shard2_ready,
          !lane_d_surface.advanced_conformance_shard2_key.empty());
  surface.advanced_core_shard4_ready =
      surface.advanced_core_shard4_ready &&
      !surface.advanced_core_shard4_key.empty();
  const bool lane_advanced_edge_compatibility_shard4_consistent =
      lane_a_surface.integration_closeout_signoff_ready &&
      lane_b_surface.integration_closeout_signoff_ready &&
      lane_c_surface.advanced_conformance_shard1_ready &&
      lane_d_surface.advanced_conformance_shard2_ready &&
      !lane_d_surface.advanced_conformance_shard2_key.empty();
  const bool advanced_edge_compatibility_shard4_consistent =
      surface.advanced_core_shard4_ready &&
      lane_advanced_edge_compatibility_shard4_consistent;
  const bool advanced_edge_compatibility_shard4_ready =
      advanced_edge_compatibility_shard4_consistent &&
      !surface.governance_key.empty() &&
      !surface.modular_split_key.empty() &&
      !surface.advanced_core_shard4_key.empty() &&
      !lane_d_surface.advanced_conformance_shard2_key.empty();
  surface.advanced_edge_compatibility_shard4_consistent =
      advanced_edge_compatibility_shard4_consistent;
  surface.advanced_edge_compatibility_shard4_ready =
      advanced_edge_compatibility_shard4_ready;
  surface.advanced_edge_compatibility_shard4_key =
      BuildObjc3FinalReadinessGateAdvancedEdgeCompatibilityShard4Key(
          surface,
          lane_a_surface.integration_closeout_signoff_ready,
          lane_b_surface.integration_closeout_signoff_ready,
          lane_c_surface.advanced_conformance_shard1_ready,
          lane_d_surface.advanced_conformance_shard2_ready,
          !lane_d_surface.advanced_conformance_shard2_key.empty());
  surface.advanced_edge_compatibility_shard4_ready =
      surface.advanced_edge_compatibility_shard4_ready &&
      !surface.advanced_edge_compatibility_shard4_key.empty();
  const bool lane_advanced_integration_closeout_signoff_consistent =
      lane_a_surface.integration_closeout_signoff_ready &&
      lane_b_surface.integration_closeout_signoff_ready &&
      lane_c_surface.integration_closeout_signoff_ready &&
      lane_d_surface.integration_closeout_signoff_ready &&
      !lane_d_surface.integration_closeout_signoff_key.empty();
  const bool advanced_integration_closeout_signoff_consistent =
      surface.advanced_edge_compatibility_shard4_ready &&
      lane_advanced_integration_closeout_signoff_consistent;
  const bool advanced_integration_closeout_signoff_ready =
      advanced_integration_closeout_signoff_consistent &&
      !surface.governance_key.empty() &&
      !surface.modular_split_key.empty() &&
      !surface.advanced_edge_compatibility_shard4_key.empty() &&
      !lane_d_surface.integration_closeout_signoff_key.empty();
  surface.advanced_integration_closeout_signoff_consistent =
      advanced_integration_closeout_signoff_consistent;
  surface.advanced_integration_closeout_signoff_ready =
      advanced_integration_closeout_signoff_ready;
  surface.advanced_integration_closeout_signoff_key =
      BuildObjc3FinalReadinessGateAdvancedIntegrationCloseoutSignoffKey(
          surface,
          lane_a_surface.integration_closeout_signoff_ready,
          lane_b_surface.integration_closeout_signoff_ready,
          lane_c_surface.integration_closeout_signoff_ready,
          lane_d_surface.integration_closeout_signoff_ready,
          !lane_d_surface.integration_closeout_signoff_key.empty());
  surface.advanced_integration_closeout_signoff_ready =
      surface.advanced_integration_closeout_signoff_ready &&
      !surface.advanced_integration_closeout_signoff_key.empty();
  const bool lane_advanced_edge_compatibility_shard2_consistent =
      lane_a_surface.recovery_determinism_ready &&
      lane_b_surface.conformance_corpus_ready &&
      lane_c_surface.performance_quality_guardrails_ready &&
      lane_d_surface.conformance_matrix_ready &&
      !lane_d_surface.conformance_matrix_key.empty();
  const bool advanced_edge_compatibility_shard2_consistent =
      surface.advanced_core_shard2_ready &&
      lane_advanced_edge_compatibility_shard2_consistent;
  const bool advanced_edge_compatibility_shard2_ready =
      advanced_edge_compatibility_shard2_consistent &&
      !surface.governance_key.empty() &&
      !surface.modular_split_key.empty() &&
      !surface.advanced_core_shard2_key.empty() &&
      !lane_d_surface.conformance_matrix_key.empty();
  surface.advanced_edge_compatibility_shard2_consistent =
      advanced_edge_compatibility_shard2_consistent;
  surface.advanced_edge_compatibility_shard2_ready =
      advanced_edge_compatibility_shard2_ready;
  surface.advanced_edge_compatibility_shard2_key =
      BuildObjc3FinalReadinessGateAdvancedEdgeCompatibilityShard2Key(
          surface,
          lane_a_surface.recovery_determinism_ready,
          lane_b_surface.conformance_corpus_ready,
          lane_c_surface.performance_quality_guardrails_ready,
          lane_d_surface.conformance_matrix_ready,
          !lane_d_surface.conformance_matrix_key.empty());
  surface.advanced_edge_compatibility_shard2_ready =
      surface.advanced_edge_compatibility_shard2_ready &&
      !surface.advanced_edge_compatibility_shard2_key.empty();
  const bool lane_advanced_diagnostics_shard2_consistent =
      lane_a_surface.conformance_matrix_ready &&
      lane_b_surface.conformance_corpus_ready &&
      lane_c_surface.performance_quality_guardrails_ready &&
      lane_d_surface.core_feature_impl_ready &&
      !lane_d_surface.core_feature_key.empty();
  const bool advanced_diagnostics_shard2_consistent =
      surface.advanced_edge_compatibility_shard2_ready &&
      lane_advanced_diagnostics_shard2_consistent;
  const bool advanced_diagnostics_shard2_ready =
      advanced_diagnostics_shard2_consistent &&
      !surface.governance_key.empty() &&
      !surface.modular_split_key.empty() &&
      !surface.advanced_edge_compatibility_shard2_key.empty() &&
      !lane_d_surface.core_feature_key.empty();
  surface.advanced_diagnostics_shard2_consistent =
      advanced_diagnostics_shard2_consistent;
  surface.advanced_diagnostics_shard2_ready = advanced_diagnostics_shard2_ready;
  surface.advanced_diagnostics_shard2_key =
      BuildObjc3FinalReadinessGateAdvancedDiagnosticsShard2Key(
          surface,
          lane_a_surface.conformance_matrix_ready,
          lane_b_surface.conformance_corpus_ready,
          lane_c_surface.performance_quality_guardrails_ready,
          lane_d_surface.core_feature_impl_ready,
          !lane_d_surface.core_feature_key.empty());
  surface.advanced_diagnostics_shard2_ready =
      surface.advanced_diagnostics_shard2_ready &&
      !surface.advanced_diagnostics_shard2_key.empty();
  const bool lane_advanced_conformance_shard2_consistent =
      lane_a_surface.conformance_matrix_ready &&
      lane_b_surface.performance_quality_guardrails_ready &&
      lane_c_surface.cross_lane_integration_ready &&
      lane_d_surface.core_feature_impl_ready &&
      !lane_d_surface.core_feature_key.empty();
  const bool advanced_conformance_shard2_consistent =
      surface.advanced_diagnostics_shard2_ready &&
      lane_advanced_conformance_shard2_consistent;
  const bool advanced_conformance_shard2_ready =
      advanced_conformance_shard2_consistent &&
      !surface.governance_key.empty() &&
      !surface.modular_split_key.empty() &&
      !surface.advanced_diagnostics_shard2_key.empty() &&
      !lane_d_surface.core_feature_key.empty();
  surface.advanced_conformance_shard2_consistent =
      advanced_conformance_shard2_consistent;
  surface.advanced_conformance_shard2_ready = advanced_conformance_shard2_ready;
  surface.advanced_conformance_shard2_key =
      BuildObjc3FinalReadinessGateAdvancedConformanceShard2Key(
          surface,
          lane_a_surface.conformance_matrix_ready,
          lane_b_surface.performance_quality_guardrails_ready,
          lane_c_surface.cross_lane_integration_ready,
          lane_d_surface.core_feature_impl_ready,
          !lane_d_surface.core_feature_key.empty());
  surface.advanced_conformance_shard2_ready =
      surface.advanced_conformance_shard2_ready &&
      !surface.advanced_conformance_shard2_key.empty();
  const bool lane_advanced_integration_shard2_consistent =
      lane_a_surface.conformance_matrix_ready &&
      lane_b_surface.performance_quality_guardrails_ready &&
      lane_c_surface.cross_lane_integration_ready &&
      lane_d_surface.edge_case_robustness_ready &&
      !lane_d_surface.edge_case_robustness_key.empty();
  const bool advanced_integration_shard2_consistent =
      surface.advanced_conformance_shard2_ready &&
      lane_advanced_integration_shard2_consistent;
  const bool advanced_integration_shard2_ready =
      advanced_integration_shard2_consistent &&
      !surface.governance_key.empty() &&
      !surface.modular_split_key.empty() &&
      !surface.advanced_conformance_shard2_key.empty() &&
      !lane_d_surface.edge_case_robustness_key.empty();
  surface.advanced_integration_shard2_consistent =
      advanced_integration_shard2_consistent;
  surface.advanced_integration_shard2_ready = advanced_integration_shard2_ready;
  surface.advanced_integration_shard2_key =
      BuildObjc3FinalReadinessGateAdvancedIntegrationShard2Key(
          surface,
          lane_a_surface.conformance_matrix_ready,
          lane_b_surface.performance_quality_guardrails_ready,
          lane_c_surface.cross_lane_integration_ready,
          lane_d_surface.edge_case_robustness_ready,
          !lane_d_surface.edge_case_robustness_key.empty());
  surface.advanced_integration_shard2_ready =
      surface.advanced_integration_shard2_ready &&
      !surface.advanced_integration_shard2_key.empty();
  const bool lane_advanced_performance_shard2_consistent =
      lane_a_surface.conformance_matrix_ready &&
      lane_b_surface.performance_quality_guardrails_ready &&
      lane_c_surface.cross_lane_integration_ready &&
      lane_d_surface.diagnostics_hardening_ready &&
      !lane_d_surface.diagnostics_hardening_key.empty();
  const bool advanced_performance_shard2_consistent =
      surface.advanced_integration_shard2_ready &&
      lane_advanced_performance_shard2_consistent;
  const bool advanced_performance_shard2_ready =
      advanced_performance_shard2_consistent &&
      !surface.governance_key.empty() &&
      !surface.modular_split_key.empty() &&
      !surface.advanced_integration_shard2_key.empty() &&
      !lane_d_surface.diagnostics_hardening_key.empty();
  surface.advanced_performance_shard2_consistent =
      advanced_performance_shard2_consistent;
  surface.advanced_performance_shard2_ready = advanced_performance_shard2_ready;
  surface.advanced_performance_shard2_key =
      BuildObjc3FinalReadinessGateAdvancedPerformanceShard2Key(
          surface,
          lane_a_surface.conformance_matrix_ready,
          lane_b_surface.performance_quality_guardrails_ready,
          lane_c_surface.cross_lane_integration_ready,
          lane_d_surface.diagnostics_hardening_ready,
          !lane_d_surface.diagnostics_hardening_key.empty());
  surface.advanced_performance_shard2_ready =
      surface.advanced_performance_shard2_ready &&
      !surface.advanced_performance_shard2_key.empty();
  const bool lane_integration_closeout_signoff_consistent =
      lane_a_surface.conformance_matrix_ready &&
      lane_b_surface.performance_quality_guardrails_ready &&
      lane_c_surface.cross_lane_integration_ready &&
      lane_d_surface.conformance_matrix_ready &&
      !lane_d_surface.conformance_matrix_key.empty();
  const bool integration_closeout_signoff_consistent =
      surface.advanced_performance_shard2_ready &&
      lane_integration_closeout_signoff_consistent;
  const bool integration_closeout_signoff_ready =
      integration_closeout_signoff_consistent &&
      !surface.governance_key.empty() &&
      !surface.modular_split_key.empty() &&
      !surface.advanced_performance_shard2_key.empty() &&
      !lane_d_surface.conformance_matrix_key.empty();
  surface.integration_closeout_signoff_consistent =
      integration_closeout_signoff_consistent;
  surface.integration_closeout_signoff_ready =
      integration_closeout_signoff_ready;
  surface.integration_closeout_signoff_key =
      BuildObjc3FinalReadinessGateIntegrationCloseoutSignoffKey(
          surface,
          lane_a_surface.conformance_matrix_ready,
          lane_b_surface.performance_quality_guardrails_ready,
          lane_c_surface.cross_lane_integration_ready,
          lane_d_surface.conformance_matrix_ready,
          !lane_d_surface.conformance_matrix_key.empty());
  surface.integration_closeout_signoff_ready =
      surface.integration_closeout_signoff_ready &&
      !surface.integration_closeout_signoff_key.empty();
  surface.core_feature_key =
      BuildObjc3FinalReadinessGateCoreFeatureImplementationKey(surface);
  surface.core_feature_impl_ready =
      surface.core_feature_impl_ready &&
      surface.core_feature_expansion_ready &&
      surface.edge_case_compatibility_ready &&
      surface.edge_case_robustness_ready &&
      surface.diagnostics_hardening_ready &&
      surface.recovery_determinism_ready &&
      surface.conformance_matrix_ready &&
      surface.conformance_corpus_ready &&
      surface.performance_quality_guardrails_ready &&
      surface.cross_lane_integration_ready &&
      surface.docs_runbook_sync_ready &&
      surface.release_candidate_replay_dry_run_ready &&
      surface.advanced_core_shard1_ready &&
      surface.advanced_edge_compatibility_shard1_ready &&
      surface.advanced_diagnostics_shard1_ready &&
      surface.advanced_conformance_shard1_ready &&
      surface.advanced_integration_shard1_ready &&
      surface.advanced_performance_shard1_ready &&
      surface.advanced_core_shard2_ready &&
      surface.advanced_core_shard3_ready &&
      surface.advanced_edge_compatibility_shard3_ready &&
      surface.advanced_diagnostics_shard3_ready &&
      surface.advanced_conformance_shard3_ready &&
      surface.advanced_integration_shard3_ready &&
      surface.advanced_performance_shard3_ready &&
      surface.advanced_core_shard4_ready &&
      surface.advanced_edge_compatibility_shard4_ready &&
      surface.advanced_integration_closeout_signoff_ready &&
      surface.advanced_edge_compatibility_shard2_ready &&
      surface.advanced_diagnostics_shard2_ready &&
      surface.advanced_conformance_shard2_ready &&
      surface.advanced_integration_shard2_ready &&
      surface.advanced_performance_shard2_ready &&
      surface.integration_closeout_signoff_ready &&
      !surface.core_feature_key.empty();

  if (surface.core_feature_impl_ready) {
    return surface;
  }

  if (!surface.governance_contract_ready) {
    surface.failure_reason =
        "final readiness gate governance contract is not ready";
  } else if (!surface.modular_split_ready) {
    surface.failure_reason =
        "final readiness gate modular split scaffold is not ready";
  } else if (!surface.lane_a_core_feature_ready) {
    surface.failure_reason =
        "final readiness gate lane-A core feature readiness is not satisfied";
  } else if (!surface.lane_b_core_feature_ready) {
    surface.failure_reason =
        "final readiness gate lane-B core feature readiness is not satisfied";
  } else if (!surface.lane_c_core_feature_ready) {
    surface.failure_reason =
        "final readiness gate lane-C core feature readiness is not satisfied";
  } else if (!surface.lane_d_core_feature_ready) {
    surface.failure_reason =
        "final readiness gate lane-D core feature readiness is not satisfied";
  } else if (!replay_keys_ready) {
    surface.failure_reason =
        "final readiness gate dependency replay keys are not ready";
  } else if (!lane_expansion_consistent) {
    surface.failure_reason =
        "final readiness gate core feature expansion is inconsistent";
  } else if (!surface.core_feature_expansion_consistent) {
    surface.failure_reason =
        "final readiness gate core feature expansion consistency is not satisfied";
  } else if (!surface.core_feature_expansion_ready) {
    surface.failure_reason =
        "final readiness gate core feature expansion is not ready";
  } else if (surface.core_feature_expansion_key.empty()) {
    surface.failure_reason =
        "final readiness gate core feature expansion key is not ready";
  } else if (!lane_edge_case_compatibility_consistent) {
    surface.failure_reason =
        "final readiness gate edge-case compatibility is inconsistent";
  } else if (!surface.edge_case_compatibility_consistent) {
    surface.failure_reason =
        "final readiness gate edge-case compatibility consistency is not satisfied";
  } else if (!surface.edge_case_compatibility_ready) {
    surface.failure_reason =
        "final readiness gate edge-case compatibility is not ready";
  } else if (surface.edge_case_compatibility_key.empty()) {
    surface.failure_reason =
        "final readiness gate edge-case compatibility key is not ready";
  } else if (!lane_edge_case_expansion_consistent) {
    surface.failure_reason =
        "final readiness gate edge-case expansion is inconsistent";
  } else if (!surface.edge_case_expansion_consistent) {
    surface.failure_reason =
        "final readiness gate edge-case expansion consistency is not satisfied";
  } else if (!surface.edge_case_robustness_ready) {
    surface.failure_reason =
        "final readiness gate edge-case robustness is not ready";
  } else if (surface.edge_case_robustness_key.empty()) {
    surface.failure_reason =
        "final readiness gate edge-case robustness key is not ready";
  } else if (!lane_diagnostics_hardening_consistent) {
    surface.failure_reason =
        "final readiness gate diagnostics hardening is inconsistent";
  } else if (!surface.diagnostics_hardening_consistent) {
    surface.failure_reason =
        "final readiness gate diagnostics hardening consistency is not satisfied";
  } else if (!surface.diagnostics_hardening_ready) {
    surface.failure_reason =
        "final readiness gate diagnostics hardening is not ready";
  } else if (surface.diagnostics_hardening_key.empty()) {
    surface.failure_reason =
        "final readiness gate diagnostics hardening key is not ready";
  } else if (!lane_recovery_determinism_consistent) {
    surface.failure_reason =
        "final readiness gate recovery and determinism hardening is inconsistent";
  } else if (!surface.recovery_determinism_consistent) {
    surface.failure_reason =
        "final readiness gate recovery and determinism consistency is not satisfied";
  } else if (!surface.recovery_determinism_ready) {
    surface.failure_reason =
        "final readiness gate recovery and determinism hardening is not ready";
  } else if (surface.recovery_determinism_key.empty()) {
    surface.failure_reason =
        "final readiness gate recovery and determinism key is not ready";
  } else if (!lane_conformance_matrix_consistent) {
    surface.failure_reason =
        "final readiness gate conformance matrix is inconsistent";
  } else if (!surface.conformance_matrix_consistent) {
    surface.failure_reason =
        "final readiness gate conformance matrix consistency is not satisfied";
  } else if (!surface.conformance_matrix_ready) {
    surface.failure_reason =
        "final readiness gate conformance matrix is not ready";
  } else if (surface.conformance_matrix_key.empty()) {
    surface.failure_reason =
        "final readiness gate conformance matrix key is not ready";
  } else if (!lane_conformance_corpus_consistent) {
    surface.failure_reason =
        "final readiness gate conformance corpus is inconsistent";
  } else if (!surface.conformance_corpus_consistent) {
    surface.failure_reason =
        "final readiness gate conformance corpus consistency is not satisfied";
  } else if (!surface.conformance_corpus_ready) {
    surface.failure_reason =
        "final readiness gate conformance corpus is not ready";
  } else if (surface.conformance_corpus_key.empty()) {
    surface.failure_reason =
        "final readiness gate conformance corpus key is not ready";
  } else if (!lane_performance_quality_guardrails_consistent) {
    surface.failure_reason =
        "final readiness gate performance and quality guardrails are inconsistent";
  } else if (!surface.performance_quality_guardrails_consistent) {
    surface.failure_reason =
        "final readiness gate performance and quality guardrails consistency is not satisfied";
  } else if (!surface.performance_quality_guardrails_ready) {
    surface.failure_reason =
        "final readiness gate performance and quality guardrails are not ready";
  } else if (surface.performance_quality_guardrails_key.empty()) {
    surface.failure_reason =
        "final readiness gate performance and quality guardrails key is not ready";
  } else if (!lane_cross_lane_integration_consistent) {
    surface.failure_reason =
        "final readiness gate cross-lane integration sync is inconsistent";
  } else if (!surface.cross_lane_integration_consistent) {
    surface.failure_reason =
        "final readiness gate cross-lane integration sync consistency is not satisfied";
  } else if (!surface.cross_lane_integration_ready) {
    surface.failure_reason =
        "final readiness gate cross-lane integration sync is not ready";
  } else if (surface.cross_lane_integration_key.empty()) {
    surface.failure_reason =
        "final readiness gate cross-lane integration sync key is not ready";
  } else if (!lane_docs_runbook_sync_consistent) {
    surface.failure_reason =
        "final readiness gate docs and operator runbook sync is inconsistent";
  } else if (!surface.docs_runbook_sync_consistent) {
    surface.failure_reason =
        "final readiness gate docs and operator runbook sync consistency is not satisfied";
  } else if (!surface.docs_runbook_sync_ready) {
    surface.failure_reason =
        "final readiness gate docs and operator runbook sync is not ready";
  } else if (surface.docs_runbook_sync_key.empty()) {
    surface.failure_reason =
        "final readiness gate docs and operator runbook sync key is not ready";
  } else if (!lane_release_candidate_replay_dry_run_consistent) {
    surface.failure_reason =
        "final readiness gate release candidate replay dry-run is inconsistent";
  } else if (!surface.release_candidate_replay_dry_run_consistent) {
    surface.failure_reason =
        "final readiness gate release candidate replay dry-run consistency is not satisfied";
  } else if (!surface.release_candidate_replay_dry_run_ready) {
    surface.failure_reason =
        "final readiness gate release candidate replay dry-run is not ready";
  } else if (surface.release_candidate_replay_dry_run_key.empty()) {
    surface.failure_reason =
        "final readiness gate release candidate replay dry-run key is not ready";
  } else if (!lane_advanced_core_shard1_consistent) {
    surface.failure_reason =
        "final readiness gate advanced core workpack shard1 is inconsistent";
  } else if (!surface.advanced_core_shard1_consistent) {
    surface.failure_reason =
        "final readiness gate advanced core workpack shard1 consistency is not satisfied";
  } else if (!surface.advanced_core_shard1_ready) {
    surface.failure_reason =
        "final readiness gate advanced core workpack shard1 is not ready";
  } else if (surface.advanced_core_shard1_key.empty()) {
    surface.failure_reason =
        "final readiness gate advanced core workpack shard1 key is not ready";
  } else if (!lane_advanced_edge_compatibility_shard1_consistent) {
    surface.failure_reason =
        "final readiness gate advanced edge compatibility workpack shard1 is inconsistent";
  } else if (!surface.advanced_edge_compatibility_shard1_consistent) {
    surface.failure_reason =
        "final readiness gate advanced edge compatibility workpack shard1 consistency is not satisfied";
  } else if (!surface.advanced_edge_compatibility_shard1_ready) {
    surface.failure_reason =
        "final readiness gate advanced edge compatibility workpack shard1 is not ready";
  } else if (surface.advanced_edge_compatibility_shard1_key.empty()) {
    surface.failure_reason =
        "final readiness gate advanced edge compatibility workpack shard1 key is not ready";
  } else if (!lane_advanced_diagnostics_shard1_consistent) {
    surface.failure_reason =
        "final readiness gate advanced diagnostics workpack shard1 is inconsistent";
  } else if (!surface.advanced_diagnostics_shard1_consistent) {
    surface.failure_reason =
        "final readiness gate advanced diagnostics workpack shard1 consistency is not satisfied";
  } else if (!surface.advanced_diagnostics_shard1_ready) {
    surface.failure_reason =
        "final readiness gate advanced diagnostics workpack shard1 is not ready";
  } else if (surface.advanced_diagnostics_shard1_key.empty()) {
    surface.failure_reason =
        "final readiness gate advanced diagnostics workpack shard1 key is not ready";
  } else if (!lane_advanced_conformance_shard1_consistent) {
    surface.failure_reason =
        "final readiness gate advanced conformance workpack shard1 is inconsistent";
  } else if (!surface.advanced_conformance_shard1_consistent) {
    surface.failure_reason =
        "final readiness gate advanced conformance workpack shard1 consistency is not satisfied";
  } else if (!surface.advanced_conformance_shard1_ready) {
    surface.failure_reason =
        "final readiness gate advanced conformance workpack shard1 is not ready";
  } else if (surface.advanced_conformance_shard1_key.empty()) {
    surface.failure_reason =
        "final readiness gate advanced conformance workpack shard1 key is not ready";
  } else if (!lane_advanced_integration_shard1_consistent) {
    surface.failure_reason =
        "final readiness gate advanced integration workpack shard1 is inconsistent";
  } else if (!surface.advanced_integration_shard1_consistent) {
    surface.failure_reason =
        "final readiness gate advanced integration workpack shard1 consistency is not satisfied";
  } else if (!surface.advanced_integration_shard1_ready) {
    surface.failure_reason =
        "final readiness gate advanced integration workpack shard1 is not ready";
  } else if (surface.advanced_integration_shard1_key.empty()) {
    surface.failure_reason =
        "final readiness gate advanced integration workpack shard1 key is not ready";
  } else if (!lane_advanced_performance_shard1_consistent) {
    surface.failure_reason =
        "final readiness gate advanced performance workpack shard1 is inconsistent";
  } else if (!surface.advanced_performance_shard1_consistent) {
    surface.failure_reason =
        "final readiness gate advanced performance workpack shard1 consistency is not satisfied";
  } else if (!surface.advanced_performance_shard1_ready) {
    surface.failure_reason =
        "final readiness gate advanced performance workpack shard1 is not ready";
  } else if (surface.advanced_performance_shard1_key.empty()) {
    surface.failure_reason =
        "final readiness gate advanced performance workpack shard1 key is not ready";
  } else if (!lane_advanced_core_shard2_consistent) {
    surface.failure_reason =
        "final readiness gate advanced core workpack shard2 is inconsistent";
  } else if (!surface.advanced_core_shard2_consistent) {
    surface.failure_reason =
        "final readiness gate advanced core workpack shard2 consistency is not satisfied";
  } else if (!surface.advanced_core_shard2_ready) {
    surface.failure_reason =
        "final readiness gate advanced core workpack shard2 is not ready";
  } else if (surface.advanced_core_shard2_key.empty()) {
    surface.failure_reason =
        "final readiness gate advanced core workpack shard2 key is not ready";
  } else if (!lane_advanced_core_shard3_consistent) {
    surface.failure_reason =
        "final readiness gate advanced core workpack shard3 is inconsistent";
  } else if (!surface.advanced_core_shard3_consistent) {
    surface.failure_reason =
        "final readiness gate advanced core workpack shard3 consistency is not satisfied";
  } else if (!surface.advanced_core_shard3_ready) {
    surface.failure_reason =
        "final readiness gate advanced core workpack shard3 is not ready";
  } else if (surface.advanced_core_shard3_key.empty()) {
    surface.failure_reason =
        "final readiness gate advanced core workpack shard3 key is not ready";
  } else if (!lane_advanced_edge_compatibility_shard3_consistent) {
    surface.failure_reason =
        "final readiness gate advanced edge compatibility workpack shard3 is inconsistent";
  } else if (!surface.advanced_edge_compatibility_shard3_consistent) {
    surface.failure_reason =
        "final readiness gate advanced edge compatibility workpack shard3 consistency is not satisfied";
  } else if (!surface.advanced_edge_compatibility_shard3_ready) {
    surface.failure_reason =
        "final readiness gate advanced edge compatibility workpack shard3 is not ready";
  } else if (surface.advanced_edge_compatibility_shard3_key.empty()) {
    surface.failure_reason =
        "final readiness gate advanced edge compatibility workpack shard3 key is not ready";
  } else if (!lane_advanced_diagnostics_shard3_consistent) {
    surface.failure_reason =
        "final readiness gate advanced diagnostics workpack shard3 is inconsistent";
  } else if (!surface.advanced_diagnostics_shard3_consistent) {
    surface.failure_reason =
        "final readiness gate advanced diagnostics workpack shard3 consistency is not satisfied";
  } else if (!surface.advanced_diagnostics_shard3_ready) {
    surface.failure_reason =
        "final readiness gate advanced diagnostics workpack shard3 is not ready";
  } else if (surface.advanced_diagnostics_shard3_key.empty()) {
    surface.failure_reason =
        "final readiness gate advanced diagnostics workpack shard3 key is not ready";
  } else if (!lane_advanced_conformance_shard3_consistent) {
    surface.failure_reason =
        "final readiness gate advanced conformance workpack shard3 is inconsistent";
  } else if (!surface.advanced_conformance_shard3_consistent) {
    surface.failure_reason =
        "final readiness gate advanced conformance workpack shard3 consistency is not satisfied";
  } else if (!surface.advanced_conformance_shard3_ready) {
    surface.failure_reason =
        "final readiness gate advanced conformance workpack shard3 is not ready";
  } else if (surface.advanced_conformance_shard3_key.empty()) {
    surface.failure_reason =
        "final readiness gate advanced conformance workpack shard3 key is not ready";
  } else if (!lane_advanced_integration_shard3_consistent) {
    surface.failure_reason =
        "final readiness gate advanced integration workpack shard3 is inconsistent";
  } else if (!surface.advanced_integration_shard3_consistent) {
    surface.failure_reason =
        "final readiness gate advanced integration workpack shard3 consistency is not satisfied";
  } else if (!surface.advanced_integration_shard3_ready) {
    surface.failure_reason =
        "final readiness gate advanced integration workpack shard3 is not ready";
  } else if (surface.advanced_integration_shard3_key.empty()) {
    surface.failure_reason =
        "final readiness gate advanced integration workpack shard3 key is not ready";
  } else if (!lane_advanced_performance_shard3_consistent) {
    surface.failure_reason =
        "final readiness gate advanced performance workpack shard3 is inconsistent";
  } else if (!surface.advanced_performance_shard3_consistent) {
    surface.failure_reason =
        "final readiness gate advanced performance workpack shard3 consistency is not satisfied";
  } else if (!surface.advanced_performance_shard3_ready) {
    surface.failure_reason =
        "final readiness gate advanced performance workpack shard3 is not ready";
  } else if (surface.advanced_performance_shard3_key.empty()) {
    surface.failure_reason =
        "final readiness gate advanced performance workpack shard3 key is not ready";
  } else if (!lane_advanced_core_shard4_consistent) {
    surface.failure_reason =
        "final readiness gate advanced core workpack shard4 is inconsistent";
  } else if (!surface.advanced_core_shard4_consistent) {
    surface.failure_reason =
        "final readiness gate advanced core workpack shard4 consistency is not satisfied";
  } else if (!surface.advanced_core_shard4_ready) {
    surface.failure_reason =
        "final readiness gate advanced core workpack shard4 is not ready";
  } else if (surface.advanced_core_shard4_key.empty()) {
    surface.failure_reason =
        "final readiness gate advanced core workpack shard4 key is not ready";
  } else if (!lane_advanced_edge_compatibility_shard4_consistent) {
    surface.failure_reason =
        "final readiness gate advanced edge compatibility workpack shard4 is inconsistent";
  } else if (!surface.advanced_edge_compatibility_shard4_consistent) {
    surface.failure_reason =
        "final readiness gate advanced edge compatibility workpack shard4 consistency is not satisfied";
  } else if (!surface.advanced_edge_compatibility_shard4_ready) {
    surface.failure_reason =
        "final readiness gate advanced edge compatibility workpack shard4 is not ready";
  } else if (surface.advanced_edge_compatibility_shard4_key.empty()) {
    surface.failure_reason =
        "final readiness gate advanced edge compatibility workpack shard4 key is not ready";
  } else if (!lane_advanced_integration_closeout_signoff_consistent) {
    surface.failure_reason =
        "final readiness gate integration closeout and gate sign-off is inconsistent";
  } else if (!surface.advanced_integration_closeout_signoff_consistent) {
    surface.failure_reason =
        "final readiness gate integration closeout and gate sign-off consistency is not satisfied";
  } else if (!surface.advanced_integration_closeout_signoff_ready) {
    surface.failure_reason =
        "final readiness gate integration closeout and gate sign-off is not ready";
  } else if (surface.advanced_integration_closeout_signoff_key.empty()) {
    surface.failure_reason =
        "final readiness gate integration closeout and gate sign-off key is not ready";
  } else if (!lane_advanced_edge_compatibility_shard2_consistent) {
    surface.failure_reason =
        "final readiness gate advanced edge compatibility workpack shard2 is inconsistent";
  } else if (!surface.advanced_edge_compatibility_shard2_consistent) {
    surface.failure_reason =
        "final readiness gate advanced edge compatibility workpack shard2 consistency is not satisfied";
  } else if (!surface.advanced_edge_compatibility_shard2_ready) {
    surface.failure_reason =
        "final readiness gate advanced edge compatibility workpack shard2 is not ready";
  } else if (surface.advanced_edge_compatibility_shard2_key.empty()) {
    surface.failure_reason =
        "final readiness gate advanced edge compatibility workpack shard2 key is not ready";
  } else if (!lane_advanced_diagnostics_shard2_consistent) {
    surface.failure_reason =
        "final readiness gate advanced diagnostics workpack shard2 is inconsistent";
  } else if (!surface.advanced_diagnostics_shard2_consistent) {
    surface.failure_reason =
        "final readiness gate advanced diagnostics workpack shard2 consistency is not satisfied";
  } else if (!surface.advanced_diagnostics_shard2_ready) {
    surface.failure_reason =
        "final readiness gate advanced diagnostics workpack shard2 is not ready";
  } else if (surface.advanced_diagnostics_shard2_key.empty()) {
    surface.failure_reason =
        "final readiness gate advanced diagnostics workpack shard2 key is not ready";
  } else if (!lane_advanced_conformance_shard2_consistent) {
    surface.failure_reason =
        "final readiness gate advanced conformance workpack shard2 is inconsistent";
  } else if (!surface.advanced_conformance_shard2_consistent) {
    surface.failure_reason =
        "final readiness gate advanced conformance workpack shard2 consistency is not satisfied";
  } else if (!surface.advanced_conformance_shard2_ready) {
    surface.failure_reason =
        "final readiness gate advanced conformance workpack shard2 is not ready";
  } else if (surface.advanced_conformance_shard2_key.empty()) {
    surface.failure_reason =
        "final readiness gate advanced conformance workpack shard2 key is not ready";
  } else if (!lane_advanced_integration_shard2_consistent) {
    surface.failure_reason =
        "final readiness gate advanced integration workpack shard2 is inconsistent";
  } else if (!surface.advanced_integration_shard2_consistent) {
    surface.failure_reason =
        "final readiness gate advanced integration workpack shard2 consistency is not satisfied";
  } else if (!surface.advanced_integration_shard2_ready) {
    surface.failure_reason =
        "final readiness gate advanced integration workpack shard2 is not ready";
  } else if (surface.advanced_integration_shard2_key.empty()) {
    surface.failure_reason =
        "final readiness gate advanced integration workpack shard2 key is not ready";
  } else if (!lane_advanced_performance_shard2_consistent) {
    surface.failure_reason =
        "final readiness gate advanced performance workpack shard2 is inconsistent";
  } else if (!surface.advanced_performance_shard2_consistent) {
    surface.failure_reason =
        "final readiness gate advanced performance workpack shard2 consistency is not satisfied";
  } else if (!surface.advanced_performance_shard2_ready) {
    surface.failure_reason =
        "final readiness gate advanced performance workpack shard2 is not ready";
  } else if (surface.advanced_performance_shard2_key.empty()) {
    surface.failure_reason =
        "final readiness gate advanced performance workpack shard2 key is not ready";
  } else if (!lane_integration_closeout_signoff_consistent) {
    surface.failure_reason =
        "final readiness gate integration closeout and gate sign-off is inconsistent";
  } else if (!surface.integration_closeout_signoff_consistent) {
    surface.failure_reason =
        "final readiness gate integration closeout and gate sign-off consistency is not satisfied";
  } else if (!surface.integration_closeout_signoff_ready) {
    surface.failure_reason =
        "final readiness gate integration closeout and gate sign-off is not ready";
  } else if (surface.integration_closeout_signoff_key.empty()) {
    surface.failure_reason =
        "final readiness gate integration closeout and gate sign-off key is not ready";
  } else {
    surface.failure_reason =
        "final readiness gate core feature implementation is not ready";
  }

  return surface;
}

inline bool IsObjc3FinalReadinessGateCoreFeatureImplementationSurfaceReady(
    const Objc3FinalReadinessGateCoreFeatureImplementationSurface &surface,
    std::string &reason) {
  if (surface.core_feature_impl_ready) {
    reason.clear();
    return true;
  }

  reason = surface.failure_reason.empty()
               ? "final readiness gate core feature implementation is not ready"
               : surface.failure_reason;
  return false;
}
