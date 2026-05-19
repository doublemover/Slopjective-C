#pragma once

#include "pipeline/objc3_frontend_types.h"

inline void PopulateObjc3LoweringRuntimeStabilitySurfaceInputs(
    Objc3LoweringRuntimeStabilityCoreFeatureImplementationSurface &surface,
    const Objc3TypedSemaToLoweringContractSurface &typed_surface,
    const Objc3ParseLoweringReadinessSurface &parse_surface,
    const Objc3LoweringRuntimeStabilityInvariantScaffold &scaffold) {
  surface.lowering_boundary_ready = scaffold.lowering_boundary_ready;
  surface.runtime_dispatch_contract_consistent =
      scaffold.runtime_dispatch_contract_consistent;
  surface.typed_handoff_key_deterministic =
      scaffold.typed_handoff_key_deterministic;
  surface.typed_core_feature_consistent = scaffold.typed_core_feature_consistent;
  surface.parse_ready_for_lowering = scaffold.parse_ready_for_lowering;
  surface.invariant_proofs_ready = scaffold.invariant_proofs_ready;
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
  surface.compatibility_handoff_consistent =
      parse_surface.compatibility_handoff_consistent;
  surface.language_version_pragma_coordinate_order_consistent =
      parse_surface.language_version_pragma_coordinate_order_consistent;
  surface.parse_edge_case_robustness_consistent =
      parse_surface.parse_artifact_edge_case_robustness_consistent;
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

  surface.lowering_boundary_replay_key = scaffold.lowering_boundary_replay_key;
  surface.typed_handoff_key = scaffold.typed_handoff_key;
  surface.parse_artifact_replay_key = scaffold.parse_artifact_replay_key;
}
