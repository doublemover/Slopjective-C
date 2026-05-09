#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

struct Objc3ParseLoweringDiagnosticCodeCoverage {
  std::size_t unique_code_count = 0;
  std::uint64_t unique_code_fingerprint = 1469598103934665603ull;
  bool deterministic_surface = true;
};

inline std::string TryExtractObjc3ParseLoweringDiagnosticCode(
    const std::string &diag_text,
    bool &ok) {
  ok = false;
  const std::size_t end = diag_text.size();
  if (end < 3u || diag_text[end - 1] != ']') {
    return std::string{};
  }
  const std::size_t begin = diag_text.rfind('[');
  if (begin == std::string::npos || begin + 2u >= end) {
    return std::string{};
  }
  const std::string code = diag_text.substr(begin + 1u, end - begin - 2u);
  if (code.empty()) {
    return std::string{};
  }
  ok = true;
  return code;
}

inline std::uint64_t MixObjc3ParseLoweringDiagnosticCodeFingerprint(
    std::uint64_t fingerprint,
    const std::string &code) {
  constexpr std::uint64_t kFnvPrime = 1099511628211ull;
  fingerprint = (fingerprint ^ static_cast<std::uint64_t>(code.size())) * kFnvPrime;
  for (const unsigned char c : code) {
    fingerprint = (fingerprint ^ static_cast<std::uint64_t>(c)) * kFnvPrime;
  }
  return fingerprint;
}

inline Objc3ParseLoweringDiagnosticCodeCoverage BuildObjc3ParseLoweringDiagnosticCodeCoverage(
    const std::vector<std::string> &parser_diagnostics) {
  Objc3ParseLoweringDiagnosticCodeCoverage coverage;
  std::vector<std::string> sorted_codes;
  sorted_codes.reserve(parser_diagnostics.size());
  for (const auto &diag_text : parser_diagnostics) {
    bool code_ok = false;
    const std::string code = TryExtractObjc3ParseLoweringDiagnosticCode(diag_text, code_ok);
    if (!code_ok) {
      coverage.deterministic_surface = false;
      continue;
    }
    sorted_codes.push_back(code);
  }
  std::sort(sorted_codes.begin(), sorted_codes.end());
  sorted_codes.erase(std::unique(sorted_codes.begin(), sorted_codes.end()), sorted_codes.end());
  coverage.unique_code_count = sorted_codes.size();
  for (const auto &code : sorted_codes) {
    coverage.unique_code_fingerprint =
        MixObjc3ParseLoweringDiagnosticCodeFingerprint(coverage.unique_code_fingerprint, code);
  }
  return coverage;
}

inline std::string BuildObjc3ParseArtifactDiagnosticsHardeningKey(
    std::size_t parser_diagnostic_count,
    std::size_t parser_snapshot_diagnostic_count,
    std::size_t parser_diagnostic_code_count,
    std::uint64_t parser_diagnostic_code_fingerprint,
    bool parser_diagnostic_surface_consistent,
    bool parser_diagnostic_code_surface_deterministic,
    bool parser_diagnostic_source_precision_scaffold_consistent,
    bool parser_diagnostic_grammar_hooks_core_feature_consistent,
    bool parser_diagnostic_grammar_hooks_core_feature_expansion_accounting_consistent,
    bool parser_diagnostic_grammar_hooks_core_feature_expansion_replay_keys_ready,
    bool parser_diagnostic_grammar_hooks_core_feature_expansion_ready,
    const std::string &parser_diagnostic_source_precision_scaffold_key,
    const std::string &parser_diagnostic_grammar_hooks_core_feature_key,
    const std::string &parser_diagnostic_grammar_hooks_core_feature_expansion_key,
    bool parse_artifact_diagnostics_hardening_consistent) {
  return "parser_diagnostics=" + std::to_string(parser_diagnostic_count) +
         ";snapshot_parser_diagnostics=" + std::to_string(parser_snapshot_diagnostic_count) +
         ";diagnostic_code_count=" + std::to_string(parser_diagnostic_code_count) +
         ";diagnostic_code_fingerprint=" + std::to_string(parser_diagnostic_code_fingerprint) +
         ";diagnostic_surface_consistent=" + (parser_diagnostic_surface_consistent ? "true" : "false") +
         ";diagnostic_code_surface_deterministic=" +
         (parser_diagnostic_code_surface_deterministic ? "true" : "false") +
         ";source_precision_scaffold_consistent=" +
         (parser_diagnostic_source_precision_scaffold_consistent ? "true" : "false") +
         ";grammar_hooks_core_feature_consistent=" +
         (parser_diagnostic_grammar_hooks_core_feature_consistent ? "true" : "false") +
         ";grammar_hooks_core_feature_expansion_accounting_consistent=" +
         (parser_diagnostic_grammar_hooks_core_feature_expansion_accounting_consistent ? "true" : "false") +
         ";grammar_hooks_core_feature_expansion_replay_keys_ready=" +
         (parser_diagnostic_grammar_hooks_core_feature_expansion_replay_keys_ready ? "true" : "false") +
         ";grammar_hooks_core_feature_expansion_ready=" +
         (parser_diagnostic_grammar_hooks_core_feature_expansion_ready ? "true" : "false") +
         ";source_precision_scaffold_key=" + parser_diagnostic_source_precision_scaffold_key +
         ";grammar_hooks_core_feature_key=" + parser_diagnostic_grammar_hooks_core_feature_key +
         ";grammar_hooks_core_feature_expansion_key=" +
         parser_diagnostic_grammar_hooks_core_feature_expansion_key +
         ";consistent=" + (parse_artifact_diagnostics_hardening_consistent ? "true" : "false");
}

inline std::string BuildObjc3ParseArtifactEdgeRobustnessKey(
    std::size_t parser_token_count,
    std::size_t parser_snapshot_breakdown_count,
    std::size_t ast_top_level_declaration_count,
    bool parser_token_count_budget_consistent,
    bool language_version_pragma_coordinate_order_consistent,
    bool parse_artifact_edge_case_robustness_consistent) {
  return "parser_tokens=" + std::to_string(parser_token_count) +
         ";snapshot_breakdown=" + std::to_string(parser_snapshot_breakdown_count) +
         ";ast_top_level=" + std::to_string(ast_top_level_declaration_count) +
         ";token_budget_consistent=" + (parser_token_count_budget_consistent ? "true" : "false") +
         ";pragma_coordinate_order_consistent=" +
         (language_version_pragma_coordinate_order_consistent ? "true" : "false") +
         ";consistent=" + (parse_artifact_edge_case_robustness_consistent ? "true" : "false");
}

inline std::string BuildObjc3DiagnosticGrammarHooksEdgeCaseRobustnessKey(
    std::size_t parser_diagnostic_count,
    std::size_t parser_diagnostic_code_count,
    std::uint64_t parser_diagnostic_code_fingerprint,
    bool parser_diagnostic_grammar_hooks_edge_case_compatibility_consistent,
    bool parser_diagnostic_grammar_hooks_edge_case_compatibility_ready,
    bool parse_artifact_edge_case_robustness_consistent,
    bool parser_diagnostic_grammar_hooks_edge_case_expansion_consistent,
    bool parser_diagnostic_grammar_hooks_edge_case_robustness_ready) {
  return "parser_diagnostic_count=" + std::to_string(parser_diagnostic_count) +
         ";parser_diagnostic_code_count=" + std::to_string(parser_diagnostic_code_count) +
         ";parser_diagnostic_code_fingerprint=" + std::to_string(parser_diagnostic_code_fingerprint) +
         ";edge_case_compatibility_consistent=" +
         (parser_diagnostic_grammar_hooks_edge_case_compatibility_consistent ? "true" : "false") +
         ";edge_case_compatibility_ready=" +
         (parser_diagnostic_grammar_hooks_edge_case_compatibility_ready ? "true" : "false") +
         ";parse_artifact_edge_case_robustness_consistent=" +
         (parse_artifact_edge_case_robustness_consistent ? "true" : "false") +
         ";edge_case_expansion_consistent=" +
         (parser_diagnostic_grammar_hooks_edge_case_expansion_consistent ? "true" : "false") +
         ";edge_case_robustness_ready=" +
         (parser_diagnostic_grammar_hooks_edge_case_robustness_ready ? "true" : "false");
}

inline std::string BuildObjc3DiagnosticGrammarHooksDiagnosticsHardeningKey(
    std::size_t parser_diagnostic_count,
    std::size_t parser_diagnostic_code_count,
    std::uint64_t parser_diagnostic_code_fingerprint,
    bool parser_diagnostic_grammar_hooks_edge_case_robustness_ready,
    bool parse_artifact_diagnostics_hardening_consistent,
    bool parser_diagnostic_grammar_hooks_diagnostics_hardening_consistent,
    bool parser_diagnostic_grammar_hooks_diagnostics_hardening_ready) {
  return "parser_diagnostic_count=" + std::to_string(parser_diagnostic_count) +
         ";parser_diagnostic_code_count=" + std::to_string(parser_diagnostic_code_count) +
         ";parser_diagnostic_code_fingerprint=" + std::to_string(parser_diagnostic_code_fingerprint) +
         ";edge_case_robustness_ready=" +
         (parser_diagnostic_grammar_hooks_edge_case_robustness_ready ? "true" : "false") +
         ";parse_artifact_diagnostics_hardening_consistent=" +
         (parse_artifact_diagnostics_hardening_consistent ? "true" : "false") +
         ";diagnostics_hardening_consistent=" +
         (parser_diagnostic_grammar_hooks_diagnostics_hardening_consistent ? "true" : "false") +
         ";diagnostics_hardening_ready=" +
         (parser_diagnostic_grammar_hooks_diagnostics_hardening_ready ? "true" : "false");
}

inline std::string BuildObjc3DiagnosticGrammarHooksRecoveryDeterminismKey(
    bool parser_recovery_replay_ready,
    bool parse_artifact_replay_key_deterministic,
    bool parse_recovery_determinism_hardening_consistent,
    bool parser_diagnostic_grammar_hooks_diagnostics_hardening_ready,
    bool parser_diagnostic_grammar_hooks_edge_case_robustness_ready,
    bool parser_diagnostic_grammar_hooks_recovery_determinism_consistent,
    bool parser_diagnostic_grammar_hooks_recovery_determinism_ready) {
  return std::string("parser_recovery_replay_ready=") +
         (parser_recovery_replay_ready ? "true" : "false") +
         ";parse_artifact_replay_key_deterministic=" +
         (parse_artifact_replay_key_deterministic ? "true" : "false") +
         ";parse_recovery_determinism_hardening_consistent=" +
         (parse_recovery_determinism_hardening_consistent ? "true" : "false") +
         ";parser_diagnostic_grammar_hooks_diagnostics_hardening_ready=" +
         (parser_diagnostic_grammar_hooks_diagnostics_hardening_ready ? "true" : "false") +
         ";parser_diagnostic_grammar_hooks_edge_case_robustness_ready=" +
         (parser_diagnostic_grammar_hooks_edge_case_robustness_ready ? "true" : "false") +
         ";recovery_determinism_consistent=" +
         (parser_diagnostic_grammar_hooks_recovery_determinism_consistent ? "true" : "false") +
         ";recovery_determinism_ready=" +
         (parser_diagnostic_grammar_hooks_recovery_determinism_ready ? "true" : "false");
}

inline std::string BuildObjc3DiagnosticGrammarHooksConformanceMatrixKey(
    std::size_t conformance_matrix_case_count,
    std::size_t conformance_corpus_case_count,
    std::size_t performance_guardrail_case_count,
    bool parse_artifact_replay_key_deterministic,
    bool parser_diagnostic_grammar_hooks_recovery_determinism_ready,
    bool parser_diagnostic_grammar_hooks_conformance_matrix_consistent,
    bool parser_diagnostic_grammar_hooks_conformance_matrix_ready) {
  return "conformance_matrix_case_count=" + std::to_string(conformance_matrix_case_count) +
         ";conformance_corpus_case_count=" + std::to_string(conformance_corpus_case_count) +
         ";performance_guardrail_case_count=" + std::to_string(performance_guardrail_case_count) +
         ";parse_artifact_replay_key_deterministic=" +
         (parse_artifact_replay_key_deterministic ? "true" : "false") +
         ";recovery_determinism_ready=" +
         (parser_diagnostic_grammar_hooks_recovery_determinism_ready ? "true" : "false") +
         ";conformance_matrix_consistent=" +
         (parser_diagnostic_grammar_hooks_conformance_matrix_consistent ? "true" : "false") +
         ";conformance_matrix_ready=" +
         (parser_diagnostic_grammar_hooks_conformance_matrix_ready ? "true" : "false");
}

inline std::string BuildObjc3DiagnosticGrammarHooksConformanceCorpusKey(
    std::size_t conformance_corpus_case_count,
    std::size_t conformance_corpus_passed_case_count,
    std::size_t conformance_corpus_failed_case_count,
    bool parser_diagnostic_grammar_hooks_conformance_matrix_ready,
    bool parse_artifact_replay_key_deterministic,
    bool parser_diagnostic_grammar_hooks_conformance_corpus_consistent,
    bool parser_diagnostic_grammar_hooks_conformance_corpus_ready) {
  return "conformance_corpus_case_count=" + std::to_string(conformance_corpus_case_count) +
         ";conformance_corpus_passed_case_count=" + std::to_string(conformance_corpus_passed_case_count) +
         ";conformance_corpus_failed_case_count=" + std::to_string(conformance_corpus_failed_case_count) +
         ";conformance_matrix_ready=" +
         (parser_diagnostic_grammar_hooks_conformance_matrix_ready ? "true" : "false") +
         ";parse_artifact_replay_key_deterministic=" +
         (parse_artifact_replay_key_deterministic ? "true" : "false") +
         ";conformance_corpus_consistent=" +
         (parser_diagnostic_grammar_hooks_conformance_corpus_consistent ? "true" : "false") +
         ";conformance_corpus_ready=" +
         (parser_diagnostic_grammar_hooks_conformance_corpus_ready ? "true" : "false");
}
