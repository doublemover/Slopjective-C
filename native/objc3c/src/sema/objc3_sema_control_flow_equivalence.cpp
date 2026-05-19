#include "sema/objc3_sema_control_flow_equivalence.h"

bool IsEquivalentConcurrencyReplayRaceGuardSummary(
    const Objc3ConcurrencyReplayRaceGuardSummary &lhs,
    const Objc3ConcurrencyReplayRaceGuardSummary &rhs) {
  return lhs.concurrency_replay_race_guard_sites ==
             rhs.concurrency_replay_race_guard_sites &&
         lhs.concurrency_replay_sites == rhs.concurrency_replay_sites &&
         lhs.replay_proof_sites == rhs.replay_proof_sites &&
         lhs.race_guard_sites == rhs.race_guard_sites &&
         lhs.task_handoff_sites == rhs.task_handoff_sites &&
         lhs.actor_isolation_sites == rhs.actor_isolation_sites &&
         lhs.deterministic_schedule_sites == rhs.deterministic_schedule_sites &&
         lhs.guard_blocked_sites == rhs.guard_blocked_sites &&
         lhs.contract_violation_sites == rhs.contract_violation_sites;
}

bool IsEquivalentUnsafePointerExtensionSummary(
    const Objc3UnsafePointerExtensionSummary &lhs,
    const Objc3UnsafePointerExtensionSummary &rhs) {
  return lhs.unsafe_pointer_extension_sites == rhs.unsafe_pointer_extension_sites &&
         lhs.unsafe_keyword_sites == rhs.unsafe_keyword_sites &&
         lhs.pointer_arithmetic_sites == rhs.pointer_arithmetic_sites &&
         lhs.raw_pointer_type_sites == rhs.raw_pointer_type_sites &&
         lhs.unsafe_operation_sites == rhs.unsafe_operation_sites &&
         lhs.normalized_sites == rhs.normalized_sites &&
         lhs.gate_blocked_sites == rhs.gate_blocked_sites &&
         lhs.contract_violation_sites == rhs.contract_violation_sites;
}

bool IsEquivalentInlineAsmIntrinsicGovernanceSummary(
    const Objc3InlineAsmIntrinsicGovernanceSummary &lhs,
    const Objc3InlineAsmIntrinsicGovernanceSummary &rhs) {
  return lhs.inline_asm_intrinsic_sites == rhs.inline_asm_intrinsic_sites &&
         lhs.inline_asm_sites == rhs.inline_asm_sites &&
         lhs.intrinsic_sites == rhs.intrinsic_sites &&
         lhs.governed_intrinsic_sites == rhs.governed_intrinsic_sites &&
         lhs.privileged_intrinsic_sites == rhs.privileged_intrinsic_sites &&
         lhs.normalized_sites == rhs.normalized_sites &&
         lhs.gate_blocked_sites == rhs.gate_blocked_sites &&
         lhs.contract_violation_sites == rhs.contract_violation_sites;
}

bool IsEquivalentNSErrorBridgingSummary(
    const Objc3NSErrorBridgingSummary &lhs,
    const Objc3NSErrorBridgingSummary &rhs) {
  return lhs.ns_error_bridging_sites == rhs.ns_error_bridging_sites &&
         lhs.ns_error_parameter_sites == rhs.ns_error_parameter_sites &&
         lhs.ns_error_out_parameter_sites == rhs.ns_error_out_parameter_sites &&
         lhs.ns_error_bridge_path_sites == rhs.ns_error_bridge_path_sites &&
         lhs.failable_call_sites == rhs.failable_call_sites &&
         lhs.normalized_sites == rhs.normalized_sites &&
         lhs.bridge_boundary_sites == rhs.bridge_boundary_sites &&
         lhs.contract_violation_sites == rhs.contract_violation_sites;
}

bool IsEquivalentErrorDiagnosticsRecoverySummary(
    const Objc3ErrorDiagnosticsRecoverySummary &lhs,
    const Objc3ErrorDiagnosticsRecoverySummary &rhs) {
  return lhs.error_diagnostics_recovery_sites ==
             rhs.error_diagnostics_recovery_sites &&
         lhs.diagnostic_emit_sites == rhs.diagnostic_emit_sites &&
         lhs.recovery_anchor_sites == rhs.recovery_anchor_sites &&
         lhs.recovery_boundary_sites == rhs.recovery_boundary_sites &&
         lhs.fail_closed_diagnostic_sites == rhs.fail_closed_diagnostic_sites &&
         lhs.normalized_sites == rhs.normalized_sites &&
         lhs.gate_blocked_sites == rhs.gate_blocked_sites &&
         lhs.contract_violation_sites == rhs.contract_violation_sites;
}

bool IsEquivalentResultLikeLoweringSummary(
    const Objc3ResultLikeLoweringSummary &lhs,
    const Objc3ResultLikeLoweringSummary &rhs) {
  return lhs.result_like_sites == rhs.result_like_sites &&
         lhs.result_success_sites == rhs.result_success_sites &&
         lhs.result_failure_sites == rhs.result_failure_sites &&
         lhs.result_branch_sites == rhs.result_branch_sites &&
         lhs.result_payload_sites == rhs.result_payload_sites &&
         lhs.normalized_sites == rhs.normalized_sites &&
         lhs.branch_merge_sites == rhs.branch_merge_sites &&
         lhs.contract_violation_sites == rhs.contract_violation_sites;
}

bool IsEquivalentUnwindCleanupSummary(
    const Objc3UnwindCleanupSummary &lhs,
    const Objc3UnwindCleanupSummary &rhs) {
  return lhs.unwind_cleanup_sites == rhs.unwind_cleanup_sites &&
         lhs.exceptional_exit_sites == rhs.exceptional_exit_sites &&
         lhs.cleanup_action_sites == rhs.cleanup_action_sites &&
         lhs.cleanup_scope_sites == rhs.cleanup_scope_sites &&
         lhs.cleanup_resume_sites == rhs.cleanup_resume_sites &&
         lhs.normalized_sites == rhs.normalized_sites &&
         lhs.fail_closed_sites == rhs.fail_closed_sites &&
         lhs.contract_violation_sites == rhs.contract_violation_sites;
}

bool IsEquivalentAwaitLoweringSuspensionStateSummary(
    const Objc3AwaitLoweringSuspensionStateSummary &lhs,
    const Objc3AwaitLoweringSuspensionStateSummary &rhs) {
  return lhs.await_suspension_sites == rhs.await_suspension_sites &&
         lhs.await_keyword_sites == rhs.await_keyword_sites &&
         lhs.await_suspension_point_sites == rhs.await_suspension_point_sites &&
         lhs.await_resume_sites == rhs.await_resume_sites &&
         lhs.await_state_machine_sites == rhs.await_state_machine_sites &&
         lhs.await_continuation_sites == rhs.await_continuation_sites &&
         lhs.normalized_sites == rhs.normalized_sites &&
         lhs.gate_blocked_sites == rhs.gate_blocked_sites &&
         lhs.contract_violation_sites == rhs.contract_violation_sites;
}
