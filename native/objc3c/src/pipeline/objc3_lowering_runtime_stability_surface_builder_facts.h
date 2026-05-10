#pragma once

#include "pipeline/objc3_frontend_types.h"

struct Objc3LoweringRuntimeStabilityReadinessFacts {
  bool typed_case_accounting_consistent = false;
  bool typed_expansion_case_accounting_consistent = false;
  bool parse_corpus_case_accounting_consistent = false;
  bool parse_guardrails_case_accounting_consistent = false;
  bool parse_matrix_case_count_ready = false;
  bool replay_keys_ready = false;
  bool typed_expansion_accounting_consistent = false;
  bool parse_conformance_accounting_consistent = false;
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
  bool edge_case_compatibility_expansion_ready = false;
  bool edge_case_expansion_ready = false;
  bool diagnostics_hardening_expansion_ready = false;
  bool expansion_ready = false;
  bool recovery_determinism_expansion_ready = false;
  bool conformance_matrix_expansion_ready = false;
  bool conformance_corpus_expansion_ready = false;
  bool performance_quality_guardrails_expansion_ready = false;
  bool cross_lane_integration_expansion_ready = false;
  bool integration_closeout_expansion_ready = false;
};

inline Objc3LoweringRuntimeStabilityReadinessFacts
BuildObjc3LoweringRuntimeStabilityReadinessFacts(
    const Objc3LoweringRuntimeStabilityCoreFeatureImplementationSurface &surface,
    const Objc3TypedSemaToLoweringContractSurface &typed_surface,
    const Objc3ParseLoweringReadinessSurface &parse_surface) {
  Objc3LoweringRuntimeStabilityReadinessFacts facts;
  facts.typed_case_accounting_consistent =
      surface.typed_core_feature_case_count > 0 &&
      surface.typed_core_feature_passed_case_count <=
          surface.typed_core_feature_case_count &&
      surface.typed_core_feature_failed_case_count ==
          (surface.typed_core_feature_case_count -
           surface.typed_core_feature_passed_case_count);
  facts.typed_expansion_case_accounting_consistent =
      surface.typed_core_feature_expansion_case_count > 0 &&
      surface.typed_core_feature_expansion_passed_case_count <=
          surface.typed_core_feature_expansion_case_count &&
      surface.typed_core_feature_expansion_failed_case_count ==
          (surface.typed_core_feature_expansion_case_count -
           surface.typed_core_feature_expansion_passed_case_count);
  facts.parse_corpus_case_accounting_consistent =
      surface.parse_lowering_conformance_corpus_case_count > 0 &&
      surface.parse_lowering_conformance_corpus_passed_case_count <=
          surface.parse_lowering_conformance_corpus_case_count &&
      surface.parse_lowering_conformance_corpus_failed_case_count ==
          (surface.parse_lowering_conformance_corpus_case_count -
           surface.parse_lowering_conformance_corpus_passed_case_count);
  facts.parse_guardrails_case_accounting_consistent =
      surface.parse_lowering_performance_quality_guardrails_case_count > 0 &&
      surface.parse_lowering_performance_quality_guardrails_passed_case_count <=
          surface.parse_lowering_performance_quality_guardrails_case_count &&
      surface.parse_lowering_performance_quality_guardrails_failed_case_count ==
          (surface.parse_lowering_performance_quality_guardrails_case_count -
           surface.parse_lowering_performance_quality_guardrails_passed_case_count);
  facts.parse_matrix_case_count_ready =
      surface.parse_lowering_conformance_matrix_case_count > 0;
  facts.replay_keys_ready =
      !surface.lowering_boundary_replay_key.empty() &&
      !surface.typed_handoff_key.empty() &&
      !surface.parse_artifact_replay_key.empty();
  facts.typed_expansion_accounting_consistent =
      facts.typed_case_accounting_consistent &&
      facts.typed_expansion_case_accounting_consistent;
  facts.parse_conformance_accounting_consistent =
      facts.parse_matrix_case_count_ready &&
      facts.parse_corpus_case_accounting_consistent &&
      facts.parse_guardrails_case_accounting_consistent;
  facts.edge_case_compatibility_ready =
      parse_surface.compatibility_handoff_consistent &&
      parse_surface.language_version_pragma_coordinate_order_consistent &&
      parse_surface.parse_artifact_edge_case_robustness_consistent &&
      parse_surface.parse_artifact_replay_key_deterministic &&
      parse_surface.parse_recovery_determinism_hardening_consistent &&
      !parse_surface.compatibility_handoff_key.empty() &&
      !parse_surface.parse_artifact_edge_robustness_key.empty();
  facts.edge_case_expansion_consistent =
      parse_surface.long_tail_grammar_edge_case_expansion_consistent &&
      parse_surface.parse_artifact_edge_case_robustness_consistent &&
      parse_surface.parse_recovery_determinism_hardening_consistent;
  facts.edge_case_robustness_ready =
      facts.edge_case_compatibility_ready &&
      parse_surface.long_tail_grammar_edge_case_robustness_ready &&
      !parse_surface.long_tail_grammar_edge_case_robustness_key.empty();
  facts.diagnostics_hardening_consistent =
      facts.edge_case_expansion_consistent &&
      parse_surface.long_tail_grammar_diagnostics_hardening_consistent &&
      parse_surface.parse_artifact_diagnostics_hardening_consistent &&
      parse_surface.parser_diagnostic_surface_consistent &&
      parse_surface.parser_diagnostic_code_surface_deterministic;
  facts.diagnostics_hardening_ready =
      facts.diagnostics_hardening_consistent &&
      facts.edge_case_robustness_ready &&
      parse_surface.long_tail_grammar_diagnostics_hardening_ready &&
      parse_surface.semantic_diagnostics_deterministic &&
      !parse_surface.long_tail_grammar_diagnostics_hardening_key.empty() &&
      !parse_surface.parse_artifact_diagnostics_hardening_key.empty();
  facts.recovery_determinism_consistent =
      facts.diagnostics_hardening_consistent &&
      parse_surface.long_tail_grammar_recovery_determinism_consistent &&
      parse_surface.parse_recovery_determinism_hardening_consistent &&
      parse_surface.parser_recovery_replay_ready &&
      parse_surface.parse_artifact_replay_key_deterministic;
  facts.recovery_determinism_ready =
      facts.recovery_determinism_consistent &&
      facts.diagnostics_hardening_ready &&
      parse_surface.long_tail_grammar_recovery_determinism_ready &&
      parse_surface.semantic_diagnostics_deterministic &&
      !parse_surface.long_tail_grammar_recovery_determinism_key.empty() &&
      !parse_surface.parse_recovery_determinism_hardening_key.empty();
  facts.conformance_matrix_consistent =
      facts.recovery_determinism_consistent &&
      parse_surface.long_tail_grammar_conformance_matrix_consistent &&
      parse_surface.parse_lowering_conformance_matrix_consistent &&
      parse_surface.parse_artifact_replay_key_deterministic;
  facts.conformance_matrix_ready =
      facts.conformance_matrix_consistent &&
      facts.recovery_determinism_ready &&
      parse_surface.long_tail_grammar_conformance_matrix_ready &&
      facts.parse_matrix_case_count_ready &&
      !parse_surface.long_tail_grammar_conformance_matrix_key.empty() &&
      !parse_surface.parse_lowering_conformance_matrix_key.empty();
  facts.conformance_corpus_consistent =
      facts.conformance_matrix_consistent &&
      parse_surface.parse_lowering_conformance_corpus_consistent &&
      parse_surface.parse_artifact_replay_key_deterministic;
  facts.conformance_corpus_ready =
      facts.conformance_corpus_consistent &&
      facts.conformance_matrix_ready &&
      facts.parse_corpus_case_accounting_consistent &&
      !parse_surface.parse_lowering_conformance_corpus_key.empty();
  facts.performance_quality_guardrails_consistent =
      facts.conformance_corpus_consistent &&
      parse_surface.parse_lowering_performance_quality_guardrails_consistent &&
      parse_surface.parse_artifact_replay_key_deterministic;
  facts.performance_quality_guardrails_ready =
      facts.performance_quality_guardrails_consistent &&
      facts.conformance_corpus_ready &&
      facts.parse_guardrails_case_accounting_consistent &&
      !parse_surface.parse_lowering_performance_quality_guardrails_key.empty();
  facts.cross_lane_integration_consistent =
      facts.performance_quality_guardrails_consistent &&
      parse_surface.long_tail_grammar_integration_closeout_consistent &&
      parse_surface.long_tail_grammar_gate_signoff_ready &&
      parse_surface.compatibility_handoff_consistent &&
      typed_surface.semantic_handoff_deterministic &&
      typed_surface.semantic_handoff_consistent;
  facts.cross_lane_integration_ready =
      facts.cross_lane_integration_consistent &&
      facts.performance_quality_guardrails_ready &&
      facts.replay_keys_ready &&
      !surface.parse_artifact_replay_key.empty();
  facts.integration_closeout_consistent =
      facts.cross_lane_integration_consistent &&
      facts.cross_lane_integration_ready &&
      facts.performance_quality_guardrails_consistent &&
      facts.replay_keys_ready;
  facts.gate_signoff_ready =
      facts.integration_closeout_consistent &&
      facts.performance_quality_guardrails_ready &&
      !surface.cross_lane_integration_key.empty();
  facts.edge_case_compatibility_expansion_ready =
      facts.typed_expansion_accounting_consistent &&
      facts.parse_conformance_accounting_consistent &&
      facts.replay_keys_ready &&
      facts.edge_case_compatibility_ready;
  facts.edge_case_expansion_ready =
      facts.edge_case_compatibility_expansion_ready &&
      facts.edge_case_robustness_ready;
  facts.diagnostics_hardening_expansion_ready =
      facts.edge_case_expansion_ready &&
      facts.diagnostics_hardening_ready;
  facts.expansion_ready = facts.diagnostics_hardening_expansion_ready;
  facts.recovery_determinism_expansion_ready =
      facts.expansion_ready &&
      facts.recovery_determinism_ready;
  facts.conformance_matrix_expansion_ready =
      facts.recovery_determinism_expansion_ready &&
      facts.conformance_matrix_ready;
  facts.conformance_corpus_expansion_ready =
      facts.conformance_matrix_expansion_ready &&
      facts.conformance_corpus_ready;
  facts.performance_quality_guardrails_expansion_ready =
      facts.conformance_corpus_expansion_ready &&
      facts.performance_quality_guardrails_ready;
  facts.cross_lane_integration_expansion_ready =
      facts.performance_quality_guardrails_expansion_ready &&
      facts.cross_lane_integration_ready &&
      !surface.cross_lane_integration_key.empty();
  facts.integration_closeout_expansion_ready =
      facts.cross_lane_integration_expansion_ready &&
      facts.gate_signoff_ready &&
      !surface.integration_closeout_key.empty();
  return facts;
}
