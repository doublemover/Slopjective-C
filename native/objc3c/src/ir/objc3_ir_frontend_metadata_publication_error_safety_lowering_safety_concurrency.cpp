#include "ir/objc3_ir_frontend_metadata_publication_error_safety_lowering_safety_concurrency.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata.h"

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
