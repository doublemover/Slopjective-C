#pragma once

#include <string>

#include "pipeline/objc3_frontend_types.h"
#include "pipeline/objc3_lowering_runtime_stability_keys.h"
#include "pipeline/objc3_lowering_runtime_stability_surface_builder_facts.h"

inline void PublishObjc3LoweringRuntimeStabilitySurfaceReadiness(
    Objc3LoweringRuntimeStabilityCoreFeatureImplementationSurface &surface,
    const Objc3LoweringRuntimeStabilityReadinessFacts &facts) {
  surface.typed_expansion_accounting_consistent =
      facts.typed_expansion_accounting_consistent;
  surface.parse_conformance_accounting_consistent =
      facts.parse_conformance_accounting_consistent;
  surface.replay_keys_ready = facts.replay_keys_ready;
  surface.edge_case_compatibility_ready =
      facts.edge_case_compatibility_ready;
  surface.edge_case_expansion_consistent =
      facts.edge_case_expansion_consistent;
  surface.edge_case_robustness_ready = facts.edge_case_robustness_ready;
  surface.diagnostics_hardening_consistent =
      facts.diagnostics_hardening_consistent;
  surface.diagnostics_hardening_ready = facts.diagnostics_hardening_ready;
  surface.recovery_determinism_consistent =
      facts.recovery_determinism_consistent;
  surface.recovery_determinism_ready = facts.recovery_determinism_ready;
  surface.conformance_matrix_consistent =
      facts.conformance_matrix_consistent;
  surface.conformance_matrix_ready = facts.conformance_matrix_ready;
  surface.conformance_corpus_consistent =
      facts.conformance_corpus_consistent;
  surface.conformance_corpus_ready = facts.conformance_corpus_ready;
  surface.performance_quality_guardrails_consistent =
      facts.performance_quality_guardrails_consistent;
  surface.performance_quality_guardrails_ready =
      facts.performance_quality_guardrails_ready;
  surface.cross_lane_integration_consistent =
      facts.cross_lane_integration_consistent;
  surface.cross_lane_integration_ready = facts.cross_lane_integration_ready;
  surface.cross_lane_integration_key =
      BuildObjc3LoweringRuntimeCrossLaneIntegrationKey(
          surface,
          facts.cross_lane_integration_consistent,
          facts.cross_lane_integration_ready);
  surface.integration_closeout_consistent =
      facts.integration_closeout_consistent;
  surface.gate_signoff_ready = facts.gate_signoff_ready;
  surface.integration_closeout_key =
      BuildObjc3LoweringRuntimeIntegrationCloseoutKey(
          surface,
          facts.integration_closeout_consistent,
          facts.gate_signoff_ready);
  surface.expansion_ready = facts.expansion_ready;
  surface.expansion_ready = facts.recovery_determinism_expansion_ready;
  surface.expansion_ready = facts.conformance_matrix_expansion_ready;
  surface.expansion_ready = facts.conformance_corpus_expansion_ready;
  surface.expansion_ready =
      facts.performance_quality_guardrails_expansion_ready;
  surface.expansion_ready = facts.cross_lane_integration_expansion_ready;
  surface.expansion_ready = facts.integration_closeout_expansion_ready;

  surface.core_feature_impl_ready =
      surface.lowering_boundary_ready &&
      surface.runtime_dispatch_contract_consistent &&
      surface.typed_handoff_key_deterministic &&
      surface.typed_core_feature_consistent &&
      surface.parse_ready_for_lowering &&
      surface.invariant_proofs_ready &&
      surface.modular_split_ready &&
      facts.typed_case_accounting_consistent &&
      facts.typed_expansion_case_accounting_consistent &&
      facts.parse_matrix_case_count_ready &&
      facts.parse_corpus_case_accounting_consistent &&
      facts.parse_guardrails_case_accounting_consistent &&
      facts.replay_keys_ready;
  surface.core_feature_impl_ready =
      surface.core_feature_impl_ready && surface.expansion_ready;
  surface.core_feature_key =
      BuildObjc3LoweringRuntimeStabilityCoreFeatureImplementationKey(surface);
}

inline void PublishObjc3LoweringRuntimeStabilitySurfaceKeys(
    Objc3LoweringRuntimeStabilityCoreFeatureImplementationSurface &surface,
    const Objc3ParseLoweringReadinessSurface &parse_surface,
    const Objc3LoweringRuntimeStabilityReadinessFacts &facts) {
  surface.expansion_key =
      "lowering-runtime-stability-core-feature-expansion:v1:"
      "typed-expansion-accounting-consistent=" +
      std::string(facts.typed_expansion_accounting_consistent ? "true" : "false") +
      ";parse-conformance-accounting-consistent=" +
      std::string(facts.parse_conformance_accounting_consistent ? "true" : "false") +
      ";replay-keys-ready=" +
      std::string(facts.replay_keys_ready ? "true" : "false") +
      ";edge-compat-ready=" +
      std::string(facts.edge_case_compatibility_ready ? "true" : "false") +
      ";edge-expansion-consistent=" +
      std::string(facts.edge_case_expansion_consistent ? "true" : "false") +
      ";edge-robustness-ready=" +
      std::string(facts.edge_case_robustness_ready ? "true" : "false") +
      ";edge-robustness-key-ready=" +
      std::string(!surface.edge_case_robustness_key.empty() ? "true" : "false") +
      ";diag-hardening-consistent=" +
      std::string(facts.diagnostics_hardening_consistent ? "true" : "false") +
      ";diag-hardening-ready=" +
      std::string(facts.diagnostics_hardening_ready ? "true" : "false") +
      ";diag-hardening-key-ready=" +
      std::string(!surface.diagnostics_hardening_key.empty() ? "true" : "false") +
      ";diag-hardening-expansion-ready=" +
      std::string(facts.diagnostics_hardening_expansion_ready ? "true" : "false") +
      ";recovery-determinism-consistent=" +
      std::string(facts.recovery_determinism_consistent ? "true" : "false") +
      ";recovery-determinism-ready=" +
      std::string(facts.recovery_determinism_ready ? "true" : "false") +
      ";recovery-determinism-key-ready=" +
      std::string(!surface.recovery_determinism_key.empty() ? "true" : "false") +
      ";recovery-determinism-expansion-ready=" +
      std::string(facts.recovery_determinism_expansion_ready ? "true" : "false") +
      ";conformance-matrix-consistent=" +
      std::string(facts.conformance_matrix_consistent ? "true" : "false") +
      ";conformance-matrix-ready=" +
      std::string(facts.conformance_matrix_ready ? "true" : "false") +
      ";conformance-matrix-key-ready=" +
      std::string(!surface.conformance_matrix_key.empty() ? "true" : "false") +
      ";conformance-matrix-expansion-ready=" +
      std::string(facts.conformance_matrix_expansion_ready ? "true" : "false") +
      ";conformance-corpus-consistent=" +
      std::string(facts.conformance_corpus_consistent ? "true" : "false") +
      ";conformance-corpus-ready=" +
      std::string(facts.conformance_corpus_ready ? "true" : "false") +
      ";conformance-corpus-key-ready=" +
      std::string(!surface.conformance_corpus_key.empty() ? "true" : "false") +
      ";conformance-corpus-expansion-ready=" +
      std::string(facts.conformance_corpus_expansion_ready ? "true" : "false") +
      ";performance-quality-guardrails-consistent=" +
      std::string(facts.performance_quality_guardrails_consistent ? "true" : "false") +
      ";performance-quality-guardrails-ready=" +
      std::string(facts.performance_quality_guardrails_ready ? "true" : "false") +
      ";performance-quality-guardrails-key-ready=" +
      std::string(!surface.performance_quality_guardrails_key.empty() ? "true" : "false") +
      ";performance-quality-guardrails-expansion-ready=" +
      std::string(facts.performance_quality_guardrails_expansion_ready ? "true" : "false") +
      ";cross-lane-integration-consistent=" +
      std::string(facts.cross_lane_integration_consistent ? "true" : "false") +
      ";cross-lane-integration-ready=" +
      std::string(facts.cross_lane_integration_ready ? "true" : "false") +
      ";cross-lane-integration-key-ready=" +
      std::string(!surface.cross_lane_integration_key.empty() ? "true" : "false") +
      ";cross-lane-integration-expansion-ready=" +
      std::string(facts.cross_lane_integration_expansion_ready ? "true" : "false") +
      ";integration-closeout-consistent=" +
      std::string(facts.integration_closeout_consistent ? "true" : "false") +
      ";gate-signoff-ready=" +
      std::string(facts.gate_signoff_ready ? "true" : "false") +
      ";integration-closeout-key-ready=" +
      std::string(!surface.integration_closeout_key.empty() ? "true" : "false") +
      ";integration-closeout-expansion-ready=" +
      std::string(facts.integration_closeout_expansion_ready ? "true" : "false") +
      ";compat-handoff-consistent=" +
      std::string(parse_surface.compatibility_handoff_consistent ? "true" : "false") +
      ";parser-diagnostic-surface-consistent=" +
      std::string(parse_surface.parser_diagnostic_surface_consistent ? "true" : "false") +
      ";semantic-diagnostics-deterministic=" +
      std::string(parse_surface.semantic_diagnostics_deterministic ? "true" : "false") +
      ";parse-edge-robustness-consistent=" +
      std::string(parse_surface.parse_artifact_edge_case_robustness_consistent ? "true" : "false") +
      ";expansion-ready=" +
      std::string(surface.expansion_ready ? "true" : "false");
  surface.edge_case_compatibility_key =
      "lowering-runtime-edge-compatibility:v1:compatibility-handoff-consistent=" +
      std::string(parse_surface.compatibility_handoff_consistent ? "true" : "false") +
      ";pragma-coordinate-order-consistent=" +
      std::string(parse_surface.language_version_pragma_coordinate_order_consistent ? "true" : "false") +
      ";parse-edge-robustness-consistent=" +
      std::string(parse_surface.parse_artifact_edge_case_robustness_consistent ? "true" : "false") +
      ";edge-compat-ready=" +
      std::string(facts.edge_case_compatibility_ready ? "true" : "false");
}
