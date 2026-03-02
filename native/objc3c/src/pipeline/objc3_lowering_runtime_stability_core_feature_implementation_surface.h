#pragma once

#include <sstream>
#include <string>

#include "pipeline/objc3_frontend_types.h"

inline std::string BuildObjc3LoweringRuntimeStabilityCoreFeatureImplementationKey(
    const Objc3LoweringRuntimeStabilityCoreFeatureImplementationSurface &surface) {
  std::ostringstream key;
  key << "lowering-runtime-stability-core-feature-impl:v1:"
      << "typed_core_feature_case_count=" << surface.typed_core_feature_case_count
      << ";typed_core_feature_passed_case_count=" << surface.typed_core_feature_passed_case_count
      << ";typed_core_feature_failed_case_count=" << surface.typed_core_feature_failed_case_count
      << ";typed_core_feature_expansion_case_count="
      << surface.typed_core_feature_expansion_case_count
      << ";typed_core_feature_expansion_passed_case_count="
      << surface.typed_core_feature_expansion_passed_case_count
      << ";typed_core_feature_expansion_failed_case_count="
      << surface.typed_core_feature_expansion_failed_case_count
      << ";parse_lowering_conformance_matrix_case_count="
      << surface.parse_lowering_conformance_matrix_case_count
      << ";parse_lowering_conformance_corpus_case_count="
      << surface.parse_lowering_conformance_corpus_case_count
      << ";parse_lowering_conformance_corpus_passed_case_count="
      << surface.parse_lowering_conformance_corpus_passed_case_count
      << ";parse_lowering_conformance_corpus_failed_case_count="
      << surface.parse_lowering_conformance_corpus_failed_case_count
      << ";parse_lowering_performance_quality_guardrails_case_count="
      << surface.parse_lowering_performance_quality_guardrails_case_count
      << ";parse_lowering_performance_quality_guardrails_passed_case_count="
      << surface.parse_lowering_performance_quality_guardrails_passed_case_count
      << ";parse_lowering_performance_quality_guardrails_failed_case_count="
      << surface.parse_lowering_performance_quality_guardrails_failed_case_count
      << ";lowering_boundary_ready=" << (surface.lowering_boundary_ready ? "true" : "false")
      << ";runtime_dispatch_contract_consistent="
      << (surface.runtime_dispatch_contract_consistent ? "true" : "false")
      << ";typed_handoff_key_deterministic="
      << (surface.typed_handoff_key_deterministic ? "true" : "false")
      << ";typed_core_feature_consistent="
      << (surface.typed_core_feature_consistent ? "true" : "false")
      << ";parse_ready_for_lowering=" << (surface.parse_ready_for_lowering ? "true" : "false")
      << ";invariant_proofs_ready=" << (surface.invariant_proofs_ready ? "true" : "false")
      << ";modular_split_ready=" << (surface.modular_split_ready ? "true" : "false")
      << ";typed_expansion_accounting_consistent="
      << (surface.typed_expansion_accounting_consistent ? "true" : "false")
      << ";parse_conformance_accounting_consistent="
      << (surface.parse_conformance_accounting_consistent ? "true" : "false")
      << ";replay_keys_ready=" << (surface.replay_keys_ready ? "true" : "false")
      << ";compatibility_handoff_consistent="
      << (surface.compatibility_handoff_consistent ? "true" : "false")
      << ";pragma_coordinate_order_consistent="
      << (surface.language_version_pragma_coordinate_order_consistent ? "true" : "false")
      << ";parse_edge_case_robustness_consistent="
      << (surface.parse_edge_case_robustness_consistent ? "true" : "false")
      << ";edge_case_compatibility_ready="
      << (surface.edge_case_compatibility_ready ? "true" : "false")
      << ";edge_case_expansion_consistent="
      << (surface.edge_case_expansion_consistent ? "true" : "false")
      << ";edge_case_robustness_ready="
      << (surface.edge_case_robustness_ready ? "true" : "false")
      << ";edge_case_robustness_key=" << surface.edge_case_robustness_key
      << ";diagnostics_hardening_consistent="
      << (surface.diagnostics_hardening_consistent ? "true" : "false")
      << ";diagnostics_hardening_ready="
      << (surface.diagnostics_hardening_ready ? "true" : "false")
      << ";diagnostics_hardening_key=" << surface.diagnostics_hardening_key
      << ";recovery_determinism_consistent="
      << (surface.recovery_determinism_consistent ? "true" : "false")
      << ";recovery_determinism_ready="
      << (surface.recovery_determinism_ready ? "true" : "false")
      << ";recovery_determinism_key=" << surface.recovery_determinism_key
      << ";conformance_matrix_consistent="
      << (surface.conformance_matrix_consistent ? "true" : "false")
      << ";conformance_matrix_ready="
      << (surface.conformance_matrix_ready ? "true" : "false")
      << ";conformance_matrix_key=" << surface.conformance_matrix_key
      << ";conformance_corpus_consistent="
      << (surface.conformance_corpus_consistent ? "true" : "false")
      << ";conformance_corpus_ready="
      << (surface.conformance_corpus_ready ? "true" : "false")
      << ";conformance_corpus_key=" << surface.conformance_corpus_key
      << ";performance_quality_guardrails_consistent="
      << (surface.performance_quality_guardrails_consistent ? "true" : "false")
      << ";performance_quality_guardrails_ready="
      << (surface.performance_quality_guardrails_ready ? "true" : "false")
      << ";performance_quality_guardrails_key="
      << surface.performance_quality_guardrails_key
      << ";cross_lane_integration_consistent="
      << (surface.cross_lane_integration_consistent ? "true" : "false")
      << ";cross_lane_integration_ready="
      << (surface.cross_lane_integration_ready ? "true" : "false")
      << ";cross_lane_integration_key=" << surface.cross_lane_integration_key
      << ";integration_closeout_consistent="
      << (surface.integration_closeout_consistent ? "true" : "false")
      << ";gate_signoff_ready="
      << (surface.gate_signoff_ready ? "true" : "false")
      << ";integration_closeout_key=" << surface.integration_closeout_key
      << ";expansion_ready=" << (surface.expansion_ready ? "true" : "false")
      << ";core_feature_impl_ready=" << (surface.core_feature_impl_ready ? "true" : "false");
  return key.str();
}

inline std::string BuildObjc3LoweringRuntimeCrossLaneIntegrationKey(
    const Objc3LoweringRuntimeStabilityCoreFeatureImplementationSurface &surface,
    bool cross_lane_integration_consistent,
    bool cross_lane_integration_ready) {
  std::ostringstream key;
  key << "lowering-runtime-cross-lane-integration:v1:"
      << "lane-a-closeout-consistent="
      << (surface.compatibility_handoff_consistent ? "true" : "false")
      << ";lane-a-gate-signoff-ready="
      << (surface.language_version_pragma_coordinate_order_consistent ? "true" : "false")
      << ";lane-b-semantic-handoff-deterministic="
      << (surface.typed_handoff_key_deterministic ? "true" : "false")
      << ";lane-b-semantic-core-ready="
      << (surface.typed_core_feature_consistent ? "true" : "false")
      << ";lane-c-performance-guardrails-ready="
      << (surface.performance_quality_guardrails_ready ? "true" : "false")
      << ";cross-lane-integration-consistent="
      << (cross_lane_integration_consistent ? "true" : "false")
      << ";cross-lane-integration-ready="
      << (cross_lane_integration_ready ? "true" : "false");
  return key.str();
}

inline std::string BuildObjc3LoweringRuntimeIntegrationCloseoutKey(
    const Objc3LoweringRuntimeStabilityCoreFeatureImplementationSurface &surface,
    bool integration_closeout_consistent,
    bool gate_signoff_ready) {
  std::ostringstream key;
  key << "lowering-runtime-integration-closeout:v1:"
      << "cross-lane-integration-consistent="
      << (surface.cross_lane_integration_consistent ? "true" : "false")
      << ";cross-lane-integration-ready="
      << (surface.cross_lane_integration_ready ? "true" : "false")
      << ";performance-guardrails-ready="
      << (surface.performance_quality_guardrails_ready ? "true" : "false")
      << ";replay-keys-ready=" << (surface.replay_keys_ready ? "true" : "false")
      << ";integration-closeout-consistent="
      << (integration_closeout_consistent ? "true" : "false")
      << ";gate-signoff-ready=" << (gate_signoff_ready ? "true" : "false");
  return key.str();
}

inline Objc3LoweringRuntimeStabilityCoreFeatureImplementationSurface
BuildObjc3LoweringRuntimeStabilityCoreFeatureImplementationSurface(
    const Objc3TypedSemaToLoweringContractSurface &typed_surface,
    const Objc3ParseLoweringReadinessSurface &parse_surface,
    const Objc3LoweringRuntimeStabilityInvariantScaffold &scaffold) {
  Objc3LoweringRuntimeStabilityCoreFeatureImplementationSurface surface;
  surface.lowering_boundary_ready = scaffold.lowering_boundary_ready;
  surface.runtime_dispatch_contract_consistent =
      scaffold.runtime_dispatch_contract_consistent;
  surface.typed_handoff_key_deterministic = scaffold.typed_handoff_key_deterministic;
  surface.typed_core_feature_consistent = scaffold.typed_core_feature_consistent;
  surface.parse_ready_for_lowering = scaffold.parse_ready_for_lowering;
  surface.invariant_proofs_ready = scaffold.invariant_proofs_ready;
  surface.modular_split_ready = scaffold.modular_split_ready;

  surface.typed_core_feature_case_count = typed_surface.typed_core_feature_case_count;
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
      parse_surface.parse_lowering_performance_quality_guardrails_passed_case_count;
  surface.parse_lowering_performance_quality_guardrails_failed_case_count =
      parse_surface.parse_lowering_performance_quality_guardrails_failed_case_count;
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

  const bool typed_case_accounting_consistent =
      surface.typed_core_feature_case_count > 0 &&
      surface.typed_core_feature_passed_case_count <=
          surface.typed_core_feature_case_count &&
      surface.typed_core_feature_failed_case_count ==
          (surface.typed_core_feature_case_count -
           surface.typed_core_feature_passed_case_count);
  const bool typed_expansion_case_accounting_consistent =
      surface.typed_core_feature_expansion_case_count > 0 &&
      surface.typed_core_feature_expansion_passed_case_count <=
          surface.typed_core_feature_expansion_case_count &&
      surface.typed_core_feature_expansion_failed_case_count ==
          (surface.typed_core_feature_expansion_case_count -
           surface.typed_core_feature_expansion_passed_case_count);
  const bool parse_corpus_case_accounting_consistent =
      surface.parse_lowering_conformance_corpus_case_count > 0 &&
      surface.parse_lowering_conformance_corpus_passed_case_count <=
          surface.parse_lowering_conformance_corpus_case_count &&
      surface.parse_lowering_conformance_corpus_failed_case_count ==
          (surface.parse_lowering_conformance_corpus_case_count -
           surface.parse_lowering_conformance_corpus_passed_case_count);
  const bool parse_guardrails_case_accounting_consistent =
      surface.parse_lowering_performance_quality_guardrails_case_count > 0 &&
      surface.parse_lowering_performance_quality_guardrails_passed_case_count <=
          surface.parse_lowering_performance_quality_guardrails_case_count &&
      surface.parse_lowering_performance_quality_guardrails_failed_case_count ==
          (surface.parse_lowering_performance_quality_guardrails_case_count -
           surface.parse_lowering_performance_quality_guardrails_passed_case_count);
  const bool parse_matrix_case_count_ready =
      surface.parse_lowering_conformance_matrix_case_count > 0;
  const bool replay_keys_ready =
      !surface.lowering_boundary_replay_key.empty() &&
      !surface.typed_handoff_key.empty() &&
      !surface.parse_artifact_replay_key.empty();
  const bool typed_expansion_accounting_consistent =
      typed_case_accounting_consistent &&
      typed_expansion_case_accounting_consistent;
  const bool parse_conformance_accounting_consistent =
      parse_matrix_case_count_ready &&
      parse_corpus_case_accounting_consistent &&
      parse_guardrails_case_accounting_consistent;
  const bool edge_case_compatibility_ready =
      parse_surface.compatibility_handoff_consistent &&
      parse_surface.language_version_pragma_coordinate_order_consistent &&
      parse_surface.parse_artifact_edge_case_robustness_consistent &&
      parse_surface.parse_artifact_replay_key_deterministic &&
      parse_surface.parse_recovery_determinism_hardening_consistent &&
      !parse_surface.compatibility_handoff_key.empty() &&
      !parse_surface.parse_artifact_edge_robustness_key.empty();
  const bool edge_case_expansion_consistent =
      parse_surface.long_tail_grammar_edge_case_expansion_consistent &&
      parse_surface.parse_artifact_edge_case_robustness_consistent &&
      parse_surface.parse_recovery_determinism_hardening_consistent;
  const bool edge_case_robustness_ready =
      edge_case_compatibility_ready &&
      parse_surface.long_tail_grammar_edge_case_robustness_ready &&
      !parse_surface.long_tail_grammar_edge_case_robustness_key.empty();
  const bool diagnostics_hardening_consistent =
      edge_case_expansion_consistent &&
      parse_surface.long_tail_grammar_diagnostics_hardening_consistent &&
      parse_surface.parse_artifact_diagnostics_hardening_consistent &&
      parse_surface.parser_diagnostic_surface_consistent &&
      parse_surface.parser_diagnostic_code_surface_deterministic;
  const bool diagnostics_hardening_ready =
      diagnostics_hardening_consistent &&
      edge_case_robustness_ready &&
      parse_surface.long_tail_grammar_diagnostics_hardening_ready &&
      parse_surface.semantic_diagnostics_deterministic &&
      !parse_surface.long_tail_grammar_diagnostics_hardening_key.empty() &&
      !parse_surface.parse_artifact_diagnostics_hardening_key.empty();
  const bool recovery_determinism_consistent =
      diagnostics_hardening_consistent &&
      parse_surface.long_tail_grammar_recovery_determinism_consistent &&
      parse_surface.parse_recovery_determinism_hardening_consistent &&
      parse_surface.parser_recovery_replay_ready &&
      parse_surface.parse_artifact_replay_key_deterministic;
  const bool recovery_determinism_ready =
      recovery_determinism_consistent &&
      diagnostics_hardening_ready &&
      parse_surface.long_tail_grammar_recovery_determinism_ready &&
      parse_surface.semantic_diagnostics_deterministic &&
      !parse_surface.long_tail_grammar_recovery_determinism_key.empty() &&
      !parse_surface.parse_recovery_determinism_hardening_key.empty();
  const bool conformance_matrix_consistent =
      recovery_determinism_consistent &&
      parse_surface.long_tail_grammar_conformance_matrix_consistent &&
      parse_surface.parse_lowering_conformance_matrix_consistent &&
      parse_surface.parse_artifact_replay_key_deterministic;
  const bool conformance_matrix_ready =
      conformance_matrix_consistent &&
      recovery_determinism_ready &&
      parse_surface.long_tail_grammar_conformance_matrix_ready &&
      parse_matrix_case_count_ready &&
      !parse_surface.long_tail_grammar_conformance_matrix_key.empty() &&
      !parse_surface.parse_lowering_conformance_matrix_key.empty();
  const bool conformance_corpus_consistent =
      conformance_matrix_consistent &&
      parse_surface.parse_lowering_conformance_corpus_consistent &&
      parse_surface.parse_artifact_replay_key_deterministic;
  const bool conformance_corpus_ready =
      conformance_corpus_consistent &&
      conformance_matrix_ready &&
      parse_corpus_case_accounting_consistent &&
      !parse_surface.parse_lowering_conformance_corpus_key.empty();
  const bool performance_quality_guardrails_consistent =
      conformance_corpus_consistent &&
      parse_surface.parse_lowering_performance_quality_guardrails_consistent &&
      parse_surface.parse_artifact_replay_key_deterministic;
  const bool performance_quality_guardrails_ready =
      performance_quality_guardrails_consistent &&
      conformance_corpus_ready &&
      parse_guardrails_case_accounting_consistent &&
      !parse_surface.parse_lowering_performance_quality_guardrails_key.empty();
  const bool cross_lane_integration_consistent =
      performance_quality_guardrails_consistent &&
      parse_surface.long_tail_grammar_integration_closeout_consistent &&
      parse_surface.long_tail_grammar_gate_signoff_ready &&
      parse_surface.compatibility_handoff_consistent &&
      typed_surface.semantic_handoff_deterministic &&
      typed_surface.semantic_handoff_consistent;
  const bool cross_lane_integration_ready =
      cross_lane_integration_consistent &&
      performance_quality_guardrails_ready &&
      replay_keys_ready &&
      !surface.parse_artifact_replay_key.empty();
  const bool integration_closeout_consistent =
      cross_lane_integration_consistent &&
      cross_lane_integration_ready &&
      performance_quality_guardrails_consistent &&
      replay_keys_ready;
  const bool gate_signoff_ready =
      integration_closeout_consistent &&
      performance_quality_guardrails_ready &&
      !surface.cross_lane_integration_key.empty();
  const bool edge_case_compatibility_expansion_ready =
      typed_expansion_accounting_consistent &&
      parse_conformance_accounting_consistent &&
      replay_keys_ready &&
      edge_case_compatibility_ready;
  const bool edge_case_expansion_ready =
      edge_case_compatibility_expansion_ready &&
      edge_case_robustness_ready;
  const bool diagnostics_hardening_expansion_ready =
      edge_case_expansion_ready &&
      diagnostics_hardening_ready;
  const bool expansion_ready = diagnostics_hardening_expansion_ready;

  surface.typed_expansion_accounting_consistent =
      typed_expansion_accounting_consistent;
  surface.parse_conformance_accounting_consistent =
      parse_conformance_accounting_consistent;
  surface.replay_keys_ready = replay_keys_ready;
  surface.edge_case_compatibility_ready = edge_case_compatibility_ready;
  surface.edge_case_expansion_consistent = edge_case_expansion_consistent;
  surface.edge_case_robustness_ready = edge_case_robustness_ready;
  surface.diagnostics_hardening_consistent = diagnostics_hardening_consistent;
  surface.diagnostics_hardening_ready = diagnostics_hardening_ready;
  surface.recovery_determinism_consistent = recovery_determinism_consistent;
  surface.recovery_determinism_ready = recovery_determinism_ready;
  surface.conformance_matrix_consistent = conformance_matrix_consistent;
  surface.conformance_matrix_ready = conformance_matrix_ready;
  surface.conformance_corpus_consistent = conformance_corpus_consistent;
  surface.conformance_corpus_ready = conformance_corpus_ready;
  surface.performance_quality_guardrails_consistent =
      performance_quality_guardrails_consistent;
  surface.performance_quality_guardrails_ready =
      performance_quality_guardrails_ready;
  surface.cross_lane_integration_consistent = cross_lane_integration_consistent;
  surface.cross_lane_integration_ready = cross_lane_integration_ready;
  surface.cross_lane_integration_key =
      BuildObjc3LoweringRuntimeCrossLaneIntegrationKey(
          surface, cross_lane_integration_consistent, cross_lane_integration_ready);
  surface.integration_closeout_consistent = integration_closeout_consistent;
  surface.gate_signoff_ready = gate_signoff_ready;
  surface.integration_closeout_key = BuildObjc3LoweringRuntimeIntegrationCloseoutKey(
      surface, integration_closeout_consistent, gate_signoff_ready);
  surface.expansion_ready = expansion_ready;
  const bool recovery_determinism_expansion_ready =
      surface.expansion_ready &&
      recovery_determinism_ready;
  surface.expansion_ready = recovery_determinism_expansion_ready;
  const bool conformance_matrix_expansion_ready =
      recovery_determinism_expansion_ready &&
      conformance_matrix_ready;
  surface.expansion_ready = conformance_matrix_expansion_ready;
  const bool conformance_corpus_expansion_ready =
      conformance_matrix_expansion_ready &&
      conformance_corpus_ready;
  surface.expansion_ready = conformance_corpus_expansion_ready;
  const bool performance_quality_guardrails_expansion_ready =
      conformance_corpus_expansion_ready &&
      performance_quality_guardrails_ready;
  surface.expansion_ready = performance_quality_guardrails_expansion_ready;
  const bool cross_lane_integration_expansion_ready =
      performance_quality_guardrails_expansion_ready &&
      cross_lane_integration_ready &&
      !surface.cross_lane_integration_key.empty();
  surface.expansion_ready = cross_lane_integration_expansion_ready;
  const bool integration_closeout_expansion_ready =
      cross_lane_integration_expansion_ready &&
      gate_signoff_ready &&
      !surface.integration_closeout_key.empty();
  surface.expansion_ready = integration_closeout_expansion_ready;

  surface.core_feature_impl_ready =
      surface.lowering_boundary_ready &&
      surface.runtime_dispatch_contract_consistent &&
      surface.typed_handoff_key_deterministic &&
      surface.typed_core_feature_consistent &&
      surface.parse_ready_for_lowering &&
      surface.invariant_proofs_ready &&
      surface.modular_split_ready &&
      typed_case_accounting_consistent &&
      typed_expansion_case_accounting_consistent &&
      parse_matrix_case_count_ready &&
      parse_corpus_case_accounting_consistent &&
      parse_guardrails_case_accounting_consistent &&
      replay_keys_ready;
  surface.core_feature_impl_ready =
      surface.core_feature_impl_ready && surface.expansion_ready;
  surface.core_feature_key =
      BuildObjc3LoweringRuntimeStabilityCoreFeatureImplementationKey(surface);
  surface.expansion_key =
      "lowering-runtime-stability-core-feature-expansion:v1:"
      "typed-expansion-accounting-consistent=" +
      std::string(typed_expansion_accounting_consistent ? "true" : "false") +
      ";parse-conformance-accounting-consistent=" +
      std::string(parse_conformance_accounting_consistent ? "true" : "false") +
      ";replay-keys-ready=" + std::string(replay_keys_ready ? "true" : "false") +
      ";edge-compat-ready=" +
      std::string(edge_case_compatibility_ready ? "true" : "false") +
      ";edge-expansion-consistent=" +
      std::string(edge_case_expansion_consistent ? "true" : "false") +
      ";edge-robustness-ready=" +
      std::string(edge_case_robustness_ready ? "true" : "false") +
      ";edge-robustness-key-ready=" +
      std::string(!surface.edge_case_robustness_key.empty() ? "true" : "false") +
      ";diag-hardening-consistent=" +
      std::string(diagnostics_hardening_consistent ? "true" : "false") +
      ";diag-hardening-ready=" +
      std::string(diagnostics_hardening_ready ? "true" : "false") +
      ";diag-hardening-key-ready=" +
      std::string(!surface.diagnostics_hardening_key.empty() ? "true" : "false") +
      ";diag-hardening-expansion-ready=" +
      std::string(diagnostics_hardening_expansion_ready ? "true" : "false") +
      ";recovery-determinism-consistent=" +
      std::string(recovery_determinism_consistent ? "true" : "false") +
      ";recovery-determinism-ready=" +
      std::string(recovery_determinism_ready ? "true" : "false") +
      ";recovery-determinism-key-ready=" +
      std::string(!surface.recovery_determinism_key.empty() ? "true" : "false") +
      ";recovery-determinism-expansion-ready=" +
      std::string(recovery_determinism_expansion_ready ? "true" : "false") +
      ";conformance-matrix-consistent=" +
      std::string(conformance_matrix_consistent ? "true" : "false") +
      ";conformance-matrix-ready=" +
      std::string(conformance_matrix_ready ? "true" : "false") +
      ";conformance-matrix-key-ready=" +
      std::string(!surface.conformance_matrix_key.empty() ? "true" : "false") +
      ";conformance-matrix-expansion-ready=" +
      std::string(conformance_matrix_expansion_ready ? "true" : "false") +
      ";conformance-corpus-consistent=" +
      std::string(conformance_corpus_consistent ? "true" : "false") +
      ";conformance-corpus-ready=" +
      std::string(conformance_corpus_ready ? "true" : "false") +
      ";conformance-corpus-key-ready=" +
      std::string(!surface.conformance_corpus_key.empty() ? "true" : "false") +
      ";conformance-corpus-expansion-ready=" +
      std::string(conformance_corpus_expansion_ready ? "true" : "false") +
      ";performance-quality-guardrails-consistent=" +
      std::string(performance_quality_guardrails_consistent ? "true" : "false") +
      ";performance-quality-guardrails-ready=" +
      std::string(performance_quality_guardrails_ready ? "true" : "false") +
      ";performance-quality-guardrails-key-ready=" +
      std::string(!surface.performance_quality_guardrails_key.empty() ? "true" : "false") +
      ";performance-quality-guardrails-expansion-ready=" +
      std::string(performance_quality_guardrails_expansion_ready ? "true" : "false") +
      ";cross-lane-integration-consistent=" +
      std::string(cross_lane_integration_consistent ? "true" : "false") +
      ";cross-lane-integration-ready=" +
      std::string(cross_lane_integration_ready ? "true" : "false") +
      ";cross-lane-integration-key-ready=" +
      std::string(!surface.cross_lane_integration_key.empty() ? "true" : "false") +
      ";cross-lane-integration-expansion-ready=" +
      std::string(cross_lane_integration_expansion_ready ? "true" : "false") +
      ";integration-closeout-consistent=" +
      std::string(integration_closeout_consistent ? "true" : "false") +
      ";gate-signoff-ready=" + std::string(gate_signoff_ready ? "true" : "false") +
      ";integration-closeout-key-ready=" +
      std::string(!surface.integration_closeout_key.empty() ? "true" : "false") +
      ";integration-closeout-expansion-ready=" +
      std::string(integration_closeout_expansion_ready ? "true" : "false") +
      ";compat-handoff-consistent=" +
      std::string(parse_surface.compatibility_handoff_consistent ? "true" : "false") +
      ";parser-diagnostic-surface-consistent=" +
      std::string(parse_surface.parser_diagnostic_surface_consistent ? "true" : "false") +
      ";semantic-diagnostics-deterministic=" +
      std::string(parse_surface.semantic_diagnostics_deterministic ? "true" : "false") +
      ";parse-edge-robustness-consistent=" +
      std::string(parse_surface.parse_artifact_edge_case_robustness_consistent ? "true" : "false") +
      ";expansion-ready=" + std::string(surface.expansion_ready ? "true" : "false");
  surface.edge_case_compatibility_key =
      "lowering-runtime-edge-compatibility:v1:compatibility-handoff-consistent=" +
      std::string(parse_surface.compatibility_handoff_consistent ? "true" : "false") +
      ";pragma-coordinate-order-consistent=" +
      std::string(parse_surface.language_version_pragma_coordinate_order_consistent ? "true" : "false") +
      ";parse-edge-robustness-consistent=" +
      std::string(parse_surface.parse_artifact_edge_case_robustness_consistent ? "true" : "false") +
      ";edge-compat-ready=" + std::string(edge_case_compatibility_ready ? "true" : "false");

  if (surface.core_feature_impl_ready) {
    return surface;
  }

  if (!surface.lowering_boundary_ready) {
    surface.failure_reason = "lowering boundary is not ready";
  } else if (!surface.runtime_dispatch_contract_consistent) {
    surface.failure_reason = "runtime dispatch contract is inconsistent";
  } else if (!surface.typed_handoff_key_deterministic) {
    surface.failure_reason = "typed handoff key is not deterministic";
  } else if (!surface.typed_core_feature_consistent) {
    surface.failure_reason = "typed core feature is inconsistent";
  } else if (!surface.parse_ready_for_lowering) {
    surface.failure_reason = "parse-lowering readiness is false";
  } else if (!surface.invariant_proofs_ready) {
    surface.failure_reason = "lowering/runtime invariant proofs are not ready";
  } else if (!surface.modular_split_ready) {
    surface.failure_reason = "lowering/runtime modular split scaffold is not ready";
  } else if (!typed_case_accounting_consistent) {
    surface.failure_reason = "typed core feature case accounting is inconsistent";
  } else if (!typed_expansion_case_accounting_consistent) {
    surface.failure_reason =
        "typed core feature expansion case accounting is inconsistent";
  } else if (!parse_matrix_case_count_ready) {
    surface.failure_reason = "parse conformance matrix case count is not ready";
  } else if (!parse_corpus_case_accounting_consistent) {
    surface.failure_reason = "parse conformance corpus case accounting is inconsistent";
  } else if (!parse_guardrails_case_accounting_consistent) {
    surface.failure_reason = "parse guardrails case accounting is inconsistent";
  } else if (!typed_expansion_accounting_consistent) {
    surface.failure_reason =
        "typed core feature expansion accounting is inconsistent";
  } else if (!parse_conformance_accounting_consistent) {
    surface.failure_reason =
        "parse conformance accounting is inconsistent";
  } else if (!replay_keys_ready) {
    surface.failure_reason = "lowering/runtime replay keys are not ready";
  } else if (!edge_case_compatibility_ready) {
    surface.failure_reason = "lowering/runtime edge-case compatibility is not ready";
  } else if (!edge_case_expansion_consistent) {
    surface.failure_reason = "lowering/runtime edge-case expansion is inconsistent";
  } else if (!edge_case_robustness_ready) {
    surface.failure_reason = "lowering/runtime edge-case robustness is not ready";
  } else if (!diagnostics_hardening_consistent) {
    surface.failure_reason = "lowering/runtime diagnostics hardening is inconsistent";
  } else if (!diagnostics_hardening_ready) {
    surface.failure_reason = "lowering/runtime diagnostics hardening is not ready";
  } else if (!recovery_determinism_consistent) {
    surface.failure_reason = "lowering/runtime recovery determinism is inconsistent";
  } else if (!recovery_determinism_ready) {
    surface.failure_reason = "lowering/runtime recovery determinism is not ready";
  } else if (!conformance_matrix_consistent) {
    surface.failure_reason = "lowering/runtime conformance matrix is inconsistent";
  } else if (!conformance_matrix_ready) {
    surface.failure_reason = "lowering/runtime conformance matrix is not ready";
  } else if (!conformance_corpus_consistent) {
    surface.failure_reason = "lowering/runtime conformance corpus is inconsistent";
  } else if (!conformance_corpus_ready) {
    surface.failure_reason = "lowering/runtime conformance corpus is not ready";
  } else if (!performance_quality_guardrails_consistent) {
    surface.failure_reason =
        "lowering/runtime performance quality guardrails are inconsistent";
  } else if (!performance_quality_guardrails_ready) {
    surface.failure_reason =
        "lowering/runtime performance quality guardrails are not ready";
  } else if (!cross_lane_integration_consistent) {
    surface.failure_reason = "lowering/runtime cross-lane integration is inconsistent";
  } else if (!cross_lane_integration_ready) {
    surface.failure_reason = "lowering/runtime cross-lane integration is not ready";
  } else if (!integration_closeout_consistent) {
    surface.failure_reason = "lowering/runtime integration closeout is inconsistent";
  } else if (!gate_signoff_ready) {
    surface.failure_reason = "lowering/runtime gate sign-off is not ready";
  } else if (!diagnostics_hardening_expansion_ready) {
    surface.failure_reason =
        "lowering/runtime core feature expansion is not ready";
  } else if (!recovery_determinism_expansion_ready) {
    surface.failure_reason =
        "lowering/runtime recovery determinism expansion is not ready";
  } else if (!conformance_matrix_expansion_ready) {
    surface.failure_reason =
        "lowering/runtime conformance matrix expansion is not ready";
  } else if (!conformance_corpus_expansion_ready) {
    surface.failure_reason =
        "lowering/runtime conformance corpus expansion is not ready";
  } else if (!performance_quality_guardrails_expansion_ready) {
    surface.failure_reason =
        "lowering/runtime performance quality guardrails expansion is not ready";
  } else if (!cross_lane_integration_expansion_ready) {
    surface.failure_reason =
        "lowering/runtime cross-lane integration expansion is not ready";
  } else if (!integration_closeout_expansion_ready) {
    surface.failure_reason =
        "lowering/runtime integration closeout expansion is not ready";
  } else {
    surface.failure_reason =
        "lowering/runtime core feature implementation is not ready";
  }

  return surface;
}

inline bool IsObjc3LoweringRuntimeStabilityCoreFeatureImplementationSurfaceReady(
    const Objc3LoweringRuntimeStabilityCoreFeatureImplementationSurface &surface,
    std::string &reason) {
  if (surface.core_feature_impl_ready) {
    reason.clear();
    return true;
  }
  reason = surface.failure_reason.empty()
               ? "lowering/runtime core feature implementation is not ready"
               : surface.failure_reason;
  return false;
}
