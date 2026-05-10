#include <string>

#include "pipeline/readiness/objc3_parse_lowering_failure_reason_readiness.h"
#include "pipeline/readiness/objc3_parse_lowering_failure_reason_readiness_typed_sema_private.h"
#include "pipeline/readiness/objc3_toolchain_runtime_ga_operations_readiness_keys.h"

namespace {

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

}  // namespace

Objc3ParseLoweringFailureReasonReadinessRecord
Objc3ParseLoweringFailureReasonReadinessReady(const std::string &failure_reason) {
  Objc3ParseLoweringFailureReasonReadinessRecord record;
  record.ready_for_lowering = true;
  record.failure_reason = failure_reason;
  return record;
}

Objc3ParseLoweringFailureReasonReadinessRecord
Objc3ParseLoweringFailureReasonReadinessFailure(const std::string &failure_reason) {
  Objc3ParseLoweringFailureReasonReadinessRecord record;
  record.failure_reason = failure_reason;
  return record;
}

Objc3ParseLoweringFailureReasonReadinessRecord
BuildObjc3ParseLoweringFailureReasonReadiness(
    const Objc3ParseLoweringReadinessSurface &surface,
    const Objc3TypedSemaLoweringReadinessRecord &typed_sema_lowering_readiness,
    const Objc3ParseLoweringConformancePerformanceReadinessRecord
        &conformance_performance_readiness,
    const Objc3ToolchainRuntimeGaOperationsCloseoutReadinessRecord
        &toolchain_runtime_ga_operations_closeout_readiness) {
  if (surface.ready_for_lowering) {
    return Objc3ParseLoweringFailureReasonReadinessReady(surface.failure_reason);
  }

  if (!surface.failure_reason.empty()) {
    return Objc3ParseLoweringFailureReasonReadinessFailure(surface.failure_reason);
  }

  if (surface.lexer_diagnostic_count != 0) {
    return Objc3ParseLoweringFailureReasonReadinessFailure("lexer diagnostics present");
  }

  if (surface.parser_diagnostic_count != 0) {
    return Objc3ParseLoweringFailureReasonReadinessFailure("parser diagnostics present");
  }

  if (surface.semantic_diagnostic_count != 0) {
    return Objc3ParseLoweringFailureReasonReadinessFailure("semantic diagnostics present");
  }

  if (!surface.parser_contract_snapshot_present) {
    return Objc3ParseLoweringFailureReasonReadinessFailure("parser contract snapshot missing");
  }

  if (!surface.parser_contract_deterministic) {
    return Objc3ParseLoweringFailureReasonReadinessFailure("parser handoff is not deterministic");
  }

  if (!surface.parser_recovery_replay_ready) {
    return Objc3ParseLoweringFailureReasonReadinessFailure(
        "parser recovery handoff is not replay ready");
  }

  if (const char *failure_reason =
          FindParseArtifactDiagnosticHandoffFailureReason(surface)) {
    return Objc3ParseLoweringFailureReasonReadinessFailure(failure_reason);
  }

  if (const char *failure_reason =
          FindParserDiagnosticGrammarHardeningFailureReason(surface)) {
    return Objc3ParseLoweringFailureReasonReadinessFailure(failure_reason);
  }

  if (const char *failure_reason =
          FindParseRecoveryConformanceFailureReason(
              surface,
              conformance_performance_readiness)) {
    return Objc3ParseLoweringFailureReasonReadinessFailure(failure_reason);
  }

  if (const char *failure_reason =
          objc3_parse_lowering_failure_reason_readiness_detail::
              FindTypedSemaLoweringSurfaceFailureReason(surface)) {
    return Objc3ParseLoweringFailureReasonReadinessFailure(failure_reason);
  }

  if (const char *failure_reason =
          objc3_parse_lowering_failure_reason_readiness_detail::
              FindTypedSemaLoweringAlignmentFailureReason(
              typed_sema_lowering_readiness)) {
    return Objc3ParseLoweringFailureReasonReadinessFailure(failure_reason);
  }

  if (const char *failure_reason =
          FindLoweringToolchainCloseoutFailureReason(
              surface,
              conformance_performance_readiness,
              toolchain_runtime_ga_operations_closeout_readiness)) {
    return Objc3ParseLoweringFailureReasonReadinessFailure(failure_reason);
  }

  return Objc3ParseLoweringFailureReasonReadinessFailure("parse-lowering readiness failed");
}
