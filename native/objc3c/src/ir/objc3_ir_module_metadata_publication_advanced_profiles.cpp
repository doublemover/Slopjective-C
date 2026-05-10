#include "ir/objc3_ir_module_metadata_publication_advanced_profiles.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata.h"

void EmitObjc3IRModuleMetadataAdvancedProfilePublication(
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
  out << "; frontend_objc_interop_interop_lowering_profile = foreign_callable_sites="
      << frontend_metadata_.interop_interop_lowering_foreign_callable_sites
      << ", c_foreign_callable_sites="
      << frontend_metadata_.interop_interop_lowering_c_foreign_callable_sites
      << ", objc_runtime_parity_callable_sites="
      << frontend_metadata_.interop_interop_lowering_objc_runtime_parity_callable_sites
      << ", ownership_bridge_callable_sites="
      << frontend_metadata_.interop_interop_lowering_ownership_bridge_callable_sites
      << ", error_surface_sites="
      << frontend_metadata_.interop_interop_lowering_error_surface_sites
      << ", async_boundary_sites="
      << frontend_metadata_.interop_interop_lowering_async_boundary_sites
      << ", swift_concurrency_metadata_sites="
      << frontend_metadata_.interop_interop_lowering_swift_concurrency_metadata_sites
      << ", interface_preserved_foreign_callable_sites="
      << frontend_metadata_.interop_interop_lowering_interface_preserved_foreign_callable_sites
      << ", interface_preserved_metadata_annotation_sites="
      << frontend_metadata_.interop_interop_lowering_interface_preserved_metadata_annotation_sites
      << ", guard_blocked_sites="
      << frontend_metadata_.interop_interop_lowering_guard_blocked_sites
      << ", contract_violation_sites="
      << frontend_metadata_.interop_interop_lowering_contract_violation_sites
      << ", deterministic_interop_interop_lowering_handoff="
      << (frontend_metadata_.deterministic_interop_interop_lowering_handoff
              ? "true"
              : "false")
      << "\n";
  out << "; frontend_objc_metaprogramming_expansion_lowering_profile = derive_inventory_sites="
      << frontend_metadata_.metaprogramming_expansion_lowering_derive_inventory_sites
      << ", derived_selector_artifact_sites="
      << frontend_metadata_
             .metaprogramming_expansion_lowering_derived_selector_artifact_sites
      << ", macro_replay_visible_sites="
      << frontend_metadata_
             .metaprogramming_expansion_lowering_macro_replay_visible_sites
      << ", property_behavior_sites="
      << frontend_metadata_.metaprogramming_expansion_lowering_property_behavior_sites
      << ", synthesized_binding_sites="
      << frontend_metadata_.metaprogramming_expansion_lowering_synthesized_binding_sites
      << ", synthesized_getter_sites="
      << frontend_metadata_.metaprogramming_expansion_lowering_synthesized_getter_sites
      << ", synthesized_setter_sites="
      << frontend_metadata_.metaprogramming_expansion_lowering_synthesized_setter_sites
      << ", replay_visible_metadata_sites="
      << frontend_metadata_
             .metaprogramming_expansion_lowering_replay_visible_metadata_sites
      << ", guard_blocked_sites="
      << frontend_metadata_.metaprogramming_expansion_lowering_guard_blocked_sites
      << ", contract_violation_sites="
      << frontend_metadata_
             .metaprogramming_expansion_lowering_contract_violation_sites
      << ", deterministic_metaprogramming_expansion_lowering_handoff="
      << (frontend_metadata_.deterministic_metaprogramming_expansion_lowering_handoff
              ? "true"
              : "false")
      << "\n";
  out << "; frontend_objc_metaprogramming_synthesized_emission_profile = emitted_derive_method_sites="
      << frontend_metadata_.metaprogramming_synthesized_emitted_derive_method_sites
      << ", emitted_macro_artifact_sites="
      << frontend_metadata_.metaprogramming_synthesized_emitted_macro_artifact_sites
      << ", emitted_property_behavior_artifact_sites="
      << frontend_metadata_
             .metaprogramming_synthesized_emitted_property_behavior_artifact_sites
      << ", emitted_global_artifact_sites="
      << frontend_metadata_.metaprogramming_synthesized_emitted_global_artifact_sites
      << ", emitted_runtime_method_list_sites="
      << frontend_metadata_
             .metaprogramming_synthesized_emitted_runtime_method_list_sites
      << ", guard_blocked_sites="
      << frontend_metadata_.metaprogramming_synthesized_guard_blocked_sites
      << ", contract_violation_sites="
      << frontend_metadata_.metaprogramming_synthesized_contract_violation_sites
      << ", deterministic_metaprogramming_synthesized_emission_handoff="
      << (frontend_metadata_.deterministic_metaprogramming_synthesized_emission_handoff
              ? "true"
              : "false")
      << "\n";
  out << "; frontend_objc_dispatch_metadata_interface_profile = local_direct_callable_record_count="
      << frontend_metadata_
             .dispatch_dispatch_metadata_local_direct_callable_record_count
      << ", local_final_callable_record_count="
      << frontend_metadata_
             .dispatch_dispatch_metadata_local_final_callable_record_count
      << ", local_final_container_record_count="
      << frontend_metadata_
             .dispatch_dispatch_metadata_local_final_container_record_count
      << ", local_sealed_container_record_count="
      << frontend_metadata_
             .dispatch_dispatch_metadata_local_sealed_container_record_count
      << ", imported_module_count="
      << frontend_metadata_.dispatch_dispatch_metadata_imported_module_count
      << ", imported_direct_callable_record_count="
      << frontend_metadata_
             .dispatch_dispatch_metadata_imported_direct_callable_record_count
      << ", imported_final_callable_record_count="
      << frontend_metadata_
             .dispatch_dispatch_metadata_imported_final_callable_record_count
      << ", imported_final_container_record_count="
      << frontend_metadata_
             .dispatch_dispatch_metadata_imported_final_container_record_count
      << ", imported_sealed_container_record_count="
      << frontend_metadata_
             .dispatch_dispatch_metadata_imported_sealed_container_record_count
      << ", runtime_import_artifact_ready="
      << (frontend_metadata_
                  .dispatch_dispatch_metadata_runtime_import_artifact_ready
              ? "true"
              : "false")
      << ", separate_compilation_preservation_ready="
      << (frontend_metadata_
                  .dispatch_dispatch_metadata_separate_compilation_preservation_ready
              ? "true"
              : "false")
      << ", deterministic_dispatch_dispatch_metadata_interface_handoff="
      << (frontend_metadata_
                  .deterministic_dispatch_dispatch_metadata_interface_handoff
              ? "true"
              : "false")
      << "\n";
  out << "; frontend_objc_system_extension_lowering_profile = cleanup_hook_sites="
      << frontend_metadata_.ownership_system_extension_lowering_cleanup_hook_sites
      << ", resource_local_sites="
      << frontend_metadata_.ownership_system_extension_lowering_resource_local_sites
      << ", cleanup_owned_local_sites="
      << frontend_metadata_
             .ownership_system_extension_lowering_cleanup_owned_local_sites
      << ", resource_move_capture_sites="
      << frontend_metadata_
             .ownership_system_extension_lowering_resource_move_capture_sites
      << ", borrowed_parameter_sites="
      << frontend_metadata_
             .ownership_system_extension_lowering_borrowed_parameter_sites
      << ", borrowed_return_callable_sites="
      << frontend_metadata_
             .ownership_system_extension_lowering_borrowed_return_callable_sites
      << ", borrowed_escape_candidate_sites="
      << frontend_metadata_
             .ownership_system_extension_lowering_borrowed_escape_candidate_sites
      << ", explicit_capture_item_sites="
      << frontend_metadata_
             .ownership_system_extension_lowering_explicit_capture_item_sites
      << ", retainable_family_callable_sites="
      << frontend_metadata_
             .ownership_system_extension_lowering_retainable_family_callable_sites
      << ", retainable_family_operation_callable_sites="
      << frontend_metadata_
             .ownership_system_extension_lowering_retainable_family_operation_callable_sites
      << ", retainable_family_alias_callable_sites="
      << frontend_metadata_
             .ownership_system_extension_lowering_retainable_family_alias_callable_sites
      << ", guard_blocked_sites="
      << frontend_metadata_.ownership_system_extension_lowering_guard_blocked_sites
      << ", contract_violation_sites="
      << frontend_metadata_
             .ownership_system_extension_lowering_contract_violation_sites
      << ", deterministic_ownership_system_extension_lowering_handoff="
      << (frontend_metadata_
                  .deterministic_ownership_system_extension_lowering_handoff
              ? "true"
              : "false")
      << "\n";
  out << "; frontend_objc_ownership_borrowed_retainable_abi_profile = returns_borrowed_attribute_sites="
      << frontend_metadata_
             .ownership_borrowed_retainable_returns_borrowed_attribute_sites
      << ", family_retain_sites="
      << frontend_metadata_.ownership_borrowed_retainable_family_retain_sites
      << ", family_release_sites="
      << frontend_metadata_.ownership_borrowed_retainable_family_release_sites
      << ", family_autorelease_sites="
      << frontend_metadata_.ownership_borrowed_retainable_family_autorelease_sites
      << ", compatibility_returns_retained_sites="
      << frontend_metadata_
             .ownership_borrowed_retainable_compatibility_returns_retained_sites
      << ", compatibility_returns_not_retained_sites="
      << frontend_metadata_
             .ownership_borrowed_retainable_compatibility_returns_not_retained_sites
      << ", compatibility_consumed_sites="
      << frontend_metadata_
             .ownership_borrowed_retainable_compatibility_consumed_sites
      << ", deterministic_ownership_borrowed_retainable_abi_completion_handoff="
      << (frontend_metadata_
                  .deterministic_ownership_borrowed_retainable_abi_completion_handoff
              ? "true"
              : "false")
      << "\n";
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
