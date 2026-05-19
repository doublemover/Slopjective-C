#include "pipeline/readiness/objc3_parse_lowering_failure_reason_readiness_private.h"
#include "pipeline/readiness/objc3_toolchain_runtime_ga_operations_readiness_keys.h"

namespace objc3_parse_lowering_failure_reason_readiness_detail {

const char *FindParseRecoveryConformanceFailureReason(
    const Objc3ParseLoweringReadinessSurface &surface,
    const Objc3ParseLoweringConformancePerformanceReadinessRecord
        &conformance_performance_readiness) {
  if (!IsObjc3ToolchainRuntimeGaOperationsRecoveryDeterminismHardeningConsistent(
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
    return "toolchain/runtime GA operations recovery/determinism hardening is inconsistent";
  }

  if (!IsObjc3ToolchainRuntimeGaOperationsRecoveryDeterminismHardeningReady(
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
    return "toolchain/runtime GA operations recovery/determinism hardening is not ready";
  }

  if (!surface.long_tail_grammar_recovery_determinism_consistent) {
    return "long-tail grammar recovery/determinism hardening is inconsistent";
  }

  if (!surface.long_tail_grammar_recovery_determinism_ready) {
    return "long-tail grammar recovery/determinism hardening is not ready";
  }

  if (!conformance_performance_readiness
           .toolchain_runtime_ga_operations_conformance_matrix_consistent) {
    return "toolchain/runtime GA operations conformance matrix is inconsistent";
  }

  if (!conformance_performance_readiness
           .toolchain_runtime_ga_operations_conformance_matrix_ready) {
    return "toolchain/runtime GA operations conformance matrix is not ready";
  }

  if (!conformance_performance_readiness
           .toolchain_runtime_ga_operations_conformance_corpus_consistent) {
    return "toolchain/runtime GA operations conformance corpus is inconsistent";
  }

  if (!conformance_performance_readiness
           .toolchain_runtime_ga_operations_conformance_corpus_ready) {
    return "toolchain/runtime GA operations conformance corpus is not ready";
  }

  if (!surface.long_tail_grammar_conformance_matrix_consistent) {
    return "long-tail grammar conformance matrix is inconsistent";
  }

  if (!surface.long_tail_grammar_conformance_matrix_ready) {
    return "long-tail grammar conformance matrix is not ready";
  }

  if (!surface.parse_recovery_determinism_hardening_consistent) {
    return "parse recovery/determinism hardening is inconsistent";
  }

  return nullptr;
}

const char *FindLoweringToolchainCloseoutFailureReason(
    const Objc3ParseLoweringReadinessSurface &surface,
    const Objc3ParseLoweringConformancePerformanceReadinessRecord
        &conformance_performance_readiness,
    const Objc3ToolchainRuntimeGaOperationsCloseoutReadinessRecord
        &toolchain_runtime_ga_operations_closeout_readiness) {
  if (!surface.lowering_boundary_ready) {
    return "lowering boundary is not ready";
  }

  if (!surface.parse_lowering_conformance_matrix_consistent) {
    return "parse-lowering conformance matrix is inconsistent";
  }

  if (!surface.parse_lowering_conformance_corpus_consistent) {
    return "parse-lowering conformance corpus is inconsistent";
  }

  if (!conformance_performance_readiness
           .toolchain_runtime_ga_operations_performance_quality_guardrails_consistent) {
    return "toolchain/runtime GA operations performance quality guardrails are inconsistent";
  }

  if (!conformance_performance_readiness
           .toolchain_runtime_ga_operations_performance_quality_guardrails_ready) {
    return "toolchain/runtime GA operations performance quality guardrails are not ready";
  }

  if (!surface.parse_lowering_performance_quality_guardrails_consistent) {
    return "parse-lowering performance/quality guardrails are inconsistent";
  }

  if (!conformance_performance_readiness
           .toolchain_runtime_ga_operations_cross_lane_integration_consistent) {
    return "toolchain/runtime GA operations cross-lane integration is inconsistent";
  }

  if (!conformance_performance_readiness
           .toolchain_runtime_ga_operations_cross_lane_integration_ready) {
    return "toolchain/runtime GA operations cross-lane integration is not ready";
  }

  if (!toolchain_runtime_ga_operations_closeout_readiness.docs_runbook_sync_consistent) {
    return "toolchain/runtime GA operations docs and runbook synchronization is inconsistent";
  }

  if (!toolchain_runtime_ga_operations_closeout_readiness.docs_runbook_sync_ready) {
    return "toolchain/runtime GA operations docs and runbook synchronization is not ready";
  }

  if (!toolchain_runtime_ga_operations_closeout_readiness.advanced_core_consistent) {
    return "toolchain/runtime GA operations advanced core workpack is inconsistent";
  }

  if (!toolchain_runtime_ga_operations_closeout_readiness.advanced_core_ready) {
    return "toolchain/runtime GA operations advanced core workpack is not ready";
  }

  if (!toolchain_runtime_ga_operations_closeout_readiness.advanced_edge_compatibility_consistent) {
    return "toolchain/runtime GA operations advanced edge compatibility workpack is inconsistent";
  }

  if (!toolchain_runtime_ga_operations_closeout_readiness.advanced_edge_compatibility_ready) {
    return "toolchain/runtime GA operations advanced edge compatibility workpack is not ready";
  }

  if (!toolchain_runtime_ga_operations_closeout_readiness.advanced_diagnostics_consistent) {
    return "toolchain/runtime GA operations advanced diagnostics workpack is inconsistent";
  }

  if (!toolchain_runtime_ga_operations_closeout_readiness.advanced_diagnostics_ready) {
    return "toolchain/runtime GA operations advanced diagnostics workpack is not ready";
  }

  if (!toolchain_runtime_ga_operations_closeout_readiness.advanced_conformance_consistent) {
    return "toolchain/runtime GA operations advanced conformance workpack is inconsistent";
  }

  if (!toolchain_runtime_ga_operations_closeout_readiness.advanced_conformance_ready) {
    return "toolchain/runtime GA operations advanced conformance workpack is not ready";
  }

  if (!toolchain_runtime_ga_operations_closeout_readiness.advanced_integration_consistent) {
    return "toolchain/runtime GA operations advanced integration workpack is inconsistent";
  }

  if (!toolchain_runtime_ga_operations_closeout_readiness.advanced_integration_ready) {
    return "toolchain/runtime GA operations advanced integration workpack is not ready";
  }

  if (!toolchain_runtime_ga_operations_closeout_readiness.advanced_performance_consistent) {
    return "toolchain/runtime GA operations advanced performance workpack is inconsistent";
  }

  if (!toolchain_runtime_ga_operations_closeout_readiness.advanced_performance_ready) {
    return "toolchain/runtime GA operations advanced performance workpack is not ready";
  }

  if (!toolchain_runtime_ga_operations_closeout_readiness.advanced_core_shard2_consistent) {
    return "toolchain/runtime GA operations advanced core workpack (shard 2) is inconsistent";
  }

  if (!toolchain_runtime_ga_operations_closeout_readiness.advanced_core_shard2_ready) {
    return "toolchain/runtime GA operations advanced core workpack (shard 2) is not ready";
  }

  if (!toolchain_runtime_ga_operations_closeout_readiness.integration_closeout_signoff_consistent) {
    return "toolchain/runtime GA operations integration closeout and sign-off is inconsistent";
  }

  if (!toolchain_runtime_ga_operations_closeout_readiness.integration_closeout_signoff_ready) {
    return "toolchain/runtime GA operations integration closeout and sign-off is not ready";
  }

  if (!surface.long_tail_grammar_integration_closeout_consistent) {
    return "long-tail grammar integration closeout is inconsistent";
  }

  if (!surface.long_tail_grammar_gate_signoff_ready) {
    return "long-tail grammar gate sign-off is not ready";
  }

  return nullptr;
}

}  // namespace objc3_parse_lowering_failure_reason_readiness_detail
