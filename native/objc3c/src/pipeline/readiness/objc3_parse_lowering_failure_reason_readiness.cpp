#include <string>

#include "pipeline/readiness/objc3_parse_lowering_failure_reason_readiness.h"
#include "pipeline/readiness/objc3_parse_lowering_failure_reason_readiness_private.h"
#include "pipeline/readiness/objc3_parse_lowering_failure_reason_readiness_typed_sema_private.h"

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
          objc3_parse_lowering_failure_reason_readiness_detail::
              FindParseArtifactDiagnosticHandoffFailureReason(surface)) {
    return Objc3ParseLoweringFailureReasonReadinessFailure(failure_reason);
  }

  if (const char *failure_reason =
          objc3_parse_lowering_failure_reason_readiness_detail::
              FindParserDiagnosticGrammarHardeningFailureReason(surface)) {
    return Objc3ParseLoweringFailureReasonReadinessFailure(failure_reason);
  }

  if (const char *failure_reason =
          objc3_parse_lowering_failure_reason_readiness_detail::
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
          objc3_parse_lowering_failure_reason_readiness_detail::
              FindLoweringToolchainCloseoutFailureReason(
              surface,
              conformance_performance_readiness,
              toolchain_runtime_ga_operations_closeout_readiness)) {
    return Objc3ParseLoweringFailureReasonReadinessFailure(failure_reason);
  }

  return Objc3ParseLoweringFailureReasonReadinessFailure("parse-lowering readiness failed");
}
