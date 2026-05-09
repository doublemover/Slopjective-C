#pragma once

#include "lower/model/lowered_owner_contracts.h"

#include <cstddef>
#include <string>

struct Objc3LoweringRuntimeStabilityInvariantScaffold {
  bool typed_surface_present = false;
  bool parse_readiness_surface_present = false;
  bool lowering_boundary_ready = false;
  bool runtime_dispatch_contract_consistent = false;
  bool typed_handoff_key_deterministic = false;
  bool typed_core_feature_consistent = false;
  bool parse_ready_for_lowering = false;
  bool invariant_proofs_ready = false;
  bool modular_split_ready = false;
  std::string lowering_boundary_replay_key;
  std::string typed_handoff_key;
  std::string parse_artifact_replay_key;
  std::string scaffold_key;
  std::string failure_reason;
};

struct Objc3LoweringPipelinePassGraphScaffold {
  bool lex_stage_ready = false;
  bool parse_stage_ready = false;
  bool sema_stage_ready = false;
  bool typed_surface_ready = false;
  bool parse_lowering_readiness_ready = false;
  bool semantic_stability_scaffold_ready = false;
  bool lowering_runtime_invariant_scaffold_ready = false;
  bool lowering_ir_boundary_ready = false;
  bool runtime_dispatch_declaration_ready = false;
  bool ir_emission_entrypoint_ready = false;
  bool pass_graph_ready = false;
  std::string lowering_boundary_replay_key;
  std::string runtime_dispatch_declaration_replay_key;
  std::string pass_graph_key;
  std::string failure_reason;
};

struct Objc3LoweringPipelinePassGraphCoreFeatureSurface {
  bool scaffold_ready = false;
  bool lowering_boundary_replay_key_consistent = false;
  bool runtime_dispatch_declaration_consistent = false;
  bool direct_ir_entrypoint_enabled = false;
  bool dispatch_shape_sharding_ready = false;
  bool llc_object_emission_route_deterministic = false;
  bool replay_proof_artifact_key_ready = false;
  bool edge_case_dispatch_shape_coverage_ready = false;
  bool replay_proof_expansion_ready = false;
  bool compatibility_handoff_consistent = false;
  bool language_version_pragma_coordinate_order_consistent = false;
  bool edge_case_compatibility_ready = false;
  bool edge_case_expansion_consistent = false;
  bool edge_case_robustness_ready = false;
  bool diagnostics_hardening_consistent = false;
  bool diagnostics_hardening_ready = false;
  bool recovery_determinism_consistent = false;
  bool recovery_determinism_ready = false;
  bool conformance_matrix_consistent = false;
  bool conformance_matrix_ready = false;
  bool conformance_corpus_consistent = false;
  bool conformance_corpus_ready = false;
  bool performance_quality_guardrails_consistent = false;
  bool performance_quality_guardrails_ready = false;
  bool expansion_ready = false;
  bool core_feature_ready = false;
  std::size_t parse_lowering_performance_quality_guardrails_case_count = 0;
  std::size_t parse_lowering_performance_quality_guardrails_passed_case_count = 0;
  std::size_t parse_lowering_performance_quality_guardrails_failed_case_count = 0;
  std::string lowering_boundary_replay_key;
  std::string runtime_dispatch_declaration_replay_key;
  std::string edge_case_compatibility_key;
  std::string edge_case_robustness_key;
  std::string diagnostics_hardening_key;
  std::string recovery_determinism_key;
  std::string conformance_matrix_key;
  std::string conformance_corpus_key;
  std::string performance_quality_guardrails_key;
  std::string expansion_key;
  std::string core_feature_key;
  std::string failure_reason;
};

struct Objc3IREmissionCompletenessScaffold {
  bool pass_graph_scaffold_ready = false;
  bool core_feature_ready = false;
  bool expansion_ready = false;
  bool edge_case_compatibility_ready = false;
  bool metadata_transport_ready = false;
  bool modular_split_ready = false;
  std::string pass_graph_key;
  std::string core_feature_key;
  std::string expansion_key;
  std::string edge_case_compatibility_key;
  std::string scaffold_key;
  std::string failure_reason;
};

struct Objc3LoweringRuntimeDiagnosticsSurfacingScaffold {
  bool stage_diagnostics_bus_consistent = false;
  bool parse_readiness_surface_present = false;
  bool parse_readiness_surface_ready = false;
  bool parse_diagnostics_hardening_consistent = false;
  bool parser_source_precision_scaffold_ready = false;
  bool typed_handoff_key_deterministic = false;
  bool diagnostics_replay_key_ready = false;
  bool modular_split_ready = false;
  std::string parse_artifact_replay_key;
  std::string lowering_boundary_replay_key;
  std::string scaffold_key;
  std::string failure_reason;
};

struct Objc3LoweringRuntimeDiagnosticsSurfacingCoreFeatureImplementationSurface {
  bool stage_diagnostics_bus_consistent = false;
  bool parse_readiness_surface_ready = false;
  bool diagnostics_surfacing_scaffold_ready = false;
  bool parser_diagnostic_surface_consistent = false;
  bool parser_diagnostic_code_surface_deterministic = false;
  bool semantic_diagnostics_deterministic = false;
  bool diagnostics_hardening_consistent = false;
  bool diagnostics_hardening_ready = false;
  bool replay_keys_ready = false;
  bool lowering_pipeline_ready = false;
  bool core_feature_impl_ready = false;
  std::size_t lexer_diagnostic_count = 0;
  std::size_t parser_diagnostic_count = 0;
  std::size_t semantic_diagnostic_count = 0;
  std::string parse_artifact_replay_key;
  std::string lowering_boundary_replay_key;
  std::string diagnostics_hardening_key;
  std::string core_feature_key;
  std::string failure_reason;
};

struct Objc3LoweringRuntimeDiagnosticsSurfacingCoreFeatureExpansionSurface {
  bool core_feature_impl_ready = false;
  bool diagnostics_surfacing_scaffold_ready = false;
  bool diagnostics_hardening_key_consistent = false;
  bool diagnostics_payload_accounting_consistent = false;
  bool expansion_replay_keys_ready = false;
  bool lowering_pipeline_expansion_ready = false;
  bool core_feature_expansion_ready = false;
  std::size_t lexer_diagnostic_count = 0;
  std::size_t parser_diagnostic_count = 0;
  std::size_t semantic_diagnostic_count = 0;
  std::size_t parser_diagnostic_code_count = 0;
  std::string parse_artifact_replay_key;
  std::string lowering_boundary_replay_key;
  std::string diagnostics_hardening_key;
  std::string core_feature_key;
  std::string expansion_key;
  std::string failure_reason;
};

struct Objc3LoweringRuntimeDiagnosticsSurfacingEdgeCaseCompatibilitySurface {
  bool core_feature_expansion_ready = false;
  bool compatibility_handoff_consistent = false;
  bool language_version_pragma_coordinate_order_consistent = false;
  bool parse_artifact_edge_case_robustness_consistent = false;
  bool parse_artifact_replay_key_deterministic = false;
  bool parse_recovery_determinism_hardening_consistent = false;
  bool parse_edge_case_surfaces_consistent = false;
  bool parse_edge_case_surfaces_ready = false;
  bool lowering_pipeline_edge_case_compatibility_ready = false;
  bool edge_case_compatibility_consistent = false;
  bool edge_case_compatibility_replay_keys_ready = false;
  bool edge_case_compatibility_ready = false;
  std::string compatibility_handoff_key;
  std::string parse_artifact_edge_robustness_key;
  std::string parse_recovery_determinism_hardening_key;
  std::string long_tail_grammar_edge_case_compatibility_key;
  std::string parser_diagnostic_grammar_hooks_edge_case_compatibility_key;
  std::string core_feature_expansion_key;
  std::string edge_case_compatibility_key;
  std::string failure_reason;
};

struct Objc3LoweringRuntimeDiagnosticsSurfacingEdgeCaseExpansionAndRobustnessSurface {
  bool edge_case_compatibility_consistent = false;
  bool edge_case_compatibility_ready = false;
  bool parse_edge_case_expansion_consistent = false;
  bool parse_edge_case_robustness_ready = false;
  bool lowering_pipeline_edge_case_robustness_ready = false;
  bool edge_case_expansion_consistent = false;
  bool edge_case_robustness_ready = false;
  std::string compatibility_handoff_key;
  std::string parse_artifact_edge_robustness_key;
  std::string parse_recovery_determinism_hardening_key;
  std::string long_tail_grammar_edge_case_robustness_key;
  std::string parser_diagnostic_grammar_hooks_edge_case_robustness_key;
  std::string lowering_pipeline_edge_case_robustness_key;
  std::string edge_case_compatibility_key;
  std::string edge_case_robustness_key;
  std::string failure_reason;
};

struct Objc3LoweringRuntimeDiagnosticsSurfacingDiagnosticsHardeningSurface {
  bool edge_case_robustness_consistent = false;
  bool edge_case_robustness_ready = false;
  bool parse_diagnostics_hardening_consistent = false;
  bool parse_diagnostics_hardening_ready = false;
  bool semantic_diagnostics_hardening_consistent = false;
  bool semantic_diagnostics_hardening_ready = false;
  bool lowering_pipeline_diagnostics_hardening_ready = false;
  bool diagnostics_hardening_consistent = false;
  bool diagnostics_hardening_ready = false;
  std::string edge_case_robustness_key;
  std::string parse_artifact_diagnostics_hardening_key;
  std::string long_tail_grammar_diagnostics_hardening_key;
  std::string parser_diagnostic_grammar_hooks_diagnostics_hardening_key;
  std::string semantic_diagnostics_hardening_key;
  std::string lowering_pipeline_diagnostics_hardening_key;
  std::string diagnostics_hardening_key;
  std::string failure_reason;
};

struct Objc3LoweringRuntimeDiagnosticsSurfacingRecoveryDeterminismHardeningSurface {
  bool diagnostics_hardening_consistent = false;
  bool diagnostics_hardening_ready = false;
  bool parse_recovery_determinism_consistent = false;
  bool parse_recovery_determinism_ready = false;
  bool semantic_recovery_determinism_consistent = false;
  bool semantic_recovery_determinism_ready = false;
  bool lowering_pipeline_recovery_determinism_ready = false;
  bool recovery_determinism_consistent = false;
  bool recovery_determinism_ready = false;
  std::string diagnostics_hardening_key;
  std::string parse_recovery_determinism_hardening_key;
  std::string long_tail_grammar_recovery_determinism_key;
  std::string parser_diagnostic_grammar_hooks_recovery_determinism_key;
  std::string semantic_recovery_determinism_key;
  std::string lowering_pipeline_recovery_determinism_key;
  std::string recovery_determinism_key;
  std::string failure_reason;
};

struct Objc3LoweringRuntimeDiagnosticsSurfacingConformanceMatrixImplementationSurface {
  bool recovery_determinism_consistent = false;
  bool recovery_determinism_ready = false;
  bool parse_conformance_matrix_consistent = false;
  bool parse_conformance_matrix_ready = false;
  bool semantic_conformance_matrix_consistent = false;
  bool semantic_conformance_matrix_ready = false;
  bool lowering_pipeline_conformance_matrix_ready = false;
  bool conformance_matrix_consistent = false;
  bool conformance_matrix_ready = false;
  std::size_t parse_lowering_conformance_matrix_case_count = 0;
  std::string recovery_determinism_key;
  std::string parse_lowering_conformance_matrix_key;
  std::string long_tail_grammar_conformance_matrix_key;
  std::string parser_diagnostic_grammar_hooks_conformance_matrix_key;
  std::string semantic_conformance_matrix_key;
  std::string lowering_pipeline_conformance_matrix_key;
  std::string conformance_matrix_key;
  std::string failure_reason;
};

struct Objc3LoweringRuntimeStabilityCoreFeatureImplementationSurface {
  bool lowering_boundary_ready = false;
  bool runtime_dispatch_contract_consistent = false;
  bool typed_handoff_key_deterministic = false;
  bool typed_core_feature_consistent = false;
  bool parse_ready_for_lowering = false;
  bool invariant_proofs_ready = false;
  bool modular_split_ready = false;
  bool typed_expansion_accounting_consistent = false;
  bool parse_conformance_accounting_consistent = false;
  bool replay_keys_ready = false;
  bool compatibility_handoff_consistent = false;
  bool language_version_pragma_coordinate_order_consistent = false;
  bool parse_edge_case_robustness_consistent = false;
  bool edge_case_compatibility_ready = false;
  bool edge_case_expansion_consistent = false;
  bool edge_case_robustness_ready = false;
  bool diagnostics_hardening_consistent = false;
  bool diagnostics_hardening_ready = false;
  bool recovery_determinism_consistent = false;
  bool recovery_determinism_ready = false;
  bool conformance_matrix_consistent = false;
  bool conformance_matrix_ready = false;
  bool conformance_corpus_consistent = false;
  bool conformance_corpus_ready = false;
  bool performance_quality_guardrails_consistent = false;
  bool performance_quality_guardrails_ready = false;
  bool cross_lane_integration_consistent = false;
  bool cross_lane_integration_ready = false;
  bool integration_closeout_consistent = false;
  bool gate_signoff_ready = false;
  bool expansion_ready = false;
  bool core_feature_impl_ready = false;
  std::size_t typed_core_feature_case_count = 0;
  std::size_t typed_core_feature_passed_case_count = 0;
  std::size_t typed_core_feature_failed_case_count = 0;
  std::size_t typed_core_feature_expansion_case_count = 0;
  std::size_t typed_core_feature_expansion_passed_case_count = 0;
  std::size_t typed_core_feature_expansion_failed_case_count = 0;
  std::size_t parse_lowering_conformance_matrix_case_count = 0;
  std::size_t parse_lowering_conformance_corpus_case_count = 0;
  std::size_t parse_lowering_conformance_corpus_passed_case_count = 0;
  std::size_t parse_lowering_conformance_corpus_failed_case_count = 0;
  std::size_t parse_lowering_performance_quality_guardrails_case_count = 0;
  std::size_t parse_lowering_performance_quality_guardrails_passed_case_count = 0;
  std::size_t parse_lowering_performance_quality_guardrails_failed_case_count = 0;
  std::string lowering_boundary_replay_key;
  std::string typed_handoff_key;
  std::string parse_artifact_replay_key;
  std::string edge_case_robustness_key;
  std::string diagnostics_hardening_key;
  std::string recovery_determinism_key;
  std::string conformance_matrix_key;
  std::string conformance_corpus_key;
  std::string performance_quality_guardrails_key;
  std::string cross_lane_integration_key;
  std::string integration_closeout_key;
  std::string edge_case_compatibility_key;
  std::string expansion_key;
  std::string core_feature_key;
  std::string failure_reason;
};

struct Objc3ToolchainRuntimeGaOperationsCoreFeatureExpansionSurface {
  bool scaffold_ready = false;
  bool backend_route_deterministic = false;
  bool compile_status_success = false;
  bool backend_output_recorded = false;
  bool backend_dispatch_consistent = false;
  bool backend_output_path_deterministic = false;
  bool backend_output_payload_consistent = false;
  bool core_feature_expansion_ready = false;
  bool edge_case_compatibility_consistent = false;
  bool edge_case_compatibility_ready = false;
  bool edge_case_expansion_consistent = false;
  bool edge_case_robustness_ready = false;
  bool diagnostics_hardening_consistent = false;
  bool diagnostics_hardening_ready = false;
  bool recovery_determinism_consistent = false;
  bool recovery_determinism_ready = false;
  bool conformance_matrix_consistent = false;
  bool conformance_matrix_ready = false;
  bool core_feature_impl_ready = false;
  std::string backend_route_key;
  std::string scaffold_key;
  std::string core_feature_key;
  std::string core_feature_expansion_key;
  std::string edge_case_compatibility_key;
  std::string edge_case_robustness_key;
  std::string diagnostics_hardening_key;
  std::string recovery_determinism_key;
  std::string conformance_matrix_key;
  std::string failure_reason;
};

struct Objc3FinalReadinessGateCoreFeatureImplementationSurface {
  bool governance_contract_ready = false;
  bool modular_split_ready = false;
  bool lane_a_core_feature_ready = false;
  bool lane_b_core_feature_ready = false;
  bool lane_c_core_feature_ready = false;
  bool lane_d_core_feature_ready = false;
  bool dependency_chain_ready = false;
  bool core_feature_expansion_consistent = false;
  bool core_feature_expansion_ready = false;
  bool edge_case_compatibility_consistent = false;
  bool edge_case_compatibility_ready = false;
  bool edge_case_expansion_consistent = false;
  bool edge_case_robustness_ready = false;
  bool diagnostics_hardening_consistent = false;
  bool diagnostics_hardening_ready = false;
  bool recovery_determinism_consistent = false;
  bool recovery_determinism_ready = false;
  bool conformance_matrix_consistent = false;
  bool conformance_matrix_ready = false;
  bool conformance_corpus_consistent = false;
  bool conformance_corpus_ready = false;
  bool performance_quality_guardrails_consistent = false;
  bool performance_quality_guardrails_ready = false;
  bool cross_lane_integration_consistent = false;
  bool cross_lane_integration_ready = false;
  bool docs_runbook_sync_consistent = false;
  bool docs_runbook_sync_ready = false;
  bool release_candidate_replay_dry_run_consistent = false;
  bool release_candidate_replay_dry_run_ready = false;
  bool advanced_core_shard1_consistent = false;
  bool advanced_core_shard1_ready = false;
  bool advanced_edge_compatibility_shard1_consistent = false;
  bool advanced_edge_compatibility_shard1_ready = false;
  bool advanced_diagnostics_shard1_consistent = false;
  bool advanced_diagnostics_shard1_ready = false;
  bool advanced_conformance_shard1_consistent = false;
  bool advanced_conformance_shard1_ready = false;
  bool advanced_integration_shard1_consistent = false;
  bool advanced_integration_shard1_ready = false;
  bool advanced_performance_shard1_consistent = false;
  bool advanced_performance_shard1_ready = false;
  bool advanced_core_shard2_consistent = false;
  bool advanced_core_shard2_ready = false;
  bool advanced_core_shard3_consistent = false;
  bool advanced_core_shard3_ready = false;
  bool advanced_edge_compatibility_shard2_consistent = false;
  bool advanced_edge_compatibility_shard2_ready = false;
  bool advanced_edge_compatibility_shard3_consistent = false;
  bool advanced_edge_compatibility_shard3_ready = false;
  bool advanced_diagnostics_shard2_consistent = false;
  bool advanced_diagnostics_shard2_ready = false;
  bool advanced_diagnostics_shard3_consistent = false;
  bool advanced_diagnostics_shard3_ready = false;
  bool advanced_conformance_shard3_consistent = false;
  bool advanced_conformance_shard3_ready = false;
  bool advanced_integration_shard3_consistent = false;
  bool advanced_integration_shard3_ready = false;
  bool advanced_performance_shard3_consistent = false;
  bool advanced_performance_shard3_ready = false;
  bool advanced_core_shard4_consistent = false;
  bool advanced_core_shard4_ready = false;
  bool advanced_edge_compatibility_shard4_consistent = false;
  bool advanced_edge_compatibility_shard4_ready = false;
  bool advanced_integration_closeout_signoff_consistent = false;
  bool advanced_integration_closeout_signoff_ready = false;
  bool advanced_conformance_shard2_consistent = false;
  bool advanced_conformance_shard2_ready = false;
  bool advanced_integration_shard2_consistent = false;
  bool advanced_integration_shard2_ready = false;
  bool advanced_performance_shard2_consistent = false;
  bool advanced_performance_shard2_ready = false;
  bool integration_closeout_signoff_consistent = false;
  bool integration_closeout_signoff_ready = false;
  bool core_feature_impl_ready = false;
  std::string governance_key;
  std::string modular_split_key;
  std::string lane_a_key;
  std::string lane_b_key;
  std::string lane_c_key;
  std::string lane_d_key;
  std::string core_feature_key;
  std::string core_feature_expansion_key;
  std::string edge_case_compatibility_key;
  std::string edge_case_robustness_key;
  std::string diagnostics_hardening_key;
  std::string recovery_determinism_key;
  std::string conformance_matrix_key;
  std::string conformance_corpus_key;
  std::string performance_quality_guardrails_key;
  std::string cross_lane_integration_key;
  std::string docs_runbook_sync_key;
  std::string release_candidate_replay_dry_run_key;
  std::string advanced_core_shard1_key;
  std::string advanced_edge_compatibility_shard1_key;
  std::string advanced_diagnostics_shard1_key;
  std::string advanced_conformance_shard1_key;
  std::string advanced_integration_shard1_key;
  std::string advanced_performance_shard1_key;
  std::string advanced_core_shard2_key;
  std::string advanced_core_shard3_key;
  std::string advanced_edge_compatibility_shard2_key;
  std::string advanced_edge_compatibility_shard3_key;
  std::string advanced_diagnostics_shard2_key;
  std::string advanced_diagnostics_shard3_key;
  std::string advanced_conformance_shard3_key;
  std::string advanced_integration_shard3_key;
  std::string advanced_performance_shard3_key;
  std::string advanced_core_shard4_key;
  std::string advanced_edge_compatibility_shard4_key;
  std::string advanced_integration_closeout_signoff_key;
  std::string advanced_conformance_shard2_key;
  std::string advanced_integration_shard2_key;
  std::string advanced_performance_shard2_key;
  std::string integration_closeout_signoff_key;
  std::string failure_reason;
};

inline Objc3LoweringOwnerContractRecord
Objc3LoweringRuntimeStabilityOwnerContractRecord(
    const Objc3LoweringRuntimeStabilityInvariantScaffold &surface) {
  const bool producer_ready =
      surface.typed_surface_present && surface.parse_readiness_surface_present;
  const bool consumer_ready =
      surface.lowering_boundary_ready && surface.parse_ready_for_lowering &&
      surface.runtime_dispatch_contract_consistent;
  const bool replay_key_deterministic =
      surface.typed_handoff_key_deterministic &&
      !surface.lowering_boundary_replay_key.empty() &&
      !surface.typed_handoff_key.empty() &&
      !surface.parse_artifact_replay_key.empty();
  const bool artifact_publication_ready =
      surface.invariant_proofs_ready && surface.modular_split_ready &&
      !surface.scaffold_key.empty();
  const bool fail_closed =
      surface.typed_core_feature_consistent && surface.failure_reason.empty();

  return BuildObjc3LoweringOwnerContractRecord(
      kObjc3LoweringOwnerRuntimeStabilityContractId,
      kObjc3LoweringOwnerRuntimeStability, surface.typed_handoff_key,
      surface.lowering_boundary_replay_key, surface.scaffold_key,
      producer_ready, consumer_ready, replay_key_deterministic,
      artifact_publication_ready, fail_closed, surface.failure_reason);
}

inline Objc3LoweringOwnerContractRecord
Objc3LoweringPipelinePassGraphOwnerContractRecord(
    const Objc3LoweringPipelinePassGraphScaffold &surface) {
  const bool producer_ready =
      surface.lex_stage_ready && surface.parse_stage_ready &&
      surface.sema_stage_ready && surface.typed_surface_ready &&
      surface.parse_lowering_readiness_ready;
  const bool consumer_ready =
      surface.lowering_ir_boundary_ready &&
      surface.runtime_dispatch_declaration_ready &&
      surface.ir_emission_entrypoint_ready;
  const bool replay_key_deterministic =
      !surface.lowering_boundary_replay_key.empty() &&
      !surface.runtime_dispatch_declaration_replay_key.empty() &&
      !surface.pass_graph_key.empty();
  const bool artifact_publication_ready =
      surface.semantic_stability_scaffold_ready &&
      surface.lowering_runtime_invariant_scaffold_ready &&
      surface.pass_graph_ready;
  const bool fail_closed = surface.failure_reason.empty();

  return BuildObjc3LoweringOwnerContractRecord(
      kObjc3LoweringOwnerPipelinePassGraphContractId,
      kObjc3LoweringOwnerPipelinePassGraph,
      surface.runtime_dispatch_declaration_replay_key,
      surface.lowering_boundary_replay_key, surface.pass_graph_key,
      producer_ready, consumer_ready, replay_key_deterministic,
      artifact_publication_ready, fail_closed, surface.failure_reason);
}

inline Objc3LoweringOwnerContractRecord
Objc3LoweringPipelinePassGraphCoreFeatureOwnerContractRecord(
    const Objc3LoweringPipelinePassGraphCoreFeatureSurface &surface) {
  const bool producer_ready =
      surface.scaffold_ready &&
      surface.lowering_boundary_replay_key_consistent &&
      surface.runtime_dispatch_declaration_consistent;
  const bool consumer_ready =
      surface.direct_ir_entrypoint_enabled &&
      surface.dispatch_shape_sharding_ready &&
      surface.llc_object_emission_route_deterministic;
  const bool replay_key_deterministic =
      surface.replay_proof_artifact_key_ready &&
      !surface.lowering_boundary_replay_key.empty() &&
      !surface.runtime_dispatch_declaration_replay_key.empty() &&
      !surface.core_feature_key.empty();
  const bool artifact_publication_ready =
      surface.expansion_ready && surface.core_feature_ready &&
      surface.edge_case_dispatch_shape_coverage_ready &&
      surface.replay_proof_expansion_ready;
  const bool fail_closed =
      surface.compatibility_handoff_consistent &&
      surface.language_version_pragma_coordinate_order_consistent &&
      surface.failure_reason.empty();

  return BuildObjc3LoweringOwnerContractRecord(
      kObjc3LoweringOwnerPassGraphCoreFeatureContractId,
      kObjc3LoweringOwnerPassGraphCoreFeature,
      surface.runtime_dispatch_declaration_replay_key,
      surface.lowering_boundary_replay_key, surface.core_feature_key,
      producer_ready, consumer_ready, replay_key_deterministic,
      artifact_publication_ready, fail_closed, surface.failure_reason);
}

inline Objc3LoweringOwnerContractRecord
Objc3IREmissionCompletenessOwnerContractRecord(
    const Objc3IREmissionCompletenessScaffold &surface) {
  const bool producer_ready =
      surface.pass_graph_scaffold_ready && surface.core_feature_ready &&
      surface.expansion_ready;
  const bool consumer_ready =
      surface.edge_case_compatibility_ready && surface.metadata_transport_ready;
  const bool replay_key_deterministic =
      !surface.pass_graph_key.empty() && !surface.core_feature_key.empty() &&
      !surface.expansion_key.empty() &&
      !surface.edge_case_compatibility_key.empty() &&
      !surface.scaffold_key.empty();
  const bool artifact_publication_ready =
      surface.modular_split_ready && replay_key_deterministic;
  const bool fail_closed = surface.failure_reason.empty();

  return BuildObjc3LoweringOwnerContractRecord(
      kObjc3LoweringOwnerIREmissionCompletenessContractId,
      kObjc3LoweringOwnerIREmissionCompleteness, surface.pass_graph_key,
      surface.edge_case_compatibility_key, surface.scaffold_key,
      producer_ready, consumer_ready, replay_key_deterministic,
      artifact_publication_ready, fail_closed, surface.failure_reason);
}

inline Objc3LoweringOwnerContractRecord
Objc3LoweringRuntimeDiagnosticsSurfacingOwnerContractRecord(
    const Objc3LoweringRuntimeDiagnosticsSurfacingScaffold &surface) {
  const bool producer_ready =
      surface.stage_diagnostics_bus_consistent &&
      surface.parse_readiness_surface_present &&
      surface.parse_readiness_surface_ready &&
      surface.parse_diagnostics_hardening_consistent;
  const bool consumer_ready =
      surface.parser_source_precision_scaffold_ready &&
      surface.diagnostics_replay_key_ready;
  const bool replay_key_deterministic =
      surface.typed_handoff_key_deterministic &&
      !surface.parse_artifact_replay_key.empty() &&
      !surface.lowering_boundary_replay_key.empty() &&
      !surface.scaffold_key.empty();
  const bool artifact_publication_ready =
      surface.modular_split_ready && replay_key_deterministic;
  const bool fail_closed = surface.failure_reason.empty();

  return BuildObjc3LoweringOwnerContractRecord(
      kObjc3LoweringOwnerDiagnosticsSurfacingContractId,
      kObjc3LoweringOwnerDiagnosticsSurfacing,
      surface.parse_artifact_replay_key, surface.lowering_boundary_replay_key,
      surface.scaffold_key, producer_ready, consumer_ready,
      replay_key_deterministic, artifact_publication_ready, fail_closed,
      surface.failure_reason);
}

inline Objc3LoweringOwnerContractRecord
Objc3ToolchainRuntimeGaOperationsOwnerContractRecord(
    const Objc3ToolchainRuntimeGaOperationsCoreFeatureExpansionSurface
        &surface) {
  const bool producer_ready =
      surface.scaffold_ready && surface.backend_route_deterministic &&
      surface.compile_status_success;
  const bool consumer_ready =
      surface.backend_dispatch_consistent &&
      surface.backend_output_path_deterministic &&
      surface.backend_output_payload_consistent;
  const bool replay_key_deterministic =
      !surface.backend_route_key.empty() && !surface.scaffold_key.empty() &&
      !surface.core_feature_key.empty() &&
      !surface.core_feature_expansion_key.empty();
  const bool artifact_publication_ready =
      surface.backend_output_recorded && surface.core_feature_expansion_ready &&
      surface.core_feature_impl_ready;
  const bool fail_closed =
      surface.edge_case_compatibility_consistent &&
      surface.edge_case_compatibility_ready &&
      surface.failure_reason.empty();

  return BuildObjc3LoweringOwnerContractRecord(
      kObjc3LoweringOwnerToolchainGaContractId,
      kObjc3LoweringOwnerToolchainGa, surface.backend_route_key,
      surface.core_feature_key, surface.core_feature_expansion_key,
      producer_ready, consumer_ready, replay_key_deterministic,
      artifact_publication_ready, fail_closed, surface.failure_reason);
}

inline Objc3LoweringOwnerContractRecord
Objc3FinalReadinessGateOwnerContractRecord(
    const Objc3FinalReadinessGateCoreFeatureImplementationSurface &surface) {
  const bool producer_ready =
      surface.governance_contract_ready && surface.modular_split_ready;
  const bool consumer_ready =
      surface.lane_a_core_feature_ready && surface.lane_b_core_feature_ready &&
      surface.lane_c_core_feature_ready && surface.lane_d_core_feature_ready &&
      surface.dependency_chain_ready;
  const bool replay_key_deterministic =
      !surface.governance_key.empty() && !surface.modular_split_key.empty() &&
      !surface.lane_a_key.empty() && !surface.lane_b_key.empty() &&
      !surface.lane_c_key.empty() && !surface.lane_d_key.empty() &&
      !surface.core_feature_key.empty();
  const bool artifact_publication_ready =
      surface.integration_closeout_signoff_consistent &&
      surface.integration_closeout_signoff_ready &&
      !surface.integration_closeout_signoff_key.empty() &&
      surface.core_feature_impl_ready;
  const bool fail_closed =
      surface.cross_lane_integration_consistent &&
      surface.cross_lane_integration_ready && surface.failure_reason.empty();

  return BuildObjc3LoweringOwnerContractRecord(
      kObjc3LoweringOwnerFinalReadinessGateContractId,
      kObjc3LoweringOwnerFinalReadinessGate, surface.governance_key,
      surface.core_feature_key, surface.integration_closeout_signoff_key,
      producer_ready, consumer_ready, replay_key_deterministic,
      artifact_publication_ready, fail_closed, surface.failure_reason);
}
