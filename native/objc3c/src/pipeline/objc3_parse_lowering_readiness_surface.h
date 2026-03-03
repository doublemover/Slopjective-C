#pragma once

#include <algorithm>
#include <cstdint>
#include <string>
#include <vector>

#include "parse/objc3_diagnostic_grammar_hooks_core_feature.h"
#include "parse/objc3_diagnostic_grammar_hooks_core_feature_expansion_surface.h"
#include "parse/objc3_diagnostic_grammar_hooks_edge_case_compatibility_surface.h"
#include "parse/objc3_diagnostic_source_precision_scaffold.h"
#include "pipeline/objc3_frontend_types.h"
#include "pipeline/objc3_typed_sema_to_lowering_contract_surface.h"

inline std::size_t Objc3ParserSnapshotDeclarationBreakdownCount(const Objc3ParserContractSnapshot &snapshot) {
  return snapshot.global_decl_count + snapshot.protocol_decl_count +
         snapshot.interface_decl_count + snapshot.implementation_decl_count +
         snapshot.function_decl_count;
}

inline std::size_t Objc3ParsedProgramTopLevelDeclarationCount(const Objc3ParsedProgram &program) {
  const Objc3Program &ast = Objc3ParsedProgramAst(program);
  return ast.globals.size() + ast.protocols.size() + ast.interfaces.size() +
         ast.implementations.size() + ast.functions.size();
}

inline std::string BuildObjc3ParseArtifactHandoffKey(
    const Objc3ParserContractSnapshot &snapshot,
    std::size_t ast_top_level_declaration_count,
    std::size_t parser_diagnostic_count,
    bool handoff_deterministic) {
  return "parser_snapshot=" + std::to_string(snapshot.top_level_declaration_count) + ":" +
         std::to_string(snapshot.global_decl_count) + ":" +
         std::to_string(snapshot.protocol_decl_count) + ":" +
         std::to_string(snapshot.interface_decl_count) + ":" +
         std::to_string(snapshot.implementation_decl_count) + ":" +
         std::to_string(snapshot.function_decl_count) + ";ast_top_level=" +
         std::to_string(ast_top_level_declaration_count) + ";parser_diagnostics=" +
         std::to_string(parser_diagnostic_count) + ";deterministic=" +
         (handoff_deterministic ? "true" : "false");
}

inline std::string BuildObjc3ParseArtifactReplayKey(
    const Objc3ParserContractSnapshot &snapshot,
    std::uint64_t parser_contract_snapshot_fingerprint,
    std::uint64_t parser_ast_top_level_layout_fingerprint,
    std::uint64_t ast_shape_fingerprint,
    std::uint64_t ast_top_level_layout_fingerprint,
    const std::string &compatibility_handoff_key,
    bool fingerprint_consistent,
    bool replay_key_deterministic) {
  return "parser_snapshot_fingerprint=" + std::to_string(parser_contract_snapshot_fingerprint) +
         ";snapshot_ast_shape_fingerprint=" + std::to_string(snapshot.ast_shape_fingerprint) +
         ";ast_shape_fingerprint=" + std::to_string(ast_shape_fingerprint) +
         ";snapshot_ast_top_level_layout_fingerprint=" +
         std::to_string(parser_ast_top_level_layout_fingerprint) +
         ";ast_top_level_layout_fingerprint=" + std::to_string(ast_top_level_layout_fingerprint) +
         ";compatibility_handoff_key=" + compatibility_handoff_key +
         ";fingerprint_consistent=" + (fingerprint_consistent ? "true" : "false") +
         ";deterministic=" + (replay_key_deterministic ? "true" : "false");
}

inline const char *Objc3FrontendCompatibilityModeName(const Objc3FrontendCompatibilityMode mode) {
  return mode == Objc3FrontendCompatibilityMode::kLegacy ? "legacy" : "canonical";
}

inline bool IsObjc3LanguageVersionPragmaContractConsistent(
    const Objc3FrontendLanguageVersionPragmaContract &pragma_contract) {
  if (!pragma_contract.seen) {
    return pragma_contract.directive_count == 0 &&
           !pragma_contract.duplicate &&
           !pragma_contract.non_leading &&
           pragma_contract.first_line == 0 &&
           pragma_contract.first_column == 0 &&
           pragma_contract.last_line == 0 &&
           pragma_contract.last_column == 0;
  }

  const bool coordinates_present =
      pragma_contract.first_line > 0 &&
      pragma_contract.first_column > 0 &&
      pragma_contract.last_line > 0 &&
      pragma_contract.last_column > 0;
  const bool duplicate_consistent =
      !pragma_contract.duplicate || pragma_contract.directive_count > 1;
  return pragma_contract.directive_count > 0 &&
         coordinates_present &&
         duplicate_consistent;
}

inline bool IsObjc3LanguageVersionPragmaCoordinateOrderConsistent(
    const Objc3FrontendLanguageVersionPragmaContract &pragma_contract) {
  if (!pragma_contract.seen) {
    return true;
  }

  const bool first_before_or_equal_last =
      pragma_contract.first_line < pragma_contract.last_line ||
      (pragma_contract.first_line == pragma_contract.last_line &&
       pragma_contract.first_column <= pragma_contract.last_column);
  const bool single_directive_coordinates_consistent =
      pragma_contract.directive_count != 1 ||
      (pragma_contract.first_line == pragma_contract.last_line &&
       pragma_contract.first_column == pragma_contract.last_column);
  return first_before_or_equal_last &&
         single_directive_coordinates_consistent;
}

inline std::string BuildObjc3CompatibilityHandoffKey(
    const Objc3FrontendOptions &options,
    const Objc3FrontendMigrationHints &migration_hints,
    const Objc3FrontendLanguageVersionPragmaContract &pragma_contract,
    bool compatibility_handoff_consistent) {
  return "compatibility_mode=" +
         std::string(Objc3FrontendCompatibilityModeName(options.compatibility_mode)) +
         ";migration_assist=" + (options.migration_assist ? "true" : "false") +
         ";legacy_literals=" + std::to_string(migration_hints.legacy_yes_count) + ":" +
         std::to_string(migration_hints.legacy_no_count) + ":" +
         std::to_string(migration_hints.legacy_null_count) +
         ";language_version_pragma=" + (pragma_contract.seen ? "seen" : "none") + ":" +
         std::to_string(pragma_contract.directive_count) + ":" +
         (pragma_contract.duplicate ? "duplicate" : "single") + ":" +
         (pragma_contract.non_leading ? "non-leading" : "leading") +
         ";consistent=" + (compatibility_handoff_consistent ? "true" : "false");
}

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

inline bool Objc3ParseLoweringReadinessKeyHasPrefix(
    const std::string &value,
    const std::string &prefix) {
  return value.size() >= prefix.size() &&
         value.compare(0, prefix.size(), prefix) == 0;
}

inline bool IsObjc3ToolchainRuntimeGaOperationsRecoveryDeterminismHardeningConsistent(
    bool parser_recovery_replay_ready,
    bool parse_artifact_replay_key_deterministic,
    bool long_tail_grammar_replay_keys_ready,
    bool long_tail_grammar_diagnostics_hardening_ready,
    bool parse_recovery_determinism_hardening_consistent,
    const std::string &parse_artifact_handoff_key,
    const std::string &parse_artifact_replay_key,
    const std::string &parse_artifact_diagnostics_hardening_key,
    const std::string &parse_artifact_edge_robustness_key,
    const std::string &long_tail_grammar_handoff_key,
    const std::string &long_tail_grammar_diagnostics_hardening_key,
    const std::string &parse_recovery_determinism_hardening_key) {
  return parser_recovery_replay_ready &&
         parse_artifact_replay_key_deterministic &&
         long_tail_grammar_replay_keys_ready &&
         long_tail_grammar_diagnostics_hardening_ready &&
         parse_recovery_determinism_hardening_consistent &&
         Objc3ParseLoweringReadinessKeyHasPrefix(
             parse_artifact_handoff_key,
             "parser_snapshot=") &&
         Objc3ParseLoweringReadinessKeyHasPrefix(
             parse_artifact_replay_key,
             "parser_snapshot_fingerprint=") &&
         Objc3ParseLoweringReadinessKeyHasPrefix(
             parse_artifact_diagnostics_hardening_key,
             "parser_diagnostics=") &&
         Objc3ParseLoweringReadinessKeyHasPrefix(
             parse_artifact_edge_robustness_key,
             "parser_tokens=") &&
         Objc3ParseLoweringReadinessKeyHasPrefix(
             long_tail_grammar_handoff_key,
             "long-tail-grammar:v1:constructs=") &&
         Objc3ParseLoweringReadinessKeyHasPrefix(
             long_tail_grammar_diagnostics_hardening_key,
             "parser_diagnostic_count=") &&
         Objc3ParseLoweringReadinessKeyHasPrefix(
             parse_recovery_determinism_hardening_key,
             "snapshot_present=");
}

inline bool IsObjc3ToolchainRuntimeGaOperationsRecoveryDeterminismHardeningReady(
    bool toolchain_runtime_ga_operations_recovery_determinism_consistent,
    bool long_tail_grammar_recovery_determinism_consistent,
    bool long_tail_grammar_recovery_determinism_ready,
    const std::string &long_tail_grammar_recovery_determinism_key) {
  return toolchain_runtime_ga_operations_recovery_determinism_consistent &&
         long_tail_grammar_recovery_determinism_consistent &&
         long_tail_grammar_recovery_determinism_ready &&
         Objc3ParseLoweringReadinessKeyHasPrefix(
             long_tail_grammar_recovery_determinism_key,
             "parser_recovery_replay_ready=");
}

inline std::string BuildObjc3ParseRecoveryDeterminismHardeningKey(
    bool parser_contract_snapshot_present,
    bool parser_contract_deterministic,
    bool parser_recovery_replay_ready,
    bool long_tail_grammar_core_feature_consistent,
    bool long_tail_grammar_handoff_key_deterministic,
    bool long_tail_grammar_expansion_accounting_consistent,
    bool long_tail_grammar_replay_keys_ready,
    bool long_tail_grammar_expansion_ready,
    bool long_tail_grammar_compatibility_handoff_ready,
    bool long_tail_grammar_edge_case_compatibility_consistent,
    bool long_tail_grammar_edge_case_compatibility_ready,
    bool long_tail_grammar_edge_case_expansion_consistent,
    bool long_tail_grammar_edge_case_robustness_ready,
    bool long_tail_grammar_diagnostics_hardening_ready,
    bool parse_artifact_handoff_deterministic,
    bool parse_artifact_replay_key_deterministic,
    bool parse_artifact_diagnostics_hardening_consistent,
    bool parse_artifact_edge_case_robustness_consistent,
    bool parse_recovery_determinism_hardening_consistent) {
  return std::string("snapshot_present=") + (parser_contract_snapshot_present ? "true" : "false") +
         ";parser_handoff_deterministic=" + (parser_contract_deterministic ? "true" : "false") +
         ";parser_recovery_replay_ready=" + (parser_recovery_replay_ready ? "true" : "false") +
         ";long_tail_grammar_core_feature_consistent=" +
         (long_tail_grammar_core_feature_consistent ? "true" : "false") +
         ";long_tail_grammar_handoff_key_deterministic=" +
         (long_tail_grammar_handoff_key_deterministic ? "true" : "false") +
         ";long_tail_grammar_expansion_accounting_consistent=" +
         (long_tail_grammar_expansion_accounting_consistent ? "true" : "false") +
         ";long_tail_grammar_replay_keys_ready=" +
         (long_tail_grammar_replay_keys_ready ? "true" : "false") +
         ";long_tail_grammar_expansion_ready=" +
         (long_tail_grammar_expansion_ready ? "true" : "false") +
         ";long_tail_grammar_compatibility_handoff_ready=" +
         (long_tail_grammar_compatibility_handoff_ready ? "true" : "false") +
         ";long_tail_grammar_edge_case_compatibility_consistent=" +
         (long_tail_grammar_edge_case_compatibility_consistent ? "true" : "false") +
         ";long_tail_grammar_edge_case_compatibility_ready=" +
         (long_tail_grammar_edge_case_compatibility_ready ? "true" : "false") +
         ";long_tail_grammar_edge_case_expansion_consistent=" +
         (long_tail_grammar_edge_case_expansion_consistent ? "true" : "false") +
         ";long_tail_grammar_edge_case_robustness_ready=" +
         (long_tail_grammar_edge_case_robustness_ready ? "true" : "false") +
         ";long_tail_grammar_diagnostics_hardening_ready=" +
         (long_tail_grammar_diagnostics_hardening_ready ? "true" : "false") +
         ";parse_artifact_handoff_deterministic=" + (parse_artifact_handoff_deterministic ? "true" : "false") +
         ";parse_artifact_replay_key_deterministic=" +
         (parse_artifact_replay_key_deterministic ? "true" : "false") +
         ";parse_artifact_diagnostics_hardening_consistent=" +
         (parse_artifact_diagnostics_hardening_consistent ? "true" : "false") +
         ";parse_artifact_edge_case_robustness_consistent=" +
         (parse_artifact_edge_case_robustness_consistent ? "true" : "false") +
         ";consistent=" + (parse_recovery_determinism_hardening_consistent ? "true" : "false");
}

inline std::string BuildObjc3ToolchainRuntimeGaOperationsRecoveryDeterminismHardeningKey(
    bool parser_recovery_replay_ready,
    bool parse_artifact_replay_key_deterministic,
    bool long_tail_grammar_replay_keys_ready,
    bool long_tail_grammar_diagnostics_hardening_ready,
    const std::string &parse_artifact_handoff_key,
    const std::string &parse_artifact_replay_key,
    const std::string &parse_artifact_diagnostics_hardening_key,
    const std::string &parse_artifact_edge_robustness_key,
    const std::string &long_tail_grammar_handoff_key,
    const std::string &long_tail_grammar_diagnostics_hardening_key,
    const std::string &parse_recovery_determinism_hardening_key,
    const std::string &long_tail_grammar_recovery_determinism_key,
    bool toolchain_runtime_ga_operations_recovery_determinism_consistent,
    bool toolchain_runtime_ga_operations_recovery_determinism_ready) {
  const bool parse_artifact_handoff_key_shape_deterministic =
      Objc3ParseLoweringReadinessKeyHasPrefix(
          parse_artifact_handoff_key,
          "parser_snapshot=");
  const bool parse_artifact_replay_key_shape_deterministic =
      Objc3ParseLoweringReadinessKeyHasPrefix(
          parse_artifact_replay_key,
          "parser_snapshot_fingerprint=");
  const bool parse_artifact_diagnostics_hardening_key_shape_deterministic =
      Objc3ParseLoweringReadinessKeyHasPrefix(
          parse_artifact_diagnostics_hardening_key,
          "parser_diagnostics=");
  const bool parse_artifact_edge_robustness_key_shape_deterministic =
      Objc3ParseLoweringReadinessKeyHasPrefix(
          parse_artifact_edge_robustness_key,
          "parser_tokens=");
  const bool long_tail_grammar_handoff_key_shape_deterministic =
      Objc3ParseLoweringReadinessKeyHasPrefix(
          long_tail_grammar_handoff_key,
          "long-tail-grammar:v1:constructs=");
  const bool long_tail_grammar_diagnostics_hardening_key_shape_deterministic =
      Objc3ParseLoweringReadinessKeyHasPrefix(
          long_tail_grammar_diagnostics_hardening_key,
          "parser_diagnostic_count=");
  const bool parse_recovery_determinism_hardening_key_shape_deterministic =
      Objc3ParseLoweringReadinessKeyHasPrefix(
          parse_recovery_determinism_hardening_key,
          "snapshot_present=");
  const bool long_tail_grammar_recovery_determinism_key_shape_deterministic =
      Objc3ParseLoweringReadinessKeyHasPrefix(
          long_tail_grammar_recovery_determinism_key,
          "parser_recovery_replay_ready=");
  return std::string("parser_recovery_replay_ready=") +
         (parser_recovery_replay_ready ? "true" : "false") +
         ";parse_artifact_replay_key_deterministic=" +
         (parse_artifact_replay_key_deterministic ? "true" : "false") +
         ";long_tail_grammar_replay_keys_ready=" +
         (long_tail_grammar_replay_keys_ready ? "true" : "false") +
         ";long_tail_grammar_diagnostics_hardening_ready=" +
         (long_tail_grammar_diagnostics_hardening_ready ? "true" : "false") +
         ";parse_artifact_handoff_key_shape_deterministic=" +
         (parse_artifact_handoff_key_shape_deterministic ? "true" : "false") +
         ";parse_artifact_replay_key_shape_deterministic=" +
         (parse_artifact_replay_key_shape_deterministic ? "true" : "false") +
         ";parse_artifact_diagnostics_hardening_key_shape_deterministic=" +
         (parse_artifact_diagnostics_hardening_key_shape_deterministic ? "true" : "false") +
         ";parse_artifact_edge_robustness_key_shape_deterministic=" +
         (parse_artifact_edge_robustness_key_shape_deterministic ? "true" : "false") +
         ";long_tail_grammar_handoff_key_shape_deterministic=" +
         (long_tail_grammar_handoff_key_shape_deterministic ? "true" : "false") +
         ";long_tail_grammar_diagnostics_hardening_key_shape_deterministic=" +
         (long_tail_grammar_diagnostics_hardening_key_shape_deterministic ? "true" : "false") +
         ";parse_recovery_determinism_hardening_key_shape_deterministic=" +
         (parse_recovery_determinism_hardening_key_shape_deterministic ? "true" : "false") +
         ";long_tail_grammar_recovery_determinism_key_shape_deterministic=" +
         (long_tail_grammar_recovery_determinism_key_shape_deterministic ? "true" : "false") +
         ";consistent=" +
         (toolchain_runtime_ga_operations_recovery_determinism_consistent ? "true" : "false") +
         ";ready=" + (toolchain_runtime_ga_operations_recovery_determinism_ready ? "true" : "false");
}

inline std::string BuildObjc3LongTailGrammarExpansionKey(
    std::size_t construct_count,
    std::size_t covered_construct_count,
    std::uint64_t fingerprint,
    bool core_feature_consistent,
    bool handoff_key_deterministic,
    bool expansion_accounting_consistent,
    bool replay_keys_ready,
    bool expansion_ready) {
  return "construct_count=" + std::to_string(construct_count) +
         ";covered_construct_count=" + std::to_string(covered_construct_count) +
         ";fingerprint=" + std::to_string(fingerprint) +
         ";core_feature_consistent=" + (core_feature_consistent ? "true" : "false") +
         ";handoff_key_deterministic=" + (handoff_key_deterministic ? "true" : "false") +
         ";expansion_accounting_consistent=" +
         (expansion_accounting_consistent ? "true" : "false") +
         ";replay_keys_ready=" + (replay_keys_ready ? "true" : "false") +
         ";expansion_ready=" + (expansion_ready ? "true" : "false");
}

inline std::string BuildObjc3LongTailGrammarEdgeCaseCompatibilityKey(
    bool compatibility_handoff_consistent,
    bool compatibility_handoff_ready,
    bool language_version_pragma_coordinate_order_consistent,
    bool parse_artifact_edge_case_robustness_consistent,
    bool edge_case_compatibility_consistent,
    bool edge_case_compatibility_ready) {
  return "compatibility_handoff_consistent=" +
         std::string(compatibility_handoff_consistent ? "true" : "false") +
         ";compatibility_handoff_ready=" +
         std::string(compatibility_handoff_ready ? "true" : "false") +
         ";pragma_coordinate_order_consistent=" +
         std::string(language_version_pragma_coordinate_order_consistent ? "true" : "false") +
         ";parse_edge_case_robustness_consistent=" +
         std::string(parse_artifact_edge_case_robustness_consistent ? "true" : "false") +
         ";edge_case_compatibility_consistent=" +
         std::string(edge_case_compatibility_consistent ? "true" : "false") +
         ";edge_case_compatibility_ready=" +
         std::string(edge_case_compatibility_ready ? "true" : "false");
}

inline std::string BuildObjc3LongTailGrammarEdgeCaseRobustnessKey(
    std::size_t construct_count,
    std::size_t covered_construct_count,
    std::uint64_t fingerprint,
    bool edge_case_compatibility_ready,
    bool edge_case_expansion_consistent,
    bool parse_artifact_edge_case_robustness_consistent,
    bool edge_case_robustness_ready) {
  return "construct_count=" + std::to_string(construct_count) +
         ";covered_construct_count=" + std::to_string(covered_construct_count) +
         ";fingerprint=" + std::to_string(fingerprint) +
         ";edge_case_compatibility_ready=" +
         std::string(edge_case_compatibility_ready ? "true" : "false") +
         ";edge_case_expansion_consistent=" +
         std::string(edge_case_expansion_consistent ? "true" : "false") +
         ";parse_edge_case_robustness_consistent=" +
         std::string(parse_artifact_edge_case_robustness_consistent ? "true" : "false") +
         ";edge_case_robustness_ready=" +
         std::string(edge_case_robustness_ready ? "true" : "false");
}

inline std::string BuildObjc3LongTailGrammarDiagnosticsHardeningKey(
    std::size_t parser_diagnostic_count,
    std::size_t parser_diagnostic_code_count,
    std::uint64_t parser_diagnostic_code_fingerprint,
    bool parser_diagnostic_surface_consistent,
    bool parser_diagnostic_code_surface_deterministic,
    bool parse_artifact_diagnostics_hardening_consistent,
    bool edge_case_robustness_ready,
    bool diagnostics_hardening_consistent,
    bool diagnostics_hardening_ready) {
  return "parser_diagnostic_count=" + std::to_string(parser_diagnostic_count) +
         ";parser_diagnostic_code_count=" + std::to_string(parser_diagnostic_code_count) +
         ";parser_diagnostic_code_fingerprint=" + std::to_string(parser_diagnostic_code_fingerprint) +
         ";parser_diagnostic_surface_consistent=" +
         std::string(parser_diagnostic_surface_consistent ? "true" : "false") +
         ";parser_diagnostic_code_surface_deterministic=" +
         std::string(parser_diagnostic_code_surface_deterministic ? "true" : "false") +
         ";parse_artifact_diagnostics_hardening_consistent=" +
         std::string(parse_artifact_diagnostics_hardening_consistent ? "true" : "false") +
         ";edge_case_robustness_ready=" +
         std::string(edge_case_robustness_ready ? "true" : "false") +
         ";diagnostics_hardening_consistent=" +
         std::string(diagnostics_hardening_consistent ? "true" : "false") +
         ";diagnostics_hardening_ready=" +
         std::string(diagnostics_hardening_ready ? "true" : "false");
}

inline std::string BuildObjc3LongTailGrammarRecoveryDeterminismKey(
    bool parser_recovery_replay_ready,
    bool parse_artifact_replay_key_deterministic,
    bool parse_recovery_determinism_hardening_consistent,
    bool diagnostics_hardening_ready,
    bool edge_case_robustness_ready,
    bool expansion_ready,
    bool recovery_determinism_consistent,
    bool recovery_determinism_ready) {
  return std::string("parser_recovery_replay_ready=") +
         (parser_recovery_replay_ready ? "true" : "false") +
         ";parse_artifact_replay_key_deterministic=" +
         (parse_artifact_replay_key_deterministic ? "true" : "false") +
         ";parse_recovery_determinism_hardening_consistent=" +
         (parse_recovery_determinism_hardening_consistent ? "true" : "false") +
         ";diagnostics_hardening_ready=" +
         (diagnostics_hardening_ready ? "true" : "false") +
         ";edge_case_robustness_ready=" +
         (edge_case_robustness_ready ? "true" : "false") +
         ";expansion_ready=" + (expansion_ready ? "true" : "false") +
         ";recovery_determinism_consistent=" +
         (recovery_determinism_consistent ? "true" : "false") +
         ";recovery_determinism_ready=" +
         (recovery_determinism_ready ? "true" : "false");
}

inline std::string BuildObjc3LongTailGrammarConformanceMatrixKey(
    std::size_t conformance_matrix_case_count,
    std::size_t conformance_corpus_case_count,
    std::size_t performance_guardrail_case_count,
    bool parse_replay_key_deterministic,
    bool recovery_determinism_ready,
    bool conformance_matrix_consistent,
    bool conformance_matrix_ready) {
  return "conformance_matrix_case_count=" + std::to_string(conformance_matrix_case_count) +
         ";conformance_corpus_case_count=" + std::to_string(conformance_corpus_case_count) +
         ";performance_guardrail_case_count=" + std::to_string(performance_guardrail_case_count) +
         ";parse_replay_key_deterministic=" +
         std::string(parse_replay_key_deterministic ? "true" : "false") +
         ";recovery_determinism_ready=" +
         std::string(recovery_determinism_ready ? "true" : "false") +
         ";conformance_matrix_consistent=" +
         std::string(conformance_matrix_consistent ? "true" : "false") +
         ";conformance_matrix_ready=" +
         std::string(conformance_matrix_ready ? "true" : "false");
}

inline bool IsObjc3ToolchainRuntimeGaOperationsConformanceMatrixConsistent(
    bool toolchain_runtime_ga_operations_recovery_determinism_consistent,
    bool toolchain_runtime_ga_operations_recovery_determinism_ready,
    bool parse_lowering_conformance_matrix_consistent,
    bool long_tail_grammar_conformance_matrix_consistent,
    bool long_tail_grammar_conformance_matrix_ready,
    const std::string &parse_recovery_determinism_hardening_key,
    const std::string &long_tail_grammar_recovery_determinism_key) {
  return toolchain_runtime_ga_operations_recovery_determinism_consistent &&
         toolchain_runtime_ga_operations_recovery_determinism_ready &&
         parse_lowering_conformance_matrix_consistent &&
         long_tail_grammar_conformance_matrix_consistent &&
         long_tail_grammar_conformance_matrix_ready &&
         parse_recovery_determinism_hardening_key.find(
             "toolchain_runtime_ga_operations_recovery_determinism_key=") != std::string::npos &&
         long_tail_grammar_recovery_determinism_key.find(
             "toolchain_runtime_ga_operations_recovery_determinism_key=") != std::string::npos;
}

inline bool IsObjc3ToolchainRuntimeGaOperationsConformanceMatrixReady(
    bool toolchain_runtime_ga_operations_conformance_matrix_consistent,
    const std::string &parse_lowering_conformance_matrix_key,
    const std::string &long_tail_grammar_conformance_matrix_key) {
  return toolchain_runtime_ga_operations_conformance_matrix_consistent &&
         Objc3ParseLoweringReadinessKeyHasPrefix(
             parse_lowering_conformance_matrix_key,
             "case_count=") &&
         Objc3ParseLoweringReadinessKeyHasPrefix(
             long_tail_grammar_conformance_matrix_key,
             "conformance_matrix_case_count=");
}

inline std::string BuildObjc3ToolchainRuntimeGaOperationsConformanceMatrixKey(
    bool parse_lowering_conformance_matrix_consistent,
    bool long_tail_grammar_conformance_matrix_consistent,
    bool long_tail_grammar_conformance_matrix_ready,
    const std::string &parse_lowering_conformance_matrix_key,
    const std::string &long_tail_grammar_conformance_matrix_key,
    bool toolchain_runtime_ga_operations_conformance_matrix_consistent,
    bool toolchain_runtime_ga_operations_conformance_matrix_ready) {
  const bool parse_lowering_conformance_matrix_key_shape_deterministic =
      Objc3ParseLoweringReadinessKeyHasPrefix(
          parse_lowering_conformance_matrix_key,
          "case_count=");
  const bool long_tail_grammar_conformance_matrix_key_shape_deterministic =
      Objc3ParseLoweringReadinessKeyHasPrefix(
          long_tail_grammar_conformance_matrix_key,
          "conformance_matrix_case_count=");
  return std::string("parse_lowering_conformance_matrix_consistent=") +
         (parse_lowering_conformance_matrix_consistent ? "true" : "false") +
         ";long_tail_grammar_conformance_matrix_consistent=" +
         (long_tail_grammar_conformance_matrix_consistent ? "true" : "false") +
         ";long_tail_grammar_conformance_matrix_ready=" +
         (long_tail_grammar_conformance_matrix_ready ? "true" : "false") +
         ";parse_lowering_conformance_matrix_key_shape_deterministic=" +
         (parse_lowering_conformance_matrix_key_shape_deterministic ? "true" : "false") +
         ";long_tail_grammar_conformance_matrix_key_shape_deterministic=" +
         (long_tail_grammar_conformance_matrix_key_shape_deterministic ? "true" : "false") +
         ";consistent=" +
         (toolchain_runtime_ga_operations_conformance_matrix_consistent ? "true" : "false") +
         ";ready=" + (toolchain_runtime_ga_operations_conformance_matrix_ready ? "true" : "false");
}

inline bool IsObjc3ToolchainRuntimeGaOperationsConformanceCorpusConsistent(
    bool toolchain_runtime_ga_operations_conformance_matrix_consistent,
    bool toolchain_runtime_ga_operations_conformance_matrix_ready,
    bool parse_lowering_conformance_matrix_consistent,
    bool parse_lowering_conformance_corpus_consistent,
    const std::string &parse_lowering_conformance_matrix_key,
    const std::string &parse_lowering_conformance_corpus_key,
    const std::string &long_tail_grammar_conformance_matrix_key) {
  return toolchain_runtime_ga_operations_conformance_matrix_consistent &&
         toolchain_runtime_ga_operations_conformance_matrix_ready &&
         parse_lowering_conformance_matrix_consistent &&
         parse_lowering_conformance_corpus_consistent &&
         Objc3ParseLoweringReadinessKeyHasPrefix(
             parse_lowering_conformance_matrix_key,
             "case_count=") &&
         Objc3ParseLoweringReadinessKeyHasPrefix(
             parse_lowering_conformance_corpus_key,
             "case_count=") &&
         Objc3ParseLoweringReadinessKeyHasPrefix(
             long_tail_grammar_conformance_matrix_key,
             "conformance_matrix_case_count=");
}

inline bool IsObjc3ToolchainRuntimeGaOperationsConformanceCorpusReady(
    bool toolchain_runtime_ga_operations_conformance_corpus_consistent,
    const std::string &parse_lowering_conformance_corpus_key) {
  return toolchain_runtime_ga_operations_conformance_corpus_consistent &&
         Objc3ParseLoweringReadinessKeyHasPrefix(
             parse_lowering_conformance_corpus_key,
             "case_count=");
}

inline std::string BuildObjc3ToolchainRuntimeGaOperationsConformanceCorpusKey(
    bool parse_lowering_conformance_matrix_consistent,
    bool parse_lowering_conformance_corpus_consistent,
    const std::string &parse_lowering_conformance_matrix_key,
    const std::string &parse_lowering_conformance_corpus_key,
    const std::string &long_tail_grammar_conformance_matrix_key,
    bool toolchain_runtime_ga_operations_conformance_corpus_consistent,
    bool toolchain_runtime_ga_operations_conformance_corpus_ready) {
  const bool parse_lowering_conformance_matrix_key_shape_deterministic =
      Objc3ParseLoweringReadinessKeyHasPrefix(
          parse_lowering_conformance_matrix_key,
          "case_count=");
  const bool parse_lowering_conformance_corpus_key_shape_deterministic =
      Objc3ParseLoweringReadinessKeyHasPrefix(
          parse_lowering_conformance_corpus_key,
          "case_count=");
  const bool long_tail_grammar_conformance_matrix_key_shape_deterministic =
      Objc3ParseLoweringReadinessKeyHasPrefix(
          long_tail_grammar_conformance_matrix_key,
          "conformance_matrix_case_count=");
  return std::string("parse_lowering_conformance_matrix_consistent=") +
         (parse_lowering_conformance_matrix_consistent ? "true" : "false") +
         ";parse_lowering_conformance_corpus_consistent=" +
         (parse_lowering_conformance_corpus_consistent ? "true" : "false") +
         ";parse_lowering_conformance_matrix_key_shape_deterministic=" +
         (parse_lowering_conformance_matrix_key_shape_deterministic ? "true" : "false") +
         ";parse_lowering_conformance_corpus_key_shape_deterministic=" +
         (parse_lowering_conformance_corpus_key_shape_deterministic ? "true" : "false") +
         ";long_tail_grammar_conformance_matrix_key_shape_deterministic=" +
         (long_tail_grammar_conformance_matrix_key_shape_deterministic ? "true" : "false") +
         ";consistent=" +
         (toolchain_runtime_ga_operations_conformance_corpus_consistent ? "true" : "false") +
         ";ready=" + (toolchain_runtime_ga_operations_conformance_corpus_ready ? "true" : "false");
}

inline bool IsObjc3ToolchainRuntimeGaOperationsPerformanceQualityGuardrailsConsistent(
    bool toolchain_runtime_ga_operations_conformance_corpus_consistent,
    bool toolchain_runtime_ga_operations_conformance_corpus_ready,
    bool parse_lowering_conformance_corpus_consistent,
    bool parse_lowering_performance_quality_guardrails_consistent,
    const std::string &parse_lowering_conformance_corpus_key,
    const std::string &parse_lowering_performance_quality_guardrails_key,
    const std::string &long_tail_grammar_conformance_matrix_key) {
  return toolchain_runtime_ga_operations_conformance_corpus_consistent &&
         toolchain_runtime_ga_operations_conformance_corpus_ready &&
         parse_lowering_conformance_corpus_consistent &&
         parse_lowering_performance_quality_guardrails_consistent &&
         Objc3ParseLoweringReadinessKeyHasPrefix(
             parse_lowering_conformance_corpus_key,
             "case_count=") &&
         Objc3ParseLoweringReadinessKeyHasPrefix(
             parse_lowering_performance_quality_guardrails_key,
             "case_count=") &&
         Objc3ParseLoweringReadinessKeyHasPrefix(
             long_tail_grammar_conformance_matrix_key,
             "conformance_matrix_case_count=");
}

inline bool IsObjc3ToolchainRuntimeGaOperationsPerformanceQualityGuardrailsReady(
    bool toolchain_runtime_ga_operations_performance_quality_guardrails_consistent,
    const std::string &parse_lowering_performance_quality_guardrails_key) {
  return toolchain_runtime_ga_operations_performance_quality_guardrails_consistent &&
         Objc3ParseLoweringReadinessKeyHasPrefix(
             parse_lowering_performance_quality_guardrails_key,
             "case_count=");
}

inline std::string BuildObjc3ToolchainRuntimeGaOperationsPerformanceQualityGuardrailsKey(
    bool parse_lowering_conformance_corpus_consistent,
    bool parse_lowering_performance_quality_guardrails_consistent,
    const std::string &parse_lowering_conformance_corpus_key,
    const std::string &parse_lowering_performance_quality_guardrails_key,
    const std::string &long_tail_grammar_conformance_matrix_key,
    bool toolchain_runtime_ga_operations_performance_quality_guardrails_consistent,
    bool toolchain_runtime_ga_operations_performance_quality_guardrails_ready) {
  const bool parse_lowering_conformance_corpus_key_shape_deterministic =
      Objc3ParseLoweringReadinessKeyHasPrefix(
          parse_lowering_conformance_corpus_key,
          "case_count=");
  const bool parse_lowering_performance_quality_guardrails_key_shape_deterministic =
      Objc3ParseLoweringReadinessKeyHasPrefix(
          parse_lowering_performance_quality_guardrails_key,
          "case_count=");
  const bool long_tail_grammar_conformance_matrix_key_shape_deterministic =
      Objc3ParseLoweringReadinessKeyHasPrefix(
          long_tail_grammar_conformance_matrix_key,
          "conformance_matrix_case_count=");
  return std::string("parse_lowering_conformance_corpus_consistent=") +
         (parse_lowering_conformance_corpus_consistent ? "true" : "false") +
         ";parse_lowering_performance_quality_guardrails_consistent=" +
         (parse_lowering_performance_quality_guardrails_consistent ? "true" : "false") +
         ";parse_lowering_conformance_corpus_key_shape_deterministic=" +
         (parse_lowering_conformance_corpus_key_shape_deterministic ? "true" : "false") +
         ";parse_lowering_performance_quality_guardrails_key_shape_deterministic=" +
         (parse_lowering_performance_quality_guardrails_key_shape_deterministic ? "true" : "false") +
         ";long_tail_grammar_conformance_matrix_key_shape_deterministic=" +
         (long_tail_grammar_conformance_matrix_key_shape_deterministic ? "true" : "false") +
         ";consistent=" +
         (toolchain_runtime_ga_operations_performance_quality_guardrails_consistent ? "true"
                                                                                    : "false") +
         ";ready=" +
         (toolchain_runtime_ga_operations_performance_quality_guardrails_ready ? "true" : "false");
}

inline bool IsObjc3ToolchainRuntimeGaOperationsCrossLaneIntegrationConsistent(
    bool toolchain_runtime_ga_operations_performance_quality_guardrails_consistent,
    bool toolchain_runtime_ga_operations_performance_quality_guardrails_ready,
    bool parse_snapshot_replay_ready,
    bool sema_handoff_ready,
    bool lowering_boundary_ready,
    bool parse_lowering_conformance_corpus_consistent,
    bool parse_lowering_performance_quality_guardrails_consistent,
    const std::string &parse_artifact_replay_key,
    const std::string &lowering_boundary_replay_key,
    const std::string &parse_lowering_conformance_corpus_key,
    const std::string &parse_lowering_performance_quality_guardrails_key) {
  return toolchain_runtime_ga_operations_performance_quality_guardrails_consistent &&
         toolchain_runtime_ga_operations_performance_quality_guardrails_ready &&
         parse_snapshot_replay_ready &&
         sema_handoff_ready &&
         lowering_boundary_ready &&
         parse_lowering_conformance_corpus_consistent &&
         parse_lowering_performance_quality_guardrails_consistent &&
         !parse_artifact_replay_key.empty() &&
         !lowering_boundary_replay_key.empty() &&
         Objc3ParseLoweringReadinessKeyHasPrefix(
             parse_lowering_conformance_corpus_key,
             "case_count=") &&
         Objc3ParseLoweringReadinessKeyHasPrefix(
             parse_lowering_performance_quality_guardrails_key,
             "case_count=");
}

inline bool IsObjc3ToolchainRuntimeGaOperationsCrossLaneIntegrationReady(
    bool toolchain_runtime_ga_operations_cross_lane_integration_consistent,
    const std::string &parse_artifact_replay_key,
    const std::string &lowering_boundary_replay_key,
    const std::string &parse_lowering_performance_quality_guardrails_key) {
  return toolchain_runtime_ga_operations_cross_lane_integration_consistent &&
         !parse_artifact_replay_key.empty() &&
         !lowering_boundary_replay_key.empty() &&
         Objc3ParseLoweringReadinessKeyHasPrefix(
             parse_lowering_performance_quality_guardrails_key,
             "case_count=");
}

inline std::string BuildObjc3ToolchainRuntimeGaOperationsCrossLaneIntegrationKey(
    bool parse_snapshot_replay_ready,
    bool sema_handoff_ready,
    bool lowering_boundary_ready,
    bool parse_lowering_conformance_corpus_consistent,
    bool parse_lowering_performance_quality_guardrails_consistent,
    const std::string &parse_artifact_replay_key,
    const std::string &lowering_boundary_replay_key,
    const std::string &parse_lowering_conformance_corpus_key,
    const std::string &parse_lowering_performance_quality_guardrails_key,
    bool toolchain_runtime_ga_operations_cross_lane_integration_consistent,
    bool toolchain_runtime_ga_operations_cross_lane_integration_ready) {
  const bool parse_lowering_conformance_corpus_key_shape_deterministic =
      Objc3ParseLoweringReadinessKeyHasPrefix(
          parse_lowering_conformance_corpus_key,
          "case_count=");
  const bool parse_lowering_performance_quality_guardrails_key_shape_deterministic =
      Objc3ParseLoweringReadinessKeyHasPrefix(
          parse_lowering_performance_quality_guardrails_key,
          "case_count=");
  return std::string("parse_snapshot_replay_ready=") +
         (parse_snapshot_replay_ready ? "true" : "false") +
         ";sema_handoff_ready=" + (sema_handoff_ready ? "true" : "false") +
         ";lowering_boundary_ready=" + (lowering_boundary_ready ? "true" : "false") +
         ";parse_lowering_conformance_corpus_consistent=" +
         (parse_lowering_conformance_corpus_consistent ? "true" : "false") +
         ";parse_lowering_performance_quality_guardrails_consistent=" +
         (parse_lowering_performance_quality_guardrails_consistent ? "true" : "false") +
         ";parse_artifact_replay_key_present=" +
         (!parse_artifact_replay_key.empty() ? "true" : "false") +
         ";lowering_boundary_replay_key_present=" +
         (!lowering_boundary_replay_key.empty() ? "true" : "false") +
         ";parse_lowering_conformance_corpus_key_shape_deterministic=" +
         (parse_lowering_conformance_corpus_key_shape_deterministic ? "true" : "false") +
         ";parse_lowering_performance_quality_guardrails_key_shape_deterministic=" +
         (parse_lowering_performance_quality_guardrails_key_shape_deterministic ? "true" : "false") +
         ";consistent=" +
         (toolchain_runtime_ga_operations_cross_lane_integration_consistent ? "true" : "false") +
         ";ready=" + (toolchain_runtime_ga_operations_cross_lane_integration_ready ? "true" : "false");
}

inline bool IsObjc3ToolchainRuntimeGaOperationsDocsRunbookSyncConsistent(
    bool toolchain_runtime_ga_operations_cross_lane_integration_consistent,
    bool toolchain_runtime_ga_operations_cross_lane_integration_ready,
    bool long_tail_grammar_integration_closeout_consistent,
    bool long_tail_grammar_gate_signoff_ready,
    const std::string &long_tail_grammar_integration_closeout_key,
    const std::string &parse_lowering_performance_quality_guardrails_key) {
  return toolchain_runtime_ga_operations_cross_lane_integration_consistent &&
         toolchain_runtime_ga_operations_cross_lane_integration_ready &&
         long_tail_grammar_integration_closeout_consistent &&
         long_tail_grammar_gate_signoff_ready &&
         Objc3ParseLoweringReadinessKeyHasPrefix(
             long_tail_grammar_integration_closeout_key,
             "conformance_matrix_ready=") &&
         Objc3ParseLoweringReadinessKeyHasPrefix(
             parse_lowering_performance_quality_guardrails_key,
             "case_count=");
}

inline bool IsObjc3ToolchainRuntimeGaOperationsDocsRunbookSyncReady(
    bool toolchain_runtime_ga_operations_docs_runbook_sync_consistent,
    const std::string &long_tail_grammar_integration_closeout_key) {
  return toolchain_runtime_ga_operations_docs_runbook_sync_consistent &&
         Objc3ParseLoweringReadinessKeyHasPrefix(
             long_tail_grammar_integration_closeout_key,
             "conformance_matrix_ready=");
}

inline std::string BuildObjc3ToolchainRuntimeGaOperationsDocsRunbookSyncKey(
    bool long_tail_grammar_integration_closeout_consistent,
    bool long_tail_grammar_gate_signoff_ready,
    const std::string &long_tail_grammar_integration_closeout_key,
    const std::string &parse_lowering_performance_quality_guardrails_key,
    bool toolchain_runtime_ga_operations_docs_runbook_sync_consistent,
    bool toolchain_runtime_ga_operations_docs_runbook_sync_ready) {
  const bool long_tail_grammar_integration_closeout_key_shape_deterministic =
      Objc3ParseLoweringReadinessKeyHasPrefix(
          long_tail_grammar_integration_closeout_key,
          "conformance_matrix_ready=");
  const bool parse_lowering_performance_quality_guardrails_key_shape_deterministic =
      Objc3ParseLoweringReadinessKeyHasPrefix(
          parse_lowering_performance_quality_guardrails_key,
          "case_count=");
  return std::string("long_tail_grammar_integration_closeout_consistent=") +
         (long_tail_grammar_integration_closeout_consistent ? "true" : "false") +
         ";long_tail_grammar_gate_signoff_ready=" +
         (long_tail_grammar_gate_signoff_ready ? "true" : "false") +
         ";long_tail_grammar_integration_closeout_key_shape_deterministic=" +
         (long_tail_grammar_integration_closeout_key_shape_deterministic ? "true" : "false") +
         ";parse_lowering_performance_quality_guardrails_key_shape_deterministic=" +
         (parse_lowering_performance_quality_guardrails_key_shape_deterministic ? "true" : "false") +
         ";consistent=" +
         (toolchain_runtime_ga_operations_docs_runbook_sync_consistent ? "true" : "false") +
         ";ready=" + (toolchain_runtime_ga_operations_docs_runbook_sync_ready ? "true" : "false");
}

inline bool IsObjc3ToolchainRuntimeGaOperationsAdvancedCoreConsistent(
    bool toolchain_runtime_ga_operations_docs_runbook_sync_consistent,
    bool toolchain_runtime_ga_operations_docs_runbook_sync_ready,
    bool long_tail_grammar_integration_closeout_consistent,
    bool long_tail_grammar_gate_signoff_ready,
    const std::string &toolchain_runtime_ga_operations_docs_runbook_sync_key,
    const std::string &long_tail_grammar_integration_closeout_key,
    const std::string &parse_lowering_performance_quality_guardrails_key) {
  return toolchain_runtime_ga_operations_docs_runbook_sync_consistent &&
         toolchain_runtime_ga_operations_docs_runbook_sync_ready &&
         long_tail_grammar_integration_closeout_consistent &&
         long_tail_grammar_gate_signoff_ready &&
         Objc3ParseLoweringReadinessKeyHasPrefix(
             toolchain_runtime_ga_operations_docs_runbook_sync_key,
             "long_tail_grammar_integration_closeout_consistent=") &&
         Objc3ParseLoweringReadinessKeyHasPrefix(
             long_tail_grammar_integration_closeout_key,
             "conformance_matrix_ready=") &&
         Objc3ParseLoweringReadinessKeyHasPrefix(
             parse_lowering_performance_quality_guardrails_key,
             "case_count=");
}

inline bool IsObjc3ToolchainRuntimeGaOperationsAdvancedCoreReady(
    bool toolchain_runtime_ga_operations_advanced_core_consistent,
    const std::string &toolchain_runtime_ga_operations_docs_runbook_sync_key,
    const std::string &long_tail_grammar_integration_closeout_key) {
  return toolchain_runtime_ga_operations_advanced_core_consistent &&
         Objc3ParseLoweringReadinessKeyHasPrefix(
             toolchain_runtime_ga_operations_docs_runbook_sync_key,
             "long_tail_grammar_integration_closeout_consistent=") &&
         Objc3ParseLoweringReadinessKeyHasPrefix(
             long_tail_grammar_integration_closeout_key,
             "conformance_matrix_ready=");
}

inline std::string BuildObjc3ToolchainRuntimeGaOperationsAdvancedCoreKey(
    bool toolchain_runtime_ga_operations_docs_runbook_sync_consistent,
    bool toolchain_runtime_ga_operations_docs_runbook_sync_ready,
    const std::string &toolchain_runtime_ga_operations_docs_runbook_sync_key,
    const std::string &long_tail_grammar_integration_closeout_key,
    const std::string &parse_lowering_performance_quality_guardrails_key,
    bool toolchain_runtime_ga_operations_advanced_core_consistent,
    bool toolchain_runtime_ga_operations_advanced_core_ready) {
  const bool docs_runbook_sync_key_shape_deterministic =
      Objc3ParseLoweringReadinessKeyHasPrefix(
          toolchain_runtime_ga_operations_docs_runbook_sync_key,
          "long_tail_grammar_integration_closeout_consistent=");
  const bool long_tail_grammar_integration_closeout_key_shape_deterministic =
      Objc3ParseLoweringReadinessKeyHasPrefix(
          long_tail_grammar_integration_closeout_key,
          "conformance_matrix_ready=");
  const bool parse_lowering_performance_quality_guardrails_key_shape_deterministic =
      Objc3ParseLoweringReadinessKeyHasPrefix(
          parse_lowering_performance_quality_guardrails_key,
          "case_count=");
  return std::string("toolchain_runtime_ga_operations_docs_runbook_sync_consistent=") +
         (toolchain_runtime_ga_operations_docs_runbook_sync_consistent ? "true" : "false") +
         ";toolchain_runtime_ga_operations_docs_runbook_sync_ready=" +
         (toolchain_runtime_ga_operations_docs_runbook_sync_ready ? "true" : "false") +
         ";docs_runbook_sync_key_shape_deterministic=" +
         (docs_runbook_sync_key_shape_deterministic ? "true" : "false") +
         ";long_tail_grammar_integration_closeout_key_shape_deterministic=" +
         (long_tail_grammar_integration_closeout_key_shape_deterministic ? "true" : "false") +
         ";parse_lowering_performance_quality_guardrails_key_shape_deterministic=" +
         (parse_lowering_performance_quality_guardrails_key_shape_deterministic ? "true" : "false") +
         ";consistent=" +
         (toolchain_runtime_ga_operations_advanced_core_consistent ? "true" : "false") +
         ";ready=" + (toolchain_runtime_ga_operations_advanced_core_ready ? "true" : "false");
}

inline bool IsObjc3ToolchainRuntimeGaOperationsAdvancedEdgeCompatibilityConsistent(
    bool toolchain_runtime_ga_operations_advanced_core_consistent,
    bool toolchain_runtime_ga_operations_advanced_core_ready,
    bool long_tail_grammar_integration_closeout_consistent,
    bool long_tail_grammar_gate_signoff_ready,
    const std::string &toolchain_runtime_ga_operations_advanced_core_key,
    const std::string &long_tail_grammar_integration_closeout_key,
    const std::string &parse_lowering_performance_quality_guardrails_key) {
  return toolchain_runtime_ga_operations_advanced_core_consistent &&
         toolchain_runtime_ga_operations_advanced_core_ready &&
         long_tail_grammar_integration_closeout_consistent &&
         long_tail_grammar_gate_signoff_ready &&
         Objc3ParseLoweringReadinessKeyHasPrefix(
             toolchain_runtime_ga_operations_advanced_core_key,
             "toolchain_runtime_ga_operations_docs_runbook_sync_consistent=") &&
         Objc3ParseLoweringReadinessKeyHasPrefix(
             long_tail_grammar_integration_closeout_key,
             "conformance_matrix_ready=") &&
         Objc3ParseLoweringReadinessKeyHasPrefix(
             parse_lowering_performance_quality_guardrails_key,
             "case_count=");
}

inline bool IsObjc3ToolchainRuntimeGaOperationsAdvancedEdgeCompatibilityReady(
    bool toolchain_runtime_ga_operations_advanced_edge_compatibility_consistent,
    const std::string &toolchain_runtime_ga_operations_advanced_core_key,
    const std::string &long_tail_grammar_integration_closeout_key) {
  return toolchain_runtime_ga_operations_advanced_edge_compatibility_consistent &&
         Objc3ParseLoweringReadinessKeyHasPrefix(
             toolchain_runtime_ga_operations_advanced_core_key,
             "toolchain_runtime_ga_operations_docs_runbook_sync_consistent=") &&
         Objc3ParseLoweringReadinessKeyHasPrefix(
             long_tail_grammar_integration_closeout_key,
             "conformance_matrix_ready=");
}

inline std::string BuildObjc3ToolchainRuntimeGaOperationsAdvancedEdgeCompatibilityKey(
    bool toolchain_runtime_ga_operations_advanced_core_consistent,
    bool toolchain_runtime_ga_operations_advanced_core_ready,
    const std::string &toolchain_runtime_ga_operations_advanced_core_key,
    const std::string &long_tail_grammar_integration_closeout_key,
    const std::string &parse_lowering_performance_quality_guardrails_key,
    bool toolchain_runtime_ga_operations_advanced_edge_compatibility_consistent,
    bool toolchain_runtime_ga_operations_advanced_edge_compatibility_ready) {
  const bool advanced_core_key_shape_deterministic =
      Objc3ParseLoweringReadinessKeyHasPrefix(
          toolchain_runtime_ga_operations_advanced_core_key,
          "toolchain_runtime_ga_operations_docs_runbook_sync_consistent=");
  const bool long_tail_grammar_integration_closeout_key_shape_deterministic =
      Objc3ParseLoweringReadinessKeyHasPrefix(
          long_tail_grammar_integration_closeout_key,
          "conformance_matrix_ready=");
  const bool parse_lowering_performance_quality_guardrails_key_shape_deterministic =
      Objc3ParseLoweringReadinessKeyHasPrefix(
          parse_lowering_performance_quality_guardrails_key,
          "case_count=");
  return std::string("toolchain_runtime_ga_operations_advanced_core_consistent=") +
         (toolchain_runtime_ga_operations_advanced_core_consistent ? "true" : "false") +
         ";toolchain_runtime_ga_operations_advanced_core_ready=" +
         (toolchain_runtime_ga_operations_advanced_core_ready ? "true" : "false") +
         ";advanced_core_key_shape_deterministic=" +
         (advanced_core_key_shape_deterministic ? "true" : "false") +
         ";long_tail_grammar_integration_closeout_key_shape_deterministic=" +
         (long_tail_grammar_integration_closeout_key_shape_deterministic ? "true" : "false") +
         ";parse_lowering_performance_quality_guardrails_key_shape_deterministic=" +
         (parse_lowering_performance_quality_guardrails_key_shape_deterministic ? "true" : "false") +
         ";consistent=" +
         (toolchain_runtime_ga_operations_advanced_edge_compatibility_consistent ? "true" : "false") +
         ";ready=" + (toolchain_runtime_ga_operations_advanced_edge_compatibility_ready ? "true" : "false");
}

inline bool IsObjc3ToolchainRuntimeGaOperationsAdvancedDiagnosticsConsistent(
    bool toolchain_runtime_ga_operations_advanced_edge_compatibility_consistent,
    bool toolchain_runtime_ga_operations_advanced_edge_compatibility_ready,
    bool long_tail_grammar_integration_closeout_consistent,
    bool long_tail_grammar_gate_signoff_ready,
    const std::string &toolchain_runtime_ga_operations_advanced_edge_compatibility_key,
    const std::string &long_tail_grammar_integration_closeout_key,
    const std::string &parse_lowering_performance_quality_guardrails_key) {
  return toolchain_runtime_ga_operations_advanced_edge_compatibility_consistent &&
         toolchain_runtime_ga_operations_advanced_edge_compatibility_ready &&
         long_tail_grammar_integration_closeout_consistent &&
         long_tail_grammar_gate_signoff_ready &&
         Objc3ParseLoweringReadinessKeyHasPrefix(
             toolchain_runtime_ga_operations_advanced_edge_compatibility_key,
             "toolchain_runtime_ga_operations_advanced_core_consistent=") &&
         Objc3ParseLoweringReadinessKeyHasPrefix(
             long_tail_grammar_integration_closeout_key,
             "conformance_matrix_ready=") &&
         Objc3ParseLoweringReadinessKeyHasPrefix(
             parse_lowering_performance_quality_guardrails_key,
             "case_count=");
}

inline bool IsObjc3ToolchainRuntimeGaOperationsAdvancedDiagnosticsReady(
    bool toolchain_runtime_ga_operations_advanced_diagnostics_consistent,
    const std::string &toolchain_runtime_ga_operations_advanced_edge_compatibility_key,
    const std::string &long_tail_grammar_integration_closeout_key) {
  return toolchain_runtime_ga_operations_advanced_diagnostics_consistent &&
         Objc3ParseLoweringReadinessKeyHasPrefix(
             toolchain_runtime_ga_operations_advanced_edge_compatibility_key,
             "toolchain_runtime_ga_operations_advanced_core_consistent=") &&
         Objc3ParseLoweringReadinessKeyHasPrefix(
             long_tail_grammar_integration_closeout_key,
             "conformance_matrix_ready=");
}

inline std::string BuildObjc3ToolchainRuntimeGaOperationsAdvancedDiagnosticsKey(
    bool toolchain_runtime_ga_operations_advanced_edge_compatibility_consistent,
    bool toolchain_runtime_ga_operations_advanced_edge_compatibility_ready,
    const std::string &toolchain_runtime_ga_operations_advanced_edge_compatibility_key,
    const std::string &long_tail_grammar_integration_closeout_key,
    const std::string &parse_lowering_performance_quality_guardrails_key,
    bool toolchain_runtime_ga_operations_advanced_diagnostics_consistent,
    bool toolchain_runtime_ga_operations_advanced_diagnostics_ready) {
  const bool advanced_edge_compatibility_key_shape_deterministic =
      Objc3ParseLoweringReadinessKeyHasPrefix(
          toolchain_runtime_ga_operations_advanced_edge_compatibility_key,
          "toolchain_runtime_ga_operations_advanced_core_consistent=");
  const bool long_tail_grammar_integration_closeout_key_shape_deterministic =
      Objc3ParseLoweringReadinessKeyHasPrefix(
          long_tail_grammar_integration_closeout_key,
          "conformance_matrix_ready=");
  const bool parse_lowering_performance_quality_guardrails_key_shape_deterministic =
      Objc3ParseLoweringReadinessKeyHasPrefix(
          parse_lowering_performance_quality_guardrails_key,
          "case_count=");
  return std::string("toolchain_runtime_ga_operations_advanced_edge_compatibility_consistent=") +
         (toolchain_runtime_ga_operations_advanced_edge_compatibility_consistent ? "true" : "false") +
         ";toolchain_runtime_ga_operations_advanced_edge_compatibility_ready=" +
         (toolchain_runtime_ga_operations_advanced_edge_compatibility_ready ? "true" : "false") +
         ";advanced_edge_compatibility_key_shape_deterministic=" +
         (advanced_edge_compatibility_key_shape_deterministic ? "true" : "false") +
         ";long_tail_grammar_integration_closeout_key_shape_deterministic=" +
         (long_tail_grammar_integration_closeout_key_shape_deterministic ? "true" : "false") +
         ";parse_lowering_performance_quality_guardrails_key_shape_deterministic=" +
         (parse_lowering_performance_quality_guardrails_key_shape_deterministic ? "true" : "false") +
         ";consistent=" +
         (toolchain_runtime_ga_operations_advanced_diagnostics_consistent ? "true" : "false") +
         ";ready=" + (toolchain_runtime_ga_operations_advanced_diagnostics_ready ? "true" : "false");
}

inline bool IsObjc3ToolchainRuntimeGaOperationsAdvancedConformanceConsistent(
    bool toolchain_runtime_ga_operations_advanced_diagnostics_consistent,
    bool toolchain_runtime_ga_operations_advanced_diagnostics_ready,
    bool long_tail_grammar_integration_closeout_consistent,
    bool long_tail_grammar_gate_signoff_ready,
    const std::string &toolchain_runtime_ga_operations_advanced_diagnostics_key,
    const std::string &long_tail_grammar_integration_closeout_key,
    const std::string &parse_lowering_performance_quality_guardrails_key) {
  return toolchain_runtime_ga_operations_advanced_diagnostics_consistent &&
         toolchain_runtime_ga_operations_advanced_diagnostics_ready &&
         long_tail_grammar_integration_closeout_consistent &&
         long_tail_grammar_gate_signoff_ready &&
         Objc3ParseLoweringReadinessKeyHasPrefix(
             toolchain_runtime_ga_operations_advanced_diagnostics_key,
             "toolchain_runtime_ga_operations_advanced_edge_compatibility_consistent=") &&
         Objc3ParseLoweringReadinessKeyHasPrefix(
             long_tail_grammar_integration_closeout_key,
             "conformance_matrix_ready=") &&
         Objc3ParseLoweringReadinessKeyHasPrefix(
             parse_lowering_performance_quality_guardrails_key,
             "case_count=");
}

inline bool IsObjc3ToolchainRuntimeGaOperationsAdvancedConformanceReady(
    bool toolchain_runtime_ga_operations_advanced_conformance_consistent,
    const std::string &toolchain_runtime_ga_operations_advanced_diagnostics_key,
    const std::string &long_tail_grammar_integration_closeout_key) {
  return toolchain_runtime_ga_operations_advanced_conformance_consistent &&
         Objc3ParseLoweringReadinessKeyHasPrefix(
             toolchain_runtime_ga_operations_advanced_diagnostics_key,
             "toolchain_runtime_ga_operations_advanced_edge_compatibility_consistent=") &&
         Objc3ParseLoweringReadinessKeyHasPrefix(
             long_tail_grammar_integration_closeout_key,
             "conformance_matrix_ready=");
}

inline std::string BuildObjc3ToolchainRuntimeGaOperationsAdvancedConformanceKey(
    bool toolchain_runtime_ga_operations_advanced_diagnostics_consistent,
    bool toolchain_runtime_ga_operations_advanced_diagnostics_ready,
    const std::string &toolchain_runtime_ga_operations_advanced_diagnostics_key,
    const std::string &long_tail_grammar_integration_closeout_key,
    const std::string &parse_lowering_performance_quality_guardrails_key,
    bool toolchain_runtime_ga_operations_advanced_conformance_consistent,
    bool toolchain_runtime_ga_operations_advanced_conformance_ready) {
  const bool advanced_diagnostics_key_shape_deterministic =
      Objc3ParseLoweringReadinessKeyHasPrefix(
          toolchain_runtime_ga_operations_advanced_diagnostics_key,
          "toolchain_runtime_ga_operations_advanced_edge_compatibility_consistent=");
  const bool long_tail_grammar_integration_closeout_key_shape_deterministic =
      Objc3ParseLoweringReadinessKeyHasPrefix(
          long_tail_grammar_integration_closeout_key,
          "conformance_matrix_ready=");
  const bool parse_lowering_performance_quality_guardrails_key_shape_deterministic =
      Objc3ParseLoweringReadinessKeyHasPrefix(
          parse_lowering_performance_quality_guardrails_key,
          "case_count=");
  return std::string("toolchain_runtime_ga_operations_advanced_diagnostics_consistent=") +
         (toolchain_runtime_ga_operations_advanced_diagnostics_consistent ? "true" : "false") +
         ";toolchain_runtime_ga_operations_advanced_diagnostics_ready=" +
         (toolchain_runtime_ga_operations_advanced_diagnostics_ready ? "true" : "false") +
         ";advanced_diagnostics_key_shape_deterministic=" +
         (advanced_diagnostics_key_shape_deterministic ? "true" : "false") +
         ";long_tail_grammar_integration_closeout_key_shape_deterministic=" +
         (long_tail_grammar_integration_closeout_key_shape_deterministic ? "true" : "false") +
         ";parse_lowering_performance_quality_guardrails_key_shape_deterministic=" +
         (parse_lowering_performance_quality_guardrails_key_shape_deterministic ? "true" : "false") +
         ";consistent=" +
         (toolchain_runtime_ga_operations_advanced_conformance_consistent ? "true" : "false") +
         ";ready=" + (toolchain_runtime_ga_operations_advanced_conformance_ready ? "true" : "false");
}

inline bool IsObjc3ToolchainRuntimeGaOperationsAdvancedIntegrationConsistent(
    bool toolchain_runtime_ga_operations_advanced_conformance_consistent,
    bool toolchain_runtime_ga_operations_advanced_conformance_ready,
    bool long_tail_grammar_integration_closeout_consistent,
    bool long_tail_grammar_gate_signoff_ready,
    const std::string &toolchain_runtime_ga_operations_advanced_conformance_key,
    const std::string &long_tail_grammar_integration_closeout_key,
    const std::string &parse_lowering_performance_quality_guardrails_key) {
  return toolchain_runtime_ga_operations_advanced_conformance_consistent &&
         toolchain_runtime_ga_operations_advanced_conformance_ready &&
         long_tail_grammar_integration_closeout_consistent &&
         long_tail_grammar_gate_signoff_ready &&
         Objc3ParseLoweringReadinessKeyHasPrefix(
             toolchain_runtime_ga_operations_advanced_conformance_key,
             "toolchain_runtime_ga_operations_advanced_diagnostics_consistent=") &&
         Objc3ParseLoweringReadinessKeyHasPrefix(
             long_tail_grammar_integration_closeout_key,
             "conformance_matrix_ready=") &&
         Objc3ParseLoweringReadinessKeyHasPrefix(
             parse_lowering_performance_quality_guardrails_key,
             "case_count=");
}

inline bool IsObjc3ToolchainRuntimeGaOperationsAdvancedIntegrationReady(
    bool toolchain_runtime_ga_operations_advanced_integration_consistent,
    const std::string &toolchain_runtime_ga_operations_advanced_conformance_key,
    const std::string &long_tail_grammar_integration_closeout_key) {
  return toolchain_runtime_ga_operations_advanced_integration_consistent &&
         Objc3ParseLoweringReadinessKeyHasPrefix(
             toolchain_runtime_ga_operations_advanced_conformance_key,
             "toolchain_runtime_ga_operations_advanced_diagnostics_consistent=") &&
         Objc3ParseLoweringReadinessKeyHasPrefix(
             long_tail_grammar_integration_closeout_key,
             "conformance_matrix_ready=");
}

inline std::string BuildObjc3ToolchainRuntimeGaOperationsAdvancedIntegrationKey(
    bool toolchain_runtime_ga_operations_advanced_conformance_consistent,
    bool toolchain_runtime_ga_operations_advanced_conformance_ready,
    const std::string &toolchain_runtime_ga_operations_advanced_conformance_key,
    const std::string &long_tail_grammar_integration_closeout_key,
    const std::string &parse_lowering_performance_quality_guardrails_key,
    bool toolchain_runtime_ga_operations_advanced_integration_consistent,
    bool toolchain_runtime_ga_operations_advanced_integration_ready) {
  const bool advanced_conformance_key_shape_deterministic =
      Objc3ParseLoweringReadinessKeyHasPrefix(
          toolchain_runtime_ga_operations_advanced_conformance_key,
          "toolchain_runtime_ga_operations_advanced_diagnostics_consistent=");
  const bool long_tail_grammar_integration_closeout_key_shape_deterministic =
      Objc3ParseLoweringReadinessKeyHasPrefix(
          long_tail_grammar_integration_closeout_key,
          "conformance_matrix_ready=");
  const bool parse_lowering_performance_quality_guardrails_key_shape_deterministic =
      Objc3ParseLoweringReadinessKeyHasPrefix(
          parse_lowering_performance_quality_guardrails_key,
          "case_count=");
  return std::string("toolchain_runtime_ga_operations_advanced_conformance_consistent=") +
         (toolchain_runtime_ga_operations_advanced_conformance_consistent ? "true" : "false") +
         ";toolchain_runtime_ga_operations_advanced_conformance_ready=" +
         (toolchain_runtime_ga_operations_advanced_conformance_ready ? "true" : "false") +
         ";advanced_conformance_key_shape_deterministic=" +
         (advanced_conformance_key_shape_deterministic ? "true" : "false") +
         ";long_tail_grammar_integration_closeout_key_shape_deterministic=" +
         (long_tail_grammar_integration_closeout_key_shape_deterministic ? "true" : "false") +
         ";parse_lowering_performance_quality_guardrails_key_shape_deterministic=" +
         (parse_lowering_performance_quality_guardrails_key_shape_deterministic ? "true" : "false") +
         ";consistent=" +
         (toolchain_runtime_ga_operations_advanced_integration_consistent ? "true" : "false") +
         ";ready=" + (toolchain_runtime_ga_operations_advanced_integration_ready ? "true" : "false");
}

inline bool IsObjc3ToolchainRuntimeGaOperationsAdvancedPerformanceConsistent(
    bool toolchain_runtime_ga_operations_advanced_integration_consistent,
    bool toolchain_runtime_ga_operations_advanced_integration_ready,
    bool long_tail_grammar_integration_closeout_consistent,
    bool long_tail_grammar_gate_signoff_ready,
    const std::string &toolchain_runtime_ga_operations_advanced_integration_key,
    const std::string &long_tail_grammar_integration_closeout_key,
    const std::string &parse_lowering_performance_quality_guardrails_key) {
  return toolchain_runtime_ga_operations_advanced_integration_consistent &&
         toolchain_runtime_ga_operations_advanced_integration_ready &&
         long_tail_grammar_integration_closeout_consistent &&
         long_tail_grammar_gate_signoff_ready &&
         Objc3ParseLoweringReadinessKeyHasPrefix(
             toolchain_runtime_ga_operations_advanced_integration_key,
             "toolchain_runtime_ga_operations_advanced_conformance_consistent=") &&
         Objc3ParseLoweringReadinessKeyHasPrefix(
             long_tail_grammar_integration_closeout_key,
             "conformance_matrix_ready=") &&
         Objc3ParseLoweringReadinessKeyHasPrefix(
             parse_lowering_performance_quality_guardrails_key,
             "case_count=");
}

inline bool IsObjc3ToolchainRuntimeGaOperationsAdvancedPerformanceReady(
    bool toolchain_runtime_ga_operations_advanced_performance_consistent,
    const std::string &toolchain_runtime_ga_operations_advanced_integration_key,
    const std::string &long_tail_grammar_integration_closeout_key) {
  return toolchain_runtime_ga_operations_advanced_performance_consistent &&
         Objc3ParseLoweringReadinessKeyHasPrefix(
             toolchain_runtime_ga_operations_advanced_integration_key,
             "toolchain_runtime_ga_operations_advanced_conformance_consistent=") &&
         Objc3ParseLoweringReadinessKeyHasPrefix(
             long_tail_grammar_integration_closeout_key,
             "conformance_matrix_ready=");
}

inline std::string BuildObjc3ToolchainRuntimeGaOperationsAdvancedPerformanceKey(
    bool toolchain_runtime_ga_operations_advanced_integration_consistent,
    bool toolchain_runtime_ga_operations_advanced_integration_ready,
    const std::string &toolchain_runtime_ga_operations_advanced_integration_key,
    const std::string &long_tail_grammar_integration_closeout_key,
    const std::string &parse_lowering_performance_quality_guardrails_key,
    bool toolchain_runtime_ga_operations_advanced_performance_consistent,
    bool toolchain_runtime_ga_operations_advanced_performance_ready) {
  const bool advanced_integration_key_shape_deterministic =
      Objc3ParseLoweringReadinessKeyHasPrefix(
          toolchain_runtime_ga_operations_advanced_integration_key,
          "toolchain_runtime_ga_operations_advanced_conformance_consistent=");
  const bool long_tail_grammar_integration_closeout_key_shape_deterministic =
      Objc3ParseLoweringReadinessKeyHasPrefix(
          long_tail_grammar_integration_closeout_key,
          "conformance_matrix_ready=");
  const bool parse_lowering_performance_quality_guardrails_key_shape_deterministic =
      Objc3ParseLoweringReadinessKeyHasPrefix(
          parse_lowering_performance_quality_guardrails_key,
          "case_count=");
  return std::string("toolchain_runtime_ga_operations_advanced_integration_consistent=") +
         (toolchain_runtime_ga_operations_advanced_integration_consistent ? "true" : "false") +
         ";toolchain_runtime_ga_operations_advanced_integration_ready=" +
         (toolchain_runtime_ga_operations_advanced_integration_ready ? "true" : "false") +
         ";advanced_integration_key_shape_deterministic=" +
         (advanced_integration_key_shape_deterministic ? "true" : "false") +
         ";long_tail_grammar_integration_closeout_key_shape_deterministic=" +
         (long_tail_grammar_integration_closeout_key_shape_deterministic ? "true" : "false") +
         ";parse_lowering_performance_quality_guardrails_key_shape_deterministic=" +
         (parse_lowering_performance_quality_guardrails_key_shape_deterministic ? "true" : "false") +
         ";consistent=" +
         (toolchain_runtime_ga_operations_advanced_performance_consistent ? "true" : "false") +
         ";ready=" + (toolchain_runtime_ga_operations_advanced_performance_ready ? "true" : "false");
}

inline bool IsObjc3ToolchainRuntimeGaOperationsAdvancedCoreShard2Consistent(
    bool toolchain_runtime_ga_operations_advanced_performance_consistent,
    bool toolchain_runtime_ga_operations_advanced_performance_ready,
    bool long_tail_grammar_integration_closeout_consistent,
    bool long_tail_grammar_gate_signoff_ready,
    const std::string &toolchain_runtime_ga_operations_advanced_performance_key,
    const std::string &long_tail_grammar_integration_closeout_key,
    const std::string &parse_lowering_performance_quality_guardrails_key) {
  return toolchain_runtime_ga_operations_advanced_performance_consistent &&
         toolchain_runtime_ga_operations_advanced_performance_ready &&
         long_tail_grammar_integration_closeout_consistent &&
         long_tail_grammar_gate_signoff_ready &&
         Objc3ParseLoweringReadinessKeyHasPrefix(
             toolchain_runtime_ga_operations_advanced_performance_key,
             "toolchain_runtime_ga_operations_advanced_integration_consistent=") &&
         Objc3ParseLoweringReadinessKeyHasPrefix(
             long_tail_grammar_integration_closeout_key,
             "conformance_matrix_ready=") &&
         Objc3ParseLoweringReadinessKeyHasPrefix(
             parse_lowering_performance_quality_guardrails_key,
             "case_count=");
}

inline bool IsObjc3ToolchainRuntimeGaOperationsAdvancedCoreShard2Ready(
    bool toolchain_runtime_ga_operations_advanced_core_shard2_consistent,
    const std::string &toolchain_runtime_ga_operations_advanced_performance_key,
    const std::string &long_tail_grammar_integration_closeout_key) {
  return toolchain_runtime_ga_operations_advanced_core_shard2_consistent &&
         Objc3ParseLoweringReadinessKeyHasPrefix(
             toolchain_runtime_ga_operations_advanced_performance_key,
             "toolchain_runtime_ga_operations_advanced_integration_consistent=") &&
         Objc3ParseLoweringReadinessKeyHasPrefix(
             long_tail_grammar_integration_closeout_key,
             "conformance_matrix_ready=");
}

inline std::string BuildObjc3ToolchainRuntimeGaOperationsAdvancedCoreShard2Key(
    bool toolchain_runtime_ga_operations_advanced_performance_consistent,
    bool toolchain_runtime_ga_operations_advanced_performance_ready,
    const std::string &toolchain_runtime_ga_operations_advanced_performance_key,
    const std::string &long_tail_grammar_integration_closeout_key,
    const std::string &parse_lowering_performance_quality_guardrails_key,
    bool toolchain_runtime_ga_operations_advanced_core_shard2_consistent,
    bool toolchain_runtime_ga_operations_advanced_core_shard2_ready) {
  const bool advanced_performance_key_shape_deterministic =
      Objc3ParseLoweringReadinessKeyHasPrefix(
          toolchain_runtime_ga_operations_advanced_performance_key,
          "toolchain_runtime_ga_operations_advanced_integration_consistent=");
  const bool long_tail_grammar_integration_closeout_key_shape_deterministic =
      Objc3ParseLoweringReadinessKeyHasPrefix(
          long_tail_grammar_integration_closeout_key,
          "conformance_matrix_ready=");
  const bool parse_lowering_performance_quality_guardrails_key_shape_deterministic =
      Objc3ParseLoweringReadinessKeyHasPrefix(
          parse_lowering_performance_quality_guardrails_key,
          "case_count=");
  return std::string("toolchain_runtime_ga_operations_advanced_performance_consistent=") +
         (toolchain_runtime_ga_operations_advanced_performance_consistent ? "true" : "false") +
         ";toolchain_runtime_ga_operations_advanced_performance_ready=" +
         (toolchain_runtime_ga_operations_advanced_performance_ready ? "true" : "false") +
         ";advanced_performance_key_shape_deterministic=" +
         (advanced_performance_key_shape_deterministic ? "true" : "false") +
         ";long_tail_grammar_integration_closeout_key_shape_deterministic=" +
         (long_tail_grammar_integration_closeout_key_shape_deterministic ? "true" : "false") +
         ";parse_lowering_performance_quality_guardrails_key_shape_deterministic=" +
         (parse_lowering_performance_quality_guardrails_key_shape_deterministic ? "true" : "false") +
         ";consistent=" +
         (toolchain_runtime_ga_operations_advanced_core_shard2_consistent ? "true" : "false") +
         ";ready=" + (toolchain_runtime_ga_operations_advanced_core_shard2_ready ? "true" : "false");
}

inline bool IsObjc3ToolchainRuntimeGaOperationsIntegrationCloseoutSignoffConsistent(
    bool toolchain_runtime_ga_operations_advanced_core_shard2_consistent,
    bool toolchain_runtime_ga_operations_advanced_core_shard2_ready,
    bool long_tail_grammar_integration_closeout_consistent,
    bool long_tail_grammar_gate_signoff_ready,
    const std::string &toolchain_runtime_ga_operations_advanced_core_shard2_key,
    const std::string &long_tail_grammar_integration_closeout_key,
    const std::string &parse_lowering_performance_quality_guardrails_key) {
  return toolchain_runtime_ga_operations_advanced_core_shard2_consistent &&
         toolchain_runtime_ga_operations_advanced_core_shard2_ready &&
         long_tail_grammar_integration_closeout_consistent &&
         long_tail_grammar_gate_signoff_ready &&
         Objc3ParseLoweringReadinessKeyHasPrefix(
             toolchain_runtime_ga_operations_advanced_core_shard2_key,
             "toolchain_runtime_ga_operations_advanced_performance_consistent=") &&
         Objc3ParseLoweringReadinessKeyHasPrefix(
             long_tail_grammar_integration_closeout_key,
             "conformance_matrix_ready=") &&
         Objc3ParseLoweringReadinessKeyHasPrefix(
             parse_lowering_performance_quality_guardrails_key,
             "case_count=");
}

inline bool IsObjc3ToolchainRuntimeGaOperationsIntegrationCloseoutSignoffReady(
    bool toolchain_runtime_ga_operations_integration_closeout_signoff_consistent,
    const std::string &toolchain_runtime_ga_operations_advanced_core_shard2_key,
    const std::string &long_tail_grammar_integration_closeout_key) {
  return toolchain_runtime_ga_operations_integration_closeout_signoff_consistent &&
         Objc3ParseLoweringReadinessKeyHasPrefix(
             toolchain_runtime_ga_operations_advanced_core_shard2_key,
             "toolchain_runtime_ga_operations_advanced_performance_consistent=") &&
         Objc3ParseLoweringReadinessKeyHasPrefix(
             long_tail_grammar_integration_closeout_key,
             "conformance_matrix_ready=");
}

inline std::string BuildObjc3ToolchainRuntimeGaOperationsIntegrationCloseoutSignoffKey(
    bool toolchain_runtime_ga_operations_advanced_core_shard2_consistent,
    bool toolchain_runtime_ga_operations_advanced_core_shard2_ready,
    const std::string &toolchain_runtime_ga_operations_advanced_core_shard2_key,
    const std::string &long_tail_grammar_integration_closeout_key,
    const std::string &parse_lowering_performance_quality_guardrails_key,
    bool toolchain_runtime_ga_operations_integration_closeout_signoff_consistent,
    bool toolchain_runtime_ga_operations_integration_closeout_signoff_ready) {
  const bool advanced_core_shard2_key_shape_deterministic =
      Objc3ParseLoweringReadinessKeyHasPrefix(
          toolchain_runtime_ga_operations_advanced_core_shard2_key,
          "toolchain_runtime_ga_operations_advanced_performance_consistent=");
  const bool long_tail_grammar_integration_closeout_key_shape_deterministic =
      Objc3ParseLoweringReadinessKeyHasPrefix(
          long_tail_grammar_integration_closeout_key,
          "conformance_matrix_ready=");
  const bool parse_lowering_performance_quality_guardrails_key_shape_deterministic =
      Objc3ParseLoweringReadinessKeyHasPrefix(
          parse_lowering_performance_quality_guardrails_key,
          "case_count=");
  return std::string("toolchain_runtime_ga_operations_advanced_core_shard2_consistent=") +
         (toolchain_runtime_ga_operations_advanced_core_shard2_consistent ? "true" : "false") +
         ";toolchain_runtime_ga_operations_advanced_core_shard2_ready=" +
         (toolchain_runtime_ga_operations_advanced_core_shard2_ready ? "true" : "false") +
         ";advanced_core_shard2_key_shape_deterministic=" +
         (advanced_core_shard2_key_shape_deterministic ? "true" : "false") +
         ";long_tail_grammar_integration_closeout_key_shape_deterministic=" +
         (long_tail_grammar_integration_closeout_key_shape_deterministic ? "true" : "false") +
         ";parse_lowering_performance_quality_guardrails_key_shape_deterministic=" +
         (parse_lowering_performance_quality_guardrails_key_shape_deterministic ? "true" : "false") +
         ";consistent=" +
         (toolchain_runtime_ga_operations_integration_closeout_signoff_consistent ? "true" : "false") +
         ";ready=" + (toolchain_runtime_ga_operations_integration_closeout_signoff_ready ? "true" : "false");
}

inline std::string BuildObjc3LongTailGrammarIntegrationCloseoutKey(
    bool conformance_matrix_ready,
    bool conformance_corpus_consistent,
    bool performance_guardrails_consistent,
    bool cross_lane_integration_consistent,
    bool cross_lane_integration_ready,
    bool recovery_determinism_ready,
    bool semantic_handoff_ready,
    bool lowering_boundary_ready,
    bool integration_closeout_consistent,
    bool gate_signoff_ready) {
  return std::string("conformance_matrix_ready=") +
         (conformance_matrix_ready ? "true" : "false") +
         ";conformance_corpus_consistent=" +
         (conformance_corpus_consistent ? "true" : "false") +
         ";performance_guardrails_consistent=" +
         (performance_guardrails_consistent ? "true" : "false") +
         ";cross_lane_integration_consistent=" +
         (cross_lane_integration_consistent ? "true" : "false") +
         ";cross_lane_integration_ready=" +
         (cross_lane_integration_ready ? "true" : "false") +
         ";recovery_determinism_ready=" +
         (recovery_determinism_ready ? "true" : "false") +
         ";semantic_handoff_ready=" +
         (semantic_handoff_ready ? "true" : "false") +
         ";lowering_boundary_ready=" +
         (lowering_boundary_ready ? "true" : "false") +
         ";integration_closeout_consistent=" +
         (integration_closeout_consistent ? "true" : "false") +
         ";gate_signoff_ready=" + (gate_signoff_ready ? "true" : "false");
}

inline constexpr std::size_t kObjc3ParseLoweringConformanceMatrixCaseCount = 8u;
inline constexpr std::size_t kObjc3ParseLoweringConformanceCorpusCaseCount = 8u;
inline constexpr std::size_t kObjc3ParseLoweringPerformanceQualityGuardrailsCaseCount = 6u;

inline std::string BuildObjc3ParseLoweringConformanceMatrixKey(
    std::size_t case_count,
    bool parser_contract_snapshot_present,
    bool parse_artifact_handoff_deterministic,
    bool parse_artifact_replay_key_deterministic,
    bool parse_artifact_diagnostics_hardening_consistent,
    bool parse_artifact_edge_case_robustness_consistent,
    bool parse_recovery_determinism_hardening_consistent,
    bool semantic_integration_surface_built,
    bool semantic_handoff_deterministic,
    bool lowering_boundary_ready,
    bool parse_lowering_conformance_matrix_consistent) {
  return "case_count=" + std::to_string(case_count) +
         ";parser_contract_snapshot_present=" + (parser_contract_snapshot_present ? "true" : "false") +
         ";parse_artifact_handoff_deterministic=" + (parse_artifact_handoff_deterministic ? "true" : "false") +
         ";parse_artifact_replay_key_deterministic=" + (parse_artifact_replay_key_deterministic ? "true" : "false") +
         ";parse_artifact_diagnostics_hardening_consistent=" +
         (parse_artifact_diagnostics_hardening_consistent ? "true" : "false") +
         ";parse_artifact_edge_case_robustness_consistent=" +
         (parse_artifact_edge_case_robustness_consistent ? "true" : "false") +
         ";parse_recovery_determinism_hardening_consistent=" +
         (parse_recovery_determinism_hardening_consistent ? "true" : "false") +
         ";semantic_integration_surface_built=" + (semantic_integration_surface_built ? "true" : "false") +
         ";semantic_handoff_deterministic=" + (semantic_handoff_deterministic ? "true" : "false") +
         ";lowering_boundary_ready=" + (lowering_boundary_ready ? "true" : "false") +
         ";consistent=" + (parse_lowering_conformance_matrix_consistent ? "true" : "false");
}

inline std::string BuildObjc3ParseLoweringConformanceCorpusKey(
    std::size_t case_count,
    std::size_t passed_case_count,
    std::size_t failed_case_count,
    bool parser_contract_snapshot_case_passed,
    bool parse_artifact_handoff_case_passed,
    bool parse_artifact_replay_case_passed,
    bool parse_artifact_diagnostics_hardening_case_passed,
    bool parse_artifact_edge_robustness_case_passed,
    bool parse_recovery_determinism_hardening_case_passed,
    bool semantic_handoff_case_passed,
    bool lowering_boundary_case_passed,
    bool parse_lowering_conformance_matrix_consistent,
    bool parse_lowering_conformance_corpus_consistent) {
  return "case_count=" + std::to_string(case_count) +
         ";passed_case_count=" + std::to_string(passed_case_count) +
         ";failed_case_count=" + std::to_string(failed_case_count) +
         ";parser_contract_snapshot_case_passed=" + (parser_contract_snapshot_case_passed ? "true" : "false") +
         ";parse_artifact_handoff_case_passed=" + (parse_artifact_handoff_case_passed ? "true" : "false") +
         ";parse_artifact_replay_case_passed=" + (parse_artifact_replay_case_passed ? "true" : "false") +
         ";parse_artifact_diagnostics_hardening_case_passed=" +
         (parse_artifact_diagnostics_hardening_case_passed ? "true" : "false") +
         ";parse_artifact_edge_robustness_case_passed=" +
         (parse_artifact_edge_robustness_case_passed ? "true" : "false") +
         ";parse_recovery_determinism_hardening_case_passed=" +
         (parse_recovery_determinism_hardening_case_passed ? "true" : "false") +
         ";semantic_handoff_case_passed=" + (semantic_handoff_case_passed ? "true" : "false") +
         ";lowering_boundary_case_passed=" + (lowering_boundary_case_passed ? "true" : "false") +
         ";parse_lowering_conformance_matrix_consistent=" +
         (parse_lowering_conformance_matrix_consistent ? "true" : "false") +
         ";consistent=" + (parse_lowering_conformance_corpus_consistent ? "true" : "false");
}

inline std::string BuildObjc3ParseLoweringPerformanceQualityGuardrailsKey(
    std::size_t case_count,
    std::size_t passed_case_count,
    std::size_t failed_case_count,
    bool parser_token_count_budget_consistent,
    bool parser_diagnostic_code_surface_deterministic,
    bool parse_artifact_diagnostics_hardening_consistent,
    bool parse_artifact_edge_case_robustness_consistent,
    bool parse_recovery_determinism_hardening_consistent,
    bool parse_lowering_conformance_corpus_consistent,
    bool parse_lowering_performance_quality_guardrails_consistent) {
  return "case_count=" + std::to_string(case_count) +
         ";passed_case_count=" + std::to_string(passed_case_count) +
         ";failed_case_count=" + std::to_string(failed_case_count) +
         ";parser_token_count_budget_consistent=" +
         (parser_token_count_budget_consistent ? "true" : "false") +
         ";parser_diagnostic_code_surface_deterministic=" +
         (parser_diagnostic_code_surface_deterministic ? "true" : "false") +
         ";parse_artifact_diagnostics_hardening_consistent=" +
         (parse_artifact_diagnostics_hardening_consistent ? "true" : "false") +
         ";parse_artifact_edge_case_robustness_consistent=" +
         (parse_artifact_edge_case_robustness_consistent ? "true" : "false") +
         ";parse_recovery_determinism_hardening_consistent=" +
         (parse_recovery_determinism_hardening_consistent ? "true" : "false") +
         ";parse_lowering_conformance_corpus_consistent=" +
         (parse_lowering_conformance_corpus_consistent ? "true" : "false") +
         ";consistent=" +
         (parse_lowering_performance_quality_guardrails_consistent ? "true" : "false");
}

inline bool HasObjc3TypedSemaToLoweringCoreFeatureSurface(
    const Objc3TypedSemaToLoweringContractSurface &surface) {
  return surface.typed_core_feature_case_count > 0 ||
         surface.typed_core_feature_expansion_case_count > 0 ||
         surface.typed_core_feature_edge_case_compatibility_ready ||
         !surface.typed_handoff_key.empty() ||
         !surface.typed_core_feature_key.empty() ||
         !surface.typed_core_feature_expansion_key.empty() ||
         !surface.typed_core_feature_edge_case_compatibility_key.empty() ||
         surface.ready_for_lowering ||
         !surface.failure_reason.empty();
}

inline Objc3ParseLoweringReadinessSurface BuildObjc3ParseLoweringReadinessSurface(
    const Objc3FrontendPipelineResult &pipeline_result,
    const Objc3FrontendOptions &options) {
  Objc3ParseLoweringReadinessSurface surface;
  surface.parse_lowering_conformance_matrix_case_count =
      kObjc3ParseLoweringConformanceMatrixCaseCount;
  surface.parse_lowering_conformance_corpus_case_count =
      kObjc3ParseLoweringConformanceCorpusCaseCount;
  surface.parse_lowering_performance_quality_guardrails_case_count =
      kObjc3ParseLoweringPerformanceQualityGuardrailsCaseCount;
  const Objc3ParserContractSnapshot &parser_snapshot = pipeline_result.parser_contract_snapshot;
  surface.lexer_diagnostic_count = pipeline_result.stage_diagnostics.lexer.size();
  surface.parser_diagnostic_count = pipeline_result.stage_diagnostics.parser.size();
  surface.semantic_diagnostic_count = pipeline_result.stage_diagnostics.semantic.size();
  surface.parser_token_count = parser_snapshot.token_count;
  surface.parser_top_level_declaration_count = parser_snapshot.top_level_declaration_count;
  surface.parser_contract_snapshot_fingerprint = BuildObjc3ParserContractSnapshotFingerprint(parser_snapshot);
  surface.parser_ast_shape_fingerprint = parser_snapshot.ast_shape_fingerprint;
  surface.parser_ast_top_level_layout_fingerprint = parser_snapshot.ast_top_level_layout_fingerprint;
  surface.ast_shape_fingerprint = BuildObjc3ParsedProgramAstShapeFingerprint(pipeline_result.program);
  surface.ast_top_level_layout_fingerprint =
      BuildObjc3ParsedProgramTopLevelLayoutFingerprint(pipeline_result.program);
  surface.parser_contract_snapshot_present =
      parser_snapshot.token_count > 0 ||
      parser_snapshot.top_level_declaration_count > 0 ||
      parser_snapshot.parser_diagnostic_count > 0;
  surface.parser_contract_deterministic = parser_snapshot.deterministic_handoff;
  surface.parser_recovery_replay_ready = parser_snapshot.parser_recovery_replay_ready;
  surface.long_tail_grammar_construct_count = parser_snapshot.long_tail_grammar_construct_count;
  surface.long_tail_grammar_covered_construct_count = parser_snapshot.long_tail_grammar_covered_construct_count;
  surface.long_tail_grammar_fingerprint = parser_snapshot.long_tail_grammar_fingerprint;
  surface.long_tail_grammar_handoff_key = parser_snapshot.long_tail_grammar_handoff_key;
  surface.long_tail_grammar_core_feature_consistent =
      parser_snapshot.long_tail_grammar_covered_construct_count <=
          parser_snapshot.long_tail_grammar_construct_count &&
      parser_snapshot.long_tail_grammar_fingerprint != 0 &&
      !parser_snapshot.long_tail_grammar_handoff_key.empty() &&
      parser_snapshot.long_tail_grammar_handoff_deterministic;
  surface.long_tail_grammar_handoff_key_deterministic =
      parser_snapshot.long_tail_grammar_handoff_deterministic &&
      surface.long_tail_grammar_core_feature_consistent;
  surface.long_tail_grammar_expansion_accounting_consistent =
      surface.long_tail_grammar_covered_construct_count <=
          surface.long_tail_grammar_construct_count &&
      surface.long_tail_grammar_fingerprint != 0 &&
      surface.long_tail_grammar_core_feature_consistent;
  const std::size_t parser_snapshot_breakdown_count =
      Objc3ParserSnapshotDeclarationBreakdownCount(parser_snapshot);
  const std::size_t ast_top_level_declaration_count =
      Objc3ParsedProgramTopLevelDeclarationCount(pipeline_result.program);
  const Objc3ParseLoweringDiagnosticCodeCoverage parser_diagnostic_code_coverage =
      BuildObjc3ParseLoweringDiagnosticCodeCoverage(pipeline_result.stage_diagnostics.parser);
  const Objc3ParserDiagnosticSourcePrecisionScaffold parser_diagnostic_source_precision_scaffold =
      BuildObjc3ParserDiagnosticSourcePrecisionScaffold(
          pipeline_result.stage_diagnostics.parser,
          parser_snapshot);
  const Objc3DiagnosticGrammarHooksCoreFeatureSurface parser_diagnostic_grammar_hooks_core_feature =
      BuildObjc3DiagnosticGrammarHooksCoreFeatureSurface(
          pipeline_result.stage_diagnostics.parser,
          parser_snapshot,
          parser_diagnostic_source_precision_scaffold);
  const Objc3DiagnosticGrammarHooksCoreFeatureExpansionSurface
      parser_diagnostic_grammar_hooks_core_feature_expansion =
          BuildObjc3DiagnosticGrammarHooksCoreFeatureExpansionSurface(
              parser_diagnostic_grammar_hooks_core_feature,
              parser_diagnostic_source_precision_scaffold,
              parser_diagnostic_code_coverage.unique_code_count,
              parser_diagnostic_code_coverage.unique_code_fingerprint,
              surface.parser_diagnostic_count,
              parser_snapshot.parser_diagnostic_count);
  const bool parser_snapshot_breakdown_consistent =
      parser_snapshot_breakdown_count == parser_snapshot.top_level_declaration_count;
  surface.parser_diagnostic_surface_consistent =
      parser_snapshot.parser_diagnostic_count == surface.parser_diagnostic_count;
  surface.parser_diagnostic_coordinate_tagged_count =
      parser_diagnostic_source_precision_scaffold.coordinate_tagged_diagnostic_count;
  surface.parser_diagnostic_source_precision_fingerprint =
      parser_diagnostic_source_precision_scaffold.coordinate_fingerprint;
  surface.parser_diagnostic_source_precision_scaffold_key =
      parser_diagnostic_source_precision_scaffold.scaffold_key;
  surface.parser_diagnostic_source_precision_scaffold_consistent =
      parser_diagnostic_source_precision_scaffold.scaffold_consistent;
  surface.parser_diagnostic_source_precision_scaffold_ready =
      IsObjc3ParserDiagnosticSourcePrecisionScaffoldReady(
          parser_diagnostic_source_precision_scaffold);
  surface.parser_diagnostic_grammar_hook_code_count =
      parser_diagnostic_grammar_hooks_core_feature.grammar_hook_code_count;
  surface.parser_diagnostic_grammar_hooks_core_feature_consistent =
      parser_diagnostic_grammar_hooks_core_feature.core_feature_consistent;
  surface.parser_diagnostic_grammar_hooks_core_feature_key =
      parser_diagnostic_grammar_hooks_core_feature.core_feature_key;
  surface.parser_diagnostic_grammar_hooks_core_feature_ready =
      IsObjc3DiagnosticGrammarHooksCoreFeatureReady(
          parser_diagnostic_grammar_hooks_core_feature);
  surface.parser_diagnostic_grammar_hook_unique_code_count =
      parser_diagnostic_grammar_hooks_core_feature_expansion.unique_diagnostic_code_count;
  surface.parser_diagnostic_grammar_hooks_core_feature_expansion_accounting_consistent =
      parser_diagnostic_grammar_hooks_core_feature_expansion.accounting_consistent;
  surface.parser_diagnostic_grammar_hooks_core_feature_expansion_replay_keys_ready =
      parser_diagnostic_grammar_hooks_core_feature_expansion.replay_keys_ready;
  surface.parser_diagnostic_grammar_hooks_core_feature_expansion_key =
      parser_diagnostic_grammar_hooks_core_feature_expansion.expansion_key;
  surface.parser_diagnostic_grammar_hooks_core_feature_expansion_ready =
      IsObjc3DiagnosticGrammarHooksCoreFeatureExpansionReady(
          parser_diagnostic_grammar_hooks_core_feature_expansion);
  surface.parser_diagnostic_code_count = parser_diagnostic_code_coverage.unique_code_count;
  surface.parser_diagnostic_code_fingerprint = parser_diagnostic_code_coverage.unique_code_fingerprint;
  surface.parser_diagnostic_code_surface_deterministic =
      parser_diagnostic_code_coverage.deterministic_surface &&
      surface.parser_diagnostic_code_count <= surface.parser_diagnostic_count;
  surface.parse_artifact_diagnostics_hardening_consistent =
      surface.parser_diagnostic_surface_consistent &&
      surface.parser_diagnostic_code_surface_deterministic &&
      surface.parser_diagnostic_source_precision_scaffold_consistent &&
      surface.parser_diagnostic_source_precision_scaffold_ready &&
      surface.parser_diagnostic_grammar_hooks_core_feature_consistent &&
      surface.parser_diagnostic_grammar_hooks_core_feature_ready &&
      surface.parser_diagnostic_grammar_hooks_core_feature_expansion_accounting_consistent &&
      surface.parser_diagnostic_grammar_hooks_core_feature_expansion_replay_keys_ready &&
      surface.parser_diagnostic_grammar_hooks_core_feature_expansion_ready &&
      !surface.parser_diagnostic_source_precision_scaffold_key.empty() &&
      !surface.parser_diagnostic_grammar_hooks_core_feature_key.empty() &&
      !surface.parser_diagnostic_grammar_hooks_core_feature_expansion_key.empty();
  surface.parser_token_count_budget_consistent =
      surface.parser_token_count >= parser_snapshot_breakdown_count &&
      surface.parser_token_count >= parser_snapshot.top_level_declaration_count &&
      surface.parser_token_count >= ast_top_level_declaration_count;
  surface.parse_artifact_handoff_consistent =
      parser_snapshot_breakdown_consistent &&
      ast_top_level_declaration_count == parser_snapshot.top_level_declaration_count;
  surface.parse_artifact_handoff_deterministic =
      surface.parse_artifact_handoff_consistent &&
      surface.parser_diagnostic_surface_consistent &&
      surface.parser_contract_deterministic;
  surface.parse_artifact_layout_fingerprint_consistent =
      surface.parser_ast_top_level_layout_fingerprint == surface.ast_top_level_layout_fingerprint;
  surface.parse_artifact_fingerprint_consistent =
      surface.parser_ast_shape_fingerprint == surface.ast_shape_fingerprint &&
      surface.parse_artifact_layout_fingerprint_consistent;
  const std::size_t legacy_literal_total = pipeline_result.migration_hints.legacy_total();
  const bool migration_hints_consistent =
      legacy_literal_total ==
          pipeline_result.migration_hints.legacy_yes_count +
              pipeline_result.migration_hints.legacy_no_count +
              pipeline_result.migration_hints.legacy_null_count &&
      legacy_literal_total <= surface.parser_token_count &&
      (options.migration_assist || legacy_literal_total == 0);
  const bool language_version_pragma_contract_consistent =
      IsObjc3LanguageVersionPragmaContractConsistent(
          pipeline_result.language_version_pragma_contract);
  surface.language_version_pragma_coordinate_order_consistent =
      IsObjc3LanguageVersionPragmaCoordinateOrderConsistent(
          pipeline_result.language_version_pragma_contract);
  surface.compatibility_handoff_consistent =
      migration_hints_consistent &&
      language_version_pragma_contract_consistent;
  surface.compatibility_handoff_key = BuildObjc3CompatibilityHandoffKey(
      options,
      pipeline_result.migration_hints,
      pipeline_result.language_version_pragma_contract,
      surface.compatibility_handoff_consistent);
  const Objc3DiagnosticGrammarHooksEdgeCaseCompatibilitySurface
      parser_diagnostic_grammar_hooks_edge_case_compatibility =
          BuildObjc3DiagnosticGrammarHooksEdgeCaseCompatibilitySurface(
              parser_diagnostic_grammar_hooks_core_feature_expansion,
              options,
              pipeline_result.language_version_pragma_contract,
              surface.parser_diagnostic_count,
              parser_snapshot.parser_diagnostic_count,
              surface.parser_token_count,
              surface.compatibility_handoff_consistent);
  surface.parser_diagnostic_grammar_hooks_edge_case_compatibility_consistent =
      parser_diagnostic_grammar_hooks_edge_case_compatibility
          .edge_case_compatibility_consistent;
  surface.parser_diagnostic_grammar_hooks_edge_case_compatibility_key =
      parser_diagnostic_grammar_hooks_edge_case_compatibility.compatibility_key;
  surface.parser_diagnostic_grammar_hooks_edge_case_compatibility_ready =
      IsObjc3DiagnosticGrammarHooksEdgeCaseCompatibilitySurfaceReady(
          parser_diagnostic_grammar_hooks_edge_case_compatibility);
  surface.parse_artifact_replay_key_deterministic =
      surface.parse_artifact_handoff_deterministic &&
      surface.parse_artifact_fingerprint_consistent &&
      surface.compatibility_handoff_consistent &&
      surface.parser_contract_snapshot_fingerprint != 0;
  surface.parse_artifact_handoff_key = BuildObjc3ParseArtifactHandoffKey(
      parser_snapshot,
      ast_top_level_declaration_count,
      surface.parser_diagnostic_count,
      surface.parse_artifact_handoff_deterministic);
  surface.parse_artifact_replay_key = BuildObjc3ParseArtifactReplayKey(
      parser_snapshot,
      surface.parser_contract_snapshot_fingerprint,
      surface.parser_ast_top_level_layout_fingerprint,
      surface.ast_shape_fingerprint,
      surface.ast_top_level_layout_fingerprint,
      surface.compatibility_handoff_key,
      surface.parse_artifact_fingerprint_consistent,
      surface.parse_artifact_replay_key_deterministic);
  surface.long_tail_grammar_replay_keys_ready =
      !surface.long_tail_grammar_handoff_key.empty() &&
      !surface.parse_artifact_handoff_key.empty() &&
      !surface.parse_artifact_replay_key.empty();
  surface.long_tail_grammar_expansion_ready =
      surface.long_tail_grammar_expansion_accounting_consistent &&
      surface.long_tail_grammar_handoff_key_deterministic &&
      surface.long_tail_grammar_replay_keys_ready;
  surface.long_tail_grammar_expansion_key = BuildObjc3LongTailGrammarExpansionKey(
      surface.long_tail_grammar_construct_count,
      surface.long_tail_grammar_covered_construct_count,
      surface.long_tail_grammar_fingerprint,
      surface.long_tail_grammar_core_feature_consistent,
      surface.long_tail_grammar_handoff_key_deterministic,
      surface.long_tail_grammar_expansion_accounting_consistent,
      surface.long_tail_grammar_replay_keys_ready,
      surface.long_tail_grammar_expansion_ready);
  surface.long_tail_grammar_compatibility_handoff_ready =
      surface.long_tail_grammar_expansion_ready &&
      (surface.compatibility_handoff_consistent) &&
      !surface.compatibility_handoff_key.empty();
  surface.parse_artifact_diagnostics_hardening_key =
      BuildObjc3ParseArtifactDiagnosticsHardeningKey(
          surface.parser_diagnostic_count,
          parser_snapshot.parser_diagnostic_count,
          surface.parser_diagnostic_code_count,
          surface.parser_diagnostic_code_fingerprint,
          surface.parser_diagnostic_surface_consistent,
          surface.parser_diagnostic_code_surface_deterministic,
          surface.parser_diagnostic_source_precision_scaffold_consistent,
          surface.parser_diagnostic_grammar_hooks_core_feature_consistent,
          surface.parser_diagnostic_grammar_hooks_core_feature_expansion_accounting_consistent,
          surface.parser_diagnostic_grammar_hooks_core_feature_expansion_replay_keys_ready,
          surface.parser_diagnostic_grammar_hooks_core_feature_expansion_ready,
          surface.parser_diagnostic_source_precision_scaffold_key,
          surface.parser_diagnostic_grammar_hooks_core_feature_key,
          surface.parser_diagnostic_grammar_hooks_core_feature_expansion_key,
          surface.parse_artifact_diagnostics_hardening_consistent);
  surface.parse_artifact_edge_case_robustness_consistent =
      surface.parser_token_count_budget_consistent &&
      surface.language_version_pragma_coordinate_order_consistent &&
      surface.parser_diagnostic_grammar_hooks_edge_case_compatibility_consistent &&
      surface.parser_diagnostic_grammar_hooks_edge_case_compatibility_ready &&
      surface.parse_artifact_diagnostics_hardening_consistent &&
      !surface.parse_artifact_handoff_key.empty() &&
      !surface.compatibility_handoff_key.empty() &&
      !surface.parse_artifact_replay_key.empty() &&
      !surface.parse_artifact_diagnostics_hardening_key.empty() &&
      !surface.parser_diagnostic_grammar_hooks_edge_case_compatibility_key.empty();
  surface.parse_artifact_edge_robustness_key = BuildObjc3ParseArtifactEdgeRobustnessKey(
      surface.parser_token_count,
      parser_snapshot_breakdown_count,
      ast_top_level_declaration_count,
      surface.parser_token_count_budget_consistent,
      surface.language_version_pragma_coordinate_order_consistent,
      surface.parse_artifact_edge_case_robustness_consistent);
  surface.parser_diagnostic_grammar_hooks_edge_case_expansion_consistent =
      surface.parser_diagnostic_grammar_hooks_edge_case_compatibility_consistent &&
      surface.parser_diagnostic_grammar_hooks_edge_case_compatibility_ready &&
      surface.parse_artifact_edge_case_robustness_consistent &&
      surface.parser_diagnostic_code_surface_deterministic &&
      surface.parser_diagnostic_source_precision_scaffold_ready &&
      !surface.parser_diagnostic_grammar_hooks_edge_case_compatibility_key.empty() &&
      !surface.parse_artifact_diagnostics_hardening_key.empty();
  surface.parser_diagnostic_grammar_hooks_edge_case_robustness_ready =
      surface.parser_diagnostic_grammar_hooks_edge_case_expansion_consistent &&
      surface.parse_artifact_replay_key_deterministic &&
      surface.language_version_pragma_coordinate_order_consistent;
  surface.parser_diagnostic_grammar_hooks_edge_case_robustness_key =
      BuildObjc3DiagnosticGrammarHooksEdgeCaseRobustnessKey(
          surface.parser_diagnostic_count,
          surface.parser_diagnostic_code_count,
          surface.parser_diagnostic_code_fingerprint,
          surface.parser_diagnostic_grammar_hooks_edge_case_compatibility_consistent,
          surface.parser_diagnostic_grammar_hooks_edge_case_compatibility_ready,
          surface.parse_artifact_edge_case_robustness_consistent,
          surface.parser_diagnostic_grammar_hooks_edge_case_expansion_consistent,
          surface.parser_diagnostic_grammar_hooks_edge_case_robustness_ready);
  surface.parser_diagnostic_grammar_hooks_diagnostics_hardening_consistent =
      surface.parser_diagnostic_grammar_hooks_edge_case_robustness_ready &&
      surface.parse_artifact_diagnostics_hardening_consistent &&
      surface.parser_diagnostic_surface_consistent &&
      surface.parser_diagnostic_code_surface_deterministic &&
      !surface.parser_diagnostic_grammar_hooks_edge_case_robustness_key.empty() &&
      !surface.parse_artifact_diagnostics_hardening_key.empty();
  surface.parser_diagnostic_grammar_hooks_diagnostics_hardening_ready =
      surface.parser_diagnostic_grammar_hooks_diagnostics_hardening_consistent &&
      surface.parse_artifact_replay_key_deterministic;
  surface.parser_diagnostic_grammar_hooks_diagnostics_hardening_key =
      BuildObjc3DiagnosticGrammarHooksDiagnosticsHardeningKey(
          surface.parser_diagnostic_count,
          surface.parser_diagnostic_code_count,
          surface.parser_diagnostic_code_fingerprint,
          surface.parser_diagnostic_grammar_hooks_edge_case_robustness_ready,
          surface.parse_artifact_diagnostics_hardening_consistent,
          surface.parser_diagnostic_grammar_hooks_diagnostics_hardening_consistent,
          surface.parser_diagnostic_grammar_hooks_diagnostics_hardening_ready);
  surface.long_tail_grammar_edge_case_compatibility_consistent =
      surface.long_tail_grammar_expansion_ready &&
      surface.long_tail_grammar_compatibility_handoff_ready &&
      surface.language_version_pragma_coordinate_order_consistent &&
      surface.parse_artifact_edge_case_robustness_consistent;
  surface.long_tail_grammar_edge_case_compatibility_ready =
      surface.long_tail_grammar_edge_case_compatibility_consistent &&
      (surface.parse_artifact_replay_key_deterministic);
  surface.long_tail_grammar_edge_case_compatibility_key =
      BuildObjc3LongTailGrammarEdgeCaseCompatibilityKey(
          surface.compatibility_handoff_consistent,
          surface.long_tail_grammar_compatibility_handoff_ready,
          surface.language_version_pragma_coordinate_order_consistent,
          surface.parse_artifact_edge_case_robustness_consistent,
          surface.long_tail_grammar_edge_case_compatibility_consistent,
          surface.long_tail_grammar_edge_case_compatibility_ready);
  surface.long_tail_grammar_edge_case_expansion_consistent =
      surface.long_tail_grammar_expansion_ready &&
      surface.long_tail_grammar_edge_case_compatibility_ready &&
      surface.parse_artifact_edge_case_robustness_consistent &&
      surface.long_tail_grammar_covered_construct_count <=
          surface.long_tail_grammar_construct_count &&
      surface.long_tail_grammar_fingerprint != 0 &&
      !surface.long_tail_grammar_expansion_key.empty() &&
      !surface.long_tail_grammar_edge_case_compatibility_key.empty();
  surface.long_tail_grammar_edge_case_robustness_ready =
      surface.long_tail_grammar_edge_case_expansion_consistent &&
      (surface.parse_artifact_replay_key_deterministic) &&
      surface.language_version_pragma_coordinate_order_consistent;
  surface.long_tail_grammar_edge_case_robustness_key =
      BuildObjc3LongTailGrammarEdgeCaseRobustnessKey(
          surface.long_tail_grammar_construct_count,
          surface.long_tail_grammar_covered_construct_count,
          surface.long_tail_grammar_fingerprint,
          surface.long_tail_grammar_edge_case_compatibility_ready,
          surface.long_tail_grammar_edge_case_expansion_consistent,
          surface.parse_artifact_edge_case_robustness_consistent,
          surface.long_tail_grammar_edge_case_robustness_ready);
  surface.long_tail_grammar_diagnostics_hardening_consistent =
      surface.long_tail_grammar_edge_case_robustness_ready &&
      surface.parse_artifact_diagnostics_hardening_consistent &&
      surface.parser_diagnostic_surface_consistent &&
      surface.parser_diagnostic_code_surface_deterministic &&
      surface.parser_diagnostic_code_fingerprint != 0 &&
      !surface.parse_artifact_diagnostics_hardening_key.empty() &&
      !surface.long_tail_grammar_edge_case_robustness_key.empty();
  surface.long_tail_grammar_diagnostics_hardening_ready =
      surface.long_tail_grammar_diagnostics_hardening_consistent &&
      (surface.parse_artifact_replay_key_deterministic);
  surface.long_tail_grammar_diagnostics_hardening_key =
      BuildObjc3LongTailGrammarDiagnosticsHardeningKey(
          surface.parser_diagnostic_count,
          surface.parser_diagnostic_code_count,
          surface.parser_diagnostic_code_fingerprint,
          surface.parser_diagnostic_surface_consistent,
          surface.parser_diagnostic_code_surface_deterministic,
          surface.parse_artifact_diagnostics_hardening_consistent,
          surface.long_tail_grammar_edge_case_robustness_ready,
          surface.long_tail_grammar_diagnostics_hardening_consistent,
          surface.long_tail_grammar_diagnostics_hardening_ready);
  surface.parse_recovery_determinism_hardening_consistent =
      surface.parser_contract_snapshot_present &&
      surface.parser_contract_deterministic &&
      surface.parser_recovery_replay_ready &&
      surface.long_tail_grammar_core_feature_consistent &&
      surface.long_tail_grammar_handoff_key_deterministic &&
      surface.long_tail_grammar_expansion_accounting_consistent &&
      surface.long_tail_grammar_replay_keys_ready &&
      surface.long_tail_grammar_expansion_ready &&
      surface.long_tail_grammar_compatibility_handoff_ready &&
      surface.long_tail_grammar_edge_case_compatibility_consistent &&
      surface.long_tail_grammar_edge_case_compatibility_ready &&
      surface.long_tail_grammar_edge_case_expansion_consistent &&
      surface.long_tail_grammar_edge_case_robustness_ready &&
      surface.long_tail_grammar_diagnostics_hardening_ready &&
      surface.parse_artifact_handoff_deterministic &&
      surface.parse_artifact_replay_key_deterministic &&
      surface.parse_artifact_diagnostics_hardening_consistent &&
      surface.parse_artifact_edge_case_robustness_consistent &&
      surface.parser_diagnostic_grammar_hooks_edge_case_expansion_consistent &&
      surface.parser_diagnostic_grammar_hooks_edge_case_robustness_ready &&
      surface.parser_diagnostic_grammar_hooks_diagnostics_hardening_consistent &&
      surface.parser_diagnostic_grammar_hooks_diagnostics_hardening_ready &&
      !surface.long_tail_grammar_handoff_key.empty() &&
      !surface.long_tail_grammar_expansion_key.empty() &&
      !surface.long_tail_grammar_edge_case_compatibility_key.empty() &&
      !surface.long_tail_grammar_edge_case_robustness_key.empty() &&
      !surface.long_tail_grammar_diagnostics_hardening_key.empty() &&
      !surface.parse_artifact_handoff_key.empty() &&
      !surface.parse_artifact_replay_key.empty() &&
      !surface.parse_artifact_diagnostics_hardening_key.empty() &&
      !surface.parse_artifact_edge_robustness_key.empty() &&
      !surface.parser_diagnostic_grammar_hooks_edge_case_robustness_key.empty() &&
      !surface.parser_diagnostic_grammar_hooks_diagnostics_hardening_key.empty();
  surface.parse_recovery_determinism_hardening_key = BuildObjc3ParseRecoveryDeterminismHardeningKey(
      surface.parser_contract_snapshot_present,
      surface.parser_contract_deterministic,
      surface.parser_recovery_replay_ready,
      surface.long_tail_grammar_core_feature_consistent,
      surface.long_tail_grammar_handoff_key_deterministic,
      surface.long_tail_grammar_expansion_accounting_consistent,
      surface.long_tail_grammar_replay_keys_ready,
      surface.long_tail_grammar_expansion_ready,
      surface.long_tail_grammar_compatibility_handoff_ready,
      surface.long_tail_grammar_edge_case_compatibility_consistent,
      surface.long_tail_grammar_edge_case_compatibility_ready,
      surface.long_tail_grammar_edge_case_expansion_consistent,
      surface.long_tail_grammar_edge_case_robustness_ready,
      surface.long_tail_grammar_diagnostics_hardening_ready,
      surface.parse_artifact_handoff_deterministic,
      surface.parse_artifact_replay_key_deterministic,
      surface.parse_artifact_diagnostics_hardening_consistent,
      surface.parse_artifact_edge_case_robustness_consistent,
      surface.parse_recovery_determinism_hardening_consistent);
  surface.parser_diagnostic_grammar_hooks_recovery_determinism_consistent =
      surface.parse_recovery_determinism_hardening_consistent &&
      surface.parser_diagnostic_grammar_hooks_diagnostics_hardening_ready &&
      surface.parser_diagnostic_grammar_hooks_edge_case_robustness_ready &&
      surface.parser_recovery_replay_ready &&
      surface.parse_artifact_replay_key_deterministic &&
      !surface.parse_recovery_determinism_hardening_key.empty() &&
      !surface.parser_diagnostic_grammar_hooks_diagnostics_hardening_key.empty() &&
      !surface.parser_diagnostic_grammar_hooks_edge_case_robustness_key.empty();
  surface.parser_diagnostic_grammar_hooks_recovery_determinism_ready =
      surface.parser_diagnostic_grammar_hooks_recovery_determinism_consistent &&
      surface.parser_diagnostic_grammar_hooks_edge_case_robustness_ready;
  surface.parser_diagnostic_grammar_hooks_recovery_determinism_key =
      BuildObjc3DiagnosticGrammarHooksRecoveryDeterminismKey(
          surface.parser_recovery_replay_ready,
          surface.parse_artifact_replay_key_deterministic,
          surface.parse_recovery_determinism_hardening_consistent,
          surface.parser_diagnostic_grammar_hooks_diagnostics_hardening_ready,
          surface.parser_diagnostic_grammar_hooks_edge_case_robustness_ready,
          surface.parser_diagnostic_grammar_hooks_recovery_determinism_consistent,
          surface.parser_diagnostic_grammar_hooks_recovery_determinism_ready);
  surface.parser_diagnostic_grammar_hooks_conformance_matrix_consistent =
      surface.parser_diagnostic_grammar_hooks_recovery_determinism_consistent &&
      surface.parser_diagnostic_grammar_hooks_recovery_determinism_ready &&
      surface.parse_lowering_conformance_matrix_case_count ==
          kObjc3ParseLoweringConformanceMatrixCaseCount &&
      surface.parse_lowering_conformance_matrix_case_count > 0 &&
      surface.parse_artifact_replay_key_deterministic &&
      !surface.parser_diagnostic_grammar_hooks_recovery_determinism_key.empty() &&
      !surface.parse_recovery_determinism_hardening_key.empty() &&
      !surface.parse_artifact_replay_key.empty();
  surface.parser_diagnostic_grammar_hooks_conformance_matrix_ready =
      surface.parser_diagnostic_grammar_hooks_conformance_matrix_consistent &&
      surface.parse_lowering_conformance_corpus_case_count > 0 &&
      surface.parse_lowering_performance_quality_guardrails_case_count > 0;
  surface.parser_diagnostic_grammar_hooks_conformance_matrix_key =
      BuildObjc3DiagnosticGrammarHooksConformanceMatrixKey(
          surface.parse_lowering_conformance_matrix_case_count,
          surface.parse_lowering_conformance_corpus_case_count,
          surface.parse_lowering_performance_quality_guardrails_case_count,
          surface.parse_artifact_replay_key_deterministic,
          surface.parser_diagnostic_grammar_hooks_recovery_determinism_ready,
          surface.parser_diagnostic_grammar_hooks_conformance_matrix_consistent,
          surface.parser_diagnostic_grammar_hooks_conformance_matrix_ready);
  surface.long_tail_grammar_recovery_determinism_consistent =
      surface.parse_recovery_determinism_hardening_consistent &&
      surface.parser_diagnostic_grammar_hooks_recovery_determinism_consistent &&
      surface.parser_diagnostic_grammar_hooks_recovery_determinism_ready &&
      surface.long_tail_grammar_diagnostics_hardening_ready &&
      surface.parser_recovery_replay_ready &&
      surface.parse_artifact_replay_key_deterministic &&
      !surface.parse_recovery_determinism_hardening_key.empty() &&
      !surface.parser_diagnostic_grammar_hooks_recovery_determinism_key.empty() &&
      !surface.long_tail_grammar_diagnostics_hardening_key.empty();
  surface.long_tail_grammar_recovery_determinism_ready =
      surface.long_tail_grammar_recovery_determinism_consistent &&
      surface.long_tail_grammar_expansion_ready &&
      surface.long_tail_grammar_edge_case_robustness_ready;
  surface.long_tail_grammar_recovery_determinism_key =
      BuildObjc3LongTailGrammarRecoveryDeterminismKey(
          surface.parser_recovery_replay_ready,
          surface.parse_artifact_replay_key_deterministic,
          surface.parse_recovery_determinism_hardening_consistent,
          surface.long_tail_grammar_diagnostics_hardening_ready,
          surface.long_tail_grammar_edge_case_robustness_ready,
          surface.long_tail_grammar_expansion_ready,
          surface.long_tail_grammar_recovery_determinism_consistent,
          surface.long_tail_grammar_recovery_determinism_ready);
  const bool toolchain_runtime_ga_operations_recovery_determinism_consistent =
      IsObjc3ToolchainRuntimeGaOperationsRecoveryDeterminismHardeningConsistent(
          surface.parser_recovery_replay_ready,
          surface.parse_artifact_replay_key_deterministic,
          surface.long_tail_grammar_replay_keys_ready,
          surface.long_tail_grammar_diagnostics_hardening_ready,
          surface.parse_recovery_determinism_hardening_consistent,
          surface.parse_artifact_handoff_key,
          surface.parse_artifact_replay_key,
          surface.parse_artifact_diagnostics_hardening_key,
          surface.parse_artifact_edge_robustness_key,
          surface.long_tail_grammar_handoff_key,
          surface.long_tail_grammar_diagnostics_hardening_key,
          surface.parse_recovery_determinism_hardening_key);
  const bool toolchain_runtime_ga_operations_recovery_determinism_ready =
      IsObjc3ToolchainRuntimeGaOperationsRecoveryDeterminismHardeningReady(
          toolchain_runtime_ga_operations_recovery_determinism_consistent,
          surface.long_tail_grammar_recovery_determinism_consistent,
          surface.long_tail_grammar_recovery_determinism_ready,
          surface.long_tail_grammar_recovery_determinism_key);
  surface.parse_recovery_determinism_hardening_consistent &=
      toolchain_runtime_ga_operations_recovery_determinism_consistent;
  surface.long_tail_grammar_recovery_determinism_consistent =
      surface.long_tail_grammar_recovery_determinism_consistent &&
      toolchain_runtime_ga_operations_recovery_determinism_consistent;
  surface.long_tail_grammar_recovery_determinism_ready =
      surface.long_tail_grammar_recovery_determinism_ready &&
      toolchain_runtime_ga_operations_recovery_determinism_ready;
  surface.parse_recovery_determinism_hardening_key =
      BuildObjc3ParseRecoveryDeterminismHardeningKey(
          surface.parser_contract_snapshot_present,
          surface.parser_contract_deterministic,
          surface.parser_recovery_replay_ready,
          surface.long_tail_grammar_core_feature_consistent,
          surface.long_tail_grammar_handoff_key_deterministic,
          surface.long_tail_grammar_expansion_accounting_consistent,
          surface.long_tail_grammar_replay_keys_ready,
          surface.long_tail_grammar_expansion_ready,
          surface.long_tail_grammar_compatibility_handoff_ready,
          surface.long_tail_grammar_edge_case_compatibility_consistent,
          surface.long_tail_grammar_edge_case_compatibility_ready,
          surface.long_tail_grammar_edge_case_expansion_consistent,
          surface.long_tail_grammar_edge_case_robustness_ready,
          surface.long_tail_grammar_diagnostics_hardening_ready,
          surface.parse_artifact_handoff_deterministic,
          surface.parse_artifact_replay_key_deterministic,
          surface.parse_artifact_diagnostics_hardening_consistent,
          surface.parse_artifact_edge_case_robustness_consistent,
          surface.parse_recovery_determinism_hardening_consistent);
  surface.long_tail_grammar_recovery_determinism_key =
      BuildObjc3LongTailGrammarRecoveryDeterminismKey(
          surface.parser_recovery_replay_ready,
          surface.parse_artifact_replay_key_deterministic,
          surface.parse_recovery_determinism_hardening_consistent,
          surface.long_tail_grammar_diagnostics_hardening_ready,
          surface.long_tail_grammar_edge_case_robustness_ready,
          surface.long_tail_grammar_expansion_ready,
          surface.long_tail_grammar_recovery_determinism_consistent,
          surface.long_tail_grammar_recovery_determinism_ready);
  const std::string toolchain_runtime_ga_operations_recovery_determinism_key =
      BuildObjc3ToolchainRuntimeGaOperationsRecoveryDeterminismHardeningKey(
          surface.parser_recovery_replay_ready,
          surface.parse_artifact_replay_key_deterministic,
          surface.long_tail_grammar_replay_keys_ready,
          surface.long_tail_grammar_diagnostics_hardening_ready,
          surface.parse_artifact_handoff_key,
          surface.parse_artifact_replay_key,
          surface.parse_artifact_diagnostics_hardening_key,
          surface.parse_artifact_edge_robustness_key,
          surface.long_tail_grammar_handoff_key,
          surface.long_tail_grammar_diagnostics_hardening_key,
          surface.parse_recovery_determinism_hardening_key,
          surface.long_tail_grammar_recovery_determinism_key,
          toolchain_runtime_ga_operations_recovery_determinism_consistent,
          toolchain_runtime_ga_operations_recovery_determinism_ready);
  surface.parse_recovery_determinism_hardening_key +=
      ";toolchain_runtime_ga_operations_recovery_determinism_key=" +
      toolchain_runtime_ga_operations_recovery_determinism_key;
  surface.long_tail_grammar_recovery_determinism_key +=
      ";toolchain_runtime_ga_operations_recovery_determinism_key=" +
      toolchain_runtime_ga_operations_recovery_determinism_key;
  const Objc3TypedSemaToLoweringContractSurface typed_sema_to_lowering_contract_surface =
      HasObjc3TypedSemaToLoweringCoreFeatureSurface(
          pipeline_result.typed_sema_to_lowering_contract_surface)
          ? pipeline_result.typed_sema_to_lowering_contract_surface
          : BuildObjc3TypedSemaToLoweringContractSurface(pipeline_result, options);
  surface.semantic_integration_surface_built =
      typed_sema_to_lowering_contract_surface.semantic_integration_surface_built;
  surface.semantic_diagnostics_deterministic =
      pipeline_result.sema_parity_surface.deterministic_semantic_diagnostics;
  surface.semantic_type_metadata_deterministic =
      typed_sema_to_lowering_contract_surface.semantic_type_metadata_handoff_deterministic;
  surface.protocol_category_deterministic =
      typed_sema_to_lowering_contract_surface.protocol_category_handoff_deterministic;
  surface.class_protocol_category_linking_deterministic =
      typed_sema_to_lowering_contract_surface.class_protocol_category_linking_handoff_deterministic;
  surface.selector_normalization_deterministic =
      typed_sema_to_lowering_contract_surface.selector_normalization_handoff_deterministic;
  surface.property_attribute_deterministic =
      typed_sema_to_lowering_contract_surface.property_attribute_handoff_deterministic;
  surface.symbol_graph_deterministic =
      typed_sema_to_lowering_contract_surface.symbol_graph_handoff_deterministic;
  surface.scope_resolution_deterministic =
      typed_sema_to_lowering_contract_surface.scope_resolution_handoff_deterministic;
  surface.object_pointer_type_handoff_deterministic =
      typed_sema_to_lowering_contract_surface.object_pointer_type_handoff_deterministic;
  surface.typed_handoff_key_deterministic =
      typed_sema_to_lowering_contract_surface.typed_handoff_key_deterministic;
  surface.typed_sema_core_feature_consistent =
      typed_sema_to_lowering_contract_surface.typed_core_feature_consistent;
  surface.typed_sema_core_feature_expansion_consistent =
      typed_sema_to_lowering_contract_surface.typed_core_feature_expansion_consistent;
  surface.typed_sema_core_feature_case_count =
      typed_sema_to_lowering_contract_surface.typed_core_feature_case_count;
  surface.typed_sema_core_feature_passed_case_count =
      typed_sema_to_lowering_contract_surface.typed_core_feature_passed_case_count;
  surface.typed_sema_core_feature_failed_case_count =
      typed_sema_to_lowering_contract_surface.typed_core_feature_failed_case_count;
  surface.typed_sema_core_feature_expansion_case_count =
      typed_sema_to_lowering_contract_surface.typed_core_feature_expansion_case_count;
  surface.typed_sema_core_feature_expansion_passed_case_count =
      typed_sema_to_lowering_contract_surface.typed_core_feature_expansion_passed_case_count;
  surface.typed_sema_core_feature_expansion_failed_case_count =
      typed_sema_to_lowering_contract_surface.typed_core_feature_expansion_failed_case_count;
  surface.typed_sema_core_feature_key =
      typed_sema_to_lowering_contract_surface.typed_core_feature_key;
  surface.typed_sema_core_feature_expansion_key =
      typed_sema_to_lowering_contract_surface.typed_core_feature_expansion_key;
  surface.typed_sema_edge_case_compatibility_consistent =
      typed_sema_to_lowering_contract_surface.typed_core_feature_edge_case_compatibility_ready;
  surface.typed_sema_edge_case_compatibility_key =
      typed_sema_to_lowering_contract_surface.typed_core_feature_edge_case_compatibility_key;
  surface.typed_sema_edge_case_compatibility_ready =
      surface.typed_sema_edge_case_compatibility_consistent &&
      !surface.typed_sema_edge_case_compatibility_key.empty();
  surface.typed_sema_edge_case_expansion_consistent =
      typed_sema_to_lowering_contract_surface.typed_core_feature_edge_case_expansion_consistent;
  surface.typed_sema_edge_case_robustness_key =
      typed_sema_to_lowering_contract_surface.typed_core_feature_edge_case_robustness_key;
  surface.typed_sema_edge_case_robustness_ready =
      surface.typed_sema_edge_case_compatibility_ready &&
      surface.typed_sema_edge_case_expansion_consistent &&
      !surface.typed_sema_edge_case_robustness_key.empty();
  surface.typed_sema_diagnostics_hardening_consistent =
      typed_sema_to_lowering_contract_surface.typed_diagnostics_hardening_consistent;
  surface.typed_sema_diagnostics_hardening_key =
      typed_sema_to_lowering_contract_surface.typed_diagnostics_hardening_key;
  surface.typed_sema_diagnostics_hardening_ready =
      surface.typed_sema_diagnostics_hardening_consistent &&
      surface.typed_sema_edge_case_robustness_ready &&
      !surface.typed_sema_diagnostics_hardening_key.empty();
  surface.typed_sema_recovery_determinism_consistent =
      typed_sema_to_lowering_contract_surface.typed_recovery_determinism_consistent;
  surface.typed_sema_recovery_determinism_key =
      typed_sema_to_lowering_contract_surface.typed_recovery_determinism_key;
  surface.typed_sema_recovery_determinism_ready =
      surface.typed_sema_recovery_determinism_consistent &&
      surface.typed_sema_diagnostics_hardening_ready &&
      !surface.typed_sema_recovery_determinism_key.empty();
  surface.typed_sema_conformance_matrix_consistent =
      typed_sema_to_lowering_contract_surface.typed_conformance_matrix_consistent;
  surface.typed_sema_conformance_matrix_key =
      typed_sema_to_lowering_contract_surface.typed_conformance_matrix_key;
  surface.typed_sema_conformance_matrix_ready =
      surface.typed_sema_conformance_matrix_consistent &&
      surface.typed_sema_recovery_determinism_ready &&
      !surface.typed_sema_conformance_matrix_key.empty();
  surface.typed_sema_conformance_corpus_consistent =
      typed_sema_to_lowering_contract_surface.typed_conformance_corpus_consistent;
  surface.typed_sema_conformance_corpus_key =
      typed_sema_to_lowering_contract_surface.typed_conformance_corpus_key;
  surface.typed_sema_conformance_corpus_ready =
      surface.typed_sema_conformance_corpus_consistent &&
      surface.typed_sema_conformance_matrix_ready &&
      !surface.typed_sema_conformance_corpus_key.empty();
  surface.typed_sema_performance_quality_guardrails_consistent =
      typed_sema_to_lowering_contract_surface.typed_performance_quality_guardrails_consistent;
  surface.typed_sema_performance_quality_guardrails_ready =
      typed_sema_to_lowering_contract_surface.typed_performance_quality_guardrails_ready;
  surface.typed_sema_performance_quality_guardrails_case_count =
      typed_sema_to_lowering_contract_surface.typed_performance_quality_guardrails_case_count;
  surface.typed_sema_performance_quality_guardrails_passed_case_count =
      typed_sema_to_lowering_contract_surface.typed_performance_quality_guardrails_passed_case_count;
  surface.typed_sema_performance_quality_guardrails_failed_case_count =
      typed_sema_to_lowering_contract_surface.typed_performance_quality_guardrails_failed_case_count;
  surface.typed_sema_performance_quality_guardrails_key =
      typed_sema_to_lowering_contract_surface.typed_performance_quality_guardrails_key;
  surface.typed_sema_cross_lane_integration_consistent =
      typed_sema_to_lowering_contract_surface.typed_cross_lane_integration_consistent;
  surface.typed_sema_cross_lane_integration_ready =
      typed_sema_to_lowering_contract_surface.typed_cross_lane_integration_ready;
  surface.typed_sema_cross_lane_integration_key =
      typed_sema_to_lowering_contract_surface.typed_cross_lane_integration_key;
  surface.typed_sema_docs_runbook_sync_consistent =
      typed_sema_to_lowering_contract_surface.typed_docs_runbook_sync_consistent;
  surface.typed_sema_docs_runbook_sync_ready =
      typed_sema_to_lowering_contract_surface.typed_docs_runbook_sync_ready;
  surface.typed_sema_docs_runbook_sync_key =
      typed_sema_to_lowering_contract_surface.typed_docs_runbook_sync_key;
  surface.typed_sema_release_candidate_replay_dry_run_consistent =
      typed_sema_to_lowering_contract_surface.typed_release_candidate_replay_dry_run_consistent;
  surface.typed_sema_release_candidate_replay_dry_run_ready =
      typed_sema_to_lowering_contract_surface.typed_release_candidate_replay_dry_run_ready;
  surface.typed_sema_release_candidate_replay_dry_run_key =
      typed_sema_to_lowering_contract_surface.typed_release_candidate_replay_dry_run_key;
  surface.typed_sema_advanced_core_shard1_consistent =
      typed_sema_to_lowering_contract_surface.typed_advanced_core_shard1_consistent;
  surface.typed_sema_advanced_core_shard1_ready =
      typed_sema_to_lowering_contract_surface.typed_advanced_core_shard1_ready;
  surface.typed_sema_advanced_core_shard1_key =
      typed_sema_to_lowering_contract_surface.typed_advanced_core_shard1_key;
  Objc3LoweringIRBoundary lowering_boundary;
  std::string lowering_error;
  const bool lowering_boundary_from_options_ready =
      TryBuildObjc3LoweringIRBoundary(options.lowering, lowering_boundary, lowering_error);
  const std::string lowering_boundary_replay_key_from_options =
      lowering_boundary_from_options_ready ? Objc3LoweringIRBoundaryReplayKey(lowering_boundary)
                                           : std::string();
  const bool lowering_boundary_replay_key_matches_typed_surface =
      !lowering_boundary_from_options_ready ||
      lowering_boundary_replay_key_from_options ==
          typed_sema_to_lowering_contract_surface.lowering_boundary_replay_key;
  surface.lowering_boundary_ready =
      typed_sema_to_lowering_contract_surface.lowering_boundary_ready &&
      lowering_boundary_from_options_ready &&
      lowering_boundary_replay_key_matches_typed_surface;
  surface.lowering_boundary_replay_key =
      typed_sema_to_lowering_contract_surface.lowering_boundary_replay_key;
  if (surface.lowering_boundary_replay_key.empty() && lowering_boundary_from_options_ready) {
    surface.lowering_boundary_replay_key = lowering_boundary_replay_key_from_options;
  }
  if (!typed_sema_to_lowering_contract_surface.failure_reason.empty()) {
    surface.failure_reason = typed_sema_to_lowering_contract_surface.failure_reason;
  } else if (!lowering_boundary_from_options_ready && !lowering_error.empty()) {
    surface.failure_reason = lowering_error;
  } else if (!lowering_boundary_replay_key_matches_typed_surface) {
    surface.failure_reason = "typed sema/lowering boundary replay key does not match lowering contract";
  }

  const bool diagnostics_clear =
      surface.lexer_diagnostic_count == 0 &&
      surface.parser_diagnostic_count == 0 &&
      surface.semantic_diagnostic_count == 0;
  const bool parse_snapshot_ready =
      surface.parser_contract_snapshot_present &&
      surface.parser_contract_deterministic &&
      surface.parser_recovery_replay_ready &&
      surface.long_tail_grammar_core_feature_consistent &&
      surface.long_tail_grammar_handoff_key_deterministic &&
      surface.long_tail_grammar_expansion_ready &&
      surface.long_tail_grammar_compatibility_handoff_ready &&
      surface.long_tail_grammar_edge_case_compatibility_ready &&
      surface.long_tail_grammar_edge_case_expansion_consistent &&
      surface.long_tail_grammar_edge_case_robustness_ready &&
      surface.long_tail_grammar_diagnostics_hardening_ready &&
      surface.long_tail_grammar_recovery_determinism_ready &&
      surface.parse_artifact_handoff_deterministic;
  const bool parse_artifact_replay_key_ready =
      surface.parse_artifact_replay_key_deterministic;
  const bool parse_artifact_diagnostics_hardening_ready =
      surface.parse_artifact_diagnostics_hardening_consistent;
  const bool parse_recovery_determinism_hardening_ready =
      surface.parse_recovery_determinism_hardening_consistent;
  const bool parse_snapshot_replay_ready =
      parse_snapshot_ready &&
      parse_artifact_replay_key_ready &&
      parse_artifact_diagnostics_hardening_ready &&
      surface.parse_artifact_edge_case_robustness_consistent &&
      surface.long_tail_grammar_edge_case_robustness_ready &&
      surface.long_tail_grammar_diagnostics_hardening_ready &&
      surface.long_tail_grammar_recovery_determinism_ready &&
      parse_recovery_determinism_hardening_ready;
  const bool typed_core_feature_expansion_case_accounting_consistent =
      surface.typed_sema_core_feature_expansion_case_count ==
          kObjc3TypedSemaToLoweringCoreFeatureExpansionCaseCount &&
      surface.typed_sema_core_feature_expansion_case_count > 0 &&
      surface.typed_sema_core_feature_expansion_case_count >=
          surface.typed_sema_core_feature_expansion_passed_case_count &&
      surface.typed_sema_core_feature_expansion_failed_case_count ==
          (surface.typed_sema_core_feature_expansion_case_count -
           surface.typed_sema_core_feature_expansion_passed_case_count);
  const bool typed_core_feature_expansion_ready =
      typed_core_feature_expansion_case_accounting_consistent &&
      surface.typed_sema_core_feature_expansion_consistent &&
      !surface.typed_sema_core_feature_expansion_key.empty();
  const bool typed_edge_case_compatibility_alignment =
      surface.compatibility_handoff_consistent ==
          typed_sema_to_lowering_contract_surface.compatibility_handoff_consistent &&
      surface.language_version_pragma_coordinate_order_consistent ==
          typed_sema_to_lowering_contract_surface.language_version_pragma_coordinate_order_consistent &&
      surface.parse_artifact_replay_key_deterministic ==
          typed_sema_to_lowering_contract_surface.parse_artifact_replay_key_deterministic &&
      surface.parse_artifact_edge_case_robustness_consistent ==
          typed_sema_to_lowering_contract_surface.parse_artifact_edge_case_robustness_consistent &&
      surface.compatibility_handoff_key ==
          typed_sema_to_lowering_contract_surface.compatibility_handoff_key &&
      surface.parse_artifact_edge_robustness_key ==
          typed_sema_to_lowering_contract_surface.parse_artifact_edge_robustness_key;
  const bool typed_edge_case_robustness_alignment =
      surface.typed_sema_edge_case_expansion_consistent ==
          typed_sema_to_lowering_contract_surface.typed_core_feature_edge_case_expansion_consistent &&
      surface.typed_sema_edge_case_robustness_ready ==
          typed_sema_to_lowering_contract_surface.typed_core_feature_edge_case_robustness_ready &&
      surface.typed_sema_edge_case_robustness_key ==
          typed_sema_to_lowering_contract_surface.typed_core_feature_edge_case_robustness_key;
  const bool typed_diagnostics_hardening_alignment =
      surface.typed_sema_diagnostics_hardening_consistent ==
          typed_sema_to_lowering_contract_surface.typed_diagnostics_hardening_consistent &&
      surface.typed_sema_diagnostics_hardening_ready ==
          typed_sema_to_lowering_contract_surface.typed_diagnostics_hardening_ready &&
      surface.typed_sema_diagnostics_hardening_key ==
          typed_sema_to_lowering_contract_surface.typed_diagnostics_hardening_key;
  const bool typed_recovery_determinism_alignment =
      surface.typed_sema_recovery_determinism_consistent ==
          typed_sema_to_lowering_contract_surface.typed_recovery_determinism_consistent &&
      surface.typed_sema_recovery_determinism_ready ==
          typed_sema_to_lowering_contract_surface.typed_recovery_determinism_ready &&
      surface.typed_sema_recovery_determinism_key ==
          typed_sema_to_lowering_contract_surface.typed_recovery_determinism_key;
  const bool typed_conformance_matrix_alignment =
      surface.typed_sema_conformance_matrix_consistent ==
          typed_sema_to_lowering_contract_surface.typed_conformance_matrix_consistent &&
      surface.typed_sema_conformance_matrix_ready ==
          typed_sema_to_lowering_contract_surface.typed_conformance_matrix_ready &&
      surface.typed_sema_conformance_matrix_key ==
          typed_sema_to_lowering_contract_surface.typed_conformance_matrix_key;
  const bool typed_conformance_corpus_alignment =
      surface.typed_sema_conformance_corpus_consistent ==
          typed_sema_to_lowering_contract_surface.typed_conformance_corpus_consistent &&
      surface.typed_sema_conformance_corpus_ready ==
          typed_sema_to_lowering_contract_surface.typed_conformance_corpus_ready &&
      surface.typed_sema_conformance_corpus_key ==
          typed_sema_to_lowering_contract_surface.typed_conformance_corpus_key;
  const bool typed_performance_quality_guardrails_alignment =
      surface.typed_sema_performance_quality_guardrails_consistent ==
          typed_sema_to_lowering_contract_surface.typed_performance_quality_guardrails_consistent &&
      surface.typed_sema_performance_quality_guardrails_ready ==
          typed_sema_to_lowering_contract_surface.typed_performance_quality_guardrails_ready &&
      surface.typed_sema_performance_quality_guardrails_case_count ==
          typed_sema_to_lowering_contract_surface.typed_performance_quality_guardrails_case_count &&
      surface.typed_sema_performance_quality_guardrails_passed_case_count ==
          typed_sema_to_lowering_contract_surface.typed_performance_quality_guardrails_passed_case_count &&
      surface.typed_sema_performance_quality_guardrails_failed_case_count ==
          typed_sema_to_lowering_contract_surface.typed_performance_quality_guardrails_failed_case_count &&
      surface.typed_sema_performance_quality_guardrails_key ==
          typed_sema_to_lowering_contract_surface.typed_performance_quality_guardrails_key;
  const bool typed_cross_lane_integration_alignment =
      surface.typed_sema_cross_lane_integration_consistent ==
          typed_sema_to_lowering_contract_surface.typed_cross_lane_integration_consistent &&
      surface.typed_sema_cross_lane_integration_ready ==
          typed_sema_to_lowering_contract_surface.typed_cross_lane_integration_ready &&
      surface.typed_sema_cross_lane_integration_key ==
          typed_sema_to_lowering_contract_surface.typed_cross_lane_integration_key;
  const bool typed_docs_runbook_sync_alignment =
      surface.typed_sema_docs_runbook_sync_consistent ==
          typed_sema_to_lowering_contract_surface.typed_docs_runbook_sync_consistent &&
      surface.typed_sema_docs_runbook_sync_ready ==
          typed_sema_to_lowering_contract_surface.typed_docs_runbook_sync_ready &&
      surface.typed_sema_docs_runbook_sync_key ==
          typed_sema_to_lowering_contract_surface.typed_docs_runbook_sync_key;
  const bool typed_release_candidate_replay_dry_run_alignment =
      surface.typed_sema_release_candidate_replay_dry_run_consistent ==
          typed_sema_to_lowering_contract_surface.typed_release_candidate_replay_dry_run_consistent &&
      surface.typed_sema_release_candidate_replay_dry_run_ready ==
          typed_sema_to_lowering_contract_surface.typed_release_candidate_replay_dry_run_ready &&
      surface.typed_sema_release_candidate_replay_dry_run_key ==
          typed_sema_to_lowering_contract_surface.typed_release_candidate_replay_dry_run_key;
  const bool typed_advanced_core_shard1_alignment =
      surface.typed_sema_advanced_core_shard1_consistent ==
          typed_sema_to_lowering_contract_surface.typed_advanced_core_shard1_consistent &&
      surface.typed_sema_advanced_core_shard1_ready ==
          typed_sema_to_lowering_contract_surface.typed_advanced_core_shard1_ready &&
      surface.typed_sema_advanced_core_shard1_key ==
          typed_sema_to_lowering_contract_surface.typed_advanced_core_shard1_key;
  const bool typed_core_feature_ready =
      surface.typed_handoff_key_deterministic &&
      surface.typed_sema_core_feature_consistent &&
      typed_core_feature_expansion_ready &&
      surface.typed_sema_edge_case_compatibility_ready &&
      surface.typed_sema_edge_case_expansion_consistent &&
      surface.typed_sema_edge_case_robustness_ready &&
      surface.typed_sema_diagnostics_hardening_consistent &&
      surface.typed_sema_diagnostics_hardening_ready &&
      surface.typed_sema_recovery_determinism_consistent &&
      surface.typed_sema_recovery_determinism_ready &&
      surface.typed_sema_conformance_matrix_consistent &&
      surface.typed_sema_conformance_matrix_ready &&
      surface.typed_sema_conformance_corpus_consistent &&
      surface.typed_sema_conformance_corpus_ready &&
      surface.typed_sema_performance_quality_guardrails_consistent &&
      surface.typed_sema_performance_quality_guardrails_ready &&
      surface.typed_sema_cross_lane_integration_consistent &&
      surface.typed_sema_cross_lane_integration_ready &&
      surface.typed_sema_docs_runbook_sync_consistent &&
      surface.typed_sema_docs_runbook_sync_ready &&
      surface.typed_sema_release_candidate_replay_dry_run_consistent &&
      surface.typed_sema_release_candidate_replay_dry_run_ready &&
      surface.typed_sema_advanced_core_shard1_consistent &&
      surface.typed_sema_advanced_core_shard1_ready &&
      typed_edge_case_compatibility_alignment &&
      typed_edge_case_robustness_alignment &&
      !surface.typed_sema_edge_case_compatibility_key.empty() &&
      !surface.typed_sema_edge_case_robustness_key.empty() &&
      typed_diagnostics_hardening_alignment &&
      !surface.typed_sema_diagnostics_hardening_key.empty() &&
      typed_recovery_determinism_alignment &&
      !surface.typed_sema_recovery_determinism_key.empty() &&
      typed_conformance_matrix_alignment &&
      !surface.typed_sema_conformance_matrix_key.empty() &&
      typed_conformance_corpus_alignment &&
      !surface.typed_sema_conformance_corpus_key.empty() &&
      typed_performance_quality_guardrails_alignment &&
      !surface.typed_sema_performance_quality_guardrails_key.empty() &&
      typed_cross_lane_integration_alignment &&
      !surface.typed_sema_cross_lane_integration_key.empty() &&
      typed_docs_runbook_sync_alignment &&
      !surface.typed_sema_docs_runbook_sync_key.empty() &&
      typed_release_candidate_replay_dry_run_alignment &&
      !surface.typed_sema_release_candidate_replay_dry_run_key.empty() &&
      typed_advanced_core_shard1_alignment &&
      !surface.typed_sema_advanced_core_shard1_key.empty() &&
      !surface.typed_sema_core_feature_key.empty();
  const bool sema_handoff_ready =
      typed_sema_to_lowering_contract_surface.ready_for_lowering &&
      typed_core_feature_ready;
  const bool semantic_handoff_deterministic =
      surface.semantic_diagnostics_deterministic &&
      surface.semantic_type_metadata_deterministic &&
      surface.protocol_category_deterministic &&
      surface.class_protocol_category_linking_deterministic &&
      surface.selector_normalization_deterministic &&
      surface.property_attribute_deterministic &&
      surface.symbol_graph_deterministic &&
      surface.scope_resolution_deterministic &&
      surface.object_pointer_type_handoff_deterministic;
  surface.parse_lowering_conformance_matrix_consistent =
      surface.parse_lowering_conformance_matrix_case_count ==
          kObjc3ParseLoweringConformanceMatrixCaseCount &&
      surface.parse_lowering_conformance_matrix_case_count > 0 &&
      surface.parser_contract_snapshot_present &&
      surface.parse_artifact_handoff_deterministic &&
      surface.parse_artifact_replay_key_deterministic &&
      surface.parse_artifact_diagnostics_hardening_consistent &&
      surface.parse_artifact_edge_case_robustness_consistent &&
      surface.parse_recovery_determinism_hardening_consistent &&
      surface.parser_diagnostic_grammar_hooks_conformance_matrix_consistent &&
      surface.parser_diagnostic_grammar_hooks_conformance_matrix_ready &&
      surface.long_tail_grammar_expansion_ready &&
      surface.long_tail_grammar_edge_case_compatibility_ready &&
      surface.long_tail_grammar_edge_case_expansion_consistent &&
      surface.long_tail_grammar_edge_case_robustness_ready &&
      surface.long_tail_grammar_diagnostics_hardening_ready &&
      surface.long_tail_grammar_recovery_determinism_ready &&
      surface.semantic_integration_surface_built &&
      semantic_handoff_deterministic &&
      typed_core_feature_ready &&
      sema_handoff_ready &&
      surface.lowering_boundary_ready &&
      !surface.parse_artifact_handoff_key.empty() &&
      !surface.parse_artifact_replay_key.empty() &&
      !surface.parse_artifact_diagnostics_hardening_key.empty() &&
      !surface.parse_artifact_edge_robustness_key.empty() &&
      !surface.parse_recovery_determinism_hardening_key.empty() &&
      !surface.parser_diagnostic_grammar_hooks_conformance_matrix_key.empty() &&
      !surface.long_tail_grammar_expansion_key.empty() &&
      !surface.long_tail_grammar_edge_case_compatibility_key.empty() &&
      !surface.long_tail_grammar_edge_case_robustness_key.empty() &&
      !surface.long_tail_grammar_diagnostics_hardening_key.empty() &&
      !surface.long_tail_grammar_recovery_determinism_key.empty() &&
      !surface.typed_sema_core_feature_key.empty() &&
      !surface.typed_sema_core_feature_expansion_key.empty() &&
      !surface.lowering_boundary_replay_key.empty();
  surface.long_tail_grammar_conformance_matrix_consistent =
      surface.long_tail_grammar_recovery_determinism_ready &&
      surface.parse_lowering_conformance_matrix_consistent &&
      surface.parse_artifact_replay_key_deterministic &&
      !surface.long_tail_grammar_recovery_determinism_key.empty() &&
      !surface.parse_artifact_replay_key.empty();
  surface.long_tail_grammar_conformance_matrix_ready =
      surface.long_tail_grammar_conformance_matrix_consistent &&
      surface.parse_lowering_conformance_corpus_case_count > 0 &&
      surface.parse_lowering_performance_quality_guardrails_case_count > 0;
  surface.long_tail_grammar_conformance_matrix_key =
      BuildObjc3LongTailGrammarConformanceMatrixKey(
          surface.parse_lowering_conformance_matrix_case_count,
          surface.parse_lowering_conformance_corpus_case_count,
          surface.parse_lowering_performance_quality_guardrails_case_count,
          surface.parse_artifact_replay_key_deterministic,
          surface.long_tail_grammar_recovery_determinism_ready,
          surface.long_tail_grammar_conformance_matrix_consistent,
          surface.long_tail_grammar_conformance_matrix_ready);
  surface.parse_lowering_conformance_matrix_key =
      BuildObjc3ParseLoweringConformanceMatrixKey(
          surface.parse_lowering_conformance_matrix_case_count,
          surface.parser_contract_snapshot_present,
          surface.parse_artifact_handoff_deterministic,
          surface.parse_artifact_replay_key_deterministic,
          surface.parse_artifact_diagnostics_hardening_consistent,
          surface.parse_artifact_edge_case_robustness_consistent,
          surface.parse_recovery_determinism_hardening_consistent,
          surface.semantic_integration_surface_built,
          semantic_handoff_deterministic,
          surface.lowering_boundary_ready,
          surface.parse_lowering_conformance_matrix_consistent);
  const bool toolchain_runtime_ga_operations_conformance_matrix_consistent =
      IsObjc3ToolchainRuntimeGaOperationsConformanceMatrixConsistent(
          toolchain_runtime_ga_operations_recovery_determinism_consistent,
          toolchain_runtime_ga_operations_recovery_determinism_ready,
          surface.parse_lowering_conformance_matrix_consistent,
          surface.long_tail_grammar_conformance_matrix_consistent,
          surface.long_tail_grammar_conformance_matrix_ready,
          surface.parse_recovery_determinism_hardening_key,
          surface.long_tail_grammar_recovery_determinism_key);
  const bool toolchain_runtime_ga_operations_conformance_matrix_ready =
      IsObjc3ToolchainRuntimeGaOperationsConformanceMatrixReady(
          toolchain_runtime_ga_operations_conformance_matrix_consistent,
          surface.parse_lowering_conformance_matrix_key,
          surface.long_tail_grammar_conformance_matrix_key);
  const std::string toolchain_runtime_ga_operations_conformance_matrix_key =
      BuildObjc3ToolchainRuntimeGaOperationsConformanceMatrixKey(
          surface.parse_lowering_conformance_matrix_consistent,
          surface.long_tail_grammar_conformance_matrix_consistent,
          surface.long_tail_grammar_conformance_matrix_ready,
          surface.parse_lowering_conformance_matrix_key,
          surface.long_tail_grammar_conformance_matrix_key,
          toolchain_runtime_ga_operations_conformance_matrix_consistent,
          toolchain_runtime_ga_operations_conformance_matrix_ready);
  surface.long_tail_grammar_conformance_matrix_consistent =
      surface.long_tail_grammar_conformance_matrix_consistent &&
      toolchain_runtime_ga_operations_conformance_matrix_consistent;
  surface.long_tail_grammar_conformance_matrix_ready =
      surface.long_tail_grammar_conformance_matrix_ready &&
      toolchain_runtime_ga_operations_conformance_matrix_ready;
  surface.long_tail_grammar_conformance_matrix_key +=
      ";toolchain_runtime_ga_operations_conformance_matrix_key=" +
      toolchain_runtime_ga_operations_conformance_matrix_key;
  surface.parse_lowering_conformance_matrix_key +=
      ";toolchain_runtime_ga_operations_conformance_matrix_key=" +
      toolchain_runtime_ga_operations_conformance_matrix_key;
  const bool parse_lowering_conformance_matrix_ready =
      surface.parse_lowering_conformance_matrix_consistent &&
      toolchain_runtime_ga_operations_conformance_matrix_ready;
  const bool parser_contract_snapshot_case_passed =
      surface.parser_contract_snapshot_present;
  const bool parse_artifact_handoff_case_passed =
      (surface.parse_artifact_handoff_deterministic);
  const bool parse_artifact_replay_case_passed =
      (surface.parse_artifact_replay_key_deterministic);
  const bool parse_artifact_diagnostics_hardening_case_passed =
      surface.parse_artifact_diagnostics_hardening_consistent;
  const bool parse_artifact_edge_robustness_case_passed =
      surface.parse_artifact_edge_case_robustness_consistent;
  const bool parse_recovery_determinism_hardening_case_passed =
      surface.parse_recovery_determinism_hardening_consistent;
  const bool semantic_handoff_case_passed =
      surface.semantic_integration_surface_built &&
      semantic_handoff_deterministic &&
      sema_handoff_ready;
  const bool lowering_boundary_case_passed =
      surface.lowering_boundary_ready &&
      !surface.lowering_boundary_replay_key.empty();
  surface.parse_lowering_conformance_corpus_passed_case_count =
      static_cast<std::size_t>(parser_contract_snapshot_case_passed) +
      static_cast<std::size_t>(parse_artifact_handoff_case_passed) +
      static_cast<std::size_t>(parse_artifact_replay_case_passed) +
      static_cast<std::size_t>(parse_artifact_diagnostics_hardening_case_passed) +
      static_cast<std::size_t>(parse_artifact_edge_robustness_case_passed) +
      static_cast<std::size_t>(parse_recovery_determinism_hardening_case_passed) +
      static_cast<std::size_t>(semantic_handoff_case_passed) +
      static_cast<std::size_t>(lowering_boundary_case_passed);
  surface.parse_lowering_conformance_corpus_failed_case_count =
      surface.parse_lowering_conformance_corpus_case_count >=
              surface.parse_lowering_conformance_corpus_passed_case_count
          ? (surface.parse_lowering_conformance_corpus_case_count -
             surface.parse_lowering_conformance_corpus_passed_case_count)
          : surface.parse_lowering_conformance_corpus_case_count;
  surface.parse_lowering_conformance_corpus_consistent =
      surface.parse_lowering_conformance_matrix_consistent &&
      surface.long_tail_grammar_conformance_matrix_ready &&
      surface.parse_lowering_conformance_corpus_case_count ==
          kObjc3ParseLoweringConformanceCorpusCaseCount &&
      surface.parse_lowering_conformance_corpus_case_count > 0 &&
      surface.parse_lowering_conformance_corpus_passed_case_count ==
          surface.parse_lowering_conformance_corpus_case_count &&
      surface.parse_lowering_conformance_corpus_failed_case_count == 0 &&
      parser_contract_snapshot_case_passed &&
      parse_artifact_handoff_case_passed &&
      parse_artifact_replay_case_passed &&
      parse_artifact_diagnostics_hardening_case_passed &&
      parse_artifact_edge_robustness_case_passed &&
      parse_recovery_determinism_hardening_case_passed &&
      semantic_handoff_case_passed &&
      lowering_boundary_case_passed;
  surface.parse_lowering_conformance_corpus_key =
      BuildObjc3ParseLoweringConformanceCorpusKey(
          surface.parse_lowering_conformance_corpus_case_count,
          surface.parse_lowering_conformance_corpus_passed_case_count,
          surface.parse_lowering_conformance_corpus_failed_case_count,
          parser_contract_snapshot_case_passed,
          parse_artifact_handoff_case_passed,
          parse_artifact_replay_case_passed,
          parse_artifact_diagnostics_hardening_case_passed,
          parse_artifact_edge_robustness_case_passed,
          parse_recovery_determinism_hardening_case_passed,
          semantic_handoff_case_passed,
          lowering_boundary_case_passed,
          surface.parse_lowering_conformance_matrix_consistent,
          surface.parse_lowering_conformance_corpus_consistent);
  const bool toolchain_runtime_ga_operations_conformance_corpus_consistent =
      IsObjc3ToolchainRuntimeGaOperationsConformanceCorpusConsistent(
          toolchain_runtime_ga_operations_conformance_matrix_consistent,
          toolchain_runtime_ga_operations_conformance_matrix_ready,
          surface.parse_lowering_conformance_matrix_consistent,
          surface.parse_lowering_conformance_corpus_consistent,
          surface.parse_lowering_conformance_matrix_key,
          surface.parse_lowering_conformance_corpus_key,
          surface.long_tail_grammar_conformance_matrix_key);
  const bool toolchain_runtime_ga_operations_conformance_corpus_ready =
      IsObjc3ToolchainRuntimeGaOperationsConformanceCorpusReady(
          toolchain_runtime_ga_operations_conformance_corpus_consistent,
          surface.parse_lowering_conformance_corpus_key);
  const std::string toolchain_runtime_ga_operations_conformance_corpus_key =
      BuildObjc3ToolchainRuntimeGaOperationsConformanceCorpusKey(
          surface.parse_lowering_conformance_matrix_consistent,
          surface.parse_lowering_conformance_corpus_consistent,
          surface.parse_lowering_conformance_matrix_key,
          surface.parse_lowering_conformance_corpus_key,
          surface.long_tail_grammar_conformance_matrix_key,
          toolchain_runtime_ga_operations_conformance_corpus_consistent,
          toolchain_runtime_ga_operations_conformance_corpus_ready);
  surface.parse_lowering_conformance_corpus_consistent =
      surface.parse_lowering_conformance_corpus_consistent &&
      toolchain_runtime_ga_operations_conformance_corpus_consistent;
  surface.parse_lowering_conformance_corpus_key +=
      ";toolchain_runtime_ga_operations_conformance_corpus_key=" +
      toolchain_runtime_ga_operations_conformance_corpus_key;
  surface.long_tail_grammar_conformance_matrix_key +=
      ";toolchain_runtime_ga_operations_conformance_corpus_key=" +
      toolchain_runtime_ga_operations_conformance_corpus_key;
  const bool parse_lowering_conformance_corpus_ready =
      surface.parse_lowering_conformance_corpus_consistent &&
      toolchain_runtime_ga_operations_conformance_corpus_ready;
  surface.parser_diagnostic_grammar_hooks_conformance_corpus_consistent =
      surface.parser_diagnostic_grammar_hooks_conformance_matrix_consistent &&
      surface.parser_diagnostic_grammar_hooks_conformance_matrix_ready &&
      surface.parse_lowering_conformance_corpus_consistent &&
      surface.parse_lowering_conformance_corpus_case_count ==
          kObjc3ParseLoweringConformanceCorpusCaseCount &&
      surface.parse_lowering_conformance_corpus_case_count > 0 &&
      !surface.parser_diagnostic_grammar_hooks_conformance_matrix_key.empty() &&
      !surface.parse_lowering_conformance_corpus_key.empty();
  const bool parser_diagnostic_grammar_hooks_conformance_corpus_ready_gate =
      surface.parser_diagnostic_grammar_hooks_conformance_corpus_consistent &&
      parse_lowering_conformance_corpus_ready &&
      surface.parse_lowering_performance_quality_guardrails_case_count > 0;
  surface.parser_diagnostic_grammar_hooks_conformance_corpus_key =
      BuildObjc3DiagnosticGrammarHooksConformanceCorpusKey(
          surface.parse_lowering_conformance_corpus_case_count,
          surface.parse_lowering_conformance_corpus_passed_case_count,
          surface.parse_lowering_conformance_corpus_failed_case_count,
          surface.parser_diagnostic_grammar_hooks_conformance_matrix_ready,
          surface.parse_artifact_replay_key_deterministic,
          surface.parser_diagnostic_grammar_hooks_conformance_corpus_consistent,
          parser_diagnostic_grammar_hooks_conformance_corpus_ready_gate);
  surface.parser_diagnostic_grammar_hooks_conformance_corpus_ready =
      parser_diagnostic_grammar_hooks_conformance_corpus_ready_gate &&
      !surface.parser_diagnostic_grammar_hooks_conformance_corpus_key.empty();
  const bool parser_token_budget_guardrail_case_passed =
      surface.parser_token_count_budget_consistent;
  const bool parser_diagnostic_code_surface_guardrail_case_passed =
      surface.parser_diagnostic_code_surface_deterministic;
  const bool parse_artifact_diagnostics_hardening_guardrail_case_passed =
      surface.parse_artifact_diagnostics_hardening_consistent;
  const bool parse_artifact_edge_robustness_guardrail_case_passed =
      surface.parse_artifact_edge_case_robustness_consistent;
  const bool parse_recovery_determinism_hardening_guardrail_case_passed =
      surface.parse_recovery_determinism_hardening_consistent;
  const bool parse_lowering_conformance_corpus_guardrail_case_passed =
      surface.parse_lowering_conformance_corpus_consistent;
  surface.parse_lowering_performance_quality_guardrails_passed_case_count =
      static_cast<std::size_t>(parser_token_budget_guardrail_case_passed) +
      static_cast<std::size_t>(parser_diagnostic_code_surface_guardrail_case_passed) +
      static_cast<std::size_t>(parse_artifact_diagnostics_hardening_guardrail_case_passed) +
      static_cast<std::size_t>(parse_artifact_edge_robustness_guardrail_case_passed) +
      static_cast<std::size_t>(parse_recovery_determinism_hardening_guardrail_case_passed) +
      static_cast<std::size_t>(parse_lowering_conformance_corpus_guardrail_case_passed);
  surface.parse_lowering_performance_quality_guardrails_failed_case_count =
      surface.parse_lowering_performance_quality_guardrails_case_count >=
              surface.parse_lowering_performance_quality_guardrails_passed_case_count
          ? (surface.parse_lowering_performance_quality_guardrails_case_count -
             surface.parse_lowering_performance_quality_guardrails_passed_case_count)
          : surface.parse_lowering_performance_quality_guardrails_case_count;
  surface.parse_lowering_performance_quality_guardrails_consistent =
      surface.parse_lowering_performance_quality_guardrails_case_count ==
          kObjc3ParseLoweringPerformanceQualityGuardrailsCaseCount &&
      surface.parse_lowering_performance_quality_guardrails_case_count > 0 &&
      surface.parse_lowering_performance_quality_guardrails_passed_case_count ==
          surface.parse_lowering_performance_quality_guardrails_case_count &&
      surface.parse_lowering_performance_quality_guardrails_failed_case_count == 0 &&
      parser_token_budget_guardrail_case_passed &&
      parser_diagnostic_code_surface_guardrail_case_passed &&
      parse_artifact_diagnostics_hardening_guardrail_case_passed &&
      parse_artifact_edge_robustness_guardrail_case_passed &&
      parse_recovery_determinism_hardening_guardrail_case_passed &&
      parse_lowering_conformance_corpus_guardrail_case_passed &&
      surface.long_tail_grammar_diagnostics_hardening_ready &&
      surface.long_tail_grammar_recovery_determinism_ready &&
      surface.long_tail_grammar_conformance_matrix_ready &&
      !surface.parse_artifact_diagnostics_hardening_key.empty() &&
      !surface.parse_artifact_edge_robustness_key.empty() &&
      !surface.parse_recovery_determinism_hardening_key.empty() &&
      !surface.long_tail_grammar_diagnostics_hardening_key.empty() &&
      !surface.long_tail_grammar_recovery_determinism_key.empty() &&
      !surface.long_tail_grammar_conformance_matrix_key.empty() &&
      !surface.parse_lowering_conformance_corpus_key.empty();
  surface.parse_lowering_performance_quality_guardrails_key =
      BuildObjc3ParseLoweringPerformanceQualityGuardrailsKey(
          surface.parse_lowering_performance_quality_guardrails_case_count,
          surface.parse_lowering_performance_quality_guardrails_passed_case_count,
          surface.parse_lowering_performance_quality_guardrails_failed_case_count,
          surface.parser_token_count_budget_consistent,
          surface.parser_diagnostic_code_surface_deterministic,
          surface.parse_artifact_diagnostics_hardening_consistent,
          surface.parse_artifact_edge_case_robustness_consistent,
          surface.parse_recovery_determinism_hardening_consistent,
          surface.parse_lowering_conformance_corpus_consistent,
          surface.parse_lowering_performance_quality_guardrails_consistent);
  const bool toolchain_runtime_ga_operations_performance_quality_guardrails_consistent =
      IsObjc3ToolchainRuntimeGaOperationsPerformanceQualityGuardrailsConsistent(
          toolchain_runtime_ga_operations_conformance_corpus_consistent,
          toolchain_runtime_ga_operations_conformance_corpus_ready,
          surface.parse_lowering_conformance_corpus_consistent,
          surface.parse_lowering_performance_quality_guardrails_consistent,
          surface.parse_lowering_conformance_corpus_key,
          surface.parse_lowering_performance_quality_guardrails_key,
          surface.long_tail_grammar_conformance_matrix_key);
  const bool toolchain_runtime_ga_operations_performance_quality_guardrails_ready =
      IsObjc3ToolchainRuntimeGaOperationsPerformanceQualityGuardrailsReady(
          toolchain_runtime_ga_operations_performance_quality_guardrails_consistent,
          surface.parse_lowering_performance_quality_guardrails_key);
  const std::string toolchain_runtime_ga_operations_performance_quality_guardrails_key =
      BuildObjc3ToolchainRuntimeGaOperationsPerformanceQualityGuardrailsKey(
          surface.parse_lowering_conformance_corpus_consistent,
          surface.parse_lowering_performance_quality_guardrails_consistent,
          surface.parse_lowering_conformance_corpus_key,
          surface.parse_lowering_performance_quality_guardrails_key,
          surface.long_tail_grammar_conformance_matrix_key,
          toolchain_runtime_ga_operations_performance_quality_guardrails_consistent,
          toolchain_runtime_ga_operations_performance_quality_guardrails_ready);
  surface.parse_lowering_performance_quality_guardrails_consistent &=
      toolchain_runtime_ga_operations_performance_quality_guardrails_consistent;
  surface.parse_lowering_performance_quality_guardrails_key +=
      ";toolchain_runtime_ga_operations_performance_quality_guardrails_key=" +
      toolchain_runtime_ga_operations_performance_quality_guardrails_key;
  surface.long_tail_grammar_conformance_matrix_key +=
      ";toolchain_runtime_ga_operations_performance_quality_guardrails_key=" +
      toolchain_runtime_ga_operations_performance_quality_guardrails_key;
  const bool parse_lowering_performance_quality_guardrails_ready =
      surface.parse_lowering_performance_quality_guardrails_consistent &&
      toolchain_runtime_ga_operations_performance_quality_guardrails_ready;
  const bool parse_lowering_performance_quality_guardrails_ready_gate =
      parse_lowering_performance_quality_guardrails_ready;
  const bool toolchain_runtime_ga_operations_cross_lane_integration_consistent =
      IsObjc3ToolchainRuntimeGaOperationsCrossLaneIntegrationConsistent(
          toolchain_runtime_ga_operations_performance_quality_guardrails_consistent,
          toolchain_runtime_ga_operations_performance_quality_guardrails_ready,
          parse_snapshot_replay_ready,
          sema_handoff_ready,
          surface.lowering_boundary_ready,
          surface.parse_lowering_conformance_corpus_consistent,
          surface.parse_lowering_performance_quality_guardrails_consistent,
          surface.parse_artifact_replay_key,
          surface.lowering_boundary_replay_key,
          surface.parse_lowering_conformance_corpus_key,
          surface.parse_lowering_performance_quality_guardrails_key);
  const bool toolchain_runtime_ga_operations_cross_lane_integration_ready =
      IsObjc3ToolchainRuntimeGaOperationsCrossLaneIntegrationReady(
          toolchain_runtime_ga_operations_cross_lane_integration_consistent,
          surface.parse_artifact_replay_key,
          surface.lowering_boundary_replay_key,
          surface.parse_lowering_performance_quality_guardrails_key);
  const std::string toolchain_runtime_ga_operations_cross_lane_integration_key =
      BuildObjc3ToolchainRuntimeGaOperationsCrossLaneIntegrationKey(
          parse_snapshot_replay_ready,
          sema_handoff_ready,
          surface.lowering_boundary_ready,
          surface.parse_lowering_conformance_corpus_consistent,
          surface.parse_lowering_performance_quality_guardrails_consistent,
          surface.parse_artifact_replay_key,
          surface.lowering_boundary_replay_key,
          surface.parse_lowering_conformance_corpus_key,
          surface.parse_lowering_performance_quality_guardrails_key,
          toolchain_runtime_ga_operations_cross_lane_integration_consistent,
          toolchain_runtime_ga_operations_cross_lane_integration_ready);
  surface.toolchain_runtime_ga_operations_cross_lane_integration_consistent =
      toolchain_runtime_ga_operations_cross_lane_integration_consistent;
  surface.toolchain_runtime_ga_operations_cross_lane_integration_ready =
      toolchain_runtime_ga_operations_cross_lane_integration_ready;
  surface.toolchain_runtime_ga_operations_cross_lane_integration_key =
      toolchain_runtime_ga_operations_cross_lane_integration_key;
  surface.parse_lowering_performance_quality_guardrails_key +=
      ";toolchain_runtime_ga_operations_cross_lane_integration_key=" +
      toolchain_runtime_ga_operations_cross_lane_integration_key;
  surface.long_tail_grammar_conformance_matrix_key +=
      ";toolchain_runtime_ga_operations_cross_lane_integration_key=" +
      toolchain_runtime_ga_operations_cross_lane_integration_key;
  surface.long_tail_grammar_integration_closeout_consistent =
      surface.long_tail_grammar_conformance_matrix_ready &&
      surface.parse_lowering_conformance_corpus_consistent &&
      surface.parse_lowering_performance_quality_guardrails_consistent &&
      toolchain_runtime_ga_operations_cross_lane_integration_consistent &&
      surface.long_tail_grammar_recovery_determinism_ready &&
      !surface.long_tail_grammar_conformance_matrix_key.empty() &&
      !surface.parse_lowering_conformance_corpus_key.empty() &&
      !surface.parse_lowering_performance_quality_guardrails_key.empty();
  surface.long_tail_grammar_gate_signoff_ready =
      surface.long_tail_grammar_integration_closeout_consistent &&
      toolchain_runtime_ga_operations_cross_lane_integration_ready &&
      diagnostics_clear &&
      sema_handoff_ready &&
      surface.lowering_boundary_ready &&
      !surface.parse_artifact_replay_key.empty() &&
      !surface.lowering_boundary_replay_key.empty();
  surface.long_tail_grammar_integration_closeout_key =
      BuildObjc3LongTailGrammarIntegrationCloseoutKey(
          surface.long_tail_grammar_conformance_matrix_ready,
          surface.parse_lowering_conformance_corpus_consistent,
          surface.parse_lowering_performance_quality_guardrails_consistent,
          toolchain_runtime_ga_operations_cross_lane_integration_consistent,
          toolchain_runtime_ga_operations_cross_lane_integration_ready,
          surface.long_tail_grammar_recovery_determinism_ready,
          sema_handoff_ready,
          surface.lowering_boundary_ready,
          surface.long_tail_grammar_integration_closeout_consistent,
          surface.long_tail_grammar_gate_signoff_ready);
  const bool toolchain_runtime_ga_operations_docs_runbook_sync_consistent =
      IsObjc3ToolchainRuntimeGaOperationsDocsRunbookSyncConsistent(
          toolchain_runtime_ga_operations_cross_lane_integration_consistent,
          toolchain_runtime_ga_operations_cross_lane_integration_ready,
          surface.long_tail_grammar_integration_closeout_consistent,
          surface.long_tail_grammar_gate_signoff_ready,
          surface.long_tail_grammar_integration_closeout_key,
          surface.parse_lowering_performance_quality_guardrails_key);
  const bool toolchain_runtime_ga_operations_docs_runbook_sync_ready =
      IsObjc3ToolchainRuntimeGaOperationsDocsRunbookSyncReady(
          toolchain_runtime_ga_operations_docs_runbook_sync_consistent,
          surface.long_tail_grammar_integration_closeout_key);
  const std::string toolchain_runtime_ga_operations_docs_runbook_sync_key =
      BuildObjc3ToolchainRuntimeGaOperationsDocsRunbookSyncKey(
          surface.long_tail_grammar_integration_closeout_consistent,
          surface.long_tail_grammar_gate_signoff_ready,
          surface.long_tail_grammar_integration_closeout_key,
          surface.parse_lowering_performance_quality_guardrails_key,
          toolchain_runtime_ga_operations_docs_runbook_sync_consistent,
          toolchain_runtime_ga_operations_docs_runbook_sync_ready);
  surface.toolchain_runtime_ga_operations_docs_runbook_sync_consistent =
      toolchain_runtime_ga_operations_docs_runbook_sync_consistent;
  surface.toolchain_runtime_ga_operations_docs_runbook_sync_ready =
      toolchain_runtime_ga_operations_docs_runbook_sync_ready;
  surface.toolchain_runtime_ga_operations_docs_runbook_sync_key =
      toolchain_runtime_ga_operations_docs_runbook_sync_key;
  surface.long_tail_grammar_integration_closeout_consistent =
      surface.long_tail_grammar_integration_closeout_consistent &&
      toolchain_runtime_ga_operations_docs_runbook_sync_consistent;
  surface.long_tail_grammar_gate_signoff_ready =
      surface.long_tail_grammar_gate_signoff_ready &&
      toolchain_runtime_ga_operations_docs_runbook_sync_ready;
  surface.long_tail_grammar_integration_closeout_key +=
      ";toolchain_runtime_ga_operations_docs_runbook_sync_key=" +
      toolchain_runtime_ga_operations_docs_runbook_sync_key;
  surface.parse_lowering_performance_quality_guardrails_key +=
      ";toolchain_runtime_ga_operations_docs_runbook_sync_key=" +
      toolchain_runtime_ga_operations_docs_runbook_sync_key;
  const bool toolchain_runtime_ga_operations_advanced_core_consistent =
      IsObjc3ToolchainRuntimeGaOperationsAdvancedCoreConsistent(
          toolchain_runtime_ga_operations_docs_runbook_sync_consistent,
          toolchain_runtime_ga_operations_docs_runbook_sync_ready,
          surface.long_tail_grammar_integration_closeout_consistent,
          surface.long_tail_grammar_gate_signoff_ready,
          toolchain_runtime_ga_operations_docs_runbook_sync_key,
          surface.long_tail_grammar_integration_closeout_key,
          surface.parse_lowering_performance_quality_guardrails_key);
  const bool toolchain_runtime_ga_operations_advanced_core_ready =
      IsObjc3ToolchainRuntimeGaOperationsAdvancedCoreReady(
          toolchain_runtime_ga_operations_advanced_core_consistent,
          toolchain_runtime_ga_operations_docs_runbook_sync_key,
          surface.long_tail_grammar_integration_closeout_key);
  const std::string toolchain_runtime_ga_operations_advanced_core_key =
      BuildObjc3ToolchainRuntimeGaOperationsAdvancedCoreKey(
          toolchain_runtime_ga_operations_docs_runbook_sync_consistent,
          toolchain_runtime_ga_operations_docs_runbook_sync_ready,
          toolchain_runtime_ga_operations_docs_runbook_sync_key,
          surface.long_tail_grammar_integration_closeout_key,
          surface.parse_lowering_performance_quality_guardrails_key,
          toolchain_runtime_ga_operations_advanced_core_consistent,
          toolchain_runtime_ga_operations_advanced_core_ready);
  surface.toolchain_runtime_ga_operations_advanced_core_consistent =
      toolchain_runtime_ga_operations_advanced_core_consistent;
  surface.toolchain_runtime_ga_operations_advanced_core_ready =
      toolchain_runtime_ga_operations_advanced_core_ready;
  surface.toolchain_runtime_ga_operations_advanced_core_key =
      toolchain_runtime_ga_operations_advanced_core_key;
  surface.long_tail_grammar_integration_closeout_consistent =
      surface.long_tail_grammar_integration_closeout_consistent &&
      toolchain_runtime_ga_operations_advanced_core_consistent;
  surface.long_tail_grammar_gate_signoff_ready =
      surface.long_tail_grammar_gate_signoff_ready &&
      toolchain_runtime_ga_operations_advanced_core_ready;
  surface.long_tail_grammar_integration_closeout_key +=
      ";toolchain_runtime_ga_operations_advanced_core_key=" +
      toolchain_runtime_ga_operations_advanced_core_key;
  surface.parse_lowering_performance_quality_guardrails_key +=
      ";toolchain_runtime_ga_operations_advanced_core_key=" +
      toolchain_runtime_ga_operations_advanced_core_key;
  const bool toolchain_runtime_ga_operations_advanced_edge_compatibility_consistent =
      IsObjc3ToolchainRuntimeGaOperationsAdvancedEdgeCompatibilityConsistent(
          toolchain_runtime_ga_operations_advanced_core_consistent,
          toolchain_runtime_ga_operations_advanced_core_ready,
          surface.long_tail_grammar_integration_closeout_consistent,
          surface.long_tail_grammar_gate_signoff_ready,
          toolchain_runtime_ga_operations_advanced_core_key,
          surface.long_tail_grammar_integration_closeout_key,
          surface.parse_lowering_performance_quality_guardrails_key);
  const bool toolchain_runtime_ga_operations_advanced_edge_compatibility_ready =
      IsObjc3ToolchainRuntimeGaOperationsAdvancedEdgeCompatibilityReady(
          toolchain_runtime_ga_operations_advanced_edge_compatibility_consistent,
          toolchain_runtime_ga_operations_advanced_core_key,
          surface.long_tail_grammar_integration_closeout_key);
  const std::string toolchain_runtime_ga_operations_advanced_edge_compatibility_key =
      BuildObjc3ToolchainRuntimeGaOperationsAdvancedEdgeCompatibilityKey(
          toolchain_runtime_ga_operations_advanced_core_consistent,
          toolchain_runtime_ga_operations_advanced_core_ready,
          toolchain_runtime_ga_operations_advanced_core_key,
          surface.long_tail_grammar_integration_closeout_key,
          surface.parse_lowering_performance_quality_guardrails_key,
          toolchain_runtime_ga_operations_advanced_edge_compatibility_consistent,
          toolchain_runtime_ga_operations_advanced_edge_compatibility_ready);
  surface.toolchain_runtime_ga_operations_advanced_edge_compatibility_consistent =
      toolchain_runtime_ga_operations_advanced_edge_compatibility_consistent;
  surface.toolchain_runtime_ga_operations_advanced_edge_compatibility_ready =
      toolchain_runtime_ga_operations_advanced_edge_compatibility_ready;
  surface.toolchain_runtime_ga_operations_advanced_edge_compatibility_key =
      toolchain_runtime_ga_operations_advanced_edge_compatibility_key;
  surface.long_tail_grammar_integration_closeout_consistent =
      surface.long_tail_grammar_integration_closeout_consistent &&
      toolchain_runtime_ga_operations_advanced_edge_compatibility_consistent;
  surface.long_tail_grammar_gate_signoff_ready =
      surface.long_tail_grammar_gate_signoff_ready &&
      toolchain_runtime_ga_operations_advanced_edge_compatibility_ready;
  surface.long_tail_grammar_integration_closeout_key +=
      ";toolchain_runtime_ga_operations_advanced_edge_compatibility_key=" +
      toolchain_runtime_ga_operations_advanced_edge_compatibility_key;
  surface.parse_lowering_performance_quality_guardrails_key +=
      ";toolchain_runtime_ga_operations_advanced_edge_compatibility_key=" +
      toolchain_runtime_ga_operations_advanced_edge_compatibility_key;
  const bool toolchain_runtime_ga_operations_advanced_diagnostics_consistent =
      IsObjc3ToolchainRuntimeGaOperationsAdvancedDiagnosticsConsistent(
          toolchain_runtime_ga_operations_advanced_edge_compatibility_consistent,
          toolchain_runtime_ga_operations_advanced_edge_compatibility_ready,
          surface.long_tail_grammar_integration_closeout_consistent,
          surface.long_tail_grammar_gate_signoff_ready,
          toolchain_runtime_ga_operations_advanced_edge_compatibility_key,
          surface.long_tail_grammar_integration_closeout_key,
          surface.parse_lowering_performance_quality_guardrails_key);
  const bool toolchain_runtime_ga_operations_advanced_diagnostics_ready =
      IsObjc3ToolchainRuntimeGaOperationsAdvancedDiagnosticsReady(
          toolchain_runtime_ga_operations_advanced_diagnostics_consistent,
          toolchain_runtime_ga_operations_advanced_edge_compatibility_key,
          surface.long_tail_grammar_integration_closeout_key);
  const std::string toolchain_runtime_ga_operations_advanced_diagnostics_key =
      BuildObjc3ToolchainRuntimeGaOperationsAdvancedDiagnosticsKey(
          toolchain_runtime_ga_operations_advanced_edge_compatibility_consistent,
          toolchain_runtime_ga_operations_advanced_edge_compatibility_ready,
          toolchain_runtime_ga_operations_advanced_edge_compatibility_key,
          surface.long_tail_grammar_integration_closeout_key,
          surface.parse_lowering_performance_quality_guardrails_key,
          toolchain_runtime_ga_operations_advanced_diagnostics_consistent,
          toolchain_runtime_ga_operations_advanced_diagnostics_ready);
  surface.toolchain_runtime_ga_operations_advanced_diagnostics_consistent =
      toolchain_runtime_ga_operations_advanced_diagnostics_consistent;
  surface.toolchain_runtime_ga_operations_advanced_diagnostics_ready =
      toolchain_runtime_ga_operations_advanced_diagnostics_ready;
  surface.toolchain_runtime_ga_operations_advanced_diagnostics_key =
      toolchain_runtime_ga_operations_advanced_diagnostics_key;
  surface.long_tail_grammar_integration_closeout_consistent =
      surface.long_tail_grammar_integration_closeout_consistent &&
      toolchain_runtime_ga_operations_advanced_diagnostics_consistent;
  surface.long_tail_grammar_gate_signoff_ready =
      surface.long_tail_grammar_gate_signoff_ready &&
      toolchain_runtime_ga_operations_advanced_diagnostics_ready;
  surface.long_tail_grammar_integration_closeout_key +=
      ";toolchain_runtime_ga_operations_advanced_diagnostics_key=" +
      toolchain_runtime_ga_operations_advanced_diagnostics_key;
  surface.parse_lowering_performance_quality_guardrails_key +=
      ";toolchain_runtime_ga_operations_advanced_diagnostics_key=" +
      toolchain_runtime_ga_operations_advanced_diagnostics_key;
  const bool toolchain_runtime_ga_operations_advanced_conformance_consistent =
      IsObjc3ToolchainRuntimeGaOperationsAdvancedConformanceConsistent(
          toolchain_runtime_ga_operations_advanced_diagnostics_consistent,
          toolchain_runtime_ga_operations_advanced_diagnostics_ready,
          surface.long_tail_grammar_integration_closeout_consistent,
          surface.long_tail_grammar_gate_signoff_ready,
          toolchain_runtime_ga_operations_advanced_diagnostics_key,
          surface.long_tail_grammar_integration_closeout_key,
          surface.parse_lowering_performance_quality_guardrails_key);
  const bool toolchain_runtime_ga_operations_advanced_conformance_ready =
      IsObjc3ToolchainRuntimeGaOperationsAdvancedConformanceReady(
          toolchain_runtime_ga_operations_advanced_conformance_consistent,
          toolchain_runtime_ga_operations_advanced_diagnostics_key,
          surface.long_tail_grammar_integration_closeout_key);
  const std::string toolchain_runtime_ga_operations_advanced_conformance_key =
      BuildObjc3ToolchainRuntimeGaOperationsAdvancedConformanceKey(
          toolchain_runtime_ga_operations_advanced_diagnostics_consistent,
          toolchain_runtime_ga_operations_advanced_diagnostics_ready,
          toolchain_runtime_ga_operations_advanced_diagnostics_key,
          surface.long_tail_grammar_integration_closeout_key,
          surface.parse_lowering_performance_quality_guardrails_key,
          toolchain_runtime_ga_operations_advanced_conformance_consistent,
          toolchain_runtime_ga_operations_advanced_conformance_ready);
  surface.toolchain_runtime_ga_operations_advanced_conformance_consistent =
      toolchain_runtime_ga_operations_advanced_conformance_consistent;
  surface.toolchain_runtime_ga_operations_advanced_conformance_ready =
      toolchain_runtime_ga_operations_advanced_conformance_ready;
  surface.toolchain_runtime_ga_operations_advanced_conformance_key =
      toolchain_runtime_ga_operations_advanced_conformance_key;
  surface.long_tail_grammar_integration_closeout_consistent =
      surface.long_tail_grammar_integration_closeout_consistent &&
      toolchain_runtime_ga_operations_advanced_conformance_consistent;
  surface.long_tail_grammar_gate_signoff_ready =
      surface.long_tail_grammar_gate_signoff_ready &&
      toolchain_runtime_ga_operations_advanced_conformance_ready;
  surface.long_tail_grammar_integration_closeout_key +=
      ";toolchain_runtime_ga_operations_advanced_conformance_key=" +
      toolchain_runtime_ga_operations_advanced_conformance_key;
  surface.parse_lowering_performance_quality_guardrails_key +=
      ";toolchain_runtime_ga_operations_advanced_conformance_key=" +
      toolchain_runtime_ga_operations_advanced_conformance_key;
  const bool toolchain_runtime_ga_operations_advanced_integration_consistent =
      IsObjc3ToolchainRuntimeGaOperationsAdvancedIntegrationConsistent(
          toolchain_runtime_ga_operations_advanced_conformance_consistent,
          toolchain_runtime_ga_operations_advanced_conformance_ready,
          surface.long_tail_grammar_integration_closeout_consistent,
          surface.long_tail_grammar_gate_signoff_ready,
          toolchain_runtime_ga_operations_advanced_conformance_key,
          surface.long_tail_grammar_integration_closeout_key,
          surface.parse_lowering_performance_quality_guardrails_key);
  const bool toolchain_runtime_ga_operations_advanced_integration_ready =
      IsObjc3ToolchainRuntimeGaOperationsAdvancedIntegrationReady(
          toolchain_runtime_ga_operations_advanced_integration_consistent,
          toolchain_runtime_ga_operations_advanced_conformance_key,
          surface.long_tail_grammar_integration_closeout_key);
  const std::string toolchain_runtime_ga_operations_advanced_integration_key =
      BuildObjc3ToolchainRuntimeGaOperationsAdvancedIntegrationKey(
          toolchain_runtime_ga_operations_advanced_conformance_consistent,
          toolchain_runtime_ga_operations_advanced_conformance_ready,
          toolchain_runtime_ga_operations_advanced_conformance_key,
          surface.long_tail_grammar_integration_closeout_key,
          surface.parse_lowering_performance_quality_guardrails_key,
          toolchain_runtime_ga_operations_advanced_integration_consistent,
          toolchain_runtime_ga_operations_advanced_integration_ready);
  surface.toolchain_runtime_ga_operations_advanced_integration_consistent =
      toolchain_runtime_ga_operations_advanced_integration_consistent;
  surface.toolchain_runtime_ga_operations_advanced_integration_ready =
      toolchain_runtime_ga_operations_advanced_integration_ready;
  surface.toolchain_runtime_ga_operations_advanced_integration_key =
      toolchain_runtime_ga_operations_advanced_integration_key;
  surface.long_tail_grammar_integration_closeout_consistent =
      surface.long_tail_grammar_integration_closeout_consistent &&
      toolchain_runtime_ga_operations_advanced_integration_consistent;
  surface.long_tail_grammar_gate_signoff_ready =
      surface.long_tail_grammar_gate_signoff_ready &&
      toolchain_runtime_ga_operations_advanced_integration_ready;
  surface.long_tail_grammar_integration_closeout_key +=
      ";toolchain_runtime_ga_operations_advanced_integration_key=" +
      toolchain_runtime_ga_operations_advanced_integration_key;
  surface.parse_lowering_performance_quality_guardrails_key +=
      ";toolchain_runtime_ga_operations_advanced_integration_key=" +
      toolchain_runtime_ga_operations_advanced_integration_key;
  const bool toolchain_runtime_ga_operations_advanced_performance_consistent =
      IsObjc3ToolchainRuntimeGaOperationsAdvancedPerformanceConsistent(
          toolchain_runtime_ga_operations_advanced_integration_consistent,
          toolchain_runtime_ga_operations_advanced_integration_ready,
          surface.long_tail_grammar_integration_closeout_consistent,
          surface.long_tail_grammar_gate_signoff_ready,
          toolchain_runtime_ga_operations_advanced_integration_key,
          surface.long_tail_grammar_integration_closeout_key,
          surface.parse_lowering_performance_quality_guardrails_key);
  const bool toolchain_runtime_ga_operations_advanced_performance_ready =
      IsObjc3ToolchainRuntimeGaOperationsAdvancedPerformanceReady(
          toolchain_runtime_ga_operations_advanced_performance_consistent,
          toolchain_runtime_ga_operations_advanced_integration_key,
          surface.long_tail_grammar_integration_closeout_key);
  const std::string toolchain_runtime_ga_operations_advanced_performance_key =
      BuildObjc3ToolchainRuntimeGaOperationsAdvancedPerformanceKey(
          toolchain_runtime_ga_operations_advanced_integration_consistent,
          toolchain_runtime_ga_operations_advanced_integration_ready,
          toolchain_runtime_ga_operations_advanced_integration_key,
          surface.long_tail_grammar_integration_closeout_key,
          surface.parse_lowering_performance_quality_guardrails_key,
          toolchain_runtime_ga_operations_advanced_performance_consistent,
          toolchain_runtime_ga_operations_advanced_performance_ready);
  surface.toolchain_runtime_ga_operations_advanced_performance_consistent =
      toolchain_runtime_ga_operations_advanced_performance_consistent;
  surface.toolchain_runtime_ga_operations_advanced_performance_ready =
      toolchain_runtime_ga_operations_advanced_performance_ready;
  surface.toolchain_runtime_ga_operations_advanced_performance_key =
      toolchain_runtime_ga_operations_advanced_performance_key;
  surface.long_tail_grammar_integration_closeout_consistent =
      surface.long_tail_grammar_integration_closeout_consistent &&
      toolchain_runtime_ga_operations_advanced_performance_consistent;
  surface.long_tail_grammar_gate_signoff_ready =
      surface.long_tail_grammar_gate_signoff_ready &&
      toolchain_runtime_ga_operations_advanced_performance_ready;
  surface.long_tail_grammar_integration_closeout_key +=
      ";toolchain_runtime_ga_operations_advanced_performance_key=" +
      toolchain_runtime_ga_operations_advanced_performance_key;
  surface.parse_lowering_performance_quality_guardrails_key +=
      ";toolchain_runtime_ga_operations_advanced_performance_key=" +
      toolchain_runtime_ga_operations_advanced_performance_key;
  const bool toolchain_runtime_ga_operations_advanced_core_shard2_consistent =
      IsObjc3ToolchainRuntimeGaOperationsAdvancedCoreShard2Consistent(
          toolchain_runtime_ga_operations_advanced_performance_consistent,
          toolchain_runtime_ga_operations_advanced_performance_ready,
          surface.long_tail_grammar_integration_closeout_consistent,
          surface.long_tail_grammar_gate_signoff_ready,
          toolchain_runtime_ga_operations_advanced_performance_key,
          surface.long_tail_grammar_integration_closeout_key,
          surface.parse_lowering_performance_quality_guardrails_key);
  const bool toolchain_runtime_ga_operations_advanced_core_shard2_ready =
      IsObjc3ToolchainRuntimeGaOperationsAdvancedCoreShard2Ready(
          toolchain_runtime_ga_operations_advanced_core_shard2_consistent,
          toolchain_runtime_ga_operations_advanced_performance_key,
          surface.long_tail_grammar_integration_closeout_key);
  const std::string toolchain_runtime_ga_operations_advanced_core_shard2_key =
      BuildObjc3ToolchainRuntimeGaOperationsAdvancedCoreShard2Key(
          toolchain_runtime_ga_operations_advanced_performance_consistent,
          toolchain_runtime_ga_operations_advanced_performance_ready,
          toolchain_runtime_ga_operations_advanced_performance_key,
          surface.long_tail_grammar_integration_closeout_key,
          surface.parse_lowering_performance_quality_guardrails_key,
          toolchain_runtime_ga_operations_advanced_core_shard2_consistent,
          toolchain_runtime_ga_operations_advanced_core_shard2_ready);
  surface.toolchain_runtime_ga_operations_advanced_core_shard2_consistent =
      toolchain_runtime_ga_operations_advanced_core_shard2_consistent;
  surface.toolchain_runtime_ga_operations_advanced_core_shard2_ready =
      toolchain_runtime_ga_operations_advanced_core_shard2_ready;
  surface.toolchain_runtime_ga_operations_advanced_core_shard2_key =
      toolchain_runtime_ga_operations_advanced_core_shard2_key;
  surface.long_tail_grammar_integration_closeout_consistent =
      surface.long_tail_grammar_integration_closeout_consistent &&
      toolchain_runtime_ga_operations_advanced_core_shard2_consistent;
  surface.long_tail_grammar_gate_signoff_ready =
      surface.long_tail_grammar_gate_signoff_ready &&
      toolchain_runtime_ga_operations_advanced_core_shard2_ready;
  surface.long_tail_grammar_integration_closeout_key +=
      ";toolchain_runtime_ga_operations_advanced_core_shard2_key=" +
      toolchain_runtime_ga_operations_advanced_core_shard2_key;
  surface.parse_lowering_performance_quality_guardrails_key +=
      ";toolchain_runtime_ga_operations_advanced_core_shard2_key=" +
      toolchain_runtime_ga_operations_advanced_core_shard2_key;
  const bool toolchain_runtime_ga_operations_integration_closeout_signoff_consistent =
      IsObjc3ToolchainRuntimeGaOperationsIntegrationCloseoutSignoffConsistent(
          toolchain_runtime_ga_operations_advanced_core_shard2_consistent,
          toolchain_runtime_ga_operations_advanced_core_shard2_ready,
          surface.long_tail_grammar_integration_closeout_consistent,
          surface.long_tail_grammar_gate_signoff_ready,
          toolchain_runtime_ga_operations_advanced_core_shard2_key,
          surface.long_tail_grammar_integration_closeout_key,
          surface.parse_lowering_performance_quality_guardrails_key);
  const bool toolchain_runtime_ga_operations_integration_closeout_signoff_ready =
      IsObjc3ToolchainRuntimeGaOperationsIntegrationCloseoutSignoffReady(
          toolchain_runtime_ga_operations_integration_closeout_signoff_consistent,
          toolchain_runtime_ga_operations_advanced_core_shard2_key,
          surface.long_tail_grammar_integration_closeout_key);
  const std::string toolchain_runtime_ga_operations_integration_closeout_signoff_key =
      BuildObjc3ToolchainRuntimeGaOperationsIntegrationCloseoutSignoffKey(
          toolchain_runtime_ga_operations_advanced_core_shard2_consistent,
          toolchain_runtime_ga_operations_advanced_core_shard2_ready,
          toolchain_runtime_ga_operations_advanced_core_shard2_key,
          surface.long_tail_grammar_integration_closeout_key,
          surface.parse_lowering_performance_quality_guardrails_key,
          toolchain_runtime_ga_operations_integration_closeout_signoff_consistent,
          toolchain_runtime_ga_operations_integration_closeout_signoff_ready);
  surface.toolchain_runtime_ga_operations_integration_closeout_signoff_consistent =
      toolchain_runtime_ga_operations_integration_closeout_signoff_consistent;
  surface.toolchain_runtime_ga_operations_integration_closeout_signoff_ready =
      toolchain_runtime_ga_operations_integration_closeout_signoff_ready;
  surface.toolchain_runtime_ga_operations_integration_closeout_signoff_key =
      toolchain_runtime_ga_operations_integration_closeout_signoff_key;
  surface.long_tail_grammar_integration_closeout_consistent =
      surface.long_tail_grammar_integration_closeout_consistent &&
      toolchain_runtime_ga_operations_integration_closeout_signoff_consistent;
  surface.long_tail_grammar_gate_signoff_ready =
      surface.long_tail_grammar_gate_signoff_ready &&
      toolchain_runtime_ga_operations_integration_closeout_signoff_ready;
  surface.long_tail_grammar_integration_closeout_key +=
      ";toolchain_runtime_ga_operations_integration_closeout_signoff_key=" +
      toolchain_runtime_ga_operations_integration_closeout_signoff_key;
  surface.parse_lowering_performance_quality_guardrails_key +=
      ";toolchain_runtime_ga_operations_integration_closeout_signoff_key=" +
      toolchain_runtime_ga_operations_integration_closeout_signoff_key;
  surface.ready_for_lowering = diagnostics_clear &&
                               parse_snapshot_replay_ready &&
                               sema_handoff_ready &&
                               surface.lowering_boundary_ready &&
                               parse_lowering_conformance_matrix_ready &&
                               surface.parser_diagnostic_grammar_hooks_conformance_corpus_ready &&
                               parse_lowering_conformance_corpus_ready &&
                               parse_lowering_performance_quality_guardrails_ready_gate &&
                               surface.long_tail_grammar_gate_signoff_ready;

  if (surface.ready_for_lowering || !surface.failure_reason.empty()) {
    return surface;
  }

  if (surface.lexer_diagnostic_count != 0) {
    surface.failure_reason = "lexer diagnostics present";
  } else if (surface.parser_diagnostic_count != 0) {
    surface.failure_reason = "parser diagnostics present";
  } else if (surface.semantic_diagnostic_count != 0) {
    surface.failure_reason = "semantic diagnostics present";
  } else if (!surface.parser_contract_snapshot_present) {
    surface.failure_reason = "parser contract snapshot missing";
  } else if (!surface.parser_contract_deterministic) {
    surface.failure_reason = "parser handoff is not deterministic";
  } else if (!surface.parser_recovery_replay_ready) {
    surface.failure_reason = "parser recovery handoff is not replay ready";
  } else if (!surface.long_tail_grammar_core_feature_consistent) {
    surface.failure_reason = "long-tail grammar core feature is inconsistent";
  } else if (!surface.long_tail_grammar_handoff_key_deterministic) {
    surface.failure_reason = "long-tail grammar handoff key is not deterministic";
  } else if (!surface.long_tail_grammar_expansion_accounting_consistent) {
    surface.failure_reason = "long-tail grammar expansion accounting is inconsistent";
  } else if (!surface.parse_artifact_handoff_consistent) {
    surface.failure_reason = "parse artifact handoff is inconsistent";
  } else if (!surface.parser_diagnostic_surface_consistent) {
    surface.failure_reason = "parser diagnostics surface is inconsistent";
  } else if (!surface.parser_diagnostic_source_precision_scaffold_ready) {
    surface.failure_reason = "parser diagnostic source-precision scaffold is not ready";
  } else if (!surface.parser_diagnostic_grammar_hooks_core_feature_ready) {
    surface.failure_reason = "parser diagnostic grammar hooks core feature is not ready";
  } else if (!surface.parser_diagnostic_grammar_hooks_core_feature_expansion_accounting_consistent) {
    surface.failure_reason =
        "parser diagnostic grammar hooks core feature expansion accounting is inconsistent";
  } else if (!surface.parser_diagnostic_grammar_hooks_core_feature_expansion_replay_keys_ready) {
    surface.failure_reason =
        "parser diagnostic grammar hooks core feature expansion replay keys are not ready";
  } else if (!surface.parser_diagnostic_grammar_hooks_core_feature_expansion_ready) {
    surface.failure_reason = "parser diagnostic grammar hooks core feature expansion is not ready";
  } else if (!surface.parser_diagnostic_code_surface_deterministic) {
    surface.failure_reason = "parser diagnostic code surface is not deterministic";
  } else if (!surface.parse_artifact_handoff_deterministic) {
    surface.failure_reason = "parse artifact handoff is not deterministic";
  } else if (!surface.parse_artifact_layout_fingerprint_consistent) {
    surface.failure_reason = "parse artifact layout fingerprint is inconsistent";
  } else if (!surface.parse_artifact_fingerprint_consistent) {
    surface.failure_reason = "parse artifact fingerprint is inconsistent";
  } else if (!surface.compatibility_handoff_consistent) {
    surface.failure_reason = "compatibility handoff is inconsistent";
  } else if (!surface.long_tail_grammar_compatibility_handoff_ready) {
    surface.failure_reason = "long-tail grammar compatibility handoff is not ready";
  } else if (!surface.parse_artifact_replay_key_deterministic) {
    surface.failure_reason = "parse artifact replay key is not deterministic";
  } else if (!surface.long_tail_grammar_replay_keys_ready) {
    surface.failure_reason = "long-tail grammar replay keys are not ready";
  } else if (!surface.long_tail_grammar_expansion_ready) {
    surface.failure_reason = "long-tail grammar core feature expansion is not ready";
  } else if (!surface.parse_artifact_diagnostics_hardening_consistent) {
    surface.failure_reason = "parse artifact diagnostics hardening is inconsistent";
  } else if (!surface.parser_token_count_budget_consistent) {
    surface.failure_reason = "parser token count budget is inconsistent";
  } else if (!surface.language_version_pragma_coordinate_order_consistent) {
    surface.failure_reason = "language-version pragma coordinate order is inconsistent";
  } else if (!surface.parser_diagnostic_grammar_hooks_edge_case_compatibility_consistent) {
    surface.failure_reason = "parser diagnostic grammar hooks edge-case compatibility is inconsistent";
  } else if (!surface.parser_diagnostic_grammar_hooks_edge_case_compatibility_ready) {
    surface.failure_reason = "parser diagnostic grammar hooks edge-case compatibility is not ready";
  } else if (!surface.parser_diagnostic_grammar_hooks_edge_case_expansion_consistent) {
    surface.failure_reason = "parser diagnostic grammar hooks edge-case expansion is inconsistent";
  } else if (!surface.parser_diagnostic_grammar_hooks_edge_case_robustness_ready) {
    surface.failure_reason = "parser diagnostic grammar hooks edge-case robustness is not ready";
  } else if (!surface.parser_diagnostic_grammar_hooks_diagnostics_hardening_consistent) {
    surface.failure_reason = "parser diagnostic grammar hooks diagnostics hardening is inconsistent";
  } else if (!surface.parser_diagnostic_grammar_hooks_diagnostics_hardening_ready) {
    surface.failure_reason = "parser diagnostic grammar hooks diagnostics hardening is not ready";
  } else if (!surface.parser_diagnostic_grammar_hooks_recovery_determinism_consistent) {
    surface.failure_reason =
        "parser diagnostic grammar hooks recovery/determinism hardening is inconsistent";
  } else if (!surface.parser_diagnostic_grammar_hooks_recovery_determinism_ready) {
    surface.failure_reason =
        "parser diagnostic grammar hooks recovery/determinism hardening is not ready";
  } else if (!surface.parser_diagnostic_grammar_hooks_conformance_matrix_consistent) {
    surface.failure_reason =
        "parser diagnostic grammar hooks conformance matrix is inconsistent";
  } else if (!surface.parser_diagnostic_grammar_hooks_conformance_matrix_ready) {
    surface.failure_reason =
        "parser diagnostic grammar hooks conformance matrix is not ready";
  } else if (!surface.parser_diagnostic_grammar_hooks_conformance_corpus_consistent) {
    surface.failure_reason =
        "parser diagnostic grammar hooks conformance corpus is inconsistent";
  } else if (!surface.parser_diagnostic_grammar_hooks_conformance_corpus_ready) {
    surface.failure_reason =
        "parser diagnostic grammar hooks conformance corpus is not ready";
  } else if (!surface.parse_artifact_edge_case_robustness_consistent) {
    surface.failure_reason = "parse artifact edge-case robustness is inconsistent";
  } else if (!surface.long_tail_grammar_edge_case_compatibility_consistent) {
    surface.failure_reason = "long-tail grammar edge-case compatibility is inconsistent";
  } else if (!surface.long_tail_grammar_edge_case_compatibility_ready) {
    surface.failure_reason = "long-tail grammar edge-case compatibility is not ready";
  } else if (!surface.long_tail_grammar_edge_case_expansion_consistent) {
    surface.failure_reason = "long-tail grammar edge-case expansion is inconsistent";
  } else if (!surface.long_tail_grammar_edge_case_robustness_ready) {
    surface.failure_reason = "long-tail grammar edge-case robustness is not ready";
  } else if (!surface.long_tail_grammar_diagnostics_hardening_consistent) {
    surface.failure_reason = "long-tail grammar diagnostics hardening is inconsistent";
  } else if (!surface.long_tail_grammar_diagnostics_hardening_ready) {
    surface.failure_reason = "long-tail grammar diagnostics hardening is not ready";
  } else if (
      !IsObjc3ToolchainRuntimeGaOperationsRecoveryDeterminismHardeningConsistent(
          surface.parser_recovery_replay_ready,
          surface.parse_artifact_replay_key_deterministic,
          surface.long_tail_grammar_replay_keys_ready,
          surface.long_tail_grammar_diagnostics_hardening_ready,
          surface.parse_recovery_determinism_hardening_consistent,
          surface.parse_artifact_handoff_key,
          surface.parse_artifact_replay_key,
          surface.parse_artifact_diagnostics_hardening_key,
          surface.parse_artifact_edge_robustness_key,
          surface.long_tail_grammar_handoff_key,
          surface.long_tail_grammar_diagnostics_hardening_key,
          surface.parse_recovery_determinism_hardening_key)) {
    surface.failure_reason = "toolchain/runtime GA operations recovery/determinism hardening is inconsistent";
  } else if (
      !IsObjc3ToolchainRuntimeGaOperationsRecoveryDeterminismHardeningReady(
          IsObjc3ToolchainRuntimeGaOperationsRecoveryDeterminismHardeningConsistent(
              surface.parser_recovery_replay_ready,
              surface.parse_artifact_replay_key_deterministic,
              surface.long_tail_grammar_replay_keys_ready,
              surface.long_tail_grammar_diagnostics_hardening_ready,
              surface.parse_recovery_determinism_hardening_consistent,
              surface.parse_artifact_handoff_key,
              surface.parse_artifact_replay_key,
              surface.parse_artifact_diagnostics_hardening_key,
              surface.parse_artifact_edge_robustness_key,
              surface.long_tail_grammar_handoff_key,
              surface.long_tail_grammar_diagnostics_hardening_key,
              surface.parse_recovery_determinism_hardening_key),
          surface.long_tail_grammar_recovery_determinism_consistent,
          surface.long_tail_grammar_recovery_determinism_ready,
          surface.long_tail_grammar_recovery_determinism_key)) {
    surface.failure_reason = "toolchain/runtime GA operations recovery/determinism hardening is not ready";
  } else if (!surface.long_tail_grammar_recovery_determinism_consistent) {
    surface.failure_reason = "long-tail grammar recovery/determinism hardening is inconsistent";
  } else if (!surface.long_tail_grammar_recovery_determinism_ready) {
    surface.failure_reason = "long-tail grammar recovery/determinism hardening is not ready";
  } else if (
      !IsObjc3ToolchainRuntimeGaOperationsConformanceMatrixConsistent(
          IsObjc3ToolchainRuntimeGaOperationsRecoveryDeterminismHardeningConsistent(
              surface.parser_recovery_replay_ready,
              surface.parse_artifact_replay_key_deterministic,
              surface.long_tail_grammar_replay_keys_ready,
              surface.long_tail_grammar_diagnostics_hardening_ready,
              surface.parse_recovery_determinism_hardening_consistent,
              surface.parse_artifact_handoff_key,
              surface.parse_artifact_replay_key,
              surface.parse_artifact_diagnostics_hardening_key,
              surface.parse_artifact_edge_robustness_key,
              surface.long_tail_grammar_handoff_key,
              surface.long_tail_grammar_diagnostics_hardening_key,
              surface.parse_recovery_determinism_hardening_key),
          IsObjc3ToolchainRuntimeGaOperationsRecoveryDeterminismHardeningReady(
              IsObjc3ToolchainRuntimeGaOperationsRecoveryDeterminismHardeningConsistent(
                  surface.parser_recovery_replay_ready,
                  surface.parse_artifact_replay_key_deterministic,
                  surface.long_tail_grammar_replay_keys_ready,
                  surface.long_tail_grammar_diagnostics_hardening_ready,
                  surface.parse_recovery_determinism_hardening_consistent,
                  surface.parse_artifact_handoff_key,
                  surface.parse_artifact_replay_key,
                  surface.parse_artifact_diagnostics_hardening_key,
                  surface.parse_artifact_edge_robustness_key,
                  surface.long_tail_grammar_handoff_key,
                  surface.long_tail_grammar_diagnostics_hardening_key,
                  surface.parse_recovery_determinism_hardening_key),
              surface.long_tail_grammar_recovery_determinism_consistent,
              surface.long_tail_grammar_recovery_determinism_ready,
              surface.long_tail_grammar_recovery_determinism_key),
          surface.parse_lowering_conformance_matrix_consistent,
          surface.long_tail_grammar_conformance_matrix_consistent,
          surface.long_tail_grammar_conformance_matrix_ready,
          surface.parse_recovery_determinism_hardening_key,
          surface.long_tail_grammar_recovery_determinism_key)) {
    surface.failure_reason =
        "toolchain/runtime GA operations conformance matrix is inconsistent";
  } else if (
      !IsObjc3ToolchainRuntimeGaOperationsConformanceMatrixReady(
          IsObjc3ToolchainRuntimeGaOperationsConformanceMatrixConsistent(
              IsObjc3ToolchainRuntimeGaOperationsRecoveryDeterminismHardeningConsistent(
                  surface.parser_recovery_replay_ready,
                  surface.parse_artifact_replay_key_deterministic,
                  surface.long_tail_grammar_replay_keys_ready,
                  surface.long_tail_grammar_diagnostics_hardening_ready,
                  surface.parse_recovery_determinism_hardening_consistent,
                  surface.parse_artifact_handoff_key,
                  surface.parse_artifact_replay_key,
                  surface.parse_artifact_diagnostics_hardening_key,
                  surface.parse_artifact_edge_robustness_key,
                  surface.long_tail_grammar_handoff_key,
                  surface.long_tail_grammar_diagnostics_hardening_key,
                  surface.parse_recovery_determinism_hardening_key),
              IsObjc3ToolchainRuntimeGaOperationsRecoveryDeterminismHardeningReady(
                  IsObjc3ToolchainRuntimeGaOperationsRecoveryDeterminismHardeningConsistent(
                      surface.parser_recovery_replay_ready,
                      surface.parse_artifact_replay_key_deterministic,
                      surface.long_tail_grammar_replay_keys_ready,
                      surface.long_tail_grammar_diagnostics_hardening_ready,
                      surface.parse_recovery_determinism_hardening_consistent,
                      surface.parse_artifact_handoff_key,
                      surface.parse_artifact_replay_key,
                      surface.parse_artifact_diagnostics_hardening_key,
                      surface.parse_artifact_edge_robustness_key,
                      surface.long_tail_grammar_handoff_key,
                      surface.long_tail_grammar_diagnostics_hardening_key,
                      surface.parse_recovery_determinism_hardening_key),
                  surface.long_tail_grammar_recovery_determinism_consistent,
                  surface.long_tail_grammar_recovery_determinism_ready,
                  surface.long_tail_grammar_recovery_determinism_key),
              surface.parse_lowering_conformance_matrix_consistent,
              surface.long_tail_grammar_conformance_matrix_consistent,
              surface.long_tail_grammar_conformance_matrix_ready,
              surface.parse_recovery_determinism_hardening_key,
              surface.long_tail_grammar_recovery_determinism_key),
          surface.parse_lowering_conformance_matrix_key,
          surface.long_tail_grammar_conformance_matrix_key)) {
    surface.failure_reason =
        "toolchain/runtime GA operations conformance matrix is not ready";
  } else if (!toolchain_runtime_ga_operations_conformance_corpus_consistent) {
    surface.failure_reason =
        "toolchain/runtime GA operations conformance corpus is inconsistent";
  } else if (!toolchain_runtime_ga_operations_conformance_corpus_ready) {
    surface.failure_reason =
        "toolchain/runtime GA operations conformance corpus is not ready";
  } else if (!surface.long_tail_grammar_conformance_matrix_consistent) {
    surface.failure_reason = "long-tail grammar conformance matrix is inconsistent";
  } else if (!surface.long_tail_grammar_conformance_matrix_ready) {
    surface.failure_reason = "long-tail grammar conformance matrix is not ready";
  } else if (!surface.parse_recovery_determinism_hardening_consistent) {
    surface.failure_reason = "parse recovery/determinism hardening is inconsistent";
  } else if (!surface.semantic_integration_surface_built) {
    surface.failure_reason = "semantic integration surface not built";
  } else if (!surface.semantic_diagnostics_deterministic) {
    surface.failure_reason = "semantic diagnostics handoff is not deterministic";
  } else if (!surface.semantic_type_metadata_deterministic) {
    surface.failure_reason = "semantic type metadata handoff is not deterministic";
  } else if (!surface.protocol_category_deterministic) {
    surface.failure_reason = "protocol/category handoff is not deterministic";
  } else if (!surface.class_protocol_category_linking_deterministic) {
    surface.failure_reason = "class/protocol/category linking handoff is not deterministic";
  } else if (!surface.selector_normalization_deterministic) {
    surface.failure_reason = "selector normalization handoff is not deterministic";
  } else if (!surface.property_attribute_deterministic) {
    surface.failure_reason = "property attribute handoff is not deterministic";
  } else if (!surface.symbol_graph_deterministic) {
    surface.failure_reason = "symbol graph handoff is not deterministic";
  } else if (!surface.scope_resolution_deterministic) {
    surface.failure_reason = "scope resolution handoff is not deterministic";
  } else if (!surface.object_pointer_type_handoff_deterministic) {
    surface.failure_reason = "object pointer/nullability handoff is not deterministic";
  } else if (!surface.typed_handoff_key_deterministic) {
    surface.failure_reason = "typed sema-to-lowering handoff key is not deterministic";
  } else if (!surface.typed_sema_core_feature_consistent) {
    surface.failure_reason = "typed sema-to-lowering core feature contract is inconsistent";
  } else if (!surface.typed_sema_core_feature_expansion_consistent) {
    surface.failure_reason = "typed sema-to-lowering core feature expansion is inconsistent";
  } else if (surface.typed_sema_core_feature_expansion_key.empty()) {
    surface.failure_reason = "typed sema-to-lowering core feature expansion key is empty";
  } else if (!surface.typed_sema_edge_case_compatibility_consistent) {
    surface.failure_reason = "typed sema-to-lowering edge-case compatibility is inconsistent";
  } else if (!surface.typed_sema_edge_case_compatibility_ready) {
    surface.failure_reason = "typed sema-to-lowering edge-case compatibility is not ready";
  } else if (surface.typed_sema_edge_case_compatibility_key.empty()) {
    surface.failure_reason = "typed sema-to-lowering edge-case compatibility key is empty";
  } else if (!surface.typed_sema_edge_case_expansion_consistent) {
    surface.failure_reason = "typed sema-to-lowering edge-case expansion is inconsistent";
  } else if (!surface.typed_sema_edge_case_robustness_ready) {
    surface.failure_reason = "typed sema-to-lowering edge-case robustness is not ready";
  } else if (surface.typed_sema_edge_case_robustness_key.empty()) {
    surface.failure_reason = "typed sema-to-lowering edge-case robustness key is empty";
  } else if (!surface.typed_sema_diagnostics_hardening_consistent) {
    surface.failure_reason = "typed sema-to-lowering diagnostics hardening is inconsistent";
  } else if (!surface.typed_sema_diagnostics_hardening_ready) {
    surface.failure_reason = "typed sema-to-lowering diagnostics hardening is not ready";
  } else if (surface.typed_sema_diagnostics_hardening_key.empty()) {
    surface.failure_reason = "typed sema-to-lowering diagnostics hardening key is empty";
  } else if (!surface.typed_sema_recovery_determinism_consistent) {
    surface.failure_reason = "typed sema-to-lowering recovery/determinism is inconsistent";
  } else if (!surface.typed_sema_recovery_determinism_ready) {
    surface.failure_reason = "typed sema-to-lowering recovery/determinism is not ready";
  } else if (surface.typed_sema_recovery_determinism_key.empty()) {
    surface.failure_reason = "typed sema-to-lowering recovery/determinism key is empty";
  } else if (!surface.typed_sema_conformance_matrix_consistent) {
    surface.failure_reason = "typed sema-to-lowering conformance matrix is inconsistent";
  } else if (!surface.typed_sema_conformance_matrix_ready) {
    surface.failure_reason = "typed sema-to-lowering conformance matrix is not ready";
  } else if (surface.typed_sema_conformance_matrix_key.empty()) {
    surface.failure_reason = "typed sema-to-lowering conformance matrix key is empty";
  } else if (!surface.typed_sema_conformance_corpus_consistent) {
    surface.failure_reason = "typed sema-to-lowering conformance corpus is inconsistent";
  } else if (!surface.typed_sema_conformance_corpus_ready) {
    surface.failure_reason = "typed sema-to-lowering conformance corpus is not ready";
  } else if (surface.typed_sema_conformance_corpus_key.empty()) {
    surface.failure_reason = "typed sema-to-lowering conformance corpus key is empty";
  } else if (!surface.typed_sema_performance_quality_guardrails_consistent) {
    surface.failure_reason = "typed sema-to-lowering performance/quality guardrails are inconsistent";
  } else if (!surface.typed_sema_performance_quality_guardrails_ready) {
    surface.failure_reason = "typed sema-to-lowering performance/quality guardrails are not ready";
  } else if (surface.typed_sema_performance_quality_guardrails_key.empty()) {
    surface.failure_reason = "typed sema-to-lowering performance/quality guardrails key is empty";
  } else if (!surface.typed_sema_cross_lane_integration_consistent) {
    surface.failure_reason = "typed sema-to-lowering cross-lane integration is inconsistent";
  } else if (!surface.typed_sema_cross_lane_integration_ready) {
    surface.failure_reason = "typed sema-to-lowering cross-lane integration is not ready";
  } else if (surface.typed_sema_cross_lane_integration_key.empty()) {
    surface.failure_reason = "typed sema-to-lowering cross-lane integration key is empty";
  } else if (!surface.typed_sema_docs_runbook_sync_consistent) {
    surface.failure_reason = "typed sema-to-lowering docs/runbook synchronization is inconsistent";
  } else if (!surface.typed_sema_docs_runbook_sync_ready) {
    surface.failure_reason = "typed sema-to-lowering docs/runbook synchronization is not ready";
  } else if (surface.typed_sema_docs_runbook_sync_key.empty()) {
    surface.failure_reason = "typed sema-to-lowering docs/runbook synchronization key is empty";
  } else if (!surface.typed_sema_release_candidate_replay_dry_run_consistent) {
    surface.failure_reason =
        "typed sema-to-lowering release-candidate replay dry-run is inconsistent";
  } else if (!surface.typed_sema_release_candidate_replay_dry_run_ready) {
    surface.failure_reason =
        "typed sema-to-lowering release-candidate replay dry-run is not ready";
  } else if (surface.typed_sema_release_candidate_replay_dry_run_key.empty()) {
    surface.failure_reason =
        "typed sema-to-lowering release-candidate replay dry-run key is empty";
  } else if (!surface.typed_sema_advanced_core_shard1_consistent) {
    surface.failure_reason = "typed sema-to-lowering advanced core shard 1 is inconsistent";
  } else if (!surface.typed_sema_advanced_core_shard1_ready) {
    surface.failure_reason = "typed sema-to-lowering advanced core shard 1 is not ready";
  } else if (surface.typed_sema_advanced_core_shard1_key.empty()) {
    surface.failure_reason = "typed sema-to-lowering advanced core shard 1 key is empty";
  } else if (!typed_edge_case_compatibility_alignment) {
    surface.failure_reason = "typed sema-to-lowering edge-case compatibility drifted from parse/lowering readiness";
  } else if (!typed_edge_case_robustness_alignment) {
    surface.failure_reason = "typed sema-to-lowering edge-case robustness drifted from parse/lowering readiness";
  } else if (!typed_diagnostics_hardening_alignment) {
    surface.failure_reason = "typed sema-to-lowering diagnostics hardening drifted from parse/lowering readiness";
  } else if (!typed_recovery_determinism_alignment) {
    surface.failure_reason = "typed sema-to-lowering recovery/determinism drifted from parse/lowering readiness";
  } else if (!typed_conformance_matrix_alignment) {
    surface.failure_reason = "typed sema-to-lowering conformance matrix drifted from parse/lowering readiness";
  } else if (!typed_conformance_corpus_alignment) {
    surface.failure_reason = "typed sema-to-lowering conformance corpus drifted from parse/lowering readiness";
  } else if (!typed_performance_quality_guardrails_alignment) {
    surface.failure_reason =
        "typed sema-to-lowering performance/quality guardrails drifted from parse/lowering readiness";
  } else if (!typed_cross_lane_integration_alignment) {
    surface.failure_reason =
        "typed sema-to-lowering cross-lane integration drifted from parse/lowering readiness";
  } else if (!typed_docs_runbook_sync_alignment) {
    surface.failure_reason =
        "typed sema-to-lowering docs/runbook synchronization drifted from parse/lowering readiness";
  } else if (!typed_release_candidate_replay_dry_run_alignment) {
    surface.failure_reason =
        "typed sema-to-lowering release-candidate replay dry-run drifted from parse/lowering readiness";
  } else if (!typed_advanced_core_shard1_alignment) {
    surface.failure_reason =
        "typed sema-to-lowering advanced core shard 1 drifted from parse/lowering readiness";
  } else if (!surface.lowering_boundary_ready) {
    surface.failure_reason = "lowering boundary is not ready";
  } else if (!surface.parse_lowering_conformance_matrix_consistent) {
    surface.failure_reason = "parse-lowering conformance matrix is inconsistent";
  } else if (!surface.parse_lowering_conformance_corpus_consistent) {
    surface.failure_reason = "parse-lowering conformance corpus is inconsistent";
  } else if (!toolchain_runtime_ga_operations_performance_quality_guardrails_consistent) {
    surface.failure_reason =
        "toolchain/runtime GA operations performance quality guardrails are inconsistent";
  } else if (!toolchain_runtime_ga_operations_performance_quality_guardrails_ready) {
    surface.failure_reason =
        "toolchain/runtime GA operations performance quality guardrails are not ready";
  } else if (!surface.parse_lowering_performance_quality_guardrails_consistent) {
    surface.failure_reason = "parse-lowering performance/quality guardrails are inconsistent";
  } else if (!toolchain_runtime_ga_operations_cross_lane_integration_consistent) {
    surface.failure_reason =
        "toolchain/runtime GA operations cross-lane integration is inconsistent";
  } else if (!toolchain_runtime_ga_operations_cross_lane_integration_ready) {
    surface.failure_reason =
        "toolchain/runtime GA operations cross-lane integration is not ready";
  } else if (!toolchain_runtime_ga_operations_docs_runbook_sync_consistent) {
    surface.failure_reason =
        "toolchain/runtime GA operations docs and runbook synchronization is inconsistent";
  } else if (!toolchain_runtime_ga_operations_docs_runbook_sync_ready) {
    surface.failure_reason =
        "toolchain/runtime GA operations docs and runbook synchronization is not ready";
  } else if (!toolchain_runtime_ga_operations_advanced_core_consistent) {
    surface.failure_reason =
        "toolchain/runtime GA operations advanced core workpack is inconsistent";
  } else if (!toolchain_runtime_ga_operations_advanced_core_ready) {
    surface.failure_reason =
        "toolchain/runtime GA operations advanced core workpack is not ready";
  } else if (!toolchain_runtime_ga_operations_advanced_edge_compatibility_consistent) {
    surface.failure_reason =
        "toolchain/runtime GA operations advanced edge compatibility workpack is inconsistent";
  } else if (!toolchain_runtime_ga_operations_advanced_edge_compatibility_ready) {
    surface.failure_reason =
        "toolchain/runtime GA operations advanced edge compatibility workpack is not ready";
  } else if (!toolchain_runtime_ga_operations_advanced_diagnostics_consistent) {
    surface.failure_reason =
        "toolchain/runtime GA operations advanced diagnostics workpack is inconsistent";
  } else if (!toolchain_runtime_ga_operations_advanced_diagnostics_ready) {
    surface.failure_reason =
        "toolchain/runtime GA operations advanced diagnostics workpack is not ready";
  } else if (!toolchain_runtime_ga_operations_advanced_conformance_consistent) {
    surface.failure_reason =
        "toolchain/runtime GA operations advanced conformance workpack is inconsistent";
  } else if (!toolchain_runtime_ga_operations_advanced_conformance_ready) {
    surface.failure_reason =
        "toolchain/runtime GA operations advanced conformance workpack is not ready";
  } else if (!toolchain_runtime_ga_operations_advanced_integration_consistent) {
    surface.failure_reason =
        "toolchain/runtime GA operations advanced integration workpack is inconsistent";
  } else if (!toolchain_runtime_ga_operations_advanced_integration_ready) {
    surface.failure_reason =
        "toolchain/runtime GA operations advanced integration workpack is not ready";
  } else if (!toolchain_runtime_ga_operations_advanced_performance_consistent) {
    surface.failure_reason =
        "toolchain/runtime GA operations advanced performance workpack is inconsistent";
  } else if (!toolchain_runtime_ga_operations_advanced_performance_ready) {
    surface.failure_reason =
        "toolchain/runtime GA operations advanced performance workpack is not ready";
  } else if (!toolchain_runtime_ga_operations_advanced_core_shard2_consistent) {
    surface.failure_reason =
        "toolchain/runtime GA operations advanced core workpack (shard 2) is inconsistent";
  } else if (!toolchain_runtime_ga_operations_advanced_core_shard2_ready) {
    surface.failure_reason =
        "toolchain/runtime GA operations advanced core workpack (shard 2) is not ready";
  } else if (!toolchain_runtime_ga_operations_integration_closeout_signoff_consistent) {
    surface.failure_reason =
        "toolchain/runtime GA operations integration closeout and sign-off is inconsistent";
  } else if (!toolchain_runtime_ga_operations_integration_closeout_signoff_ready) {
    surface.failure_reason =
        "toolchain/runtime GA operations integration closeout and sign-off is not ready";
  } else if (!surface.long_tail_grammar_integration_closeout_consistent) {
    surface.failure_reason = "long-tail grammar integration closeout is inconsistent";
  } else if (!surface.long_tail_grammar_gate_signoff_ready) {
    surface.failure_reason = "long-tail grammar gate sign-off is not ready";
  } else {
    surface.failure_reason = "parse-lowering readiness failed";
  }

  return surface;
}

inline bool IsObjc3ParseLoweringReadinessSurfaceReady(const Objc3ParseLoweringReadinessSurface &surface,
                                                      std::string &reason) {
  if (surface.ready_for_lowering) {
    reason.clear();
    return true;
  }
  reason = surface.failure_reason.empty() ? "parse-lowering readiness surface not ready" : surface.failure_reason;
  return false;
}
