#include "pipeline/readiness/objc3_parse_lowering_failure_reason_readiness_private.h"

namespace objc3_parse_lowering_failure_reason_readiness_detail {

const char *FindParseArtifactDiagnosticHandoffFailureReason(
    const Objc3ParseLoweringReadinessSurface &surface) {
  if (!surface.long_tail_grammar_core_feature_consistent) {
    return "long-tail grammar core feature is inconsistent";
  }

  if (!surface.long_tail_grammar_handoff_key_deterministic) {
    return "long-tail grammar handoff key is not deterministic";
  }

  if (!surface.long_tail_grammar_expansion_accounting_consistent) {
    return "long-tail grammar expansion accounting is inconsistent";
  }

  if (!surface.parse_artifact_handoff_consistent) {
    return "parse artifact handoff is inconsistent";
  }

  if (!surface.parser_diagnostic_surface_consistent) {
    return "parser diagnostics surface is inconsistent";
  }

  if (!surface.parser_diagnostic_source_precision_scaffold_ready) {
    return "parser diagnostic source-precision scaffold is not ready";
  }

  if (!surface.parser_diagnostic_grammar_hooks_core_feature_ready) {
    return "parser diagnostic grammar hooks core feature is not ready";
  }

  if (!surface.parser_diagnostic_grammar_hooks_core_feature_expansion_accounting_consistent) {
    return "parser diagnostic grammar hooks core feature expansion accounting is inconsistent";
  }

  if (!surface.parser_diagnostic_grammar_hooks_core_feature_expansion_replay_keys_ready) {
    return "parser diagnostic grammar hooks core feature expansion replay keys are not ready";
  }

  if (!surface.parser_diagnostic_grammar_hooks_core_feature_expansion_ready) {
    return "parser diagnostic grammar hooks core feature expansion is not ready";
  }

  if (!surface.parser_diagnostic_code_surface_deterministic) {
    return "parser diagnostic code surface is not deterministic";
  }

  if (!surface.parse_artifact_handoff_deterministic) {
    return "parse artifact handoff is not deterministic";
  }

  if (!surface.parse_artifact_layout_fingerprint_consistent) {
    return "parse artifact layout fingerprint is inconsistent";
  }

  if (!surface.parse_artifact_fingerprint_consistent) {
    return "parse artifact fingerprint is inconsistent";
  }

  if (!surface.compatibility_handoff_consistent) {
    return "compatibility handoff is inconsistent";
  }

  if (!surface.long_tail_grammar_compatibility_handoff_ready) {
    return "long-tail grammar compatibility handoff is not ready";
  }

  if (!surface.parse_artifact_replay_key_deterministic) {
    return "parse artifact replay key is not deterministic";
  }

  if (!surface.long_tail_grammar_replay_keys_ready) {
    return "long-tail grammar replay keys are not ready";
  }

  if (!surface.long_tail_grammar_expansion_ready) {
    return "long-tail grammar core feature expansion is not ready";
  }

  if (!surface.parse_artifact_diagnostics_hardening_consistent) {
    return "parse artifact diagnostics hardening is inconsistent";
  }

  if (!surface.parser_token_count_budget_consistent) {
    return "parser token count budget is inconsistent";
  }

  if (!surface.language_version_pragma_coordinate_order_consistent) {
    return "language-version pragma coordinate order is inconsistent";
  }

  return nullptr;
}

const char *FindParserDiagnosticGrammarHardeningFailureReason(
    const Objc3ParseLoweringReadinessSurface &surface) {
  if (!surface.parser_diagnostic_grammar_hooks_edge_case_compatibility_consistent) {
    return "parser diagnostic grammar hooks edge-case compatibility is inconsistent";
  }

  if (!surface.parser_diagnostic_grammar_hooks_edge_case_compatibility_ready) {
    return "parser diagnostic grammar hooks edge-case compatibility is not ready";
  }

  if (!surface.parser_diagnostic_grammar_hooks_edge_case_expansion_consistent) {
    return "parser diagnostic grammar hooks edge-case expansion is inconsistent";
  }

  if (!surface.parser_diagnostic_grammar_hooks_edge_case_robustness_ready) {
    return "parser diagnostic grammar hooks edge-case robustness is not ready";
  }

  if (!surface.parser_diagnostic_grammar_hooks_diagnostics_hardening_consistent) {
    return "parser diagnostic grammar hooks diagnostics hardening is inconsistent";
  }

  if (!surface.parser_diagnostic_grammar_hooks_diagnostics_hardening_ready) {
    return "parser diagnostic grammar hooks diagnostics hardening is not ready";
  }

  if (!surface.parser_diagnostic_grammar_hooks_recovery_determinism_consistent) {
    return "parser diagnostic grammar hooks recovery/determinism hardening is inconsistent";
  }

  if (!surface.parser_diagnostic_grammar_hooks_recovery_determinism_ready) {
    return "parser diagnostic grammar hooks recovery/determinism hardening is not ready";
  }

  if (!surface.parser_diagnostic_grammar_hooks_conformance_matrix_consistent) {
    return "parser diagnostic grammar hooks conformance matrix is inconsistent";
  }

  if (!surface.parser_diagnostic_grammar_hooks_conformance_matrix_ready) {
    return "parser diagnostic grammar hooks conformance matrix is not ready";
  }

  if (!surface.parser_diagnostic_grammar_hooks_conformance_corpus_consistent) {
    return "parser diagnostic grammar hooks conformance corpus is inconsistent";
  }

  if (!surface.parser_diagnostic_grammar_hooks_conformance_corpus_ready) {
    return "parser diagnostic grammar hooks conformance corpus is not ready";
  }

  if (!surface.parse_artifact_edge_case_robustness_consistent) {
    return "parse artifact edge-case robustness is inconsistent";
  }

  if (!surface.long_tail_grammar_edge_case_compatibility_consistent) {
    return "long-tail grammar edge-case compatibility is inconsistent";
  }

  if (!surface.long_tail_grammar_edge_case_compatibility_ready) {
    return "long-tail grammar edge-case compatibility is not ready";
  }

  if (!surface.long_tail_grammar_edge_case_expansion_consistent) {
    return "long-tail grammar edge-case expansion is inconsistent";
  }

  if (!surface.long_tail_grammar_edge_case_robustness_ready) {
    return "long-tail grammar edge-case robustness is not ready";
  }

  if (!surface.long_tail_grammar_diagnostics_hardening_consistent) {
    return "long-tail grammar diagnostics hardening is inconsistent";
  }

  if (!surface.long_tail_grammar_diagnostics_hardening_ready) {
    return "long-tail grammar diagnostics hardening is not ready";
  }

  return nullptr;
}

}  // namespace objc3_parse_lowering_failure_reason_readiness_detail
