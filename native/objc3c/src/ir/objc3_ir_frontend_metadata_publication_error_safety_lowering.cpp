#include "ir/objc3_ir_frontend_metadata_publication_error_safety_lowering.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata.h"

void EmitObjc3IRErrorHandlingLoweringCounterNodes(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  out << "!34 = !{i64 "
      << static_cast<unsigned long long>(metadata.throws_propagation_lowering_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.throws_propagation_lowering_namespace_segment_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.throws_propagation_lowering_import_edge_candidate_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.throws_propagation_lowering_object_pointer_type_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.throws_propagation_lowering_pointer_declarator_sites)
      << ", i64 "
      << static_cast<unsigned long long>(metadata.throws_propagation_lowering_normalized_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.throws_propagation_lowering_cache_invalidation_candidate_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.throws_propagation_lowering_contract_violation_sites)
      << ", i1 "
      << (metadata.deterministic_throws_propagation_lowering_handoff ? 1 : 0)
      << "}\n\n";
  out << "!35 = !{i64 "
      << static_cast<unsigned long long>(metadata.unwind_cleanup_lowering_sites)
      << ", i64 "
      << static_cast<unsigned long long>(metadata.unwind_cleanup_lowering_unwind_edge_sites)
      << ", i64 "
      << static_cast<unsigned long long>(metadata.unwind_cleanup_lowering_cleanup_scope_sites)
      << ", i64 "
      << static_cast<unsigned long long>(metadata.unwind_cleanup_lowering_cleanup_emit_sites)
      << ", i64 "
      << static_cast<unsigned long long>(metadata.unwind_cleanup_lowering_landing_pad_sites)
      << ", i64 "
      << static_cast<unsigned long long>(metadata.unwind_cleanup_lowering_cleanup_resume_sites)
      << ", i64 "
      << static_cast<unsigned long long>(metadata.unwind_cleanup_lowering_normalized_sites)
      << ", i64 "
      << static_cast<unsigned long long>(metadata.unwind_cleanup_lowering_guard_blocked_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.unwind_cleanup_lowering_contract_violation_sites)
      << ", i1 "
      << (metadata.deterministic_unwind_cleanup_lowering_handoff ? 1 : 0)
      << "}\n\n";
  out << "!36 = !{i64 "
      << static_cast<unsigned long long>(metadata.ns_error_bridging_lowering_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.ns_error_bridging_lowering_ns_error_parameter_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.ns_error_bridging_lowering_ns_error_out_parameter_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.ns_error_bridging_lowering_ns_error_bridge_path_sites)
      << ", i64 "
      << static_cast<unsigned long long>(metadata.ns_error_bridging_lowering_failable_call_sites)
      << ", i64 "
      << static_cast<unsigned long long>(metadata.ns_error_bridging_lowering_normalized_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.ns_error_bridging_lowering_bridge_boundary_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.ns_error_bridging_lowering_contract_violation_sites)
      << ", i1 "
      << (metadata.deterministic_ns_error_bridging_lowering_handoff ? 1 : 0)
      << "}\n\n";
}

void EmitObjc3IRSafetyConcurrencyLoweringCounterNodes(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  out << "!37 = !{i64 "
      << static_cast<unsigned long long>(
             metadata.unsafe_pointer_extension_lowering_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.unsafe_pointer_extension_lowering_unsafe_keyword_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.unsafe_pointer_extension_lowering_pointer_arithmetic_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.unsafe_pointer_extension_lowering_raw_pointer_type_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.unsafe_pointer_extension_lowering_unsafe_operation_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.unsafe_pointer_extension_lowering_normalized_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.unsafe_pointer_extension_lowering_gate_blocked_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.unsafe_pointer_extension_lowering_contract_violation_sites)
      << ", i1 "
      << (metadata.deterministic_unsafe_pointer_extension_lowering_handoff ? 1 : 0)
      << "}\n\n";
  out << "!38 = !{i64 "
      << static_cast<unsigned long long>(
             metadata.inline_asm_intrinsic_governance_lowering_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.inline_asm_intrinsic_governance_lowering_inline_asm_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.inline_asm_intrinsic_governance_lowering_intrinsic_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.inline_asm_intrinsic_governance_lowering_governed_intrinsic_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.inline_asm_intrinsic_governance_lowering_privileged_intrinsic_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.inline_asm_intrinsic_governance_lowering_normalized_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.inline_asm_intrinsic_governance_lowering_gate_blocked_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.inline_asm_intrinsic_governance_lowering_contract_violation_sites)
      << ", i1 "
      << (metadata.deterministic_inline_asm_intrinsic_governance_lowering_handoff ? 1 : 0)
      << "}\n\n";
  out << "!39 = !{i64 "
      << static_cast<unsigned long long>(
             metadata.concurrency_replay_race_guard_lowering_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.concurrency_replay_race_guard_lowering_replay_proof_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.concurrency_replay_race_guard_lowering_race_guard_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.concurrency_replay_race_guard_lowering_task_handoff_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.concurrency_replay_race_guard_lowering_actor_isolation_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.concurrency_replay_race_guard_lowering_deterministic_schedule_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.concurrency_replay_race_guard_lowering_guard_blocked_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.concurrency_replay_race_guard_lowering_contract_violation_sites)
      << ", i1 "
      << (metadata.deterministic_concurrency_replay_race_guard_lowering_handoff ? 1 : 0)
      << "}\n\n";
  out << "!40 = !{i64 "
      << static_cast<unsigned long long>(
             metadata.task_runtime_interop_cancellation_lowering_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.task_runtime_interop_cancellation_lowering_runtime_interop_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.task_runtime_interop_cancellation_lowering_cancellation_probe_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.task_runtime_interop_cancellation_lowering_cancellation_handler_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.task_runtime_interop_cancellation_lowering_runtime_resume_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.task_runtime_interop_cancellation_lowering_runtime_cancel_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.task_runtime_interop_cancellation_lowering_normalized_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.task_runtime_interop_cancellation_lowering_guard_blocked_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.task_runtime_interop_cancellation_lowering_contract_violation_sites)
      << ", i1 "
      << (metadata.deterministic_task_runtime_interop_cancellation_lowering_handoff ? 1 : 0)
      << "}\n\n";
  out << "!41 = !{i64 "
      << static_cast<unsigned long long>(
             metadata.actor_isolation_sendability_lowering_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.actor_isolation_sendability_lowering_sendability_check_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.actor_isolation_sendability_lowering_cross_actor_hop_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.actor_isolation_sendability_lowering_non_sendable_capture_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.actor_isolation_sendability_lowering_sendable_transfer_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.actor_isolation_sendability_lowering_isolation_boundary_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.actor_isolation_sendability_lowering_guard_blocked_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.actor_isolation_sendability_lowering_contract_violation_sites)
      << ", i1 "
      << (metadata.deterministic_actor_isolation_sendability_lowering_handoff ? 1 : 0)
      << "}\n\n";
}

void EmitObjc3IRAsyncDiagnosticLoweringCounterNodes(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  out << "!42 = !{i64 "
      << static_cast<unsigned long long>(
             metadata.await_lowering_suspension_state_lowering_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.await_lowering_suspension_state_lowering_await_keyword_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.await_lowering_suspension_state_lowering_await_suspension_point_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.await_lowering_suspension_state_lowering_await_resume_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.await_lowering_suspension_state_lowering_await_state_machine_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.await_lowering_suspension_state_lowering_await_continuation_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.await_lowering_suspension_state_lowering_normalized_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.await_lowering_suspension_state_lowering_gate_blocked_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.await_lowering_suspension_state_lowering_contract_violation_sites)
      << ", i1 "
      << (metadata.deterministic_await_lowering_suspension_state_lowering_handoff ? 1 : 0)
      << "}\n\n";
  out << "!43 = !{i64 "
      << static_cast<unsigned long long>(
             metadata.async_continuation_lowering_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.async_continuation_lowering_async_keyword_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.async_continuation_lowering_async_function_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.async_continuation_lowering_continuation_allocation_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.async_continuation_lowering_continuation_resume_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.async_continuation_lowering_continuation_suspend_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.async_continuation_lowering_async_state_machine_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.async_continuation_lowering_normalized_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.async_continuation_lowering_gate_blocked_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.async_continuation_lowering_contract_violation_sites)
      << ", i1 "
      << (metadata.deterministic_async_continuation_lowering_handoff ? 1 : 0)
      << "}\n\n";
  out << "!44 = !{i64 "
      << static_cast<unsigned long long>(
             metadata.error_diagnostics_recovery_lowering_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.error_diagnostics_recovery_lowering_parser_diagnostic_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.error_diagnostics_recovery_lowering_semantic_diagnostic_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.error_diagnostics_recovery_lowering_fixit_hint_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.error_diagnostics_recovery_lowering_recovery_candidate_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.error_diagnostics_recovery_lowering_recovery_applied_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.error_diagnostics_recovery_lowering_normalized_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.error_diagnostics_recovery_lowering_guard_blocked_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.error_diagnostics_recovery_lowering_contract_violation_sites)
      << ", i1 "
      << (metadata.deterministic_error_diagnostics_recovery_lowering_handoff ? 1 : 0)
      << "}\n\n";
}
