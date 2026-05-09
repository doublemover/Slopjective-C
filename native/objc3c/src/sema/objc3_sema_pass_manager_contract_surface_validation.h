             surface.protocol_qualified_object_type_pointer_declarator_sites_total &&
         surface.protocol_qualified_object_type_summary.normalized_protocol_composition_sites ==
             surface.protocol_qualified_object_type_normalized_protocol_composition_sites_total &&
         surface.protocol_qualified_object_type_summary.contract_violation_sites ==
             surface.protocol_qualified_object_type_contract_violation_sites_total &&
         surface.protocol_qualified_object_type_summary.terminated_protocol_composition_sites <=
             surface.protocol_qualified_object_type_summary.protocol_composition_sites &&
         surface.protocol_qualified_object_type_summary.normalized_protocol_composition_sites <=
             surface.protocol_qualified_object_type_summary.protocol_qualified_object_type_sites &&
         surface.protocol_qualified_object_type_summary.contract_violation_sites <=
             surface.protocol_qualified_object_type_summary.protocol_qualified_object_type_sites &&
         surface.protocol_qualified_object_type_summary.deterministic &&
         surface.deterministic_protocol_qualified_object_type_handoff &&
         surface.deterministic_module_type_abi_summary_readiness_record &&
         IsReadyObjc3SemaModuleTypeAbiSummaryReadinessRecord(
             surface.module_type_abi_summary_readiness_record) &&
         surface.deterministic_module_boundary_summary_readiness_record &&
         IsReadyObjc3SemaModuleBoundarySummaryReadinessRecord(
             surface.module_boundary_summary_readiness_record) &&
         surface.deterministic_intermodule_flow_summary_readiness_record &&
         IsReadyObjc3SemaIntermoduleFlowSummaryReadinessRecord(
             surface.intermodule_flow_summary_readiness_record) &&
         surface.actor_isolation_sendability_summary
                 .actor_isolation_sendability_sites ==
             surface.actor_isolation_sendability_sites_total &&
         surface.actor_isolation_sendability_summary
                 .actor_isolation_decl_sites ==
             surface.actor_isolation_decl_sites_total &&
         surface.actor_isolation_sendability_summary.actor_hop_sites ==
             surface.actor_hop_sites_total &&
         surface.actor_isolation_sendability_summary
                 .sendable_annotation_sites ==
             surface.sendable_annotation_sites_total &&
         surface.actor_isolation_sendability_summary
                 .non_sendable_crossing_sites ==
             surface.non_sendable_crossing_sites_total &&
         surface.actor_isolation_sendability_summary
                 .isolation_boundary_sites ==
             surface.actor_isolation_sendability_isolation_boundary_sites_total &&
         surface.actor_isolation_sendability_summary.normalized_sites ==
             surface.actor_isolation_sendability_normalized_sites_total &&
         surface.actor_isolation_sendability_summary.gate_blocked_sites ==
             surface.actor_isolation_sendability_gate_blocked_sites_total &&
         surface.actor_isolation_sendability_summary.contract_violation_sites ==
             surface.actor_isolation_sendability_contract_violation_sites_total &&
         surface.actor_isolation_sendability_summary
                 .actor_isolation_decl_sites <=
             surface.actor_isolation_sendability_summary
                 .actor_isolation_sendability_sites &&
         surface.actor_isolation_sendability_summary.actor_hop_sites <=
             surface.actor_isolation_sendability_summary
                 .actor_isolation_sendability_sites &&
         surface.actor_isolation_sendability_summary
                 .sendable_annotation_sites <=
             surface.actor_isolation_sendability_summary
                 .actor_isolation_sendability_sites &&
         surface.actor_isolation_sendability_summary
                 .non_sendable_crossing_sites <=
             surface.actor_isolation_sendability_summary
                 .actor_isolation_sendability_sites &&
         surface.actor_isolation_sendability_summary
                 .isolation_boundary_sites <=
             surface.actor_isolation_sendability_summary
                 .actor_isolation_sendability_sites &&
         surface.actor_isolation_sendability_summary.normalized_sites <=
             surface.actor_isolation_sendability_summary
                 .actor_isolation_sendability_sites &&
         surface.actor_isolation_sendability_summary.gate_blocked_sites <=
             surface.actor_isolation_sendability_summary
                 .actor_isolation_sendability_sites &&
         surface.actor_isolation_sendability_summary.gate_blocked_sites <=
             surface.actor_isolation_sendability_summary
                 .non_sendable_crossing_sites &&
         surface.actor_isolation_sendability_summary
                 .contract_violation_sites <=
             surface.actor_isolation_sendability_summary
                 .actor_isolation_sendability_sites &&
         surface.actor_isolation_sendability_summary.normalized_sites +
                 surface.actor_isolation_sendability_summary.gate_blocked_sites ==
             surface.actor_isolation_sendability_summary
                 .actor_isolation_sendability_sites &&
         surface.actor_isolation_sendability_summary.deterministic &&
         surface.deterministic_actor_isolation_sendability_handoff &&
         surface.task_runtime_cancellation_summary.task_runtime_interop_sites ==
             surface.task_runtime_cancellation_sites_total &&
         surface.task_runtime_cancellation_summary.runtime_hook_sites ==
             surface.task_runtime_cancellation_runtime_hook_sites_total &&
         surface.task_runtime_cancellation_summary.cancellation_check_sites ==
             surface.task_runtime_cancellation_cancellation_check_sites_total &&
         surface.task_runtime_cancellation_summary.cancellation_handler_sites ==
             surface
                 .task_runtime_cancellation_cancellation_handler_sites_total &&
         surface.task_runtime_cancellation_summary.suspension_point_sites ==
             surface.task_runtime_cancellation_suspension_point_sites_total &&
         surface.task_runtime_cancellation_summary
                 .cancellation_propagation_sites ==
             surface
                 .task_runtime_cancellation_cancellation_propagation_sites_total &&
         surface.task_runtime_cancellation_summary.normalized_sites ==
             surface.task_runtime_cancellation_normalized_sites_total &&
         surface.task_runtime_cancellation_summary.gate_blocked_sites ==
             surface.task_runtime_cancellation_gate_blocked_sites_total &&
         surface.task_runtime_cancellation_summary.contract_violation_sites ==
             surface.task_runtime_cancellation_contract_violation_sites_total &&
         surface.task_runtime_cancellation_summary.runtime_hook_sites <=
             surface.task_runtime_cancellation_summary
                 .task_runtime_interop_sites &&
         surface.task_runtime_cancellation_summary.cancellation_check_sites <=
             surface.task_runtime_cancellation_summary
                 .task_runtime_interop_sites &&
         surface.task_runtime_cancellation_summary.cancellation_handler_sites <=
             surface.task_runtime_cancellation_summary
                 .task_runtime_interop_sites &&
         surface.task_runtime_cancellation_summary.suspension_point_sites <=
             surface.task_runtime_cancellation_summary
                 .task_runtime_interop_sites &&
         surface.task_runtime_cancellation_summary
                 .cancellation_propagation_sites <=
             surface.task_runtime_cancellation_summary
                 .cancellation_check_sites &&
         surface.task_runtime_cancellation_summary
                 .cancellation_propagation_sites <=
             surface.task_runtime_cancellation_summary
                 .cancellation_handler_sites &&
         surface.task_runtime_cancellation_summary.normalized_sites <=
             surface.task_runtime_cancellation_summary
                 .task_runtime_interop_sites &&
         surface.task_runtime_cancellation_summary.gate_blocked_sites <=
             surface.task_runtime_cancellation_summary
                 .task_runtime_interop_sites &&
         surface.task_runtime_cancellation_summary.gate_blocked_sites <=
             surface.task_runtime_cancellation_summary
                 .cancellation_propagation_sites &&
         surface.task_runtime_cancellation_summary.contract_violation_sites <=
             surface.task_runtime_cancellation_summary
                 .task_runtime_interop_sites &&
         surface.task_runtime_cancellation_summary.normalized_sites +
                 surface.task_runtime_cancellation_summary.gate_blocked_sites ==
             surface.task_runtime_cancellation_summary
                 .task_runtime_interop_sites &&
         surface.task_runtime_cancellation_summary.deterministic &&
         surface.deterministic_task_runtime_cancellation_handoff &&
         surface.concurrency_replay_race_guard_summary
                 .concurrency_replay_race_guard_sites ==
             surface.concurrency_replay_race_guard_sites_total &&
         surface.concurrency_replay_race_guard_summary
                 .concurrency_replay_sites ==
             surface
                 .concurrency_replay_race_guard_concurrency_replay_sites_total &&
         surface.concurrency_replay_race_guard_summary.replay_proof_sites ==
             surface.concurrency_replay_race_guard_replay_proof_sites_total &&
         surface.concurrency_replay_race_guard_summary.race_guard_sites ==
             surface.concurrency_replay_race_guard_race_guard_sites_total &&
         surface.concurrency_replay_race_guard_summary.task_handoff_sites ==
             surface.concurrency_replay_race_guard_task_handoff_sites_total &&
         surface.concurrency_replay_race_guard_summary.actor_isolation_sites ==
             surface
                 .concurrency_replay_race_guard_actor_isolation_sites_total &&
         surface.concurrency_replay_race_guard_summary
                 .deterministic_schedule_sites ==
             surface
                 .concurrency_replay_race_guard_deterministic_schedule_sites_total &&
         surface.concurrency_replay_race_guard_summary.guard_blocked_sites ==
             surface.concurrency_replay_race_guard_guard_blocked_sites_total &&
         surface.concurrency_replay_race_guard_summary.contract_violation_sites ==
             surface
                 .concurrency_replay_race_guard_contract_violation_sites_total &&
         surface.concurrency_replay_race_guard_summary.concurrency_replay_sites <=
             surface.concurrency_replay_race_guard_summary
                 .concurrency_replay_race_guard_sites &&
         surface.concurrency_replay_race_guard_summary.replay_proof_sites <=
             surface.concurrency_replay_race_guard_summary
                 .concurrency_replay_race_guard_sites &&
         surface.concurrency_replay_race_guard_summary.race_guard_sites <=
             surface.concurrency_replay_race_guard_summary
                 .concurrency_replay_race_guard_sites &&
         surface.concurrency_replay_race_guard_summary.task_handoff_sites <=
             surface.concurrency_replay_race_guard_summary
                 .concurrency_replay_race_guard_sites &&
         surface.concurrency_replay_race_guard_summary.actor_isolation_sites <=
             surface.concurrency_replay_race_guard_summary
                 .concurrency_replay_race_guard_sites &&
         surface.concurrency_replay_race_guard_summary
                 .deterministic_schedule_sites <=
             surface.concurrency_replay_race_guard_summary
                 .concurrency_replay_sites &&
         surface.concurrency_replay_race_guard_summary.guard_blocked_sites <=
             surface.concurrency_replay_race_guard_summary
                 .concurrency_replay_sites &&
         surface.concurrency_replay_race_guard_summary.contract_violation_sites <=
             surface.concurrency_replay_race_guard_summary
                 .concurrency_replay_race_guard_sites &&
         surface.concurrency_replay_race_guard_summary
                 .deterministic_schedule_sites +
                 surface.concurrency_replay_race_guard_summary
                     .guard_blocked_sites ==
             surface.concurrency_replay_race_guard_summary
                 .concurrency_replay_sites &&
         surface.concurrency_replay_race_guard_summary.deterministic &&
         surface.deterministic_concurrency_replay_race_guard_handoff &&
         surface.unsafe_pointer_extension_summary.unsafe_pointer_extension_sites ==
             surface.unsafe_pointer_extension_sites_total &&
         surface.unsafe_pointer_extension_summary.unsafe_keyword_sites ==
             surface.unsafe_pointer_extension_unsafe_keyword_sites_total &&
         surface.unsafe_pointer_extension_summary.pointer_arithmetic_sites ==
             surface.unsafe_pointer_extension_pointer_arithmetic_sites_total &&
         surface.unsafe_pointer_extension_summary.raw_pointer_type_sites ==
             surface.unsafe_pointer_extension_raw_pointer_type_sites_total &&
         surface.unsafe_pointer_extension_summary.unsafe_operation_sites ==
             surface.unsafe_pointer_extension_unsafe_operation_sites_total &&
         surface.unsafe_pointer_extension_summary.normalized_sites ==
             surface.unsafe_pointer_extension_normalized_sites_total &&
         surface.unsafe_pointer_extension_summary.gate_blocked_sites ==
             surface.unsafe_pointer_extension_gate_blocked_sites_total &&
         surface.unsafe_pointer_extension_summary.contract_violation_sites ==
             surface.unsafe_pointer_extension_contract_violation_sites_total &&
         surface.unsafe_pointer_extension_summary.unsafe_keyword_sites <=
             surface.unsafe_pointer_extension_summary.unsafe_pointer_extension_sites &&
         surface.unsafe_pointer_extension_summary.pointer_arithmetic_sites <=
             surface.unsafe_pointer_extension_summary.unsafe_pointer_extension_sites &&
         surface.unsafe_pointer_extension_summary.raw_pointer_type_sites <=
             surface.unsafe_pointer_extension_summary.unsafe_pointer_extension_sites &&
         surface.unsafe_pointer_extension_summary.unsafe_operation_sites <=
             surface.unsafe_pointer_extension_summary.unsafe_pointer_extension_sites &&
         surface.unsafe_pointer_extension_summary.normalized_sites <=
             surface.unsafe_pointer_extension_summary.unsafe_pointer_extension_sites &&
         surface.unsafe_pointer_extension_summary.gate_blocked_sites <=
             surface.unsafe_pointer_extension_summary.unsafe_pointer_extension_sites &&
         surface.unsafe_pointer_extension_summary.contract_violation_sites <=
             surface.unsafe_pointer_extension_summary.unsafe_pointer_extension_sites &&
         surface.unsafe_pointer_extension_summary.normalized_sites +
                 surface.unsafe_pointer_extension_summary.gate_blocked_sites ==
             surface.unsafe_pointer_extension_summary.unsafe_pointer_extension_sites &&
         surface.unsafe_pointer_extension_summary.deterministic &&
         surface.deterministic_unsafe_pointer_extension_handoff &&
         surface.inline_asm_intrinsic_governance_summary.inline_asm_intrinsic_sites ==
             surface.inline_asm_intrinsic_governance_sites_total &&
         surface.inline_asm_intrinsic_governance_summary.inline_asm_sites ==
             surface.inline_asm_intrinsic_governance_inline_asm_sites_total &&
         surface.inline_asm_intrinsic_governance_summary.intrinsic_sites ==
             surface.inline_asm_intrinsic_governance_intrinsic_sites_total &&
         surface.inline_asm_intrinsic_governance_summary.governed_intrinsic_sites ==
             surface
                 .inline_asm_intrinsic_governance_governed_intrinsic_sites_total &&
         surface.inline_asm_intrinsic_governance_summary
                 .privileged_intrinsic_sites ==
             surface
                 .inline_asm_intrinsic_governance_privileged_intrinsic_sites_total &&
         surface.inline_asm_intrinsic_governance_summary.normalized_sites ==
             surface.inline_asm_intrinsic_governance_normalized_sites_total &&
         surface.inline_asm_intrinsic_governance_summary.gate_blocked_sites ==
             surface.inline_asm_intrinsic_governance_gate_blocked_sites_total &&
         surface.inline_asm_intrinsic_governance_summary.contract_violation_sites ==
             surface.inline_asm_intrinsic_governance_contract_violation_sites_total &&
         surface.inline_asm_intrinsic_governance_summary.inline_asm_sites <=
             surface.inline_asm_intrinsic_governance_summary
                 .inline_asm_intrinsic_sites &&
         surface.inline_asm_intrinsic_governance_summary.intrinsic_sites <=
             surface.inline_asm_intrinsic_governance_summary
                 .inline_asm_intrinsic_sites &&
         surface.inline_asm_intrinsic_governance_summary.governed_intrinsic_sites <=
             surface.inline_asm_intrinsic_governance_summary.intrinsic_sites &&
         surface
                 .inline_asm_intrinsic_governance_summary.privileged_intrinsic_sites <=
             surface.inline_asm_intrinsic_governance_summary
                 .governed_intrinsic_sites &&
         surface.inline_asm_intrinsic_governance_summary.normalized_sites <=
             surface.inline_asm_intrinsic_governance_summary
                 .inline_asm_intrinsic_sites &&
         surface.inline_asm_intrinsic_governance_summary.gate_blocked_sites <=
             surface.inline_asm_intrinsic_governance_summary
                 .inline_asm_intrinsic_sites &&
         surface.inline_asm_intrinsic_governance_summary.contract_violation_sites <=
             surface.inline_asm_intrinsic_governance_summary
                 .inline_asm_intrinsic_sites &&
         surface.inline_asm_intrinsic_governance_summary.inline_asm_sites ==
             surface.throws_propagation_summary
                 .cache_invalidation_candidate_sites &&
         surface.inline_asm_intrinsic_governance_summary.intrinsic_sites ==
             surface.unsafe_pointer_extension_summary.unsafe_operation_sites &&
         surface.inline_asm_intrinsic_governance_summary.governed_intrinsic_sites <=
             surface.throws_propagation_summary.normalized_sites &&
         surface
                 .inline_asm_intrinsic_governance_summary.privileged_intrinsic_sites <=
             surface.unsafe_pointer_extension_summary.normalized_sites &&
         surface.inline_asm_intrinsic_governance_summary.normalized_sites >=
             surface.inline_asm_intrinsic_governance_summary.inline_asm_sites &&
         surface.inline_asm_intrinsic_governance_summary.normalized_sites -
                 surface.inline_asm_intrinsic_governance_summary.inline_asm_sites ==
             surface.inline_asm_intrinsic_governance_summary
                 .governed_intrinsic_sites &&
         surface.inline_asm_intrinsic_governance_summary.gate_blocked_sites ==
             surface.inline_asm_intrinsic_governance_summary.intrinsic_sites -
                 surface.inline_asm_intrinsic_governance_summary
                     .governed_intrinsic_sites &&
         surface.inline_asm_intrinsic_governance_summary.normalized_sites +
                 surface.inline_asm_intrinsic_governance_summary.gate_blocked_sites ==
             surface.inline_asm_intrinsic_governance_summary
                 .inline_asm_intrinsic_sites &&
         surface.inline_asm_intrinsic_governance_summary.deterministic &&
         surface.deterministic_inline_asm_intrinsic_governance_handoff &&
         surface.ns_error_bridging_summary.ns_error_bridging_sites ==
             surface.ns_error_bridging_sites_total &&
         surface.ns_error_bridging_summary.ns_error_parameter_sites ==
             surface.ns_error_bridging_ns_error_parameter_sites_total &&
         surface.ns_error_bridging_summary.ns_error_out_parameter_sites ==
             surface.ns_error_bridging_ns_error_out_parameter_sites_total &&
         surface.ns_error_bridging_summary.ns_error_bridge_path_sites ==
             surface.ns_error_bridging_ns_error_bridge_path_sites_total &&
         surface.ns_error_bridging_summary.failable_call_sites ==
             surface.ns_error_bridging_failable_call_sites_total &&
         surface.ns_error_bridging_summary.normalized_sites ==
             surface.ns_error_bridging_normalized_sites_total &&
         surface.ns_error_bridging_summary.bridge_boundary_sites ==
             surface.ns_error_bridging_bridge_boundary_sites_total &&
         surface.ns_error_bridging_summary.contract_violation_sites ==
             surface.ns_error_bridging_contract_violation_sites_total &&
         surface.ns_error_bridging_summary.ns_error_parameter_sites <=
             surface.ns_error_bridging_summary.ns_error_bridging_sites &&
         surface.ns_error_bridging_summary.ns_error_out_parameter_sites <=
             surface.ns_error_bridging_summary.ns_error_bridging_sites &&
         surface.ns_error_bridging_summary.ns_error_bridge_path_sites <=
             surface.ns_error_bridging_summary.ns_error_bridging_sites &&
         surface.ns_error_bridging_summary.failable_call_sites <=
             surface.ns_error_bridging_summary.ns_error_bridging_sites &&
         surface.ns_error_bridging_summary.normalized_sites <=
             surface.ns_error_bridging_summary.ns_error_bridging_sites &&
         surface.ns_error_bridging_summary.bridge_boundary_sites <=
             surface.ns_error_bridging_summary.ns_error_bridging_sites &&
         surface.ns_error_bridging_summary.normalized_sites +
                 surface.ns_error_bridging_summary.bridge_boundary_sites ==
             surface.ns_error_bridging_summary.ns_error_bridging_sites &&
         surface.ns_error_bridging_summary.contract_violation_sites <=
             surface.ns_error_bridging_summary.ns_error_bridging_sites &&
         surface.ns_error_bridging_summary.deterministic &&
         surface.deterministic_ns_error_bridging_handoff &&
         surface.error_diagnostics_recovery_summary
                 .error_diagnostics_recovery_sites ==
             surface.error_diagnostics_recovery_sites_total &&
         surface.error_diagnostics_recovery_summary.diagnostic_emit_sites ==
             surface.error_diagnostics_recovery_diagnostic_emit_sites_total &&
         surface.error_diagnostics_recovery_summary.recovery_anchor_sites ==
             surface.error_diagnostics_recovery_recovery_anchor_sites_total &&
         surface.error_diagnostics_recovery_summary.recovery_boundary_sites ==
             surface.error_diagnostics_recovery_recovery_boundary_sites_total &&
         surface.error_diagnostics_recovery_summary
                 .fail_closed_diagnostic_sites ==
             surface
                 .error_diagnostics_recovery_fail_closed_diagnostic_sites_total &&
         surface.error_diagnostics_recovery_summary.normalized_sites ==
             surface.error_diagnostics_recovery_normalized_sites_total &&
         surface.error_diagnostics_recovery_summary.gate_blocked_sites ==
             surface.error_diagnostics_recovery_gate_blocked_sites_total &&
         surface.error_diagnostics_recovery_summary.contract_violation_sites ==
             surface.error_diagnostics_recovery_contract_violation_sites_total &&
         surface.error_diagnostics_recovery_summary.diagnostic_emit_sites <=
             surface.error_diagnostics_recovery_summary
                 .error_diagnostics_recovery_sites &&
         surface.error_diagnostics_recovery_summary.recovery_anchor_sites <=
             surface.error_diagnostics_recovery_summary
                 .error_diagnostics_recovery_sites &&
         surface.error_diagnostics_recovery_summary.recovery_boundary_sites <=
             surface.error_diagnostics_recovery_summary
                 .error_diagnostics_recovery_sites &&
         surface.error_diagnostics_recovery_summary
                 .fail_closed_diagnostic_sites <=
             surface.error_diagnostics_recovery_summary
                 .error_diagnostics_recovery_sites &&
         surface.error_diagnostics_recovery_summary
                 .fail_closed_diagnostic_sites <=
             surface.error_diagnostics_recovery_summary
                 .diagnostic_emit_sites &&
         surface.error_diagnostics_recovery_summary.normalized_sites <=
             surface.error_diagnostics_recovery_summary
                 .error_diagnostics_recovery_sites &&
         surface.error_diagnostics_recovery_summary.gate_blocked_sites <=
             surface.error_diagnostics_recovery_summary
                 .error_diagnostics_recovery_sites &&
         surface.error_diagnostics_recovery_summary.contract_violation_sites <=
             surface.error_diagnostics_recovery_summary
                 .error_diagnostics_recovery_sites &&
         surface.error_diagnostics_recovery_summary.normalized_sites +
                 surface.error_diagnostics_recovery_summary.gate_blocked_sites ==
             surface.error_diagnostics_recovery_summary
                 .error_diagnostics_recovery_sites &&
         surface.error_diagnostics_recovery_summary.deterministic &&
         surface.deterministic_error_diagnostics_recovery_handoff &&
         surface.result_like_lowering_summary.result_like_sites ==
             surface.result_like_lowering_sites_total &&
         surface.result_like_lowering_summary.result_success_sites ==
             surface.result_like_lowering_result_success_sites_total &&
         surface.result_like_lowering_summary.result_failure_sites ==
             surface.result_like_lowering_result_failure_sites_total &&
         surface.result_like_lowering_summary.result_branch_sites ==
             surface.result_like_lowering_result_branch_sites_total &&
         surface.result_like_lowering_summary.result_payload_sites ==
             surface.result_like_lowering_result_payload_sites_total &&
         surface.result_like_lowering_summary.normalized_sites ==
             surface.result_like_lowering_normalized_sites_total &&
         surface.result_like_lowering_summary.branch_merge_sites ==
             surface.result_like_lowering_branch_merge_sites_total &&
         surface.result_like_lowering_summary.contract_violation_sites ==
             surface.result_like_lowering_contract_violation_sites_total &&
         surface.result_like_lowering_summary.result_success_sites <=
             surface.result_like_lowering_summary.result_like_sites &&
         surface.result_like_lowering_summary.result_failure_sites <=
             surface.result_like_lowering_summary.result_like_sites &&
         surface.result_like_lowering_summary.result_branch_sites <=
             surface.result_like_lowering_summary.result_like_sites &&
         surface.result_like_lowering_summary.result_payload_sites <=
             surface.result_like_lowering_summary.result_like_sites &&
         surface.result_like_lowering_summary.normalized_sites <=
             surface.result_like_lowering_summary.result_like_sites &&
         surface.result_like_lowering_summary.branch_merge_sites <=
             surface.result_like_lowering_summary.result_like_sites &&
         surface.result_like_lowering_summary.contract_violation_sites <=
             surface.result_like_lowering_summary.result_like_sites &&
         surface.result_like_lowering_summary.result_success_sites +
                 surface.result_like_lowering_summary.result_failure_sites ==
             surface.result_like_lowering_summary.normalized_sites &&
         surface.result_like_lowering_summary.normalized_sites +
                 surface.result_like_lowering_summary.branch_merge_sites ==
             surface.result_like_lowering_summary.result_like_sites &&
         surface.result_like_lowering_summary.deterministic &&
         surface.deterministic_result_like_lowering_handoff &&
         surface.unwind_cleanup_summary.unwind_cleanup_sites ==
             surface.unwind_cleanup_sites_total &&
         surface.unwind_cleanup_summary.exceptional_exit_sites ==
             surface.unwind_cleanup_exceptional_exit_sites_total &&
         surface.unwind_cleanup_summary.cleanup_action_sites ==
             surface.unwind_cleanup_action_sites_total &&
         surface.unwind_cleanup_summary.cleanup_scope_sites ==
             surface.unwind_cleanup_scope_sites_total &&
         surface.unwind_cleanup_summary.cleanup_resume_sites ==
             surface.unwind_cleanup_resume_sites_total &&
         surface.unwind_cleanup_summary.normalized_sites ==
             surface.unwind_cleanup_normalized_sites_total &&
         surface.unwind_cleanup_summary.fail_closed_sites ==
             surface.unwind_cleanup_fail_closed_sites_total &&
         surface.unwind_cleanup_summary.contract_violation_sites ==
             surface.unwind_cleanup_contract_violation_sites_total &&
         surface.unwind_cleanup_summary.exceptional_exit_sites <=
             surface.unwind_cleanup_summary.unwind_cleanup_sites &&
         surface.unwind_cleanup_summary.cleanup_action_sites <=
             surface.unwind_cleanup_summary.unwind_cleanup_sites &&
         surface.unwind_cleanup_summary.cleanup_scope_sites <=
             surface.unwind_cleanup_summary.unwind_cleanup_sites &&
         surface.unwind_cleanup_summary.cleanup_resume_sites <=
             surface.unwind_cleanup_summary.unwind_cleanup_sites &&
         surface.unwind_cleanup_summary.normalized_sites <=
             surface.unwind_cleanup_summary.unwind_cleanup_sites &&
         surface.unwind_cleanup_summary.fail_closed_sites <=
             surface.unwind_cleanup_summary.unwind_cleanup_sites &&
         surface.unwind_cleanup_summary.contract_violation_sites <=
             surface.unwind_cleanup_summary.unwind_cleanup_sites &&
         surface.unwind_cleanup_summary.normalized_sites +
                 surface.unwind_cleanup_summary.fail_closed_sites ==
             surface.unwind_cleanup_summary.unwind_cleanup_sites &&
         surface.unwind_cleanup_summary.deterministic &&
         surface.deterministic_unwind_cleanup_handoff &&
         surface.async_continuation_summary.async_continuation_sites ==
             surface.async_continuation_sites_total &&
         surface.async_continuation_summary.async_keyword_sites ==
             surface.async_continuation_async_keyword_sites_total &&
         surface.async_continuation_summary.async_function_sites ==
             surface.async_continuation_async_function_sites_total &&
         surface.async_continuation_summary.continuation_allocation_sites ==
             surface.async_continuation_allocation_sites_total &&
         surface.async_continuation_summary.continuation_resume_sites ==
             surface.async_continuation_resume_sites_total &&
         surface.async_continuation_summary.continuation_suspend_sites ==
             surface.async_continuation_suspend_sites_total &&
         surface.async_continuation_summary.async_state_machine_sites ==
             surface.async_continuation_state_machine_sites_total &&
         surface.async_continuation_summary.normalized_sites ==
             surface.async_continuation_normalized_sites_total &&
         surface.async_continuation_summary.gate_blocked_sites ==
             surface.async_continuation_gate_blocked_sites_total &&
         surface.async_continuation_summary.contract_violation_sites ==
             surface.async_continuation_contract_violation_sites_total &&
         surface.async_continuation_summary.async_keyword_sites <=
             surface.async_continuation_summary.async_continuation_sites &&
         surface.async_continuation_summary.async_function_sites <=
             surface.async_continuation_summary.async_continuation_sites &&
         surface.async_continuation_summary.continuation_allocation_sites <=
             surface.async_continuation_summary.async_continuation_sites &&
         surface.async_continuation_summary.continuation_resume_sites <=
             surface.async_continuation_summary.async_continuation_sites &&
         surface.async_continuation_summary.continuation_suspend_sites <=
             surface.async_continuation_summary.async_continuation_sites &&
         surface.async_continuation_summary.async_state_machine_sites <=
             surface.async_continuation_summary.async_continuation_sites &&
         surface.async_continuation_summary.normalized_sites <=
             surface.async_continuation_summary.async_continuation_sites &&
         surface.async_continuation_summary.gate_blocked_sites <=
             surface.async_continuation_summary.async_continuation_sites &&
         surface.async_continuation_summary.contract_violation_sites <=
             surface.async_continuation_summary.async_continuation_sites &&
         surface.async_continuation_summary.normalized_sites +
                 surface.async_continuation_summary.gate_blocked_sites ==
             surface.async_continuation_summary.async_continuation_sites &&
         surface.async_continuation_summary.deterministic &&
         surface.deterministic_async_continuation_handoff &&
         surface.await_lowering_suspension_state_lowering_summary
                 .await_suspension_sites ==
             surface.await_lowering_suspension_state_lowering_sites_total &&
         surface.await_lowering_suspension_state_lowering_summary
                 .await_keyword_sites ==
             surface
                 .await_lowering_suspension_state_lowering_await_keyword_sites_total &&
         surface.await_lowering_suspension_state_lowering_summary
                 .await_suspension_point_sites ==
             surface.await_lowering_suspension_state_lowering_await_suspension_point_sites_total &&
         surface.await_lowering_suspension_state_lowering_summary
                 .await_resume_sites ==
             surface.await_lowering_suspension_state_lowering_await_resume_sites_total &&
         surface.await_lowering_suspension_state_lowering_summary
                 .await_state_machine_sites ==
             surface.await_lowering_suspension_state_lowering_await_state_machine_sites_total &&
         surface.await_lowering_suspension_state_lowering_summary
                 .await_continuation_sites ==
             surface.await_lowering_suspension_state_lowering_await_continuation_sites_total &&
         surface.await_lowering_suspension_state_lowering_summary
                 .normalized_sites ==
             surface.await_lowering_suspension_state_lowering_normalized_sites_total &&
         surface.await_lowering_suspension_state_lowering_summary
                 .gate_blocked_sites ==
             surface.await_lowering_suspension_state_lowering_gate_blocked_sites_total &&
         surface.await_lowering_suspension_state_lowering_summary
                 .contract_violation_sites ==
             surface.await_lowering_suspension_state_lowering_contract_violation_sites_total &&
         surface.await_lowering_suspension_state_lowering_summary
                 .await_keyword_sites <=
             surface.await_lowering_suspension_state_lowering_summary
                 .await_suspension_sites &&
         surface.await_lowering_suspension_state_lowering_summary
                 .await_suspension_point_sites <=
             surface.await_lowering_suspension_state_lowering_summary
                 .await_suspension_sites &&
         surface.await_lowering_suspension_state_lowering_summary
                 .await_resume_sites <=
             surface.await_lowering_suspension_state_lowering_summary
                 .await_suspension_sites &&
         surface.await_lowering_suspension_state_lowering_summary
                 .await_state_machine_sites <=
             surface.await_lowering_suspension_state_lowering_summary
                 .await_suspension_point_sites &&
         surface.await_lowering_suspension_state_lowering_summary
                 .await_continuation_sites <=
             surface.await_lowering_suspension_state_lowering_summary
                 .await_suspension_point_sites &&
         surface.await_lowering_suspension_state_lowering_summary
                 .normalized_sites <=
             surface.await_lowering_suspension_state_lowering_summary
                 .await_suspension_sites &&
         surface.await_lowering_suspension_state_lowering_summary
                 .gate_blocked_sites <=
             surface.await_lowering_suspension_state_lowering_summary
                 .await_suspension_sites &&
         surface.await_lowering_suspension_state_lowering_summary
                 .contract_violation_sites <=
             surface.await_lowering_suspension_state_lowering_summary
                 .await_suspension_sites &&
         surface.await_lowering_suspension_state_lowering_summary
                 .normalized_sites +
                 surface.await_lowering_suspension_state_lowering_summary
                     .gate_blocked_sites ==
             surface.await_lowering_suspension_state_lowering_summary
                 .await_suspension_sites &&
         surface.await_lowering_suspension_state_lowering_summary.deterministic &&
         surface.deterministic_await_lowering_suspension_state_lowering_handoff &&
         surface.symbol_graph_scope_resolution_summary.global_symbol_nodes ==
             surface.symbol_graph_global_symbol_nodes_total &&
         surface.symbol_graph_scope_resolution_summary.function_symbol_nodes ==
             surface.symbol_graph_function_symbol_nodes_total &&
         surface.symbol_graph_scope_resolution_summary.interface_symbol_nodes ==
             surface.symbol_graph_interface_symbol_nodes_total &&
         surface.symbol_graph_scope_resolution_summary.implementation_symbol_nodes ==
             surface.symbol_graph_implementation_symbol_nodes_total &&
         surface.symbol_graph_scope_resolution_summary.interface_property_symbol_nodes ==
             surface.symbol_graph_interface_property_symbol_nodes_total &&
         surface.symbol_graph_scope_resolution_summary.implementation_property_symbol_nodes ==
             surface.symbol_graph_implementation_property_symbol_nodes_total &&
         surface.symbol_graph_scope_resolution_summary.interface_method_symbol_nodes ==
             surface.symbol_graph_interface_method_symbol_nodes_total &&
         surface.symbol_graph_scope_resolution_summary.implementation_method_symbol_nodes ==
             surface.symbol_graph_implementation_method_symbol_nodes_total &&
         surface.symbol_graph_scope_resolution_summary.top_level_scope_symbols ==
             surface.symbol_graph_top_level_scope_symbols_total &&
         surface.symbol_graph_scope_resolution_summary.nested_scope_symbols ==
             surface.symbol_graph_nested_scope_symbols_total &&
         surface.symbol_graph_scope_resolution_summary.scope_frames_total == surface.symbol_graph_scope_frames_total &&
         surface.symbol_graph_scope_resolution_summary.implementation_interface_resolution_sites ==
             surface.symbol_graph_implementation_interface_resolution_sites_total &&
         surface.symbol_graph_scope_resolution_summary.implementation_interface_resolution_hits ==
             surface.symbol_graph_implementation_interface_resolution_hits_total &&
         surface.symbol_graph_scope_resolution_summary.implementation_interface_resolution_misses ==
             surface.symbol_graph_implementation_interface_resolution_misses_total &&
         surface.symbol_graph_scope_resolution_summary.method_resolution_sites ==
             surface.symbol_graph_method_resolution_sites_total &&
         surface.symbol_graph_scope_resolution_summary.method_resolution_hits ==
             surface.symbol_graph_method_resolution_hits_total &&
         surface.symbol_graph_scope_resolution_summary.method_resolution_misses ==
             surface.symbol_graph_method_resolution_misses_total &&
         surface.symbol_graph_scope_resolution_summary.symbol_nodes_total() ==
             surface.symbol_graph_scope_resolution_summary.top_level_scope_symbols +
                 surface.symbol_graph_scope_resolution_summary.nested_scope_symbols &&
         surface.symbol_graph_scope_resolution_summary.implementation_interface_resolution_hits <=
             surface.symbol_graph_scope_resolution_summary.implementation_interface_resolution_sites &&
         surface.symbol_graph_scope_resolution_summary.implementation_interface_resolution_hits +
                 surface.symbol_graph_scope_resolution_summary.implementation_interface_resolution_misses ==
             surface.symbol_graph_scope_resolution_summary.implementation_interface_resolution_sites &&
         surface.symbol_graph_scope_resolution_summary.method_resolution_hits <=
             surface.symbol_graph_scope_resolution_summary.method_resolution_sites &&
         surface.symbol_graph_scope_resolution_summary.method_resolution_hits +
                 surface.symbol_graph_scope_resolution_summary.method_resolution_misses ==
             surface.symbol_graph_scope_resolution_summary.method_resolution_sites &&
         surface.symbol_graph_scope_resolution_summary.resolution_hits_total() <=
             surface.symbol_graph_scope_resolution_summary.resolution_sites_total() &&
         surface.symbol_graph_scope_resolution_summary.resolution_hits_total() +
                 surface.symbol_graph_scope_resolution_summary.resolution_misses_total() ==
             surface.symbol_graph_scope_resolution_summary.resolution_sites_total() &&
         surface.symbol_graph_scope_resolution_summary.deterministic &&
         surface.deterministic_symbol_graph_scope_resolution_handoff &&
         surface.method_lookup_override_conflict_summary.method_lookup_sites ==
             surface.method_lookup_override_conflict_lookup_sites_total &&
         surface.method_lookup_override_conflict_summary.method_lookup_hits ==
             surface.method_lookup_override_conflict_lookup_hits_total &&
         surface.method_lookup_override_conflict_summary.method_lookup_misses ==
             surface.method_lookup_override_conflict_lookup_misses_total &&
         surface.method_lookup_override_conflict_summary.override_lookup_sites ==
             surface.method_lookup_override_conflict_override_sites_total &&
         surface.method_lookup_override_conflict_summary.override_lookup_hits ==
             surface.method_lookup_override_conflict_override_hits_total &&
         surface.method_lookup_override_conflict_summary.override_lookup_misses ==
             surface.method_lookup_override_conflict_override_misses_total &&
         surface.method_lookup_override_conflict_summary.override_conflicts ==
             surface.method_lookup_override_conflict_override_conflicts_total &&
         surface.method_lookup_override_conflict_summary.unresolved_base_interfaces ==
             surface.method_lookup_override_conflict_unresolved_base_interfaces_total &&
         surface.method_lookup_override_conflict_summary.method_lookup_hits <=
             surface.method_lookup_override_conflict_summary.method_lookup_sites &&
         surface.method_lookup_override_conflict_summary.method_lookup_hits +
                 surface.method_lookup_override_conflict_summary.method_lookup_misses ==
             surface.method_lookup_override_conflict_summary.method_lookup_sites &&
         surface.method_lookup_override_conflict_summary.override_lookup_hits <=
             surface.method_lookup_override_conflict_summary.override_lookup_sites &&
         surface.method_lookup_override_conflict_summary.override_lookup_hits +
                 surface.method_lookup_override_conflict_summary.override_lookup_misses ==
             surface.method_lookup_override_conflict_summary.override_lookup_sites &&
         surface.method_lookup_override_conflict_summary.override_conflicts <=
             surface.method_lookup_override_conflict_summary.override_lookup_hits &&
         surface.method_lookup_override_conflict_summary.deterministic &&
         surface.deterministic_method_lookup_override_conflict_handoff &&
         surface.property_synthesis_ivar_binding_summary.property_synthesis_sites ==
             surface.property_synthesis_ivar_binding_property_synthesis_sites_total &&
         surface.property_synthesis_ivar_binding_summary.property_synthesis_explicit_ivar_bindings ==
             surface.property_synthesis_ivar_binding_explicit_ivar_bindings_total &&
         surface.property_synthesis_ivar_binding_summary.property_synthesis_default_ivar_bindings ==
             surface.property_synthesis_ivar_binding_default_ivar_bindings_total &&
         surface.property_synthesis_ivar_binding_summary.ivar_binding_sites ==
             surface.property_synthesis_ivar_binding_ivar_binding_sites_total &&
         surface.property_synthesis_ivar_binding_summary.ivar_binding_resolved ==
             surface.property_synthesis_ivar_binding_ivar_binding_resolved_total &&
         surface.property_synthesis_ivar_binding_summary.ivar_binding_missing ==
             surface.property_synthesis_ivar_binding_ivar_binding_missing_total &&
         surface.property_synthesis_ivar_binding_summary.ivar_binding_conflicts ==
             surface.property_synthesis_ivar_binding_ivar_binding_conflicts_total &&
         surface.property_synthesis_ivar_binding_summary.property_synthesis_explicit_ivar_bindings +
                 surface.property_synthesis_ivar_binding_summary.property_synthesis_default_ivar_bindings ==
             surface.property_synthesis_ivar_binding_summary.property_synthesis_sites &&
         surface.property_synthesis_ivar_binding_summary.ivar_binding_sites ==
             surface.property_synthesis_ivar_binding_summary.property_synthesis_sites &&
         surface.property_synthesis_ivar_binding_summary.ivar_binding_resolved +
                 surface.property_synthesis_ivar_binding_summary.ivar_binding_missing +
                 surface.property_synthesis_ivar_binding_summary.ivar_binding_conflicts ==
             surface.property_synthesis_ivar_binding_summary.ivar_binding_sites &&
         surface.property_synthesis_ivar_binding_summary.deterministic &&
         surface.deterministic_property_synthesis_ivar_binding_handoff &&
         surface.id_class_sel_object_pointer_type_checking_summary.param_type_sites ==
             surface.id_class_sel_object_pointer_param_type_sites_total &&
         surface.id_class_sel_object_pointer_type_checking_summary.param_id_spelling_sites ==
             surface.id_class_sel_object_pointer_param_id_spelling_sites_total &&
         surface.id_class_sel_object_pointer_type_checking_summary.param_class_spelling_sites ==
             surface.id_class_sel_object_pointer_param_class_spelling_sites_total &&
         surface.id_class_sel_object_pointer_type_checking_summary.param_sel_spelling_sites ==
             surface.id_class_sel_object_pointer_param_sel_spelling_sites_total &&
         surface.id_class_sel_object_pointer_type_checking_summary.param_instancetype_spelling_sites ==
             surface.id_class_sel_object_pointer_param_instancetype_spelling_sites_total &&
         surface.id_class_sel_object_pointer_type_checking_summary.param_object_pointer_type_sites ==
             surface.id_class_sel_object_pointer_param_object_pointer_type_sites_total &&
         surface.id_class_sel_object_pointer_type_checking_summary.return_type_sites ==
             surface.id_class_sel_object_pointer_return_type_sites_total &&
         surface.id_class_sel_object_pointer_type_checking_summary.return_id_spelling_sites ==
             surface.id_class_sel_object_pointer_return_id_spelling_sites_total &&
         surface.id_class_sel_object_pointer_type_checking_summary.return_class_spelling_sites ==
             surface.id_class_sel_object_pointer_return_class_spelling_sites_total &&
         surface.id_class_sel_object_pointer_type_checking_summary.return_sel_spelling_sites ==
             surface.id_class_sel_object_pointer_return_sel_spelling_sites_total &&
         surface.id_class_sel_object_pointer_type_checking_summary.return_instancetype_spelling_sites ==
             surface.id_class_sel_object_pointer_return_instancetype_spelling_sites_total &&
         surface.id_class_sel_object_pointer_type_checking_summary.return_object_pointer_type_sites ==
             surface.id_class_sel_object_pointer_return_object_pointer_type_sites_total &&
         surface.id_class_sel_object_pointer_type_checking_summary.property_type_sites ==
             surface.id_class_sel_object_pointer_property_type_sites_total &&
         surface.id_class_sel_object_pointer_type_checking_summary.property_id_spelling_sites ==
             surface.id_class_sel_object_pointer_property_id_spelling_sites_total &&
         surface.id_class_sel_object_pointer_type_checking_summary.property_class_spelling_sites ==
             surface.id_class_sel_object_pointer_property_class_spelling_sites_total &&
         surface.id_class_sel_object_pointer_type_checking_summary.property_sel_spelling_sites ==
             surface.id_class_sel_object_pointer_property_sel_spelling_sites_total &&
         surface.id_class_sel_object_pointer_type_checking_summary.property_instancetype_spelling_sites ==
             surface.id_class_sel_object_pointer_property_instancetype_spelling_sites_total &&
         surface.id_class_sel_object_pointer_type_checking_summary.property_object_pointer_type_sites ==
             surface.id_class_sel_object_pointer_property_object_pointer_type_sites_total &&
         surface.id_class_sel_object_pointer_type_checking_summary.param_id_spelling_sites +
                 surface.id_class_sel_object_pointer_type_checking_summary.param_class_spelling_sites +
                 surface.id_class_sel_object_pointer_type_checking_summary.param_sel_spelling_sites +
                 surface.id_class_sel_object_pointer_type_checking_summary.param_instancetype_spelling_sites +
                 surface.id_class_sel_object_pointer_type_checking_summary.param_object_pointer_type_sites <=
             surface.id_class_sel_object_pointer_type_checking_summary.param_type_sites &&
         surface.id_class_sel_object_pointer_type_checking_summary.return_id_spelling_sites +
                 surface.id_class_sel_object_pointer_type_checking_summary.return_class_spelling_sites +
                 surface.id_class_sel_object_pointer_type_checking_summary.return_sel_spelling_sites +
                 surface.id_class_sel_object_pointer_type_checking_summary.return_instancetype_spelling_sites +
                 surface.id_class_sel_object_pointer_type_checking_summary.return_object_pointer_type_sites <=
             surface.id_class_sel_object_pointer_type_checking_summary.return_type_sites &&
         surface.id_class_sel_object_pointer_type_checking_summary.property_id_spelling_sites +
                 surface.id_class_sel_object_pointer_type_checking_summary.property_class_spelling_sites +
                 surface.id_class_sel_object_pointer_type_checking_summary.property_sel_spelling_sites +
                 surface.id_class_sel_object_pointer_type_checking_summary.property_instancetype_spelling_sites +
                 surface.id_class_sel_object_pointer_type_checking_summary.property_object_pointer_type_sites <=
             surface.id_class_sel_object_pointer_type_checking_summary.property_type_sites &&
         surface.id_class_sel_object_pointer_type_checking_summary.deterministic &&
         surface.deterministic_id_class_sel_object_pointer_type_checking_handoff &&
         surface.block_literal_capture_semantics_summary.block_literal_sites ==
             surface.block_literal_capture_semantics_sites_total &&
         surface.block_literal_capture_semantics_summary.block_parameter_entries ==
             surface.block_literal_capture_semantics_parameter_entries_total &&
         surface.block_literal_capture_semantics_summary.block_capture_entries ==
             surface.block_literal_capture_semantics_capture_entries_total &&
         surface.block_literal_capture_semantics_summary.block_body_statement_entries ==
             surface.block_literal_capture_semantics_body_statement_entries_total &&
         surface.block_literal_capture_semantics_summary.block_empty_capture_sites ==
             surface.block_literal_capture_semantics_empty_capture_sites_total &&
         surface.block_literal_capture_semantics_summary.block_nondeterministic_capture_sites ==
             surface.block_literal_capture_semantics_nondeterministic_capture_sites_total &&
         surface.block_literal_capture_semantics_summary.block_non_normalized_sites ==
             surface.block_literal_capture_semantics_non_normalized_sites_total &&
         surface.block_literal_capture_semantics_summary.contract_violation_sites ==
             surface.block_literal_capture_semantics_contract_violation_sites_total &&
         surface.block_literal_capture_semantics_summary.block_empty_capture_sites <=
             surface.block_literal_capture_semantics_summary.block_literal_sites &&
         surface.block_literal_capture_semantics_summary.block_nondeterministic_capture_sites <=
             surface.block_literal_capture_semantics_summary.block_literal_sites &&
         surface.block_literal_capture_semantics_summary.block_non_normalized_sites <=
             surface.block_literal_capture_semantics_summary.block_literal_sites &&
         surface.block_literal_capture_semantics_summary.contract_violation_sites <=
             surface.block_literal_capture_semantics_summary.block_literal_sites &&
         surface.block_literal_capture_semantics_summary.deterministic &&
         surface.deterministic_block_literal_capture_semantics_handoff &&
         surface.block_abi_invoke_trampoline_semantics_summary.block_literal_sites ==
             surface.block_abi_invoke_trampoline_sites_total &&
         surface.block_abi_invoke_trampoline_semantics_summary.invoke_argument_slots_total ==
             surface.block_abi_invoke_trampoline_invoke_argument_slots_total &&
         surface.block_abi_invoke_trampoline_semantics_summary.capture_word_count_total ==
             surface.block_abi_invoke_trampoline_capture_word_count_total &&
         surface.block_abi_invoke_trampoline_semantics_summary.parameter_entries_total ==
             surface.block_abi_invoke_trampoline_parameter_entries_total &&
         surface.block_abi_invoke_trampoline_semantics_summary.capture_entries_total ==
             surface.block_abi_invoke_trampoline_capture_entries_total &&
         surface.block_abi_invoke_trampoline_semantics_summary.body_statement_entries_total ==
             surface.block_abi_invoke_trampoline_body_statement_entries_total &&
         surface.block_abi_invoke_trampoline_semantics_summary.descriptor_symbolized_sites ==
             surface.block_abi_invoke_trampoline_descriptor_symbolized_sites_total &&
         surface.block_abi_invoke_trampoline_semantics_summary.invoke_trampoline_symbolized_sites ==
             surface.block_abi_invoke_trampoline_invoke_symbolized_sites_total &&
         surface.block_abi_invoke_trampoline_semantics_summary.missing_invoke_trampoline_sites ==
             surface.block_abi_invoke_trampoline_missing_invoke_sites_total &&
         surface.block_abi_invoke_trampoline_semantics_summary.non_normalized_layout_sites ==
             surface.block_abi_invoke_trampoline_non_normalized_layout_sites_total &&
         surface.block_abi_invoke_trampoline_semantics_summary.contract_violation_sites ==
             surface.block_abi_invoke_trampoline_contract_violation_sites_total &&
         surface.block_abi_invoke_trampoline_semantics_summary.descriptor_symbolized_sites <=
             surface.block_abi_invoke_trampoline_semantics_summary.block_literal_sites &&
         surface.block_abi_invoke_trampoline_semantics_summary.invoke_trampoline_symbolized_sites <=
             surface.block_abi_invoke_trampoline_semantics_summary.block_literal_sites &&
         surface.block_abi_invoke_trampoline_semantics_summary.missing_invoke_trampoline_sites <=
             surface.block_abi_invoke_trampoline_semantics_summary.block_literal_sites &&
         surface.block_abi_invoke_trampoline_semantics_summary.non_normalized_layout_sites <=
             surface.block_abi_invoke_trampoline_semantics_summary.block_literal_sites &&
         surface.block_abi_invoke_trampoline_semantics_summary.contract_violation_sites <=
             surface.block_abi_invoke_trampoline_semantics_summary.block_literal_sites &&
         surface.block_abi_invoke_trampoline_semantics_summary.invoke_trampoline_symbolized_sites +
                 surface.block_abi_invoke_trampoline_semantics_summary.missing_invoke_trampoline_sites ==
             surface.block_abi_invoke_trampoline_semantics_summary.block_literal_sites &&
         surface.block_abi_invoke_trampoline_semantics_summary.invoke_argument_slots_total ==
             surface.block_abi_invoke_trampoline_semantics_summary.parameter_entries_total &&
         surface.block_abi_invoke_trampoline_semantics_summary.capture_word_count_total ==
             surface.block_abi_invoke_trampoline_semantics_summary.capture_entries_total &&
         surface.block_abi_invoke_trampoline_semantics_summary.deterministic &&
         surface.deterministic_block_abi_invoke_trampoline_handoff &&
         surface.block_storage_escape_semantics_summary.block_literal_sites ==
             surface.block_storage_escape_sites_total &&
         surface.block_storage_escape_semantics_summary.mutable_capture_count_total ==
             surface.block_storage_escape_mutable_capture_count_total &&
         surface.block_storage_escape_semantics_summary.byref_slot_count_total ==
             surface.block_storage_escape_byref_slot_count_total &&
         surface.block_storage_escape_semantics_summary.parameter_entries_total ==
             surface.block_storage_escape_parameter_entries_total &&
         surface.block_storage_escape_semantics_summary.capture_entries_total ==
             surface.block_storage_escape_capture_entries_total &&
         surface.block_storage_escape_semantics_summary.body_statement_entries_total ==
             surface.block_storage_escape_body_statement_entries_total &&
         surface.block_storage_escape_semantics_summary.requires_byref_cells_sites ==
             surface.block_storage_escape_requires_byref_cells_sites_total &&
         surface.block_storage_escape_semantics_summary.escape_analysis_enabled_sites ==
             surface.block_storage_escape_escape_analysis_enabled_sites_total &&
         surface.block_storage_escape_semantics_summary.escape_to_heap_sites ==
             surface.block_storage_escape_escape_to_heap_sites_total &&
         surface.block_storage_escape_semantics_summary.escape_profile_normalized_sites ==
             surface.block_storage_escape_escape_profile_normalized_sites_total &&
         surface.block_storage_escape_semantics_summary.byref_layout_symbolized_sites ==
             surface.block_storage_escape_byref_layout_symbolized_sites_total &&
         surface.block_storage_escape_semantics_summary.contract_violation_sites ==
             surface.block_storage_escape_contract_violation_sites_total &&
         surface.block_storage_escape_semantics_summary.requires_byref_cells_sites <=
             surface.block_storage_escape_semantics_summary.block_literal_sites &&
         surface.block_storage_escape_semantics_summary.escape_analysis_enabled_sites <=
             surface.block_storage_escape_semantics_summary.block_literal_sites &&
         surface.block_storage_escape_semantics_summary.escape_to_heap_sites <=
             surface.block_storage_escape_semantics_summary.block_literal_sites &&
         surface.block_storage_escape_semantics_summary.escape_profile_normalized_sites <=
             surface.block_storage_escape_semantics_summary.block_literal_sites &&
         surface.block_storage_escape_semantics_summary.byref_layout_symbolized_sites <=
             surface.block_storage_escape_semantics_summary.block_literal_sites &&
         surface.block_storage_escape_semantics_summary.contract_violation_sites <=
             surface.block_storage_escape_semantics_summary.block_literal_sites &&
         surface.block_storage_escape_semantics_summary.mutable_capture_count_total <=
             surface.block_storage_escape_semantics_summary.capture_entries_total &&
         surface.block_storage_escape_semantics_summary.byref_slot_count_total <=
             surface.block_storage_escape_semantics_summary.mutable_capture_count_total &&
         surface.block_storage_escape_semantics_summary.escape_analysis_enabled_sites ==
             surface.block_storage_escape_semantics_summary.block_literal_sites &&
         surface.block_storage_escape_semantics_summary.deterministic &&
         surface.deterministic_block_storage_escape_handoff &&
         surface.block_copy_dispose_semantics_summary.block_literal_sites ==
             surface.block_copy_dispose_sites_total &&
         surface.block_copy_dispose_semantics_summary.mutable_capture_count_total ==
             surface.block_copy_dispose_mutable_capture_count_total &&
         surface.block_copy_dispose_semantics_summary.byref_slot_count_total ==
             surface.block_copy_dispose_byref_slot_count_total &&
         surface.block_copy_dispose_semantics_summary.parameter_entries_total ==
             surface.block_copy_dispose_parameter_entries_total &&
         surface.block_copy_dispose_semantics_summary.capture_entries_total ==
             surface.block_copy_dispose_capture_entries_total &&
         surface.block_copy_dispose_semantics_summary.body_statement_entries_total ==
             surface.block_copy_dispose_body_statement_entries_total &&
         surface.block_copy_dispose_semantics_summary.copy_helper_required_sites ==
             surface.block_copy_dispose_copy_helper_required_sites_total &&
         surface.block_copy_dispose_semantics_summary.dispose_helper_required_sites ==
             surface.block_copy_dispose_dispose_helper_required_sites_total &&
         surface.block_copy_dispose_semantics_summary.profile_normalized_sites ==
             surface.block_copy_dispose_profile_normalized_sites_total &&
         surface.block_copy_dispose_semantics_summary.copy_helper_symbolized_sites ==
             surface.block_copy_dispose_copy_helper_symbolized_sites_total &&
         surface.block_copy_dispose_semantics_summary.dispose_helper_symbolized_sites ==
             surface.block_copy_dispose_dispose_helper_symbolized_sites_total &&
         surface.block_copy_dispose_semantics_summary.contract_violation_sites ==
             surface.block_copy_dispose_contract_violation_sites_total &&
         surface.block_copy_dispose_semantics_summary.copy_helper_required_sites <=
             surface.block_copy_dispose_semantics_summary.block_literal_sites &&
         surface.block_copy_dispose_semantics_summary.dispose_helper_required_sites <=
             surface.block_copy_dispose_semantics_summary.block_literal_sites &&
         surface.block_copy_dispose_semantics_summary.profile_normalized_sites <=
             surface.block_copy_dispose_semantics_summary.block_literal_sites &&
         surface.block_copy_dispose_semantics_summary.copy_helper_symbolized_sites <=
             surface.block_copy_dispose_semantics_summary.block_literal_sites &&
         surface.block_copy_dispose_semantics_summary.dispose_helper_symbolized_sites <=
             surface.block_copy_dispose_semantics_summary.block_literal_sites &&
         surface.block_copy_dispose_semantics_summary.contract_violation_sites <=
             surface.block_copy_dispose_semantics_summary.block_literal_sites &&
         surface.block_copy_dispose_semantics_summary.mutable_capture_count_total <=
             surface.block_copy_dispose_semantics_summary.capture_entries_total &&
         surface.block_copy_dispose_semantics_summary.byref_slot_count_total <=
             surface.block_copy_dispose_semantics_summary.mutable_capture_count_total &&
         surface.block_copy_dispose_semantics_summary.copy_helper_required_sites <=
             surface.block_copy_dispose_semantics_summary.dispose_helper_required_sites &&
         surface.block_copy_dispose_semantics_summary.deterministic &&
         surface.deterministic_block_copy_dispose_handoff &&
         surface.block_determinism_perf_baseline_summary.block_literal_sites ==
             surface.block_determinism_perf_baseline_sites_total &&
         surface.block_determinism_perf_baseline_summary.baseline_weight_total ==
             surface.block_determinism_perf_baseline_weight_total &&
         surface.block_determinism_perf_baseline_summary.parameter_entries_total ==
             surface.block_determinism_perf_baseline_parameter_entries_total &&
         surface.block_determinism_perf_baseline_summary.capture_entries_total ==
             surface.block_determinism_perf_baseline_capture_entries_total &&
         surface.block_determinism_perf_baseline_summary.body_statement_entries_total ==
             surface.block_determinism_perf_baseline_body_statement_entries_total &&
         surface.block_determinism_perf_baseline_summary.deterministic_capture_sites ==
             surface.block_determinism_perf_baseline_deterministic_capture_sites_total &&
         surface.block_determinism_perf_baseline_summary.heavy_tier_sites ==
             surface.block_determinism_perf_baseline_heavy_tier_sites_total &&
         surface.block_determinism_perf_baseline_summary.normalized_profile_sites ==
             surface.block_determinism_perf_baseline_normalized_profile_sites_total &&
         surface.block_determinism_perf_baseline_summary.contract_violation_sites ==
             surface.block_determinism_perf_baseline_contract_violation_sites_total &&
         surface.block_determinism_perf_baseline_summary.deterministic_capture_sites <=
             surface.block_determinism_perf_baseline_summary.block_literal_sites &&
         surface.block_determinism_perf_baseline_summary.heavy_tier_sites <=
             surface.block_determinism_perf_baseline_summary.block_literal_sites &&
         surface.block_determinism_perf_baseline_summary.normalized_profile_sites <=
             surface.block_determinism_perf_baseline_summary.block_literal_sites &&
         surface.block_determinism_perf_baseline_summary.contract_violation_sites <=
             surface.block_determinism_perf_baseline_summary.block_literal_sites &&
         surface.block_determinism_perf_baseline_summary.deterministic &&
         surface.deterministic_block_determinism_perf_baseline_handoff &&
         surface.message_send_selector_lowering_summary.message_send_sites ==
             surface.message_send_selector_lowering_sites_total &&
         surface.message_send_selector_lowering_summary.unary_form_sites ==
             surface.message_send_selector_lowering_unary_form_sites_total &&
         surface.message_send_selector_lowering_summary.keyword_form_sites ==
             surface.message_send_selector_lowering_keyword_form_sites_total &&
         surface.message_send_selector_lowering_summary.selector_lowering_symbol_sites ==
             surface.message_send_selector_lowering_symbol_sites_total &&
         surface.message_send_selector_lowering_summary.selector_lowering_piece_entries ==
             surface.message_send_selector_lowering_piece_entries_total &&
         surface.message_send_selector_lowering_summary.selector_lowering_argument_piece_entries ==
             surface.message_send_selector_lowering_argument_piece_entries_total &&
         surface.message_send_selector_lowering_summary.selector_lowering_normalized_sites ==
             surface.message_send_selector_lowering_normalized_sites_total &&
         surface.message_send_selector_lowering_summary.selector_lowering_form_mismatch_sites ==
             surface.message_send_selector_lowering_form_mismatch_sites_total &&
         surface.message_send_selector_lowering_summary.selector_lowering_arity_mismatch_sites ==
             surface.message_send_selector_lowering_arity_mismatch_sites_total &&
         surface.message_send_selector_lowering_summary.selector_lowering_symbol_mismatch_sites ==
             surface.message_send_selector_lowering_symbol_mismatch_sites_total &&
         surface.message_send_selector_lowering_summary.selector_lowering_missing_symbol_sites ==
             surface.message_send_selector_lowering_missing_symbol_sites_total &&
         surface.message_send_selector_lowering_summary.selector_lowering_contract_violation_sites ==
             surface.message_send_selector_lowering_contract_violation_sites_total &&
         surface.message_send_selector_lowering_summary.unary_form_sites +
                 surface.message_send_selector_lowering_summary.keyword_form_sites ==
             surface.message_send_selector_lowering_summary.message_send_sites &&
         surface.message_send_selector_lowering_summary.selector_lowering_symbol_sites <=
             surface.message_send_selector_lowering_summary.message_send_sites &&
         surface.message_send_selector_lowering_summary.selector_lowering_piece_entries >=
             surface.message_send_selector_lowering_summary.selector_lowering_argument_piece_entries &&
         surface.message_send_selector_lowering_summary.selector_lowering_normalized_sites <=
             surface.message_send_selector_lowering_summary.selector_lowering_symbol_sites &&
         surface.message_send_selector_lowering_summary.selector_lowering_form_mismatch_sites <=
             surface.message_send_selector_lowering_summary.message_send_sites &&
         surface.message_send_selector_lowering_summary.selector_lowering_arity_mismatch_sites <=
             surface.message_send_selector_lowering_summary.message_send_sites &&
         surface.message_send_selector_lowering_summary.selector_lowering_symbol_mismatch_sites <=
             surface.message_send_selector_lowering_summary.message_send_sites &&
         surface.message_send_selector_lowering_summary.selector_lowering_missing_symbol_sites <=
             surface.message_send_selector_lowering_summary.message_send_sites &&
         surface.message_send_selector_lowering_summary.selector_lowering_contract_violation_sites <=
             surface.message_send_selector_lowering_summary.message_send_sites &&
         surface.message_send_selector_lowering_summary.deterministic &&
         surface.deterministic_message_send_selector_lowering_handoff &&
         surface.dispatch_abi_marshalling_summary.message_send_sites ==
             surface.dispatch_abi_marshalling_sites_total &&
         surface.dispatch_abi_marshalling_summary.receiver_slots ==
             surface.dispatch_abi_marshalling_receiver_slots_total &&
         surface.dispatch_abi_marshalling_summary.selector_symbol_slots ==
             surface.dispatch_abi_marshalling_selector_symbol_slots_total &&
         surface.dispatch_abi_marshalling_summary.argument_slots ==
             surface.dispatch_abi_marshalling_argument_slots_total &&
         surface.dispatch_abi_marshalling_summary.keyword_argument_slots ==
             surface.dispatch_abi_marshalling_keyword_argument_slots_total &&
         surface.dispatch_abi_marshalling_summary.unary_argument_slots ==
             surface.dispatch_abi_marshalling_unary_argument_slots_total &&
         surface.dispatch_abi_marshalling_summary.arity_mismatch_sites ==
             surface.dispatch_abi_marshalling_arity_mismatch_sites_total &&
         surface.dispatch_abi_marshalling_summary.missing_selector_symbol_sites ==
             surface.dispatch_abi_marshalling_missing_selector_symbol_sites_total &&
         surface.dispatch_abi_marshalling_summary.contract_violation_sites ==
             surface.dispatch_abi_marshalling_contract_violation_sites_total &&
         surface.dispatch_abi_marshalling_summary.receiver_slots ==
             surface.dispatch_abi_marshalling_summary.message_send_sites &&
         surface.dispatch_abi_marshalling_summary.selector_symbol_slots +
                 surface.dispatch_abi_marshalling_summary.missing_selector_symbol_sites ==
             surface.dispatch_abi_marshalling_summary.message_send_sites &&
         surface.dispatch_abi_marshalling_summary.keyword_argument_slots +
                 surface.dispatch_abi_marshalling_summary.unary_argument_slots ==
             surface.dispatch_abi_marshalling_summary.argument_slots &&
         surface.dispatch_abi_marshalling_summary.keyword_argument_slots <=
             surface.dispatch_abi_marshalling_summary.argument_slots &&
         surface.dispatch_abi_marshalling_summary.unary_argument_slots <=
             surface.dispatch_abi_marshalling_summary.argument_slots &&
         surface.dispatch_abi_marshalling_summary.selector_symbol_slots <=
             surface.dispatch_abi_marshalling_summary.message_send_sites &&
         surface.dispatch_abi_marshalling_summary.missing_selector_symbol_sites <=
             surface.dispatch_abi_marshalling_summary.message_send_sites &&
         surface.dispatch_abi_marshalling_summary.arity_mismatch_sites <=
             surface.dispatch_abi_marshalling_summary.message_send_sites &&
         surface.dispatch_abi_marshalling_summary.contract_violation_sites <=
             surface.dispatch_abi_marshalling_summary.message_send_sites &&
         surface.dispatch_abi_marshalling_summary.deterministic &&
         surface.deterministic_dispatch_abi_marshalling_handoff &&
         surface.nil_receiver_semantics_foldability_summary.message_send_sites ==
             surface.nil_receiver_semantics_foldability_sites_total &&
         surface.nil_receiver_semantics_foldability_summary.receiver_nil_literal_sites ==
             surface.nil_receiver_semantics_foldability_receiver_nil_literal_sites_total &&
         surface.nil_receiver_semantics_foldability_summary.nil_receiver_semantics_enabled_sites ==
             surface.nil_receiver_semantics_foldability_enabled_sites_total &&
         surface.nil_receiver_semantics_foldability_summary.nil_receiver_foldable_sites ==
             surface.nil_receiver_semantics_foldability_foldable_sites_total &&
         surface.nil_receiver_semantics_foldability_summary.nil_receiver_runtime_dispatch_required_sites ==
             surface.nil_receiver_semantics_foldability_runtime_dispatch_required_sites_total &&
         surface.nil_receiver_semantics_foldability_summary.non_nil_receiver_sites ==
             surface.nil_receiver_semantics_foldability_non_nil_receiver_sites_total &&
         surface.nil_receiver_semantics_foldability_summary.contract_violation_sites ==
             surface.nil_receiver_semantics_foldability_contract_violation_sites_total &&
         surface.nil_receiver_semantics_foldability_summary.receiver_nil_literal_sites ==
             surface.nil_receiver_semantics_foldability_summary.nil_receiver_semantics_enabled_sites &&
         surface.nil_receiver_semantics_foldability_summary.nil_receiver_foldable_sites <=
             surface.nil_receiver_semantics_foldability_summary.nil_receiver_semantics_enabled_sites &&
         surface.nil_receiver_semantics_foldability_summary.nil_receiver_runtime_dispatch_required_sites +
                 surface.nil_receiver_semantics_foldability_summary.nil_receiver_foldable_sites ==
             surface.nil_receiver_semantics_foldability_summary.message_send_sites &&
         surface.nil_receiver_semantics_foldability_summary.nil_receiver_semantics_enabled_sites +
                 surface.nil_receiver_semantics_foldability_summary.non_nil_receiver_sites ==
             surface.nil_receiver_semantics_foldability_summary.message_send_sites &&
         surface.nil_receiver_semantics_foldability_summary.contract_violation_sites <=
             surface.nil_receiver_semantics_foldability_summary.message_send_sites &&
         surface.nil_receiver_semantics_foldability_summary.deterministic &&
         surface.deterministic_nil_receiver_semantics_foldability_handoff &&
         surface.super_dispatch_method_family_summary.message_send_sites ==
             surface.super_dispatch_method_family_sites_total &&
         surface.super_dispatch_method_family_summary.receiver_super_identifier_sites ==
             surface.super_dispatch_method_family_receiver_super_identifier_sites_total &&
         surface.super_dispatch_method_family_summary.super_dispatch_enabled_sites ==
             surface.super_dispatch_method_family_enabled_sites_total &&
         surface.super_dispatch_method_family_summary.super_dispatch_requires_class_context_sites ==
             surface.super_dispatch_method_family_requires_class_context_sites_total &&
         surface.super_dispatch_method_family_summary.method_family_init_sites ==
             surface.super_dispatch_method_family_init_sites_total &&
         surface.super_dispatch_method_family_summary.method_family_copy_sites ==
             surface.super_dispatch_method_family_copy_sites_total &&
         surface.super_dispatch_method_family_summary.method_family_mutable_copy_sites ==
             surface.super_dispatch_method_family_mutable_copy_sites_total &&
         surface.super_dispatch_method_family_summary.method_family_new_sites ==
             surface.super_dispatch_method_family_new_sites_total &&
         surface.super_dispatch_method_family_summary.method_family_none_sites ==
             surface.super_dispatch_method_family_none_sites_total &&
         surface.super_dispatch_method_family_summary.method_family_returns_retained_result_sites ==
             surface.super_dispatch_method_family_returns_retained_result_sites_total &&
         surface.super_dispatch_method_family_summary.method_family_returns_related_result_sites ==
             surface.super_dispatch_method_family_returns_related_result_sites_total &&
         surface.super_dispatch_method_family_summary.contract_violation_sites ==
             surface.super_dispatch_method_family_contract_violation_sites_total &&
         surface.super_dispatch_method_family_summary.receiver_super_identifier_sites ==
             surface.super_dispatch_method_family_summary.super_dispatch_enabled_sites &&
         surface.super_dispatch_method_family_summary.super_dispatch_requires_class_context_sites ==
             surface.super_dispatch_method_family_summary.super_dispatch_enabled_sites &&
         surface.super_dispatch_method_family_summary.method_family_init_sites +
                 surface.super_dispatch_method_family_summary.method_family_copy_sites +
                 surface.super_dispatch_method_family_summary.method_family_mutable_copy_sites +
                 surface.super_dispatch_method_family_summary.method_family_new_sites +
                 surface.super_dispatch_method_family_summary.method_family_none_sites ==
             surface.super_dispatch_method_family_summary.message_send_sites &&
         surface.super_dispatch_method_family_summary.method_family_returns_related_result_sites <=
             surface.super_dispatch_method_family_summary.method_family_init_sites &&
         surface.super_dispatch_method_family_summary.method_family_returns_retained_result_sites <=
             surface.super_dispatch_method_family_summary.message_send_sites &&
         surface.super_dispatch_method_family_summary.contract_violation_sites <=
             surface.super_dispatch_method_family_summary.message_send_sites &&
         surface.super_dispatch_method_family_summary.deterministic &&
         surface.deterministic_super_dispatch_method_family_handoff &&
         surface.runtime_link_host_link_summary.message_send_sites ==
             surface.runtime_link_host_link_message_send_sites_total &&
         surface.runtime_link_host_link_summary.runtime_link_required_sites ==
             surface.runtime_link_host_link_required_sites_total &&
         surface.runtime_link_host_link_summary.runtime_link_elided_sites ==
             surface.runtime_link_host_link_elided_sites_total &&
         surface.runtime_link_host_link_summary.runtime_dispatch_arg_slots ==
             surface.runtime_link_host_link_runtime_dispatch_arg_slots_total &&
         surface.runtime_link_host_link_summary.runtime_dispatch_declaration_parameter_count ==
             surface.runtime_link_host_link_runtime_dispatch_declaration_parameter_count_total &&
         surface.runtime_link_host_link_summary.contract_violation_sites ==
             surface.runtime_link_host_link_contract_violation_sites_total &&
         surface.runtime_link_host_link_summary.runtime_dispatch_symbol ==
             surface.runtime_link_host_link_runtime_dispatch_symbol &&
         surface.runtime_link_host_link_summary.default_runtime_dispatch_symbol_binding ==
             surface.runtime_link_host_link_default_runtime_dispatch_symbol_binding &&
         surface.runtime_link_host_link_summary.runtime_link_required_sites +
                 surface.runtime_link_host_link_summary.runtime_link_elided_sites ==
             surface.runtime_link_host_link_summary.message_send_sites &&
         surface.runtime_link_host_link_summary.contract_violation_sites <=
             surface.runtime_link_host_link_summary.message_send_sites &&
         (surface.runtime_link_host_link_summary.message_send_sites == 0 ||
          surface.runtime_link_host_link_summary.runtime_dispatch_declaration_parameter_count ==
              surface.runtime_link_host_link_summary.runtime_dispatch_arg_slots + 2u) &&
         (surface.runtime_link_host_link_summary.default_runtime_dispatch_symbol_binding ==
          (surface.runtime_link_host_link_summary.runtime_dispatch_symbol ==
           kObjc3RuntimeLinkHostLinkDefaultDispatchSymbol)) &&
         surface.runtime_link_host_link_summary.deterministic &&
         surface.deterministic_runtime_link_host_link_handoff &&
         surface.retain_release_operation_summary.ownership_qualified_sites ==
             surface.retain_release_operation_ownership_qualified_sites_total &&
         surface.retain_release_operation_summary.retain_insertion_sites ==
             surface.retain_release_operation_retain_insertion_sites_total &&
         surface.retain_release_operation_summary.release_insertion_sites ==
             surface.retain_release_operation_release_insertion_sites_total &&
         surface.retain_release_operation_summary.autorelease_insertion_sites ==
             surface.retain_release_operation_autorelease_insertion_sites_total &&
         surface.retain_release_operation_summary.contract_violation_sites ==
             surface.retain_release_operation_contract_violation_sites_total &&
         surface.retain_release_operation_summary.retain_insertion_sites <=
             surface.retain_release_operation_summary.ownership_qualified_sites +
                 surface.retain_release_operation_summary.contract_violation_sites &&
         surface.retain_release_operation_summary.release_insertion_sites <=
             surface.retain_release_operation_summary.ownership_qualified_sites +
                 surface.retain_release_operation_summary.contract_violation_sites &&
         surface.retain_release_operation_summary.autorelease_insertion_sites <=
             surface.retain_release_operation_summary.ownership_qualified_sites +
                 surface.retain_release_operation_summary.contract_violation_sites &&
         surface.retain_release_operation_summary.deterministic &&
         surface.deterministic_retain_release_operation_handoff &&
         surface.weak_unowned_semantics_summary.ownership_candidate_sites ==
             surface.weak_unowned_semantics_ownership_candidate_sites_total &&
         surface.weak_unowned_semantics_summary.weak_reference_sites ==
             surface.weak_unowned_semantics_weak_reference_sites_total &&
         surface.weak_unowned_semantics_summary.unowned_reference_sites ==
             surface.weak_unowned_semantics_unowned_reference_sites_total &&
         surface.weak_unowned_semantics_summary.unowned_safe_reference_sites ==
             surface.weak_unowned_semantics_unowned_safe_reference_sites_total &&
         surface.weak_unowned_semantics_summary.weak_unowned_conflict_sites ==
             surface.weak_unowned_semantics_conflict_sites_total &&
         surface.weak_unowned_semantics_summary.contract_violation_sites ==
             surface.weak_unowned_semantics_contract_violation_sites_total &&
         surface.weak_unowned_semantics_summary.unowned_safe_reference_sites <=
             surface.weak_unowned_semantics_summary.unowned_reference_sites &&
         surface.weak_unowned_semantics_summary.weak_unowned_conflict_sites <=
             surface.weak_unowned_semantics_summary.ownership_candidate_sites &&
         surface.weak_unowned_semantics_summary.contract_violation_sites <=
             surface.weak_unowned_semantics_summary.ownership_candidate_sites +
                 surface.weak_unowned_semantics_summary.weak_unowned_conflict_sites &&
         surface.weak_unowned_semantics_summary.deterministic &&
         surface.deterministic_weak_unowned_semantics_handoff &&
         surface.arc_diagnostics_fixit_summary.ownership_arc_diagnostic_candidate_sites ==
             surface.ownership_arc_diagnostic_candidate_sites_total &&
         surface.arc_diagnostics_fixit_summary.ownership_arc_fixit_available_sites ==
             surface.ownership_arc_fixit_available_sites_total &&
         surface.arc_diagnostics_fixit_summary.ownership_arc_profiled_sites ==
             surface.ownership_arc_profiled_sites_total &&
         surface.arc_diagnostics_fixit_summary.ownership_arc_weak_unowned_conflict_diagnostic_sites ==
             surface.ownership_arc_weak_unowned_conflict_diagnostic_sites_total &&
         surface.arc_diagnostics_fixit_summary.ownership_arc_empty_fixit_hint_sites ==
             surface.ownership_arc_empty_fixit_hint_sites_total &&
         surface.arc_diagnostics_fixit_summary.contract_violation_sites ==
             surface.ownership_arc_contract_violation_sites_total &&
         surface.arc_diagnostics_fixit_summary.ownership_arc_fixit_available_sites <=
             surface.arc_diagnostics_fixit_summary.ownership_arc_diagnostic_candidate_sites +
                 surface.arc_diagnostics_fixit_summary.contract_violation_sites &&
         surface.arc_diagnostics_fixit_summary.ownership_arc_profiled_sites <=
             surface.arc_diagnostics_fixit_summary.ownership_arc_diagnostic_candidate_sites +
                 surface.arc_diagnostics_fixit_summary.contract_violation_sites &&
         surface.arc_diagnostics_fixit_summary.ownership_arc_weak_unowned_conflict_diagnostic_sites <=
             surface.arc_diagnostics_fixit_summary.ownership_arc_diagnostic_candidate_sites +
                 surface.arc_diagnostics_fixit_summary.contract_violation_sites &&
         surface.arc_diagnostics_fixit_summary.ownership_arc_empty_fixit_hint_sites <=
             surface.arc_diagnostics_fixit_summary.ownership_arc_fixit_available_sites +
                 surface.arc_diagnostics_fixit_summary.contract_violation_sites &&
         surface.arc_diagnostics_fixit_summary.deterministic &&
         surface.deterministic_arc_diagnostics_fixit_handoff &&
         surface.autoreleasepool_scope_summary.scope_sites == surface.autoreleasepool_scope_sites_total &&
         surface.autoreleasepool_scope_summary.scope_symbolized_sites ==
             surface.autoreleasepool_scope_symbolized_sites_total &&
         surface.autoreleasepool_scope_summary.contract_violation_sites ==
             surface.autoreleasepool_scope_contract_violation_sites_total &&
         surface.autoreleasepool_scope_summary.max_scope_depth == surface.autoreleasepool_scope_max_depth_total &&
         surface.autoreleasepool_scope_summary.scope_symbolized_sites <=
             surface.autoreleasepool_scope_summary.scope_sites &&
         surface.autoreleasepool_scope_summary.contract_violation_sites <=
             surface.autoreleasepool_scope_summary.scope_sites &&
         (surface.autoreleasepool_scope_summary.scope_sites > 0u ||
          surface.autoreleasepool_scope_summary.max_scope_depth == 0u) &&
         surface.autoreleasepool_scope_summary.max_scope_depth <=
             static_cast<unsigned>(surface.autoreleasepool_scope_summary.scope_sites) &&
         surface.autoreleasepool_scope_summary.deterministic &&
         surface.deterministic_autoreleasepool_scope_handoff;
}

struct Objc3SemaPassManagerResult {
  Objc3ParserContractSnapshot parser_contract_snapshot;
  bool deterministic_parser_sema_handoff = false;
  Objc3ParserSemaConformanceMatrix parser_sema_conformance_matrix;
  bool deterministic_parser_sema_conformance_matrix = false;
  Objc3ParserSemaConformanceCorpus parser_sema_conformance_corpus;
  bool deterministic_parser_sema_conformance_corpus = false;
  Objc3ParserSemaPerformanceQualityGuardrails parser_sema_performance_quality_guardrails;
  bool deterministic_parser_sema_performance_quality_guardrails = false;
  Objc3ParserSemaCrossLaneIntegrationSync parser_sema_cross_lane_integration_sync;
  bool deterministic_parser_sema_cross_lane_integration_sync = false;
