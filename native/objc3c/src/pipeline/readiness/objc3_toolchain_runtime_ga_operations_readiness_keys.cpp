#include <string>

#include "pipeline/readiness/objc3_toolchain_runtime_ga_operations_readiness_keys.h"
#include "support/objc3_string_predicates.h"

bool IsObjc3ToolchainRuntimeGaOperationsRecoveryDeterminismHardeningConsistent(
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

bool IsObjc3ToolchainRuntimeGaOperationsRecoveryDeterminismHardeningReady(
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

std::string BuildObjc3ParseRecoveryDeterminismHardeningKey(
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

std::string BuildObjc3ToolchainRuntimeGaOperationsRecoveryDeterminismHardeningKey(
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
