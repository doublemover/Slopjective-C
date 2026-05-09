#pragma once

#include <algorithm>
#include <cstdint>
#include <string>
#include <vector>

#include "parse/objc3_diagnostic_grammar_hooks_core_feature.h"
#include "parse/objc3_diagnostic_grammar_hooks_core_feature_expansion_surface.h"
#include "parse/objc3_diagnostic_grammar_hooks_edge_case_compatibility_surface.h"
#include "parse/objc3_diagnostic_source_precision_scaffold.h"
#include "pipeline/parse_lowering_artifact_keys.h"
#include "pipeline/parse_lowering_diagnostic_keys.h"
#include "pipeline/objc3_frontend_types.h"
#include "pipeline/readiness/objc3_long_tail_grammar_readiness_keys.h"
#include "pipeline/readiness/objc3_parse_lowering_conformance_performance_readiness.h"
#include "pipeline/readiness/objc3_parse_lowering_conformance_keys.h"
#include "pipeline/readiness/objc3_parse_lowering_failure_reason_readiness.h"
#include "pipeline/readiness/objc3_parse_lowering_parser_behavior_readiness.h"
#include "pipeline/readiness/objc3_typed_sema_lowering_readiness.h"
#include "pipeline/readiness/objc3_toolchain_runtime_ga_operations_closeout_readiness.h"
#include "pipeline/readiness/objc3_toolchain_runtime_ga_operations_readiness_keys.h"

inline Objc3ParseLoweringReadinessSurface BuildObjc3ParseLoweringReadinessSurface(
    const Objc3FrontendPipelineResult &pipeline_result,
    const Objc3FrontendOptions &options) {
  Objc3ParseLoweringReadinessSurface surface;
  BuildObjc3ParseLoweringParserBehaviorReadiness(surface, pipeline_result, options);
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
  const Objc3TypedSemaLoweringReadinessRecord typed_sema_lowering_readiness =
      BuildObjc3TypedSemaLoweringReadiness(surface, pipeline_result, options);

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
  const bool typed_core_feature_ready =
      typed_sema_lowering_readiness.typed_core_feature_ready;
  const bool sema_handoff_ready =
      typed_sema_lowering_readiness.sema_handoff_ready;
  const bool semantic_handoff_deterministic =
      typed_sema_lowering_readiness.semantic_handoff_deterministic;
  const Objc3ParseLoweringConformancePerformanceReadinessRecord
      conformance_performance_readiness =
          ApplyObjc3ParseLoweringConformancePerformanceReadiness(
              surface,
              toolchain_runtime_ga_operations_recovery_determinism_consistent,
              toolchain_runtime_ga_operations_recovery_determinism_ready,
              parse_snapshot_replay_ready,
              sema_handoff_ready,
              semantic_handoff_deterministic,
              typed_core_feature_ready);
  const Objc3ToolchainRuntimeGaOperationsCloseoutReadinessRecord
      toolchain_runtime_ga_operations_closeout_readiness =
          ApplyObjc3ToolchainRuntimeGaOperationsCloseoutReadiness(
              surface,
              diagnostics_clear,
              sema_handoff_ready,
              conformance_performance_readiness
                  .toolchain_runtime_ga_operations_cross_lane_integration_consistent,
              conformance_performance_readiness
                  .toolchain_runtime_ga_operations_cross_lane_integration_ready);
  surface.ready_for_lowering = diagnostics_clear &&
                               parse_snapshot_replay_ready &&
                               sema_handoff_ready &&
                               surface.lowering_boundary_ready &&
                               conformance_performance_readiness
                                   .parse_lowering_conformance_matrix_ready &&
                               surface.parser_diagnostic_grammar_hooks_conformance_corpus_ready &&
                               conformance_performance_readiness
                                   .parse_lowering_conformance_corpus_ready &&
                               conformance_performance_readiness
                                   .parse_lowering_performance_quality_guardrails_ready_gate &&
                               surface.long_tail_grammar_gate_signoff_ready;

  const Objc3ParseLoweringFailureReasonReadinessRecord failure_reason_readiness =
      BuildObjc3ParseLoweringFailureReasonReadiness(
          surface,
          typed_sema_lowering_readiness,
          conformance_performance_readiness,
          toolchain_runtime_ga_operations_closeout_readiness);
  surface.ready_for_lowering = failure_reason_readiness.ready_for_lowering;
  surface.failure_reason = failure_reason_readiness.failure_reason;
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
