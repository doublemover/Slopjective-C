#include <string>

#include "pipeline/readiness/objc3_toolchain_runtime_ga_operations_readiness_keys.h"
#include "support/objc3_string_predicates.h"

bool IsObjc3ToolchainRuntimeGaOperationsCrossLaneIntegrationConsistent(
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

bool IsObjc3ToolchainRuntimeGaOperationsCrossLaneIntegrationReady(
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

std::string BuildObjc3ToolchainRuntimeGaOperationsCrossLaneIntegrationKey(
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

bool IsObjc3ToolchainRuntimeGaOperationsDocsRunbookSyncConsistent(
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

bool IsObjc3ToolchainRuntimeGaOperationsDocsRunbookSyncReady(
    bool toolchain_runtime_ga_operations_docs_runbook_sync_consistent,
    const std::string &long_tail_grammar_integration_closeout_key) {
  return toolchain_runtime_ga_operations_docs_runbook_sync_consistent &&
         objc3c::support::StartsWith(
             long_tail_grammar_integration_closeout_key,
             "conformance_matrix_ready=");
}

std::string BuildObjc3ToolchainRuntimeGaOperationsDocsRunbookSyncKey(
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
