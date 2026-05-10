#pragma once

#include <sstream>
#include <string>

#include "pipeline/objc3_frontend_types.h"

inline std::string BuildObjc3SemanticStabilityCoreFeatureImplementationKey(
    const Objc3SemanticStabilityCoreFeatureImplementationSurface &surface) {
  std::ostringstream key;
  key << "semantic-stability-core-feature-impl:v1:"
      << "typed_core_feature_case_count=" << surface.typed_core_feature_case_count
      << ";typed_core_feature_passed_case_count=" << surface.typed_core_feature_passed_case_count
      << ";typed_core_feature_failed_case_count=" << surface.typed_core_feature_failed_case_count
      << ";typed_core_feature_expansion_case_count=" << surface.typed_core_feature_expansion_case_count
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
      << ";semantic_handoff_deterministic="
      << (surface.semantic_handoff_deterministic ? "true" : "false")
      << ";spec_delta_closed=" << (surface.spec_delta_closed ? "true" : "false")
      << ";modular_split_ready=" << (surface.modular_split_ready ? "true" : "false")
      << ";typed_core_feature_expansion_accounting_consistent="
      << (surface.typed_core_feature_expansion_accounting_consistent ? "true" : "false")
      << ";parse_conformance_accounting_consistent="
      << (surface.parse_conformance_accounting_consistent ? "true" : "false")
      << ";replay_keys_ready=" << (surface.replay_keys_ready ? "true" : "false")
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
      << ";integration_closeout_consistent="
      << (surface.integration_closeout_consistent ? "true" : "false")
      << ";gate_signoff_ready="
      << (surface.gate_signoff_ready ? "true" : "false")
      << ";integration_closeout_key=" << surface.integration_closeout_key
      << ";expansion_ready=" << (surface.expansion_ready ? "true" : "false")
      << ";core_feature_impl_ready=" << (surface.core_feature_impl_ready ? "true" : "false");
  return key.str();
}

inline std::string BuildObjc3SemanticStabilityIntegrationCloseoutKey(
    const Objc3SemanticStabilityCoreFeatureImplementationSurface &surface,
    bool integration_closeout_consistent,
    bool gate_signoff_ready) {
  std::ostringstream key;
  key << "semantic-stability-integration-closeout:v1:"
      << "typed-parse-core-consistent="
      << (surface.typed_core_feature_consistent &&
                  surface.typed_core_feature_expansion_consistent &&
                  surface.typed_sema_core_feature_consistent &&
                  surface.typed_sema_core_feature_expansion_consistent
              ? "true"
              : "false")
      << ";parse-conformance-accounting-consistent="
      << (surface.parse_conformance_accounting_consistent ? "true" : "false")
      << ";replay-keys-ready=" << (surface.replay_keys_ready ? "true" : "false")
      << ";performance-quality-guardrails-ready="
      << (surface.performance_quality_guardrails_ready ? "true" : "false")
      << ";integration-closeout-consistent="
      << (integration_closeout_consistent ? "true" : "false")
      << ";gate-signoff-ready=" << (gate_signoff_ready ? "true" : "false");
  return key.str();
}
