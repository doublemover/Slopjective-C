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

inline Objc3SemanticStabilityCoreFeatureImplementationSurface
BuildObjc3SemanticStabilityCoreFeatureImplementationSurface(
    const Objc3TypedSemaToLoweringContractSurface &typed_surface,
    const Objc3ParseLoweringReadinessSurface &parse_surface,
    const Objc3SemanticStabilitySpecDeltaClosureScaffold &scaffold) {
  Objc3SemanticStabilityCoreFeatureImplementationSurface surface;
  surface.semantic_handoff_deterministic = scaffold.semantic_handoff_deterministic;
  surface.typed_core_feature_consistent = typed_surface.typed_core_feature_consistent;
  surface.typed_core_feature_expansion_consistent =
      typed_surface.typed_core_feature_expansion_consistent;
  surface.typed_sema_core_feature_consistent = parse_surface.typed_sema_core_feature_consistent;
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

  const bool typed_core_feature_case_accounting_consistent =
      surface.typed_core_feature_case_count > 0 &&
      surface.typed_core_feature_passed_case_count <=
          surface.typed_core_feature_case_count &&
      surface.typed_core_feature_failed_case_count ==
          (surface.typed_core_feature_case_count -
           surface.typed_core_feature_passed_case_count);
  const bool typed_core_feature_expansion_case_accounting_consistent =
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
  const bool typed_parse_core_feature_consistent =
      surface.typed_core_feature_consistent &&
      surface.typed_core_feature_expansion_consistent &&
      surface.typed_sema_core_feature_consistent &&
      surface.typed_sema_core_feature_expansion_consistent;
  const bool parse_conformance_consistent =
      surface.parse_lowering_conformance_matrix_consistent &&
      surface.parse_lowering_conformance_corpus_consistent &&
      surface.parse_lowering_performance_quality_guardrails_consistent;
  const bool typed_core_feature_expansion_accounting_consistent =
      typed_core_feature_case_accounting_consistent &&
      typed_core_feature_expansion_case_accounting_consistent;
  const bool parse_conformance_accounting_consistent =
      parse_matrix_case_count_ready &&
      parse_corpus_case_accounting_consistent &&
      parse_guardrails_case_accounting_consistent;
  const bool replay_keys_ready =
      !surface.typed_handoff_key.empty() && !surface.parse_artifact_replay_key.empty();
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
  const bool integration_closeout_consistent =
      performance_quality_guardrails_consistent &&
      typed_parse_core_feature_consistent &&
      parse_conformance_accounting_consistent &&
      replay_keys_ready;
  const bool gate_signoff_ready =
      integration_closeout_consistent &&
      performance_quality_guardrails_ready &&
      !surface.performance_quality_guardrails_key.empty();
  const bool edge_case_compatibility_expansion_ready =
      typed_parse_core_feature_consistent &&
      parse_conformance_consistent &&
      typed_core_feature_expansion_accounting_consistent &&
      parse_conformance_accounting_consistent &&
      replay_keys_ready &&
      edge_case_compatibility_ready;
  const bool expansion_ready =
      edge_case_compatibility_expansion_ready &&
      edge_case_robustness_ready;
  const bool diagnostics_hardening_expansion_ready =
      expansion_ready && diagnostics_hardening_ready;
  surface.typed_core_feature_expansion_accounting_consistent =
      typed_core_feature_expansion_accounting_consistent;
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
  surface.integration_closeout_consistent = integration_closeout_consistent;
  surface.gate_signoff_ready = gate_signoff_ready;
  surface.integration_closeout_key =
      BuildObjc3SemanticStabilityIntegrationCloseoutKey(
          surface, integration_closeout_consistent, gate_signoff_ready);
  surface.expansion_ready = diagnostics_hardening_expansion_ready;
  const bool recovery_determinism_expansion_ready =
      surface.expansion_ready && recovery_determinism_ready;
  surface.expansion_ready = recovery_determinism_expansion_ready;
  const bool conformance_matrix_expansion_ready =
      recovery_determinism_expansion_ready && conformance_matrix_ready;
  surface.expansion_ready = conformance_matrix_expansion_ready;
  const bool conformance_corpus_expansion_ready =
      conformance_matrix_expansion_ready && conformance_corpus_ready;
  surface.expansion_ready = conformance_corpus_expansion_ready;
  const bool performance_quality_guardrails_expansion_ready =
      conformance_corpus_expansion_ready &&
      performance_quality_guardrails_ready;
  surface.expansion_ready = performance_quality_guardrails_expansion_ready;
  const bool integration_closeout_expansion_ready =
      performance_quality_guardrails_expansion_ready &&
      gate_signoff_ready &&
      !surface.integration_closeout_key.empty();
  surface.expansion_ready = integration_closeout_expansion_ready;

  surface.core_feature_impl_ready =
      surface.semantic_handoff_deterministic &&
      surface.spec_delta_closed &&
      surface.modular_split_ready &&
      surface.expansion_ready;
  surface.core_feature_key =
      BuildObjc3SemanticStabilityCoreFeatureImplementationKey(surface);
  surface.expansion_key =
      "semantic-stability-core-feature-expansion:v1:typed-expansion-consistent=" +
      std::string(surface.typed_core_feature_expansion_accounting_consistent ? "true" : "false") +
      ";parse-accounting-consistent=" +
      std::string(surface.parse_conformance_accounting_consistent ? "true" : "false") +
      ";replay-keys-ready=" +
      std::string(surface.replay_keys_ready ? "true" : "false") +
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
      ";integration-closeout-consistent=" +
      std::string(integration_closeout_consistent ? "true" : "false") +
      ";gate-signoff-ready=" +
      std::string(gate_signoff_ready ? "true" : "false") +
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
      ";parser-recovery-replay-ready=" +
      std::string(parse_surface.parser_recovery_replay_ready ? "true" : "false") +
      ";parse-edge-robustness-consistent=" +
      std::string(parse_surface.parse_artifact_edge_case_robustness_consistent ? "true" : "false");

  if (surface.core_feature_impl_ready) {
    return surface;
  }

  if (!surface.semantic_handoff_deterministic) {
    surface.failure_reason = "semantic handoff is not deterministic";
  } else if (!surface.spec_delta_closed) {
    surface.failure_reason = "semantic stability spec delta is not closed";
  } else if (!surface.modular_split_ready) {
    surface.failure_reason = "semantic stability modular split scaffold is not ready";
  } else if (!typed_parse_core_feature_consistent) {
    surface.failure_reason = "typed/parse core feature consistency is incomplete";
  } else if (!parse_conformance_consistent) {
    surface.failure_reason = "parse conformance consistency is incomplete";
  } else if (!typed_core_feature_expansion_accounting_consistent) {
    surface.failure_reason =
        "typed core feature expansion case accounting is inconsistent";
  } else if (!parse_conformance_accounting_consistent) {
    surface.failure_reason = "parse guardrails case accounting is inconsistent";
  } else if (!replay_keys_ready) {
    surface.failure_reason = "typed/parse replay keys are not ready";
  } else if (!edge_case_compatibility_ready) {
    surface.failure_reason = "semantic stability edge-case compatibility is not ready";
  } else if (!edge_case_expansion_consistent) {
    surface.failure_reason = "semantic stability edge-case expansion is inconsistent";
  } else if (!edge_case_robustness_ready) {
    surface.failure_reason = "semantic stability edge-case robustness is not ready";
  } else if (!diagnostics_hardening_consistent) {
    surface.failure_reason = "semantic stability diagnostics hardening is inconsistent";
  } else if (!diagnostics_hardening_ready) {
    surface.failure_reason = "semantic stability diagnostics hardening is not ready";
  } else if (!recovery_determinism_consistent) {
    surface.failure_reason = "semantic stability recovery determinism is inconsistent";
  } else if (!recovery_determinism_ready) {
    surface.failure_reason = "semantic stability recovery determinism is not ready";
  } else if (!conformance_matrix_consistent) {
    surface.failure_reason = "semantic stability conformance matrix is inconsistent";
  } else if (!conformance_matrix_ready) {
    surface.failure_reason = "semantic stability conformance matrix is not ready";
  } else if (!conformance_corpus_consistent) {
    surface.failure_reason = "semantic stability conformance corpus is inconsistent";
  } else if (!conformance_corpus_ready) {
    surface.failure_reason = "semantic stability conformance corpus is not ready";
  } else if (!performance_quality_guardrails_consistent) {
    surface.failure_reason =
        "semantic stability performance quality guardrails are inconsistent";
  } else if (!performance_quality_guardrails_ready) {
    surface.failure_reason =
        "semantic stability performance quality guardrails are not ready";
  } else if (!integration_closeout_consistent) {
    surface.failure_reason = "semantic stability integration closeout is inconsistent";
  } else if (!gate_signoff_ready) {
    surface.failure_reason = "semantic stability gate sign-off is not ready";
  } else if (!diagnostics_hardening_expansion_ready) {
    surface.failure_reason = "semantic stability core feature expansion is not ready";
  } else if (!recovery_determinism_expansion_ready) {
    surface.failure_reason = "semantic stability recovery determinism expansion is not ready";
  } else if (!conformance_matrix_expansion_ready) {
    surface.failure_reason = "semantic stability conformance matrix expansion is not ready";
  } else if (!conformance_corpus_expansion_ready) {
    surface.failure_reason = "semantic stability conformance corpus expansion is not ready";
  } else if (!performance_quality_guardrails_expansion_ready) {
    surface.failure_reason =
        "semantic stability performance quality guardrails expansion is not ready";
  } else if (!integration_closeout_expansion_ready) {
    surface.failure_reason =
        "semantic stability integration closeout expansion is not ready";
  } else {
    surface.failure_reason = "semantic stability core feature implementation is not ready";
  }

  return surface;
}

inline bool IsObjc3SemanticStabilityCoreFeatureImplementationSurfaceReady(
    const Objc3SemanticStabilityCoreFeatureImplementationSurface &surface,
    std::string &reason) {
  if (surface.core_feature_impl_ready) {
    reason.clear();
    return true;
  }
  reason = surface.failure_reason.empty()
               ? "semantic stability core feature implementation is not ready"
               : surface.failure_reason;
  return false;
}
