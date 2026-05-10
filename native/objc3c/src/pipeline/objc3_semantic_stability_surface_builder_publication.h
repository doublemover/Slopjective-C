#pragma once

#include <string>

#include "pipeline/objc3_semantic_stability_core_feature_implementation_surface_keys.h"
#include "pipeline/objc3_semantic_stability_surface_builder_facts.h"

inline void PublishObjc3SemanticStabilitySurfaceReadiness(
    Objc3SemanticStabilityCoreFeatureImplementationSurface &surface,
    Objc3SemanticStabilityReadinessFacts &facts) {
  surface.typed_core_feature_expansion_accounting_consistent =
      facts.typed_core_feature_expansion_accounting_consistent;
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
  surface.integration_closeout_consistent =
      facts.integration_closeout_consistent;
  surface.gate_signoff_ready = facts.gate_signoff_ready;
  surface.integration_closeout_key =
      BuildObjc3SemanticStabilityIntegrationCloseoutKey(
          surface,
          facts.integration_closeout_consistent,
          facts.gate_signoff_ready);
  surface.expansion_ready = facts.diagnostics_hardening_expansion_ready;
  surface.expansion_ready = facts.recovery_determinism_expansion_ready;
  surface.expansion_ready = facts.conformance_matrix_expansion_ready;
  surface.expansion_ready = facts.conformance_corpus_expansion_ready;
  surface.expansion_ready =
      facts.performance_quality_guardrails_expansion_ready;
  facts.integration_closeout_expansion_ready =
      facts.performance_quality_guardrails_expansion_ready &&
      facts.gate_signoff_ready &&
      !surface.integration_closeout_key.empty();
  surface.expansion_ready = facts.integration_closeout_expansion_ready;

  surface.core_feature_impl_ready =
      surface.semantic_handoff_deterministic &&
      surface.spec_delta_closed &&
      surface.modular_split_ready &&
      surface.expansion_ready;
  surface.core_feature_key =
      BuildObjc3SemanticStabilityCoreFeatureImplementationKey(surface);
}

inline void PublishObjc3SemanticStabilitySurfaceKeys(
    Objc3SemanticStabilityCoreFeatureImplementationSurface &surface,
    const Objc3ParseLoweringReadinessSurface &parse_surface,
    const Objc3SemanticStabilityReadinessFacts &facts) {
  surface.expansion_key =
      "semantic-stability-core-feature-expansion:v1:typed-expansion-consistent=" +
      std::string(surface.typed_core_feature_expansion_accounting_consistent
                      ? "true"
                      : "false") +
      ";parse-accounting-consistent=" +
      std::string(surface.parse_conformance_accounting_consistent
                      ? "true"
                      : "false") +
      ";replay-keys-ready=" +
      std::string(surface.replay_keys_ready ? "true" : "false") +
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
      std::string(facts.performance_quality_guardrails_consistent
                      ? "true"
                      : "false") +
      ";performance-quality-guardrails-ready=" +
      std::string(facts.performance_quality_guardrails_ready ? "true" : "false") +
      ";performance-quality-guardrails-key-ready=" +
      std::string(!surface.performance_quality_guardrails_key.empty()
                      ? "true"
                      : "false") +
      ";performance-quality-guardrails-expansion-ready=" +
      std::string(facts.performance_quality_guardrails_expansion_ready
                      ? "true"
                      : "false") +
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
      std::string(parse_surface.parser_diagnostic_surface_consistent
                      ? "true"
                      : "false") +
      ";semantic-diagnostics-deterministic=" +
      std::string(parse_surface.semantic_diagnostics_deterministic
                      ? "true"
                      : "false") +
      ";parser-recovery-replay-ready=" +
      std::string(parse_surface.parser_recovery_replay_ready ? "true" : "false") +
      ";parse-edge-robustness-consistent=" +
      std::string(parse_surface.parse_artifact_edge_case_robustness_consistent
                      ? "true"
                      : "false");
}
