#pragma once

#include <string>

#include "lower/model/lowered_module_runtime_surfaces.h"
#include "pipeline/results/phase_result.h"
#include "sema/model/semantic_program.h"

struct Objc3FinalReadinessGateLaneSurface {
  bool core_feature_ready = false;
  bool core_feature_impl_ready = false;
  bool core_feature_expansion_ready = false;
  bool expansion_ready = false;
  bool edge_case_compatibility_ready = false;
  bool edge_case_robustness_ready = false;
  bool diagnostics_hardening_ready = false;
  bool recovery_determinism_ready = false;
  bool conformance_matrix_ready = false;
  bool conformance_corpus_ready = false;
  bool performance_quality_guardrails_ready = false;
  bool cross_lane_integration_ready = false;
  bool docs_runbook_sync_ready = false;
  bool release_candidate_replay_dry_run_ready = false;
  bool advanced_core_shard1_ready = false;
  bool advanced_edge_compatibility_shard1_ready = false;
  bool advanced_diagnostics_shard1_ready = false;
  bool advanced_conformance_shard1_ready = false;
  bool advanced_integration_shard1_ready = false;
  bool advanced_performance_shard1_ready = false;
  bool advanced_core_shard2_ready = false;
  bool advanced_edge_compatibility_shard2_ready = false;
  bool advanced_diagnostics_shard2_ready = false;
  bool advanced_conformance_shard2_ready = false;
  bool advanced_integration_shard2_ready = false;
  bool advanced_performance_shard2_ready = false;
  bool integration_closeout_signoff_ready = false;
  std::string core_feature_key;
  std::string recovery_determinism_key;
  std::string conformance_matrix_key;
  std::string edge_case_robustness_key;
  std::string diagnostics_hardening_key;
  std::string advanced_integration_shard1_key;
  std::string advanced_performance_shard1_key;
  std::string advanced_core_shard2_key;
  std::string advanced_edge_compatibility_shard2_key;
  std::string advanced_diagnostics_shard2_key;
  std::string advanced_conformance_shard2_key;
  std::string integration_closeout_signoff_key;
};

using Objc3FrontendLongTailGrammarCoreFeatureSurface =
    Objc3FinalReadinessGateLaneSurface;

inline Objc3FrontendLongTailGrammarCoreFeatureSurface
BuildObjc3FrontendLongTailGrammarCoreFeatureSurface(
    const Objc3ParseLoweringReadinessSurface &surface) {
  Objc3FrontendLongTailGrammarCoreFeatureSurface lane;
  lane.core_feature_ready =
      surface.long_tail_grammar_core_feature_consistent &&
      !surface.long_tail_grammar_handoff_key.empty();
  lane.core_feature_impl_ready = lane.core_feature_ready;
  lane.core_feature_expansion_ready = surface.long_tail_grammar_expansion_ready;
  lane.expansion_ready = surface.long_tail_grammar_expansion_ready;
  lane.edge_case_compatibility_ready =
      surface.long_tail_grammar_edge_case_compatibility_ready;
  lane.edge_case_robustness_ready =
      surface.long_tail_grammar_edge_case_robustness_ready;
  lane.diagnostics_hardening_ready =
      surface.long_tail_grammar_diagnostics_hardening_ready;
  lane.recovery_determinism_ready =
      surface.long_tail_grammar_recovery_determinism_ready;
  lane.conformance_matrix_ready =
      surface.long_tail_grammar_conformance_matrix_ready;
  lane.conformance_corpus_ready =
      surface.parse_lowering_conformance_corpus_consistent &&
      !surface.parse_lowering_conformance_corpus_key.empty();
  lane.performance_quality_guardrails_ready =
      surface.parse_lowering_performance_quality_guardrails_consistent &&
      !surface.parse_lowering_performance_quality_guardrails_key.empty();
  lane.cross_lane_integration_ready =
      surface.long_tail_grammar_integration_closeout_consistent &&
      !surface.long_tail_grammar_integration_closeout_key.empty();
  lane.docs_runbook_sync_ready = lane.cross_lane_integration_ready;
  lane.release_candidate_replay_dry_run_ready =
      surface.long_tail_grammar_gate_signoff_ready;
  lane.advanced_core_shard1_ready = lane.edge_case_robustness_ready;
  lane.advanced_edge_compatibility_shard1_ready =
      lane.edge_case_robustness_ready;
  lane.advanced_diagnostics_shard1_ready = lane.diagnostics_hardening_ready;
  lane.advanced_conformance_shard1_ready = lane.conformance_matrix_ready;
  lane.advanced_integration_shard1_ready = lane.cross_lane_integration_ready;
  lane.advanced_performance_shard1_ready =
      lane.performance_quality_guardrails_ready;
  lane.advanced_core_shard2_ready = surface.long_tail_grammar_gate_signoff_ready;
  lane.advanced_edge_compatibility_shard2_ready =
      lane.edge_case_compatibility_ready;
  lane.advanced_diagnostics_shard2_ready = lane.diagnostics_hardening_ready;
  lane.advanced_conformance_shard2_ready = lane.conformance_matrix_ready;
  lane.advanced_integration_shard2_ready = lane.cross_lane_integration_ready;
  lane.advanced_performance_shard2_ready =
      lane.performance_quality_guardrails_ready;
  lane.integration_closeout_signoff_ready =
      surface.long_tail_grammar_gate_signoff_ready &&
      !surface.long_tail_grammar_integration_closeout_key.empty();
  lane.core_feature_key = surface.long_tail_grammar_handoff_key;
  lane.recovery_determinism_key =
      surface.long_tail_grammar_recovery_determinism_key;
  lane.conformance_matrix_key = surface.long_tail_grammar_conformance_matrix_key;
  lane.edge_case_robustness_key =
      surface.long_tail_grammar_edge_case_robustness_key;
  lane.diagnostics_hardening_key =
      surface.long_tail_grammar_diagnostics_hardening_key;
  lane.advanced_integration_shard1_key =
      surface.long_tail_grammar_integration_closeout_key;
  lane.advanced_performance_shard1_key =
      surface.parse_lowering_performance_quality_guardrails_key;
  lane.advanced_core_shard2_key =
      surface.long_tail_grammar_integration_closeout_key;
  lane.advanced_edge_compatibility_shard2_key =
      surface.long_tail_grammar_edge_case_compatibility_key;
  lane.advanced_diagnostics_shard2_key =
      surface.long_tail_grammar_diagnostics_hardening_key;
  lane.advanced_conformance_shard2_key =
      surface.long_tail_grammar_conformance_matrix_key;
  lane.integration_closeout_signoff_key =
      surface.long_tail_grammar_integration_closeout_key;
  return lane;
}

inline Objc3FinalReadinessGateLaneSurface
BuildObjc3SemanticStabilityFinalReadinessGateLaneSurface(
    const Objc3SemanticStabilityCoreFeatureImplementationSurface &surface) {
  Objc3FinalReadinessGateLaneSurface lane;
  lane.core_feature_ready = surface.core_feature_impl_ready;
  lane.core_feature_impl_ready = surface.core_feature_impl_ready;
  lane.core_feature_expansion_ready = surface.expansion_ready;
  lane.expansion_ready = surface.expansion_ready;
  lane.edge_case_compatibility_ready = surface.edge_case_compatibility_ready;
  lane.edge_case_robustness_ready = surface.edge_case_robustness_ready;
  lane.diagnostics_hardening_ready = surface.diagnostics_hardening_ready;
  lane.recovery_determinism_ready = surface.recovery_determinism_ready;
  lane.conformance_matrix_ready = surface.conformance_matrix_ready;
  lane.conformance_corpus_ready = surface.conformance_corpus_ready;
  lane.performance_quality_guardrails_ready =
      surface.performance_quality_guardrails_ready;
  lane.cross_lane_integration_ready =
      surface.integration_closeout_consistent &&
      !surface.integration_closeout_key.empty();
  lane.docs_runbook_sync_ready = lane.cross_lane_integration_ready;
  lane.release_candidate_replay_dry_run_ready = surface.gate_signoff_ready;
  lane.advanced_core_shard1_ready = surface.core_feature_impl_ready;
  lane.advanced_edge_compatibility_shard1_ready =
      surface.edge_case_robustness_ready;
  lane.advanced_diagnostics_shard1_ready = surface.diagnostics_hardening_ready;
  lane.advanced_conformance_shard1_ready = surface.conformance_matrix_ready;
  lane.advanced_integration_shard1_ready = lane.cross_lane_integration_ready;
  lane.advanced_performance_shard1_ready =
      surface.performance_quality_guardrails_ready;
  lane.advanced_core_shard2_ready = surface.gate_signoff_ready;
  lane.advanced_edge_compatibility_shard2_ready =
      surface.edge_case_compatibility_ready;
  lane.advanced_diagnostics_shard2_ready = surface.diagnostics_hardening_ready;
  lane.advanced_conformance_shard2_ready = surface.conformance_matrix_ready;
  lane.advanced_integration_shard2_ready = lane.cross_lane_integration_ready;
  lane.advanced_performance_shard2_ready =
      surface.performance_quality_guardrails_ready;
  lane.integration_closeout_signoff_ready = surface.gate_signoff_ready;
  lane.core_feature_key = surface.core_feature_key;
  lane.recovery_determinism_key = surface.recovery_determinism_key;
  lane.conformance_matrix_key = surface.conformance_matrix_key;
  lane.edge_case_robustness_key = surface.edge_case_robustness_key;
  lane.diagnostics_hardening_key = surface.diagnostics_hardening_key;
  lane.advanced_integration_shard1_key = surface.integration_closeout_key;
  lane.advanced_performance_shard1_key =
      surface.performance_quality_guardrails_key;
  lane.advanced_core_shard2_key = surface.integration_closeout_key;
  lane.advanced_edge_compatibility_shard2_key =
      surface.edge_case_robustness_key;
  lane.advanced_diagnostics_shard2_key = surface.diagnostics_hardening_key;
  lane.advanced_conformance_shard2_key = surface.conformance_matrix_key;
  lane.integration_closeout_signoff_key = surface.integration_closeout_key;
  return lane;
}

inline Objc3FinalReadinessGateLaneSurface
BuildObjc3LoweringRuntimeStabilityFinalReadinessGateLaneSurface(
    const Objc3LoweringRuntimeStabilityCoreFeatureImplementationSurface
        &surface) {
  Objc3FinalReadinessGateLaneSurface lane;
  lane.core_feature_ready = surface.core_feature_impl_ready;
  lane.core_feature_impl_ready = surface.core_feature_impl_ready;
  lane.core_feature_expansion_ready = surface.expansion_ready;
  lane.expansion_ready = surface.expansion_ready;
  lane.edge_case_compatibility_ready = surface.edge_case_compatibility_ready;
  lane.edge_case_robustness_ready = surface.edge_case_robustness_ready;
  lane.diagnostics_hardening_ready = surface.diagnostics_hardening_ready;
  lane.recovery_determinism_ready = surface.recovery_determinism_ready;
  lane.conformance_matrix_ready = surface.conformance_matrix_ready;
  lane.conformance_corpus_ready = surface.conformance_corpus_ready;
  lane.performance_quality_guardrails_ready =
      surface.performance_quality_guardrails_ready;
  lane.cross_lane_integration_ready = surface.cross_lane_integration_ready;
  lane.docs_runbook_sync_ready = surface.cross_lane_integration_ready;
  lane.release_candidate_replay_dry_run_ready = surface.gate_signoff_ready;
  lane.advanced_core_shard1_ready = surface.core_feature_impl_ready;
  lane.advanced_edge_compatibility_shard1_ready =
      surface.edge_case_robustness_ready;
  lane.advanced_diagnostics_shard1_ready = surface.diagnostics_hardening_ready;
  lane.advanced_conformance_shard1_ready = surface.conformance_matrix_ready;
  lane.advanced_integration_shard1_ready = surface.cross_lane_integration_ready;
  lane.advanced_performance_shard1_ready =
      surface.performance_quality_guardrails_ready;
  lane.advanced_core_shard2_ready = surface.gate_signoff_ready;
  lane.advanced_edge_compatibility_shard2_ready =
      surface.edge_case_compatibility_ready;
  lane.advanced_diagnostics_shard2_ready = surface.diagnostics_hardening_ready;
  lane.advanced_conformance_shard2_ready = surface.conformance_matrix_ready;
  lane.advanced_integration_shard2_ready = surface.cross_lane_integration_ready;
  lane.advanced_performance_shard2_ready =
      surface.performance_quality_guardrails_ready;
  lane.integration_closeout_signoff_ready = surface.gate_signoff_ready;
  lane.core_feature_key = surface.core_feature_key;
  lane.recovery_determinism_key = surface.recovery_determinism_key;
  lane.conformance_matrix_key = surface.conformance_matrix_key;
  lane.edge_case_robustness_key = surface.edge_case_robustness_key;
  lane.diagnostics_hardening_key = surface.diagnostics_hardening_key;
  lane.advanced_integration_shard1_key = surface.cross_lane_integration_key;
  lane.advanced_performance_shard1_key =
      surface.performance_quality_guardrails_key;
  lane.advanced_core_shard2_key = surface.integration_closeout_key;
  lane.advanced_edge_compatibility_shard2_key =
      surface.edge_case_compatibility_key;
  lane.advanced_diagnostics_shard2_key = surface.diagnostics_hardening_key;
  lane.advanced_conformance_shard2_key = surface.conformance_matrix_key;
  lane.integration_closeout_signoff_key = surface.integration_closeout_key;
  return lane;
}

inline Objc3FinalReadinessGateLaneSurface
BuildObjc3ToolchainRuntimeGaOperationsFinalReadinessGateLaneSurface(
    const Objc3ToolchainRuntimeGaOperationsCoreFeatureExpansionSurface
        &surface) {
  Objc3FinalReadinessGateLaneSurface lane;
  lane.core_feature_ready = surface.core_feature_impl_ready;
  lane.core_feature_impl_ready = surface.core_feature_impl_ready;
  lane.core_feature_expansion_ready = surface.core_feature_expansion_ready;
  lane.expansion_ready = surface.core_feature_expansion_ready;
  lane.edge_case_compatibility_ready = surface.edge_case_compatibility_ready;
  lane.edge_case_robustness_ready = surface.edge_case_robustness_ready;
  lane.diagnostics_hardening_ready = surface.diagnostics_hardening_ready;
  lane.recovery_determinism_ready = surface.recovery_determinism_ready;
  lane.conformance_matrix_ready = surface.conformance_matrix_ready;
  lane.conformance_corpus_ready = surface.conformance_matrix_ready;
  lane.performance_quality_guardrails_ready = surface.conformance_matrix_ready;
  lane.cross_lane_integration_ready = surface.core_feature_impl_ready;
  lane.docs_runbook_sync_ready = surface.core_feature_impl_ready;
  lane.release_candidate_replay_dry_run_ready = surface.core_feature_impl_ready;
  lane.advanced_core_shard1_ready = surface.core_feature_impl_ready;
  lane.advanced_edge_compatibility_shard1_ready =
      surface.edge_case_compatibility_ready;
  lane.advanced_diagnostics_shard1_ready = surface.diagnostics_hardening_ready;
  lane.advanced_conformance_shard1_ready = surface.conformance_matrix_ready;
  lane.advanced_integration_shard1_ready = surface.core_feature_impl_ready;
  lane.advanced_performance_shard1_ready = surface.conformance_matrix_ready;
  lane.advanced_core_shard2_ready = surface.core_feature_impl_ready;
  lane.advanced_edge_compatibility_shard2_ready =
      surface.edge_case_compatibility_ready;
  lane.advanced_diagnostics_shard2_ready = surface.diagnostics_hardening_ready;
  lane.advanced_conformance_shard2_ready = surface.conformance_matrix_ready;
  lane.advanced_integration_shard2_ready = surface.core_feature_impl_ready;
  lane.advanced_performance_shard2_ready = surface.conformance_matrix_ready;
  lane.integration_closeout_signoff_ready = surface.core_feature_impl_ready;
  lane.core_feature_key = surface.core_feature_key;
  lane.recovery_determinism_key = surface.recovery_determinism_key;
  lane.conformance_matrix_key = surface.conformance_matrix_key;
  lane.edge_case_robustness_key = surface.edge_case_robustness_key;
  lane.diagnostics_hardening_key = surface.diagnostics_hardening_key;
  lane.advanced_integration_shard1_key = surface.core_feature_key;
  lane.advanced_performance_shard1_key = surface.conformance_matrix_key;
  lane.advanced_core_shard2_key = surface.core_feature_key;
  lane.advanced_edge_compatibility_shard2_key =
      surface.edge_case_compatibility_key;
  lane.advanced_diagnostics_shard2_key = surface.diagnostics_hardening_key;
  lane.advanced_conformance_shard2_key = surface.conformance_matrix_key;
  lane.integration_closeout_signoff_key = surface.core_feature_key;
  return lane;
}

inline Objc3FinalReadinessGateLaneSurface
BuildObjc3ToolchainRuntimeGaOperationsFinalReadinessGateLaneSurface(
    const Objc3ParseLoweringReadinessSurface &surface) {
  Objc3FinalReadinessGateLaneSurface lane;
  lane.core_feature_ready =
      surface.toolchain_runtime_ga_operations_cross_lane_integration_ready;
  lane.core_feature_impl_ready = lane.core_feature_ready;
  lane.core_feature_expansion_ready = lane.core_feature_ready;
  lane.expansion_ready = lane.core_feature_ready;
  lane.edge_case_compatibility_ready =
      surface.toolchain_runtime_ga_operations_advanced_edge_compatibility_ready;
  lane.edge_case_robustness_ready =
      surface.toolchain_runtime_ga_operations_advanced_edge_compatibility_ready;
  lane.diagnostics_hardening_ready =
      surface.toolchain_runtime_ga_operations_advanced_diagnostics_ready;
  lane.recovery_determinism_ready =
      surface.toolchain_runtime_ga_operations_advanced_core_ready;
  lane.conformance_matrix_ready =
      surface.toolchain_runtime_ga_operations_advanced_conformance_ready;
  lane.conformance_corpus_ready =
      surface.toolchain_runtime_ga_operations_advanced_conformance_ready;
  lane.performance_quality_guardrails_ready =
      surface.toolchain_runtime_ga_operations_advanced_performance_ready;
  lane.cross_lane_integration_ready =
      surface.toolchain_runtime_ga_operations_cross_lane_integration_ready;
  lane.docs_runbook_sync_ready =
      surface.toolchain_runtime_ga_operations_docs_runbook_sync_ready;
  lane.release_candidate_replay_dry_run_ready =
      surface.toolchain_runtime_ga_operations_advanced_integration_ready;
  lane.advanced_core_shard1_ready =
      surface.toolchain_runtime_ga_operations_advanced_core_ready;
  lane.advanced_edge_compatibility_shard1_ready =
      surface.toolchain_runtime_ga_operations_advanced_edge_compatibility_ready;
  lane.advanced_diagnostics_shard1_ready =
      surface.toolchain_runtime_ga_operations_advanced_diagnostics_ready;
  lane.advanced_conformance_shard1_ready =
      surface.toolchain_runtime_ga_operations_advanced_conformance_ready;
  lane.advanced_integration_shard1_ready =
      surface.toolchain_runtime_ga_operations_advanced_integration_ready;
  lane.advanced_performance_shard1_ready =
      surface.toolchain_runtime_ga_operations_advanced_performance_ready;
  lane.advanced_core_shard2_ready =
      surface.toolchain_runtime_ga_operations_advanced_core_shard2_ready;
  lane.advanced_edge_compatibility_shard2_ready =
      surface.toolchain_runtime_ga_operations_advanced_edge_compatibility_ready;
  lane.advanced_diagnostics_shard2_ready =
      surface.toolchain_runtime_ga_operations_advanced_diagnostics_ready;
  lane.advanced_conformance_shard2_ready =
      surface.toolchain_runtime_ga_operations_advanced_conformance_ready;
  lane.advanced_integration_shard2_ready =
      surface.toolchain_runtime_ga_operations_advanced_integration_ready;
  lane.advanced_performance_shard2_ready =
      surface.toolchain_runtime_ga_operations_advanced_performance_ready;
  lane.integration_closeout_signoff_ready =
      surface.toolchain_runtime_ga_operations_integration_closeout_signoff_ready;
  lane.core_feature_key =
      surface.toolchain_runtime_ga_operations_cross_lane_integration_key;
  lane.recovery_determinism_key =
      surface.toolchain_runtime_ga_operations_advanced_core_key;
  lane.conformance_matrix_key =
      surface.toolchain_runtime_ga_operations_advanced_conformance_key;
  lane.edge_case_robustness_key =
      surface.toolchain_runtime_ga_operations_advanced_edge_compatibility_key;
  lane.diagnostics_hardening_key =
      surface.toolchain_runtime_ga_operations_advanced_diagnostics_key;
  lane.advanced_integration_shard1_key =
      surface.toolchain_runtime_ga_operations_advanced_integration_key;
  lane.advanced_performance_shard1_key =
      surface.toolchain_runtime_ga_operations_advanced_performance_key;
  lane.advanced_core_shard2_key =
      surface.toolchain_runtime_ga_operations_advanced_core_shard2_key;
  lane.advanced_edge_compatibility_shard2_key =
      surface.toolchain_runtime_ga_operations_advanced_edge_compatibility_key;
  lane.advanced_diagnostics_shard2_key =
      surface.toolchain_runtime_ga_operations_advanced_diagnostics_key;
  lane.advanced_conformance_shard2_key =
      surface.toolchain_runtime_ga_operations_advanced_conformance_key;
  lane.integration_closeout_signoff_key =
      surface.toolchain_runtime_ga_operations_integration_closeout_signoff_key;
  return lane;
}
