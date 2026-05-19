#include "ir/objc3_ir_module_metadata_publication_advanced_profiles_async_actor.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata.h"

void EmitObjc3IRModuleMetadataAsyncActorAdvancedProfilePublication(
    const Objc3IRFrontendMetadata &frontend_metadata_,
    std::ostringstream &out) {
  out << "; frontend_objc_async_continuation_lowering_profile = async_continuation_sites="
      << frontend_metadata_.async_continuation_lowering_sites
      << ", async_keyword_sites="
      << frontend_metadata_.async_continuation_lowering_async_keyword_sites
      << ", async_function_sites="
      << frontend_metadata_.async_continuation_lowering_async_function_sites
      << ", continuation_allocation_sites="
      << frontend_metadata_
             .async_continuation_lowering_continuation_allocation_sites
      << ", continuation_resume_sites="
      << frontend_metadata_
             .async_continuation_lowering_continuation_resume_sites
      << ", continuation_suspend_sites="
      << frontend_metadata_
             .async_continuation_lowering_continuation_suspend_sites
      << ", async_state_machine_sites="
      << frontend_metadata_
             .async_continuation_lowering_async_state_machine_sites
      << ", normalized_sites="
      << frontend_metadata_.async_continuation_lowering_normalized_sites
      << ", gate_blocked_sites="
      << frontend_metadata_.async_continuation_lowering_gate_blocked_sites
      << ", contract_violation_sites="
      << frontend_metadata_
             .async_continuation_lowering_contract_violation_sites
      << ", deterministic_async_continuation_lowering_handoff="
      << (frontend_metadata_.deterministic_async_continuation_lowering_handoff
              ? "true"
              : "false")
      << "\n";
  out << "; frontend_objc_await_lowering_suspension_state_lowering_profile = await_suspension_sites="
      << frontend_metadata_.await_lowering_suspension_state_lowering_sites
      << ", await_keyword_sites="
      << frontend_metadata_
             .await_lowering_suspension_state_lowering_await_keyword_sites
      << ", await_suspension_point_sites="
      << frontend_metadata_
             .await_lowering_suspension_state_lowering_await_suspension_point_sites
      << ", await_resume_sites="
      << frontend_metadata_
             .await_lowering_suspension_state_lowering_await_resume_sites
      << ", await_state_machine_sites="
      << frontend_metadata_
             .await_lowering_suspension_state_lowering_await_state_machine_sites
      << ", await_continuation_sites="
      << frontend_metadata_
             .await_lowering_suspension_state_lowering_await_continuation_sites
      << ", normalized_sites="
      << frontend_metadata_
             .await_lowering_suspension_state_lowering_normalized_sites
      << ", gate_blocked_sites="
      << frontend_metadata_
             .await_lowering_suspension_state_lowering_gate_blocked_sites
      << ", contract_violation_sites="
      << frontend_metadata_
             .await_lowering_suspension_state_lowering_contract_violation_sites
      << ", deterministic_await_lowering_suspension_state_lowering_handoff="
      << (frontend_metadata_
                  .deterministic_await_lowering_suspension_state_lowering_handoff
              ? "true"
              : "false")
      << "\n";
  out << "; frontend_objc_actor_isolation_sendability_lowering_profile = actor_isolation_sites="
      << frontend_metadata_.actor_isolation_sendability_lowering_sites
      << ", sendability_check_sites="
      << frontend_metadata_
             .actor_isolation_sendability_lowering_sendability_check_sites
      << ", cross_actor_hop_sites="
      << frontend_metadata_
             .actor_isolation_sendability_lowering_cross_actor_hop_sites
      << ", non_sendable_capture_sites="
      << frontend_metadata_
             .actor_isolation_sendability_lowering_non_sendable_capture_sites
      << ", sendable_transfer_sites="
      << frontend_metadata_
             .actor_isolation_sendability_lowering_sendable_transfer_sites
      << ", isolation_boundary_sites="
      << frontend_metadata_
             .actor_isolation_sendability_lowering_isolation_boundary_sites
      << ", guard_blocked_sites="
      << frontend_metadata_
             .actor_isolation_sendability_lowering_guard_blocked_sites
      << ", contract_violation_sites="
      << frontend_metadata_
             .actor_isolation_sendability_lowering_contract_violation_sites
      << ", deterministic_actor_isolation_sendability_lowering_handoff="
      << (frontend_metadata_
                  .deterministic_actor_isolation_sendability_lowering_handoff
              ? "true"
              : "false")
      << "\n";
  out << "; frontend_objc_actor_lowering_metadata_profile = actor_interface_sites="
      << frontend_metadata_.actor_lowering_metadata_actor_interface_sites
      << ", actor_method_sites="
      << frontend_metadata_.actor_lowering_metadata_actor_method_sites
      << ", actor_metadata_record_sites="
      << frontend_metadata_
             .actor_lowering_metadata_actor_metadata_record_sites
      << ", nonisolated_entry_sites="
      << frontend_metadata_.actor_lowering_metadata_nonisolated_entry_sites
      << ", executor_affinity_sites="
      << frontend_metadata_.actor_lowering_metadata_executor_affinity_sites
      << ", actor_hop_artifact_sites="
      << frontend_metadata_.actor_lowering_metadata_actor_hop_artifact_sites
      << ", actor_isolation_thunk_sites="
      << frontend_metadata_.actor_lowering_metadata_actor_isolation_thunk_sites
      << ", replay_proof_dependency_sites="
      << frontend_metadata_
             .actor_lowering_metadata_replay_proof_dependency_sites
      << ", race_guard_dependency_sites="
      << frontend_metadata_
             .actor_lowering_metadata_race_guard_dependency_sites
      << ", task_handoff_sites="
      << frontend_metadata_.actor_lowering_metadata_task_handoff_sites
      << ", guard_blocked_sites="
      << frontend_metadata_.actor_lowering_metadata_guard_blocked_sites
      << ", contract_violation_sites="
      << frontend_metadata_.actor_lowering_metadata_contract_violation_sites
      << ", deterministic_actor_lowering_metadata_handoff="
      << (frontend_metadata_.deterministic_actor_lowering_metadata_handoff
              ? "true"
              : "false")
      << "\n";
  out << "; frontend_objc_dispatch_control_lowering_profile = direct_call_candidate_sites="
      << frontend_metadata_
             .dispatch_dispatch_control_lowering_direct_call_candidate_sites
      << ", direct_members_defaulted_sites="
      << frontend_metadata_
             .dispatch_dispatch_control_lowering_direct_members_defaulted_sites
      << ", dynamic_opt_out_sites="
      << frontend_metadata_
             .dispatch_dispatch_control_lowering_dynamic_opt_out_sites
      << ", final_container_sites="
      << frontend_metadata_
             .dispatch_dispatch_control_lowering_final_container_sites
      << ", sealed_container_sites="
      << frontend_metadata_
             .dispatch_dispatch_control_lowering_sealed_container_sites
      << ", override_legality_sites="
      << frontend_metadata_
             .dispatch_dispatch_control_lowering_override_legality_sites
      << ", metadata_preserved_callable_sites="
      << frontend_metadata_
             .dispatch_dispatch_control_lowering_metadata_preserved_callable_sites
      << ", metadata_preserved_container_sites="
      << frontend_metadata_
             .dispatch_dispatch_control_lowering_metadata_preserved_container_sites
      << ", guard_blocked_sites="
      << frontend_metadata_.dispatch_dispatch_control_lowering_guard_blocked_sites
      << ", contract_violation_sites="
      << frontend_metadata_
             .dispatch_dispatch_control_lowering_contract_violation_sites
      << ", deterministic_dispatch_dispatch_control_lowering_handoff="
      << (frontend_metadata_
                  .deterministic_dispatch_dispatch_control_lowering_handoff
              ? "true"
              : "false")
      << "\n";
}
