#include "ir/objc3_ir_module_metadata_publication_advanced_profiles_safety.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata.h"

void EmitObjc3IRModuleMetadataSafetyAdvancedProfilePublication(
    const Objc3IRFrontendMetadata &frontend_metadata_,
    std::ostringstream &out) {
  out << "; frontend_objc_task_runtime_interop_cancellation_lowering_profile = task_runtime_sites="
      << frontend_metadata_.task_runtime_interop_cancellation_lowering_sites
      << ", task_runtime_interop_sites="
      << frontend_metadata_
             .task_runtime_interop_cancellation_lowering_runtime_interop_sites
      << ", cancellation_probe_sites="
      << frontend_metadata_
             .task_runtime_interop_cancellation_lowering_cancellation_probe_sites
      << ", cancellation_handler_sites="
      << frontend_metadata_
             .task_runtime_interop_cancellation_lowering_cancellation_handler_sites
      << ", runtime_resume_sites="
      << frontend_metadata_
             .task_runtime_interop_cancellation_lowering_runtime_resume_sites
      << ", runtime_cancel_sites="
      << frontend_metadata_
             .task_runtime_interop_cancellation_lowering_runtime_cancel_sites
      << ", normalized_sites="
      << frontend_metadata_
             .task_runtime_interop_cancellation_lowering_normalized_sites
      << ", guard_blocked_sites="
      << frontend_metadata_
             .task_runtime_interop_cancellation_lowering_guard_blocked_sites
      << ", contract_violation_sites="
      << frontend_metadata_
             .task_runtime_interop_cancellation_lowering_contract_violation_sites
      << ", deterministic_task_runtime_interop_cancellation_lowering_handoff="
      << (frontend_metadata_
                  .deterministic_task_runtime_interop_cancellation_lowering_handoff
              ? "true"
              : "false")
      << "\n";
  out << "; frontend_objc_concurrency_replay_race_guard_lowering_profile = concurrency_replay_sites="
      << frontend_metadata_.concurrency_replay_race_guard_lowering_sites
      << ", replay_proof_sites="
      << frontend_metadata_
             .concurrency_replay_race_guard_lowering_replay_proof_sites
      << ", race_guard_sites="
      << frontend_metadata_
             .concurrency_replay_race_guard_lowering_race_guard_sites
      << ", task_handoff_sites="
      << frontend_metadata_
             .concurrency_replay_race_guard_lowering_task_handoff_sites
      << ", actor_isolation_sites="
      << frontend_metadata_
             .concurrency_replay_race_guard_lowering_actor_isolation_sites
      << ", deterministic_schedule_sites="
      << frontend_metadata_
             .concurrency_replay_race_guard_lowering_deterministic_schedule_sites
      << ", guard_blocked_sites="
      << frontend_metadata_
             .concurrency_replay_race_guard_lowering_guard_blocked_sites
      << ", contract_violation_sites="
      << frontend_metadata_
             .concurrency_replay_race_guard_lowering_contract_violation_sites
      << ", deterministic_concurrency_replay_race_guard_lowering_handoff="
      << (frontend_metadata_
                  .deterministic_concurrency_replay_race_guard_lowering_handoff
              ? "true"
              : "false")
      << "\n";
  out << "; frontend_objc_unsafe_pointer_extension_lowering_profile = unsafe_pointer_extension_sites="
      << frontend_metadata_.unsafe_pointer_extension_lowering_sites
      << ", unsafe_keyword_sites="
      << frontend_metadata_
             .unsafe_pointer_extension_lowering_unsafe_keyword_sites
      << ", pointer_arithmetic_sites="
      << frontend_metadata_
             .unsafe_pointer_extension_lowering_pointer_arithmetic_sites
      << ", raw_pointer_type_sites="
      << frontend_metadata_
             .unsafe_pointer_extension_lowering_raw_pointer_type_sites
      << ", unsafe_operation_sites="
      << frontend_metadata_
             .unsafe_pointer_extension_lowering_unsafe_operation_sites
      << ", normalized_sites="
      << frontend_metadata_.unsafe_pointer_extension_lowering_normalized_sites
      << ", gate_blocked_sites="
      << frontend_metadata_
             .unsafe_pointer_extension_lowering_gate_blocked_sites
      << ", contract_violation_sites="
      << frontend_metadata_
             .unsafe_pointer_extension_lowering_contract_violation_sites
      << ", deterministic_unsafe_pointer_extension_lowering_handoff="
      << (frontend_metadata_
                  .deterministic_unsafe_pointer_extension_lowering_handoff
              ? "true"
              : "false")
      << "\n";
  out << "; frontend_objc_inline_asm_intrinsic_governance_lowering_profile = inline_asm_intrinsic_sites="
      << frontend_metadata_.inline_asm_intrinsic_governance_lowering_sites
      << ", inline_asm_sites="
      << frontend_metadata_
             .inline_asm_intrinsic_governance_lowering_inline_asm_sites
      << ", intrinsic_sites="
      << frontend_metadata_
             .inline_asm_intrinsic_governance_lowering_intrinsic_sites
      << ", governed_intrinsic_sites="
      << frontend_metadata_
             .inline_asm_intrinsic_governance_lowering_governed_intrinsic_sites
      << ", privileged_intrinsic_sites="
      << frontend_metadata_
             .inline_asm_intrinsic_governance_lowering_privileged_intrinsic_sites
      << ", normalized_sites="
      << frontend_metadata_
             .inline_asm_intrinsic_governance_lowering_normalized_sites
      << ", gate_blocked_sites="
      << frontend_metadata_
             .inline_asm_intrinsic_governance_lowering_gate_blocked_sites
      << ", contract_violation_sites="
      << frontend_metadata_
             .inline_asm_intrinsic_governance_lowering_contract_violation_sites
      << ", deterministic_inline_asm_intrinsic_governance_lowering_handoff="
      << (frontend_metadata_
                  .deterministic_inline_asm_intrinsic_governance_lowering_handoff
              ? "true"
              : "false")
      << "\n";
}
