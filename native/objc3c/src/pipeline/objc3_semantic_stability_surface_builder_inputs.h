#pragma once

#include "pipeline/objc3_semantic_stability_core_feature_implementation_surface_keys.h"

inline void PopulateObjc3SemanticStabilitySurfaceInputs(
    Objc3SemanticStabilityCoreFeatureImplementationSurface &surface,
    const Objc3TypedSemaToLoweringContractSurface &typed_surface,
    const Objc3ParseLoweringReadinessSurface &parse_surface,
    const Objc3SemanticStabilitySpecDeltaClosureScaffold &scaffold) {
  surface.semantic_handoff_deterministic =
      scaffold.semantic_handoff_deterministic;
  surface.typed_core_feature_consistent =
      typed_surface.typed_core_feature_consistent;
  surface.typed_core_feature_expansion_consistent =
      typed_surface.typed_core_feature_expansion_consistent;
  surface.typed_sema_core_feature_consistent =
      parse_surface.typed_sema_core_feature_consistent;
  surface.typed_sema_core_feature_expansion_consistent =
      parse_surface.typed_sema_core_feature_expansion_consistent;
  surface.parse_lowering_conformance_matrix_consistent =
      parse_surface.parse_lowering_conformance_matrix_consistent;
  surface.parse_lowering_conformance_corpus_consistent =
      parse_surface.parse_lowering_conformance_corpus_consistent;
  surface.parse_lowering_performance_quality_guardrails_consistent =
      parse_surface.parse_lowering_performance_quality_guardrails_consistent;
  surface.spec_delta_closed = scaffold.spec_delta_closed;
  surface.modular_split_ready = scaffold.modular_split_ready;
  surface.typed_core_feature_case_count =
      typed_surface.typed_core_feature_case_count;
  surface.typed_core_feature_passed_case_count =
      typed_surface.typed_core_feature_passed_case_count;
  surface.typed_core_feature_failed_case_count =
      typed_surface.typed_core_feature_failed_case_count;
  surface.typed_core_feature_expansion_case_count =
      typed_surface.typed_core_feature_expansion_case_count;
  surface.typed_core_feature_expansion_passed_case_count =
      typed_surface.typed_core_feature_expansion_passed_case_count;
  surface.typed_core_feature_expansion_failed_case_count =
      typed_surface.typed_core_feature_expansion_failed_case_count;
  surface.parse_lowering_conformance_matrix_case_count =
      parse_surface.parse_lowering_conformance_matrix_case_count;
  surface.parse_lowering_conformance_corpus_case_count =
      parse_surface.parse_lowering_conformance_corpus_case_count;
  surface.parse_lowering_conformance_corpus_passed_case_count =
      parse_surface.parse_lowering_conformance_corpus_passed_case_count;
  surface.parse_lowering_conformance_corpus_failed_case_count =
      parse_surface.parse_lowering_conformance_corpus_failed_case_count;
  surface.parse_lowering_performance_quality_guardrails_case_count =
      parse_surface.parse_lowering_performance_quality_guardrails_case_count;
  surface.parse_lowering_performance_quality_guardrails_passed_case_count =
      parse_surface
          .parse_lowering_performance_quality_guardrails_passed_case_count;
  surface.parse_lowering_performance_quality_guardrails_failed_case_count =
      parse_surface
          .parse_lowering_performance_quality_guardrails_failed_case_count;
  surface.typed_handoff_key = typed_surface.typed_handoff_key;
  surface.parse_artifact_replay_key = parse_surface.parse_artifact_replay_key;
  surface.edge_case_robustness_key =
      parse_surface.long_tail_grammar_edge_case_robustness_key;
  surface.diagnostics_hardening_key =
      parse_surface.long_tail_grammar_diagnostics_hardening_key;
  surface.recovery_determinism_key =
      parse_surface.long_tail_grammar_recovery_determinism_key;
  surface.conformance_matrix_key =
      parse_surface.long_tail_grammar_conformance_matrix_key;
  surface.conformance_corpus_key =
      parse_surface.parse_lowering_conformance_corpus_key;
  surface.performance_quality_guardrails_key =
      parse_surface.parse_lowering_performance_quality_guardrails_key;
}
