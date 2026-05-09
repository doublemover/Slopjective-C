#pragma once

#include <string>

#include "support/objc3_string_predicates.h"

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
