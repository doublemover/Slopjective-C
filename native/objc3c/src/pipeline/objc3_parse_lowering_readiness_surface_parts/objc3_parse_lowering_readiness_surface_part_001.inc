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
#include "support/objc3_string_predicates.h"

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

inline const char *Objc3FrontendLanguageProfileName(const Objc3FrontendLanguageProfile mode) {
  return mode == Objc3FrontendLanguageProfile::kLegacy ? "legacy" : "canonical";
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
  return "language_profile=" +
         std::string(Objc3FrontendLanguageProfileName(options.language_profile)) +
         ";legacy_literal_diagnostics=" + (options.legacy_literal_diagnostics ? "true" : "false") +
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
         objc3c::support::StartsWith(
             parse_artifact_handoff_key,
             "parser_snapshot=") &&
         objc3c::support::StartsWith(
             parse_artifact_replay_key,
             "parser_snapshot_fingerprint=") &&
         objc3c::support::StartsWith(
             parse_artifact_diagnostics_hardening_key,
             "parser_diagnostics=") &&
         objc3c::support::StartsWith(
             parse_artifact_edge_robustness_key,
             "parser_tokens=") &&
         objc3c::support::StartsWith(
             long_tail_grammar_handoff_key,
             "long-tail-grammar:v1:constructs=") &&
         objc3c::support::StartsWith(
             long_tail_grammar_diagnostics_hardening_key,
             "parser_diagnostic_count=") &&
         objc3c::support::StartsWith(
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
         objc3c::support::StartsWith(
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
      objc3c::support::StartsWith(
          parse_artifact_handoff_key,
          "parser_snapshot=");
  const bool parse_artifact_replay_key_shape_deterministic =
      objc3c::support::StartsWith(
          parse_artifact_replay_key,
          "parser_snapshot_fingerprint=");
  const bool parse_artifact_diagnostics_hardening_key_shape_deterministic =
      objc3c::support::StartsWith(
          parse_artifact_diagnostics_hardening_key,
          "parser_diagnostics=");
  const bool parse_artifact_edge_robustness_key_shape_deterministic =
      objc3c::support::StartsWith(
          parse_artifact_edge_robustness_key,
          "parser_tokens=");
  const bool long_tail_grammar_handoff_key_shape_deterministic =
      objc3c::support::StartsWith(
          long_tail_grammar_handoff_key,
          "long-tail-grammar:v1:constructs=");
  const bool long_tail_grammar_diagnostics_hardening_key_shape_deterministic =
      objc3c::support::StartsWith(
          long_tail_grammar_diagnostics_hardening_key,
          "parser_diagnostic_count=");
  const bool parse_recovery_determinism_hardening_key_shape_deterministic =
      objc3c::support::StartsWith(
          parse_recovery_determinism_hardening_key,
          "snapshot_present=");
  const bool long_tail_grammar_recovery_determinism_key_shape_deterministic =
      objc3c::support::StartsWith(
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
         objc3c::support::StartsWith(
             parse_lowering_conformance_matrix_key,
             "case_count=") &&
         objc3c::support::StartsWith(
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
      objc3c::support::StartsWith(
          parse_lowering_conformance_matrix_key,
          "case_count=");
  const bool long_tail_grammar_conformance_matrix_key_shape_deterministic =
      objc3c::support::StartsWith(
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
         objc3c::support::StartsWith(
             parse_lowering_conformance_matrix_key,
             "case_count=") &&
         objc3c::support::StartsWith(
             parse_lowering_conformance_corpus_key,
             "case_count=") &&
         objc3c::support::StartsWith(
             long_tail_grammar_conformance_matrix_key,
             "conformance_matrix_case_count=");
}

inline bool IsObjc3ToolchainRuntimeGaOperationsConformanceCorpusReady(
    bool toolchain_runtime_ga_operations_conformance_corpus_consistent,
    const std::string &parse_lowering_conformance_corpus_key) {
  return toolchain_runtime_ga_operations_conformance_corpus_consistent &&
         objc3c::support::StartsWith(
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
      objc3c::support::StartsWith(
          parse_lowering_conformance_matrix_key,
          "case_count=");
  const bool parse_lowering_conformance_corpus_key_shape_deterministic =
      objc3c::support::StartsWith(
          parse_lowering_conformance_corpus_key,
          "case_count=");
  const bool long_tail_grammar_conformance_matrix_key_shape_deterministic =
      objc3c::support::StartsWith(
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
         objc3c::support::StartsWith(
             parse_lowering_conformance_corpus_key,
             "case_count=") &&
         objc3c::support::StartsWith(
             parse_lowering_performance_quality_guardrails_key,
             "case_count=") &&
         objc3c::support::StartsWith(
             long_tail_grammar_conformance_matrix_key,
             "conformance_matrix_case_count=");
}

inline bool IsObjc3ToolchainRuntimeGaOperationsPerformanceQualityGuardrailsReady(
    bool toolchain_runtime_ga_operations_performance_quality_guardrails_consistent,
    const std::string &parse_lowering_performance_quality_guardrails_key) {
  return toolchain_runtime_ga_operations_performance_quality_guardrails_consistent &&
         objc3c::support::StartsWith(
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
      objc3c::support::StartsWith(
          parse_lowering_conformance_corpus_key,
          "case_count=");
  const bool parse_lowering_performance_quality_guardrails_key_shape_deterministic =
      objc3c::support::StartsWith(
          parse_lowering_performance_quality_guardrails_key,
          "case_count=");
  const bool long_tail_grammar_conformance_matrix_key_shape_deterministic =
      objc3c::support::StartsWith(
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
         objc3c::support::StartsWith(
             parse_lowering_conformance_corpus_key,
             "case_count=") &&
         objc3c::support::StartsWith(
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
         objc3c::support::StartsWith(
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
      objc3c::support::StartsWith(
          parse_lowering_conformance_corpus_key,
          "case_count=");
  const bool parse_lowering_performance_quality_guardrails_key_shape_deterministic =
      objc3c::support::StartsWith(
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
         objc3c::support::StartsWith(
             long_tail_grammar_integration_closeout_key,
             "conformance_matrix_ready=") &&
         objc3c::support::StartsWith(
             parse_lowering_performance_quality_guardrails_key,
             "case_count=");
}

inline bool IsObjc3ToolchainRuntimeGaOperationsDocsRunbookSyncReady(
    bool toolchain_runtime_ga_operations_docs_runbook_sync_consistent,
    const std::string &long_tail_grammar_integration_closeout_key) {
  return toolchain_runtime_ga_operations_docs_runbook_sync_consistent &&
         objc3c::support::StartsWith(
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
      objc3c::support::StartsWith(
          long_tail_grammar_integration_closeout_key,
          "conformance_matrix_ready=");
  const bool parse_lowering_performance_quality_guardrails_key_shape_deterministic =
      objc3c::support::StartsWith(
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
         objc3c::support::StartsWith(
             toolchain_runtime_ga_operations_docs_runbook_sync_key,
             "long_tail_grammar_integration_closeout_consistent=") &&
         objc3c::support::StartsWith(
             long_tail_grammar_integration_closeout_key,
             "conformance_matrix_ready=") &&
         objc3c::support::StartsWith(
             parse_lowering_performance_quality_guardrails_key,
             "case_count=");
}

inline bool IsObjc3ToolchainRuntimeGaOperationsAdvancedCoreReady(
    bool toolchain_runtime_ga_operations_advanced_core_consistent,
    const std::string &toolchain_runtime_ga_operations_docs_runbook_sync_key,
    const std::string &long_tail_grammar_integration_closeout_key) {
  return toolchain_runtime_ga_operations_advanced_core_consistent &&
         objc3c::support::StartsWith(
             toolchain_runtime_ga_operations_docs_runbook_sync_key,
             "long_tail_grammar_integration_closeout_consistent=") &&
         objc3c::support::StartsWith(
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
      objc3c::support::StartsWith(
          toolchain_runtime_ga_operations_docs_runbook_sync_key,
          "long_tail_grammar_integration_closeout_consistent=");
  const bool long_tail_grammar_integration_closeout_key_shape_deterministic =
      objc3c::support::StartsWith(
          long_tail_grammar_integration_closeout_key,
          "conformance_matrix_ready=");
  const bool parse_lowering_performance_quality_guardrails_key_shape_deterministic =
      objc3c::support::StartsWith(
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
         objc3c::support::StartsWith(
             toolchain_runtime_ga_operations_advanced_core_key,
             "toolchain_runtime_ga_operations_docs_runbook_sync_consistent=") &&
         objc3c::support::StartsWith(
             long_tail_grammar_integration_closeout_key,
             "conformance_matrix_ready=") &&
         objc3c::support::StartsWith(
             parse_lowering_performance_quality_guardrails_key,
             "case_count=");
}

inline bool IsObjc3ToolchainRuntimeGaOperationsAdvancedEdgeCompatibilityReady(
    bool toolchain_runtime_ga_operations_advanced_edge_compatibility_consistent,
    const std::string &toolchain_runtime_ga_operations_advanced_core_key,
    const std::string &long_tail_grammar_integration_closeout_key) {
  return toolchain_runtime_ga_operations_advanced_edge_compatibility_consistent &&
         objc3c::support::StartsWith(
             toolchain_runtime_ga_operations_advanced_core_key,
             "toolchain_runtime_ga_operations_docs_runbook_sync_consistent=") &&
         objc3c::support::StartsWith(
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
      objc3c::support::StartsWith(
          toolchain_runtime_ga_operations_advanced_core_key,
          "toolchain_runtime_ga_operations_docs_runbook_sync_consistent=");
  const bool long_tail_grammar_integration_closeout_key_shape_deterministic =
      objc3c::support::StartsWith(
          long_tail_grammar_integration_closeout_key,
          "conformance_matrix_ready=");
  const bool parse_lowering_performance_quality_guardrails_key_shape_deterministic =
      objc3c::support::StartsWith(
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
         objc3c::support::StartsWith(
             toolchain_runtime_ga_operations_advanced_edge_compatibility_key,
             "toolchain_runtime_ga_operations_advanced_core_consistent=") &&
         objc3c::support::StartsWith(
             long_tail_grammar_integration_closeout_key,
             "conformance_matrix_ready=") &&
         objc3c::support::StartsWith(
             parse_lowering_performance_quality_guardrails_key,
             "case_count=");
}

inline bool IsObjc3ToolchainRuntimeGaOperationsAdvancedDiagnosticsReady(
    bool toolchain_runtime_ga_operations_advanced_diagnostics_consistent,
    const std::string &toolchain_runtime_ga_operations_advanced_edge_compatibility_key,
    const std::string &long_tail_grammar_integration_closeout_key) {
  return toolchain_runtime_ga_operations_advanced_diagnostics_consistent &&
         objc3c::support::StartsWith(
             toolchain_runtime_ga_operations_advanced_edge_compatibility_key,
             "toolchain_runtime_ga_operations_advanced_core_consistent=") &&
         objc3c::support::StartsWith(
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
      objc3c::support::StartsWith(
          toolchain_runtime_ga_operations_advanced_edge_compatibility_key,
          "toolchain_runtime_ga_operations_advanced_core_consistent=");
  const bool long_tail_grammar_integration_closeout_key_shape_deterministic =
      objc3c::support::StartsWith(
          long_tail_grammar_integration_closeout_key,
          "conformance_matrix_ready=");
  const bool parse_lowering_performance_quality_guardrails_key_shape_deterministic =
      objc3c::support::StartsWith(
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
         objc3c::support::StartsWith(
             toolchain_runtime_ga_operations_advanced_diagnostics_key,
             "toolchain_runtime_ga_operations_advanced_edge_compatibility_consistent=") &&
         objc3c::support::StartsWith(
             long_tail_grammar_integration_closeout_key,
             "conformance_matrix_ready=") &&
         objc3c::support::StartsWith(
             parse_lowering_performance_quality_guardrails_key,
             "case_count=");
}

inline bool IsObjc3ToolchainRuntimeGaOperationsAdvancedConformanceReady(
    bool toolchain_runtime_ga_operations_advanced_conformance_consistent,
    const std::string &toolchain_runtime_ga_operations_advanced_diagnostics_key,
    const std::string &long_tail_grammar_integration_closeout_key) {
  return toolchain_runtime_ga_operations_advanced_conformance_consistent &&
         objc3c::support::StartsWith(
             toolchain_runtime_ga_operations_advanced_diagnostics_key,
             "toolchain_runtime_ga_operations_advanced_edge_compatibility_consistent=") &&
         objc3c::support::StartsWith(
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
      objc3c::support::StartsWith(
          toolchain_runtime_ga_operations_advanced_diagnostics_key,
          "toolchain_runtime_ga_operations_advanced_edge_compatibility_consistent=");
  const bool long_tail_grammar_integration_closeout_key_shape_deterministic =
      objc3c::support::StartsWith(
          long_tail_grammar_integration_closeout_key,
          "conformance_matrix_ready=");
  const bool parse_lowering_performance_quality_guardrails_key_shape_deterministic =
      objc3c::support::StartsWith(
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
         objc3c::support::StartsWith(
             toolchain_runtime_ga_operations_advanced_conformance_key,
             "toolchain_runtime_ga_operations_advanced_diagnostics_consistent=") &&
         objc3c::support::StartsWith(
             long_tail_grammar_integration_closeout_key,
             "conformance_matrix_ready=") &&
         objc3c::support::StartsWith(
             parse_lowering_performance_quality_guardrails_key,
             "case_count=");
}

inline bool IsObjc3ToolchainRuntimeGaOperationsAdvancedIntegrationReady(
    bool toolchain_runtime_ga_operations_advanced_integration_consistent,
    const std::string &toolchain_runtime_ga_operations_advanced_conformance_key,
    const std::string &long_tail_grammar_integration_closeout_key) {
  return toolchain_runtime_ga_operations_advanced_integration_consistent &&
         objc3c::support::StartsWith(
             toolchain_runtime_ga_operations_advanced_conformance_key,
             "toolchain_runtime_ga_operations_advanced_diagnostics_consistent=") &&
         objc3c::support::StartsWith(
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
      objc3c::support::StartsWith(
          toolchain_runtime_ga_operations_advanced_conformance_key,
          "toolchain_runtime_ga_operations_advanced_diagnostics_consistent=");
  const bool long_tail_grammar_integration_closeout_key_shape_deterministic =
      objc3c::support::StartsWith(
          long_tail_grammar_integration_closeout_key,
          "conformance_matrix_ready=");
  const bool parse_lowering_performance_quality_guardrails_key_shape_deterministic =
      objc3c::support::StartsWith(
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
         objc3c::support::StartsWith(
             toolchain_runtime_ga_operations_advanced_integration_key,
             "toolchain_runtime_ga_operations_advanced_conformance_consistent=") &&
         objc3c::support::StartsWith(
             long_tail_grammar_integration_closeout_key,
             "conformance_matrix_ready=") &&
         objc3c::support::StartsWith(
             parse_lowering_performance_quality_guardrails_key,
             "case_count=");
}

inline bool IsObjc3ToolchainRuntimeGaOperationsAdvancedPerformanceReady(
    bool toolchain_runtime_ga_operations_advanced_performance_consistent,
    const std::string &toolchain_runtime_ga_operations_advanced_integration_key,
    const std::string &long_tail_grammar_integration_closeout_key) {
  return toolchain_runtime_ga_operations_advanced_performance_consistent &&
         objc3c::support::StartsWith(
             toolchain_runtime_ga_operations_advanced_integration_key,
