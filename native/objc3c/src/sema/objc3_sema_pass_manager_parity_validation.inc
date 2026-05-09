              .concurrency_replay_race_guard_sites &&
      result.parity_surface.concurrency_replay_race_guard_summary
              .actor_isolation_sites <=
          result.parity_surface.concurrency_replay_race_guard_summary
              .concurrency_replay_race_guard_sites &&
      result.parity_surface.concurrency_replay_race_guard_summary
              .deterministic_schedule_sites <=
          result.parity_surface.concurrency_replay_race_guard_summary
              .concurrency_replay_sites &&
      result.parity_surface.concurrency_replay_race_guard_summary
              .guard_blocked_sites <=
          result.parity_surface.concurrency_replay_race_guard_summary
              .concurrency_replay_sites &&
      result.parity_surface.concurrency_replay_race_guard_summary
              .contract_violation_sites <=
          result.parity_surface.concurrency_replay_race_guard_summary
              .concurrency_replay_race_guard_sites &&
      result.parity_surface.concurrency_replay_race_guard_summary
              .deterministic_schedule_sites +
              result.parity_surface.concurrency_replay_race_guard_summary
                  .guard_blocked_sites ==
          result.parity_surface.concurrency_replay_race_guard_summary
              .concurrency_replay_sites &&
      result.parity_surface.concurrency_replay_race_guard_summary.deterministic;
  result.parity_surface.deterministic_unsafe_pointer_extension_handoff =
      result.deterministic_unsafe_pointer_extension_handoff &&
      result.parity_surface.unsafe_pointer_extension_summary
              .unsafe_pointer_extension_sites ==
          result.parity_surface.unsafe_pointer_extension_sites_total &&
      result.parity_surface.unsafe_pointer_extension_summary.unsafe_keyword_sites ==
          result.parity_surface
              .unsafe_pointer_extension_unsafe_keyword_sites_total &&
      result.parity_surface.unsafe_pointer_extension_summary
              .pointer_arithmetic_sites ==
          result.parity_surface
              .unsafe_pointer_extension_pointer_arithmetic_sites_total &&
      result.parity_surface.unsafe_pointer_extension_summary.raw_pointer_type_sites ==
          result.parity_surface
              .unsafe_pointer_extension_raw_pointer_type_sites_total &&
      result.parity_surface.unsafe_pointer_extension_summary
              .unsafe_operation_sites ==
          result.parity_surface
              .unsafe_pointer_extension_unsafe_operation_sites_total &&
      result.parity_surface.unsafe_pointer_extension_summary.normalized_sites ==
          result.parity_surface.unsafe_pointer_extension_normalized_sites_total &&
      result.parity_surface.unsafe_pointer_extension_summary.gate_blocked_sites ==
          result.parity_surface
              .unsafe_pointer_extension_gate_blocked_sites_total &&
      result.parity_surface.unsafe_pointer_extension_summary
              .contract_violation_sites ==
          result.parity_surface
              .unsafe_pointer_extension_contract_violation_sites_total &&
      result.parity_surface.unsafe_pointer_extension_summary.unsafe_keyword_sites <=
          result.parity_surface.unsafe_pointer_extension_summary
              .unsafe_pointer_extension_sites &&
      result.parity_surface.unsafe_pointer_extension_summary
              .pointer_arithmetic_sites <=
          result.parity_surface.unsafe_pointer_extension_summary
              .unsafe_pointer_extension_sites &&
      result.parity_surface.unsafe_pointer_extension_summary
              .raw_pointer_type_sites <=
          result.parity_surface.unsafe_pointer_extension_summary
              .unsafe_pointer_extension_sites &&
      result.parity_surface.unsafe_pointer_extension_summary
              .unsafe_operation_sites <=
          result.parity_surface.unsafe_pointer_extension_summary
              .unsafe_pointer_extension_sites &&
      result.parity_surface.unsafe_pointer_extension_summary.normalized_sites <=
          result.parity_surface.unsafe_pointer_extension_summary
              .unsafe_pointer_extension_sites &&
      result.parity_surface.unsafe_pointer_extension_summary.gate_blocked_sites <=
          result.parity_surface.unsafe_pointer_extension_summary
              .unsafe_pointer_extension_sites &&
      result.parity_surface.unsafe_pointer_extension_summary
              .contract_violation_sites <=
          result.parity_surface.unsafe_pointer_extension_summary
              .unsafe_pointer_extension_sites &&
      result.parity_surface.unsafe_pointer_extension_summary.normalized_sites +
              result.parity_surface.unsafe_pointer_extension_summary
                  .gate_blocked_sites ==
          result.parity_surface.unsafe_pointer_extension_summary
              .unsafe_pointer_extension_sites &&
      result.parity_surface.unsafe_pointer_extension_summary.deterministic;
  result.parity_surface.deterministic_inline_asm_intrinsic_governance_handoff =
      result.deterministic_inline_asm_intrinsic_governance_handoff &&
      result.parity_surface.inline_asm_intrinsic_governance_summary
              .inline_asm_intrinsic_sites ==
          result.parity_surface.inline_asm_intrinsic_governance_sites_total &&
      result.parity_surface.inline_asm_intrinsic_governance_summary
              .inline_asm_sites ==
          result.parity_surface
              .inline_asm_intrinsic_governance_inline_asm_sites_total &&
      result.parity_surface.inline_asm_intrinsic_governance_summary
              .intrinsic_sites ==
          result.parity_surface
              .inline_asm_intrinsic_governance_intrinsic_sites_total &&
      result.parity_surface.inline_asm_intrinsic_governance_summary
              .governed_intrinsic_sites ==
          result.parity_surface
              .inline_asm_intrinsic_governance_governed_intrinsic_sites_total &&
      result.parity_surface.inline_asm_intrinsic_governance_summary
              .privileged_intrinsic_sites ==
          result.parity_surface
              .inline_asm_intrinsic_governance_privileged_intrinsic_sites_total &&
      result.parity_surface.inline_asm_intrinsic_governance_summary
              .normalized_sites ==
          result.parity_surface
              .inline_asm_intrinsic_governance_normalized_sites_total &&
      result.parity_surface.inline_asm_intrinsic_governance_summary
              .gate_blocked_sites ==
          result.parity_surface
              .inline_asm_intrinsic_governance_gate_blocked_sites_total &&
      result.parity_surface.inline_asm_intrinsic_governance_summary
              .contract_violation_sites ==
          result.parity_surface
              .inline_asm_intrinsic_governance_contract_violation_sites_total &&
      result.parity_surface.inline_asm_intrinsic_governance_summary
              .inline_asm_sites <=
          result.parity_surface.inline_asm_intrinsic_governance_summary
              .inline_asm_intrinsic_sites &&
      result.parity_surface.inline_asm_intrinsic_governance_summary
              .intrinsic_sites <=
          result.parity_surface.inline_asm_intrinsic_governance_summary
              .inline_asm_intrinsic_sites &&
      result.parity_surface.inline_asm_intrinsic_governance_summary
              .governed_intrinsic_sites <=
          result.parity_surface.inline_asm_intrinsic_governance_summary
              .intrinsic_sites &&
      result.parity_surface.inline_asm_intrinsic_governance_summary
              .privileged_intrinsic_sites <=
          result.parity_surface.inline_asm_intrinsic_governance_summary
              .governed_intrinsic_sites &&
      result.parity_surface.inline_asm_intrinsic_governance_summary
              .normalized_sites <=
          result.parity_surface.inline_asm_intrinsic_governance_summary
              .inline_asm_intrinsic_sites &&
      result.parity_surface.inline_asm_intrinsic_governance_summary
              .gate_blocked_sites <=
          result.parity_surface.inline_asm_intrinsic_governance_summary
              .inline_asm_intrinsic_sites &&
      result.parity_surface.inline_asm_intrinsic_governance_summary
              .contract_violation_sites <=
          result.parity_surface.inline_asm_intrinsic_governance_summary
              .inline_asm_intrinsic_sites &&
      result.parity_surface.inline_asm_intrinsic_governance_summary
              .inline_asm_sites ==
          result.parity_surface.throws_propagation_summary
              .cache_invalidation_candidate_sites &&
      result.parity_surface.inline_asm_intrinsic_governance_summary
              .intrinsic_sites ==
          result.parity_surface.unsafe_pointer_extension_summary
              .unsafe_operation_sites &&
      result.parity_surface.inline_asm_intrinsic_governance_summary
              .governed_intrinsic_sites <=
          result.parity_surface.throws_propagation_summary.normalized_sites &&
      result.parity_surface.inline_asm_intrinsic_governance_summary
              .privileged_intrinsic_sites <=
          result.parity_surface.unsafe_pointer_extension_summary
              .normalized_sites &&
      result.parity_surface.inline_asm_intrinsic_governance_summary
              .normalized_sites >=
          result.parity_surface.inline_asm_intrinsic_governance_summary
              .inline_asm_sites &&
      result.parity_surface.inline_asm_intrinsic_governance_summary
              .normalized_sites -
              result.parity_surface
                  .inline_asm_intrinsic_governance_summary
                  .inline_asm_sites ==
          result.parity_surface.inline_asm_intrinsic_governance_summary
              .governed_intrinsic_sites &&
      result.parity_surface.inline_asm_intrinsic_governance_summary
              .gate_blocked_sites ==
          result.parity_surface.inline_asm_intrinsic_governance_summary
                  .intrinsic_sites -
              result.parity_surface
                  .inline_asm_intrinsic_governance_summary
                  .governed_intrinsic_sites &&
      result.parity_surface.inline_asm_intrinsic_governance_summary
              .normalized_sites +
              result.parity_surface.inline_asm_intrinsic_governance_summary
                  .gate_blocked_sites ==
          result.parity_surface.inline_asm_intrinsic_governance_summary
              .inline_asm_intrinsic_sites &&
      result.parity_surface.inline_asm_intrinsic_governance_summary
          .deterministic;
  result.parity_surface.deterministic_ns_error_bridging_handoff =
      result.deterministic_ns_error_bridging_handoff &&
      result.parity_surface.ns_error_bridging_summary.ns_error_bridging_sites ==
          result.parity_surface.ns_error_bridging_sites_total &&
      result.parity_surface.ns_error_bridging_summary.ns_error_parameter_sites ==
          result.parity_surface
              .ns_error_bridging_ns_error_parameter_sites_total &&
      result.parity_surface.ns_error_bridging_summary
              .ns_error_out_parameter_sites ==
          result.parity_surface
              .ns_error_bridging_ns_error_out_parameter_sites_total &&
      result.parity_surface.ns_error_bridging_summary.ns_error_bridge_path_sites ==
          result.parity_surface
              .ns_error_bridging_ns_error_bridge_path_sites_total &&
      result.parity_surface.ns_error_bridging_summary.failable_call_sites ==
          result.parity_surface.ns_error_bridging_failable_call_sites_total &&
      result.parity_surface.ns_error_bridging_summary.normalized_sites ==
          result.parity_surface.ns_error_bridging_normalized_sites_total &&
      result.parity_surface.ns_error_bridging_summary.bridge_boundary_sites ==
          result.parity_surface.ns_error_bridging_bridge_boundary_sites_total &&
      result.parity_surface.ns_error_bridging_summary.contract_violation_sites ==
          result.parity_surface.ns_error_bridging_contract_violation_sites_total &&
      result.parity_surface.ns_error_bridging_summary.ns_error_parameter_sites <=
          result.parity_surface.ns_error_bridging_summary.ns_error_bridging_sites &&
      result.parity_surface.ns_error_bridging_summary
              .ns_error_out_parameter_sites <=
          result.parity_surface.ns_error_bridging_summary.ns_error_bridging_sites &&
      result.parity_surface.ns_error_bridging_summary.ns_error_bridge_path_sites <=
          result.parity_surface.ns_error_bridging_summary.ns_error_bridging_sites &&
      result.parity_surface.ns_error_bridging_summary.failable_call_sites <=
          result.parity_surface.ns_error_bridging_summary.ns_error_bridging_sites &&
      result.parity_surface.ns_error_bridging_summary.normalized_sites <=
          result.parity_surface.ns_error_bridging_summary.ns_error_bridging_sites &&
      result.parity_surface.ns_error_bridging_summary.bridge_boundary_sites <=
          result.parity_surface.ns_error_bridging_summary.ns_error_bridging_sites &&
      result.parity_surface.ns_error_bridging_summary.normalized_sites +
              result.parity_surface.ns_error_bridging_summary.bridge_boundary_sites ==
          result.parity_surface.ns_error_bridging_summary.ns_error_bridging_sites &&
      result.parity_surface.ns_error_bridging_summary.contract_violation_sites <=
          result.parity_surface.ns_error_bridging_summary.ns_error_bridging_sites &&
      result.parity_surface.ns_error_bridging_summary.deterministic;
  result.parity_surface.deterministic_error_diagnostics_recovery_handoff =
      result.deterministic_error_diagnostics_recovery_handoff &&
      result.parity_surface.error_diagnostics_recovery_summary
              .error_diagnostics_recovery_sites ==
          result.parity_surface.error_diagnostics_recovery_sites_total &&
      result.parity_surface.error_diagnostics_recovery_summary
              .diagnostic_emit_sites ==
          result.parity_surface
              .error_diagnostics_recovery_diagnostic_emit_sites_total &&
      result.parity_surface.error_diagnostics_recovery_summary
              .recovery_anchor_sites ==
          result.parity_surface
              .error_diagnostics_recovery_recovery_anchor_sites_total &&
      result.parity_surface.error_diagnostics_recovery_summary
              .recovery_boundary_sites ==
          result.parity_surface
              .error_diagnostics_recovery_recovery_boundary_sites_total &&
      result.parity_surface.error_diagnostics_recovery_summary
              .fail_closed_diagnostic_sites ==
          result.parity_surface
              .error_diagnostics_recovery_fail_closed_diagnostic_sites_total &&
      result.parity_surface.error_diagnostics_recovery_summary
              .normalized_sites ==
          result.parity_surface.error_diagnostics_recovery_normalized_sites_total &&
      result.parity_surface.error_diagnostics_recovery_summary
              .gate_blocked_sites ==
          result.parity_surface.error_diagnostics_recovery_gate_blocked_sites_total &&
      result.parity_surface.error_diagnostics_recovery_summary
              .contract_violation_sites ==
          result.parity_surface
              .error_diagnostics_recovery_contract_violation_sites_total &&
      result.parity_surface.error_diagnostics_recovery_summary
              .diagnostic_emit_sites <=
          result.parity_surface.error_diagnostics_recovery_summary
              .error_diagnostics_recovery_sites &&
      result.parity_surface.error_diagnostics_recovery_summary
              .recovery_anchor_sites <=
          result.parity_surface.error_diagnostics_recovery_summary
              .error_diagnostics_recovery_sites &&
      result.parity_surface.error_diagnostics_recovery_summary
              .recovery_boundary_sites <=
          result.parity_surface.error_diagnostics_recovery_summary
              .error_diagnostics_recovery_sites &&
      result.parity_surface.error_diagnostics_recovery_summary
              .fail_closed_diagnostic_sites <=
          result.parity_surface.error_diagnostics_recovery_summary
              .error_diagnostics_recovery_sites &&
      result.parity_surface.error_diagnostics_recovery_summary
              .fail_closed_diagnostic_sites <=
          result.parity_surface.error_diagnostics_recovery_summary
              .diagnostic_emit_sites &&
      result.parity_surface.error_diagnostics_recovery_summary
              .normalized_sites <=
          result.parity_surface.error_diagnostics_recovery_summary
              .error_diagnostics_recovery_sites &&
      result.parity_surface.error_diagnostics_recovery_summary
              .gate_blocked_sites <=
          result.parity_surface.error_diagnostics_recovery_summary
              .error_diagnostics_recovery_sites &&
      result.parity_surface.error_diagnostics_recovery_summary
              .contract_violation_sites <=
          result.parity_surface.error_diagnostics_recovery_summary
              .error_diagnostics_recovery_sites &&
      result.parity_surface.error_diagnostics_recovery_summary
              .normalized_sites +
              result.parity_surface.error_diagnostics_recovery_summary
                  .gate_blocked_sites ==
          result.parity_surface.error_diagnostics_recovery_summary
              .error_diagnostics_recovery_sites &&
      result.parity_surface.error_diagnostics_recovery_summary.deterministic;
  result.parity_surface.deterministic_result_like_lowering_handoff =
      result.deterministic_result_like_lowering_handoff &&
      result.parity_surface.result_like_lowering_summary.result_like_sites ==
          result.parity_surface.result_like_lowering_sites_total &&
      result.parity_surface.result_like_lowering_summary.result_success_sites ==
          result.parity_surface
              .result_like_lowering_result_success_sites_total &&
      result.parity_surface.result_like_lowering_summary.result_failure_sites ==
          result.parity_surface
              .result_like_lowering_result_failure_sites_total &&
      result.parity_surface.result_like_lowering_summary.result_branch_sites ==
          result.parity_surface
              .result_like_lowering_result_branch_sites_total &&
      result.parity_surface.result_like_lowering_summary.result_payload_sites ==
          result.parity_surface
              .result_like_lowering_result_payload_sites_total &&
      result.parity_surface.result_like_lowering_summary.normalized_sites ==
          result.parity_surface.result_like_lowering_normalized_sites_total &&
      result.parity_surface.result_like_lowering_summary.branch_merge_sites ==
          result.parity_surface.result_like_lowering_branch_merge_sites_total &&
      result.parity_surface.result_like_lowering_summary.contract_violation_sites ==
          result.parity_surface
              .result_like_lowering_contract_violation_sites_total &&
      result.parity_surface.result_like_lowering_summary.result_success_sites <=
          result.parity_surface.result_like_lowering_summary.result_like_sites &&
      result.parity_surface.result_like_lowering_summary.result_failure_sites <=
          result.parity_surface.result_like_lowering_summary.result_like_sites &&
      result.parity_surface.result_like_lowering_summary.result_branch_sites <=
          result.parity_surface.result_like_lowering_summary.result_like_sites &&
      result.parity_surface.result_like_lowering_summary.result_payload_sites <=
          result.parity_surface.result_like_lowering_summary.result_like_sites &&
      result.parity_surface.result_like_lowering_summary.normalized_sites <=
          result.parity_surface.result_like_lowering_summary.result_like_sites &&
      result.parity_surface.result_like_lowering_summary.branch_merge_sites <=
          result.parity_surface.result_like_lowering_summary.result_like_sites &&
      result.parity_surface.result_like_lowering_summary.contract_violation_sites <=
          result.parity_surface.result_like_lowering_summary.result_like_sites &&
      result.parity_surface.result_like_lowering_summary.result_success_sites +
              result.parity_surface.result_like_lowering_summary
                  .result_failure_sites ==
          result.parity_surface.result_like_lowering_summary.normalized_sites &&
      result.parity_surface.result_like_lowering_summary.normalized_sites +
              result.parity_surface.result_like_lowering_summary
                  .branch_merge_sites ==
          result.parity_surface.result_like_lowering_summary.result_like_sites &&
      result.parity_surface.result_like_lowering_summary.deterministic;
  result.parity_surface.deterministic_unwind_cleanup_handoff =
      result.deterministic_unwind_cleanup_handoff &&
      result.parity_surface.unwind_cleanup_summary.unwind_cleanup_sites ==
          result.parity_surface.unwind_cleanup_sites_total &&
      result.parity_surface.unwind_cleanup_summary.exceptional_exit_sites ==
          result.parity_surface.unwind_cleanup_exceptional_exit_sites_total &&
      result.parity_surface.unwind_cleanup_summary.cleanup_action_sites ==
          result.parity_surface.unwind_cleanup_action_sites_total &&
      result.parity_surface.unwind_cleanup_summary.cleanup_scope_sites ==
          result.parity_surface.unwind_cleanup_scope_sites_total &&
      result.parity_surface.unwind_cleanup_summary.cleanup_resume_sites ==
          result.parity_surface.unwind_cleanup_resume_sites_total &&
      result.parity_surface.unwind_cleanup_summary.normalized_sites ==
          result.parity_surface.unwind_cleanup_normalized_sites_total &&
      result.parity_surface.unwind_cleanup_summary.fail_closed_sites ==
          result.parity_surface.unwind_cleanup_fail_closed_sites_total &&
      result.parity_surface.unwind_cleanup_summary.contract_violation_sites ==
          result.parity_surface.unwind_cleanup_contract_violation_sites_total &&
      result.parity_surface.unwind_cleanup_summary.exceptional_exit_sites <=
          result.parity_surface.unwind_cleanup_summary.unwind_cleanup_sites &&
      result.parity_surface.unwind_cleanup_summary.cleanup_action_sites <=
          result.parity_surface.unwind_cleanup_summary.unwind_cleanup_sites &&
      result.parity_surface.unwind_cleanup_summary.cleanup_scope_sites <=
          result.parity_surface.unwind_cleanup_summary.unwind_cleanup_sites &&
      result.parity_surface.unwind_cleanup_summary.cleanup_resume_sites <=
          result.parity_surface.unwind_cleanup_summary.unwind_cleanup_sites &&
      result.parity_surface.unwind_cleanup_summary.normalized_sites <=
          result.parity_surface.unwind_cleanup_summary.unwind_cleanup_sites &&
      result.parity_surface.unwind_cleanup_summary.fail_closed_sites <=
          result.parity_surface.unwind_cleanup_summary.unwind_cleanup_sites &&
      result.parity_surface.unwind_cleanup_summary.contract_violation_sites <=
          result.parity_surface.unwind_cleanup_summary.unwind_cleanup_sites &&
      result.parity_surface.unwind_cleanup_summary.normalized_sites +
              result.parity_surface.unwind_cleanup_summary.fail_closed_sites ==
          result.parity_surface.unwind_cleanup_summary.unwind_cleanup_sites &&
      result.parity_surface.unwind_cleanup_summary.deterministic;
  result.parity_surface.deterministic_async_continuation_handoff =
      result.deterministic_async_continuation_handoff &&
      result.parity_surface.async_continuation_summary.async_continuation_sites ==
          result.parity_surface.async_continuation_sites_total &&
      result.parity_surface.async_continuation_summary.async_keyword_sites ==
          result.parity_surface.async_continuation_async_keyword_sites_total &&
      result.parity_surface.async_continuation_summary.async_function_sites ==
          result.parity_surface.async_continuation_async_function_sites_total &&
      result.parity_surface.async_continuation_summary
              .continuation_allocation_sites ==
          result.parity_surface.async_continuation_allocation_sites_total &&
      result.parity_surface.async_continuation_summary.continuation_resume_sites ==
          result.parity_surface.async_continuation_resume_sites_total &&
      result.parity_surface.async_continuation_summary
              .continuation_suspend_sites ==
          result.parity_surface.async_continuation_suspend_sites_total &&
      result.parity_surface.async_continuation_summary.async_state_machine_sites ==
          result.parity_surface.async_continuation_state_machine_sites_total &&
      result.parity_surface.async_continuation_summary.normalized_sites ==
          result.parity_surface.async_continuation_normalized_sites_total &&
      result.parity_surface.async_continuation_summary.gate_blocked_sites ==
          result.parity_surface.async_continuation_gate_blocked_sites_total &&
      result.parity_surface.async_continuation_summary
              .contract_violation_sites ==
          result.parity_surface.async_continuation_contract_violation_sites_total &&
      result.parity_surface.async_continuation_summary.async_keyword_sites <=
          result.parity_surface.async_continuation_summary
              .async_continuation_sites &&
      result.parity_surface.async_continuation_summary.async_function_sites <=
          result.parity_surface.async_continuation_summary
              .async_continuation_sites &&
      result.parity_surface.async_continuation_summary
              .continuation_allocation_sites <=
          result.parity_surface.async_continuation_summary
              .async_continuation_sites &&
      result.parity_surface.async_continuation_summary
              .continuation_resume_sites <=
          result.parity_surface.async_continuation_summary
              .async_continuation_sites &&
      result.parity_surface.async_continuation_summary
              .continuation_suspend_sites <=
          result.parity_surface.async_continuation_summary
              .async_continuation_sites &&
      result.parity_surface.async_continuation_summary.async_state_machine_sites <=
          result.parity_surface.async_continuation_summary
              .async_continuation_sites &&
      result.parity_surface.async_continuation_summary.normalized_sites <=
          result.parity_surface.async_continuation_summary
              .async_continuation_sites &&
      result.parity_surface.async_continuation_summary.gate_blocked_sites <=
          result.parity_surface.async_continuation_summary
              .async_continuation_sites &&
      result.parity_surface.async_continuation_summary
              .contract_violation_sites <=
          result.parity_surface.async_continuation_summary
              .async_continuation_sites &&
      result.parity_surface.async_continuation_summary.normalized_sites +
              result.parity_surface.async_continuation_summary.gate_blocked_sites ==
          result.parity_surface.async_continuation_summary
              .async_continuation_sites &&
      result.parity_surface.async_continuation_summary.deterministic;
  result.parity_surface
      .deterministic_await_lowering_suspension_state_lowering_handoff =
      result.deterministic_await_lowering_suspension_state_lowering_handoff &&
      result.parity_surface.await_lowering_suspension_state_lowering_summary
              .await_suspension_sites ==
          result.parity_surface
              .await_lowering_suspension_state_lowering_sites_total &&
      result.parity_surface.await_lowering_suspension_state_lowering_summary
              .await_keyword_sites ==
          result.parity_surface
              .await_lowering_suspension_state_lowering_await_keyword_sites_total &&
      result.parity_surface.await_lowering_suspension_state_lowering_summary
              .await_suspension_point_sites ==
          result.parity_surface
              .await_lowering_suspension_state_lowering_await_suspension_point_sites_total &&
      result.parity_surface.await_lowering_suspension_state_lowering_summary
              .await_resume_sites ==
          result.parity_surface
              .await_lowering_suspension_state_lowering_await_resume_sites_total &&
      result.parity_surface.await_lowering_suspension_state_lowering_summary
              .await_state_machine_sites ==
          result.parity_surface
              .await_lowering_suspension_state_lowering_await_state_machine_sites_total &&
      result.parity_surface.await_lowering_suspension_state_lowering_summary
              .await_continuation_sites ==
          result.parity_surface
              .await_lowering_suspension_state_lowering_await_continuation_sites_total &&
      result.parity_surface.await_lowering_suspension_state_lowering_summary
              .normalized_sites ==
          result.parity_surface
              .await_lowering_suspension_state_lowering_normalized_sites_total &&
      result.parity_surface.await_lowering_suspension_state_lowering_summary
              .gate_blocked_sites ==
          result.parity_surface
              .await_lowering_suspension_state_lowering_gate_blocked_sites_total &&
      result.parity_surface.await_lowering_suspension_state_lowering_summary
              .contract_violation_sites ==
          result.parity_surface
              .await_lowering_suspension_state_lowering_contract_violation_sites_total &&
      result.parity_surface.await_lowering_suspension_state_lowering_summary
              .await_keyword_sites <=
          result.parity_surface.await_lowering_suspension_state_lowering_summary
              .await_suspension_sites &&
      result.parity_surface.await_lowering_suspension_state_lowering_summary
              .await_suspension_point_sites <=
          result.parity_surface.await_lowering_suspension_state_lowering_summary
              .await_suspension_sites &&
      result.parity_surface.await_lowering_suspension_state_lowering_summary
              .await_resume_sites <=
          result.parity_surface.await_lowering_suspension_state_lowering_summary
              .await_suspension_sites &&
      result.parity_surface.await_lowering_suspension_state_lowering_summary
              .await_state_machine_sites <=
          result.parity_surface.await_lowering_suspension_state_lowering_summary
              .await_suspension_point_sites &&
      result.parity_surface.await_lowering_suspension_state_lowering_summary
              .await_continuation_sites <=
          result.parity_surface.await_lowering_suspension_state_lowering_summary
              .await_suspension_point_sites &&
      result.parity_surface.await_lowering_suspension_state_lowering_summary
              .normalized_sites <=
          result.parity_surface.await_lowering_suspension_state_lowering_summary
              .await_suspension_sites &&
      result.parity_surface.await_lowering_suspension_state_lowering_summary
              .gate_blocked_sites <=
          result.parity_surface.await_lowering_suspension_state_lowering_summary
              .await_suspension_sites &&
      result.parity_surface.await_lowering_suspension_state_lowering_summary
              .contract_violation_sites <=
          result.parity_surface.await_lowering_suspension_state_lowering_summary
              .await_suspension_sites &&
      result.parity_surface.await_lowering_suspension_state_lowering_summary
              .normalized_sites +
              result.parity_surface.await_lowering_suspension_state_lowering_summary
                  .gate_blocked_sites ==
          result.parity_surface.await_lowering_suspension_state_lowering_summary
              .await_suspension_sites &&
      result.parity_surface.await_lowering_suspension_state_lowering_summary
          .deterministic;
  result.parity_surface.deterministic_symbol_graph_scope_resolution_handoff =
      result.deterministic_symbol_graph_scope_resolution_handoff &&
      result.parity_surface.symbol_graph_scope_resolution_summary.global_symbol_nodes ==
          result.parity_surface.symbol_graph_global_symbol_nodes_total &&
      result.parity_surface.symbol_graph_scope_resolution_summary.function_symbol_nodes ==
          result.parity_surface.symbol_graph_function_symbol_nodes_total &&
      result.parity_surface.symbol_graph_scope_resolution_summary.interface_symbol_nodes ==
          result.parity_surface.symbol_graph_interface_symbol_nodes_total &&
      result.parity_surface.symbol_graph_scope_resolution_summary.implementation_symbol_nodes ==
          result.parity_surface.symbol_graph_implementation_symbol_nodes_total &&
      result.parity_surface.symbol_graph_scope_resolution_summary.interface_property_symbol_nodes ==
          result.parity_surface.symbol_graph_interface_property_symbol_nodes_total &&
      result.parity_surface.symbol_graph_scope_resolution_summary.implementation_property_symbol_nodes ==
          result.parity_surface.symbol_graph_implementation_property_symbol_nodes_total &&
      result.parity_surface.symbol_graph_scope_resolution_summary.interface_method_symbol_nodes ==
          result.parity_surface.symbol_graph_interface_method_symbol_nodes_total &&
      result.parity_surface.symbol_graph_scope_resolution_summary.implementation_method_symbol_nodes ==
          result.parity_surface.symbol_graph_implementation_method_symbol_nodes_total &&
      result.parity_surface.symbol_graph_scope_resolution_summary.top_level_scope_symbols ==
          result.parity_surface.symbol_graph_top_level_scope_symbols_total &&
      result.parity_surface.symbol_graph_scope_resolution_summary.nested_scope_symbols ==
          result.parity_surface.symbol_graph_nested_scope_symbols_total &&
      result.parity_surface.symbol_graph_scope_resolution_summary.scope_frames_total ==
          result.parity_surface.symbol_graph_scope_frames_total &&
      result.parity_surface.symbol_graph_scope_resolution_summary.implementation_interface_resolution_sites ==
          result.parity_surface.symbol_graph_implementation_interface_resolution_sites_total &&
      result.parity_surface.symbol_graph_scope_resolution_summary.implementation_interface_resolution_hits ==
          result.parity_surface.symbol_graph_implementation_interface_resolution_hits_total &&
      result.parity_surface.symbol_graph_scope_resolution_summary.implementation_interface_resolution_misses ==
          result.parity_surface.symbol_graph_implementation_interface_resolution_misses_total &&
      result.parity_surface.symbol_graph_scope_resolution_summary.method_resolution_sites ==
          result.parity_surface.symbol_graph_method_resolution_sites_total &&
      result.parity_surface.symbol_graph_scope_resolution_summary.method_resolution_hits ==
          result.parity_surface.symbol_graph_method_resolution_hits_total &&
      result.parity_surface.symbol_graph_scope_resolution_summary.method_resolution_misses ==
          result.parity_surface.symbol_graph_method_resolution_misses_total &&
      result.parity_surface.symbol_graph_scope_resolution_summary.symbol_nodes_total() ==
          result.parity_surface.symbol_graph_scope_resolution_summary.top_level_scope_symbols +
              result.parity_surface.symbol_graph_scope_resolution_summary.nested_scope_symbols &&
      result.parity_surface.symbol_graph_scope_resolution_summary.implementation_interface_resolution_hits <=
          result.parity_surface.symbol_graph_scope_resolution_summary.implementation_interface_resolution_sites &&
      result.parity_surface.symbol_graph_scope_resolution_summary.implementation_interface_resolution_hits +
              result.parity_surface.symbol_graph_scope_resolution_summary
                  .implementation_interface_resolution_misses ==
          result.parity_surface.symbol_graph_scope_resolution_summary.implementation_interface_resolution_sites &&
      result.parity_surface.symbol_graph_scope_resolution_summary.method_resolution_hits <=
          result.parity_surface.symbol_graph_scope_resolution_summary.method_resolution_sites &&
      result.parity_surface.symbol_graph_scope_resolution_summary.method_resolution_hits +
              result.parity_surface.symbol_graph_scope_resolution_summary.method_resolution_misses ==
          result.parity_surface.symbol_graph_scope_resolution_summary.method_resolution_sites &&
      result.parity_surface.symbol_graph_scope_resolution_summary.resolution_hits_total() <=
          result.parity_surface.symbol_graph_scope_resolution_summary.resolution_sites_total() &&
      result.parity_surface.symbol_graph_scope_resolution_summary.resolution_hits_total() +
              result.parity_surface.symbol_graph_scope_resolution_summary.resolution_misses_total() ==
          result.parity_surface.symbol_graph_scope_resolution_summary.resolution_sites_total() &&
      result.parity_surface.symbol_graph_scope_resolution_summary.deterministic;
  result.parity_surface.deterministic_method_lookup_override_conflict_handoff =
      result.deterministic_method_lookup_override_conflict_handoff &&
      result.parity_surface.method_lookup_override_conflict_summary.method_lookup_sites ==
          result.parity_surface.method_lookup_override_conflict_lookup_sites_total &&
      result.parity_surface.method_lookup_override_conflict_summary.method_lookup_hits ==
          result.parity_surface.method_lookup_override_conflict_lookup_hits_total &&
      result.parity_surface.method_lookup_override_conflict_summary.method_lookup_misses ==
          result.parity_surface.method_lookup_override_conflict_lookup_misses_total &&
      result.parity_surface.method_lookup_override_conflict_summary.override_lookup_sites ==
          result.parity_surface.method_lookup_override_conflict_override_sites_total &&
      result.parity_surface.method_lookup_override_conflict_summary.override_lookup_hits ==
          result.parity_surface.method_lookup_override_conflict_override_hits_total &&
      result.parity_surface.method_lookup_override_conflict_summary.override_lookup_misses ==
          result.parity_surface.method_lookup_override_conflict_override_misses_total &&
      result.parity_surface.method_lookup_override_conflict_summary.override_conflicts ==
          result.parity_surface.method_lookup_override_conflict_override_conflicts_total &&
      result.parity_surface.method_lookup_override_conflict_summary.unresolved_base_interfaces ==
          result.parity_surface.method_lookup_override_conflict_unresolved_base_interfaces_total &&
      result.parity_surface.method_lookup_override_conflict_summary.method_lookup_hits <=
          result.parity_surface.method_lookup_override_conflict_summary.method_lookup_sites &&
      result.parity_surface.method_lookup_override_conflict_summary.method_lookup_hits +
              result.parity_surface.method_lookup_override_conflict_summary.method_lookup_misses ==
          result.parity_surface.method_lookup_override_conflict_summary.method_lookup_sites &&
      result.parity_surface.method_lookup_override_conflict_summary.override_lookup_hits <=
          result.parity_surface.method_lookup_override_conflict_summary.override_lookup_sites &&
      result.parity_surface.method_lookup_override_conflict_summary.override_lookup_hits +
              result.parity_surface.method_lookup_override_conflict_summary.override_lookup_misses ==
          result.parity_surface.method_lookup_override_conflict_summary.override_lookup_sites &&
      result.parity_surface.method_lookup_override_conflict_summary.override_conflicts <=
          result.parity_surface.method_lookup_override_conflict_summary.override_lookup_hits &&
      result.parity_surface.method_lookup_override_conflict_summary.deterministic;
  result.parity_surface.deterministic_property_synthesis_ivar_binding_handoff =
      result.deterministic_property_synthesis_ivar_binding_handoff &&
      result.parity_surface.property_synthesis_ivar_binding_summary.property_synthesis_sites ==
          result.parity_surface.property_synthesis_ivar_binding_property_synthesis_sites_total &&
      result.parity_surface.property_synthesis_ivar_binding_summary.property_synthesis_explicit_ivar_bindings ==
          result.parity_surface.property_synthesis_ivar_binding_explicit_ivar_bindings_total &&
      result.parity_surface.property_synthesis_ivar_binding_summary.property_synthesis_default_ivar_bindings ==
          result.parity_surface.property_synthesis_ivar_binding_default_ivar_bindings_total &&
      result.parity_surface.property_synthesis_ivar_binding_summary.ivar_binding_sites ==
          result.parity_surface.property_synthesis_ivar_binding_ivar_binding_sites_total &&
      result.parity_surface.property_synthesis_ivar_binding_summary.ivar_binding_resolved ==
          result.parity_surface.property_synthesis_ivar_binding_ivar_binding_resolved_total &&
      result.parity_surface.property_synthesis_ivar_binding_summary.ivar_binding_missing ==
          result.parity_surface.property_synthesis_ivar_binding_ivar_binding_missing_total &&
      result.parity_surface.property_synthesis_ivar_binding_summary.ivar_binding_conflicts ==
          result.parity_surface.property_synthesis_ivar_binding_ivar_binding_conflicts_total &&
      result.parity_surface.property_synthesis_ivar_binding_summary.property_synthesis_explicit_ivar_bindings +
              result.parity_surface.property_synthesis_ivar_binding_summary
                  .property_synthesis_default_ivar_bindings ==
          result.parity_surface.property_synthesis_ivar_binding_summary.property_synthesis_sites &&
      result.parity_surface.property_synthesis_ivar_binding_summary.ivar_binding_sites ==
          result.parity_surface.property_synthesis_ivar_binding_summary.property_synthesis_sites &&
      result.parity_surface.property_synthesis_ivar_binding_summary.ivar_binding_resolved +
              result.parity_surface.property_synthesis_ivar_binding_summary.ivar_binding_missing +
              result.parity_surface.property_synthesis_ivar_binding_summary.ivar_binding_conflicts ==
          result.parity_surface.property_synthesis_ivar_binding_summary.ivar_binding_sites &&
      result.parity_surface.property_synthesis_ivar_binding_summary.deterministic;
  result.parity_surface.deterministic_id_class_sel_object_pointer_type_checking_handoff =
      result.deterministic_id_class_sel_object_pointer_type_checking_handoff &&
      result.parity_surface.id_class_sel_object_pointer_type_checking_summary.param_type_sites ==
          result.parity_surface.id_class_sel_object_pointer_param_type_sites_total &&
      result.parity_surface.id_class_sel_object_pointer_type_checking_summary.param_id_spelling_sites ==
          result.parity_surface.id_class_sel_object_pointer_param_id_spelling_sites_total &&
      result.parity_surface.id_class_sel_object_pointer_type_checking_summary.param_class_spelling_sites ==
          result.parity_surface.id_class_sel_object_pointer_param_class_spelling_sites_total &&
      result.parity_surface.id_class_sel_object_pointer_type_checking_summary.param_sel_spelling_sites ==
          result.parity_surface.id_class_sel_object_pointer_param_sel_spelling_sites_total &&
      result.parity_surface.id_class_sel_object_pointer_type_checking_summary.param_instancetype_spelling_sites ==
          result.parity_surface.id_class_sel_object_pointer_param_instancetype_spelling_sites_total &&
      result.parity_surface.id_class_sel_object_pointer_type_checking_summary.param_object_pointer_type_sites ==
          result.parity_surface.id_class_sel_object_pointer_param_object_pointer_type_sites_total &&
      result.parity_surface.id_class_sel_object_pointer_type_checking_summary.return_type_sites ==
          result.parity_surface.id_class_sel_object_pointer_return_type_sites_total &&
      result.parity_surface.id_class_sel_object_pointer_type_checking_summary.return_id_spelling_sites ==
          result.parity_surface.id_class_sel_object_pointer_return_id_spelling_sites_total &&
      result.parity_surface.id_class_sel_object_pointer_type_checking_summary.return_class_spelling_sites ==
          result.parity_surface.id_class_sel_object_pointer_return_class_spelling_sites_total &&
      result.parity_surface.id_class_sel_object_pointer_type_checking_summary.return_sel_spelling_sites ==
          result.parity_surface.id_class_sel_object_pointer_return_sel_spelling_sites_total &&
      result.parity_surface.id_class_sel_object_pointer_type_checking_summary.return_instancetype_spelling_sites ==
          result.parity_surface.id_class_sel_object_pointer_return_instancetype_spelling_sites_total &&
      result.parity_surface.id_class_sel_object_pointer_type_checking_summary.return_object_pointer_type_sites ==
          result.parity_surface.id_class_sel_object_pointer_return_object_pointer_type_sites_total &&
      result.parity_surface.id_class_sel_object_pointer_type_checking_summary.property_type_sites ==
          result.parity_surface.id_class_sel_object_pointer_property_type_sites_total &&
      result.parity_surface.id_class_sel_object_pointer_type_checking_summary.property_id_spelling_sites ==
          result.parity_surface.id_class_sel_object_pointer_property_id_spelling_sites_total &&
      result.parity_surface.id_class_sel_object_pointer_type_checking_summary.property_class_spelling_sites ==
          result.parity_surface.id_class_sel_object_pointer_property_class_spelling_sites_total &&
      result.parity_surface.id_class_sel_object_pointer_type_checking_summary.property_sel_spelling_sites ==
          result.parity_surface.id_class_sel_object_pointer_property_sel_spelling_sites_total &&
      result.parity_surface.id_class_sel_object_pointer_type_checking_summary.property_instancetype_spelling_sites ==
          result.parity_surface.id_class_sel_object_pointer_property_instancetype_spelling_sites_total &&
      result.parity_surface.id_class_sel_object_pointer_type_checking_summary.property_object_pointer_type_sites ==
          result.parity_surface.id_class_sel_object_pointer_property_object_pointer_type_sites_total &&
      result.parity_surface.id_class_sel_object_pointer_type_checking_summary.param_id_spelling_sites +
              result.parity_surface.id_class_sel_object_pointer_type_checking_summary.param_class_spelling_sites +
              result.parity_surface.id_class_sel_object_pointer_type_checking_summary.param_sel_spelling_sites +
              result.parity_surface.id_class_sel_object_pointer_type_checking_summary
                  .param_instancetype_spelling_sites +
              result.parity_surface.id_class_sel_object_pointer_type_checking_summary.param_object_pointer_type_sites <=
          result.parity_surface.id_class_sel_object_pointer_type_checking_summary.param_type_sites &&
      result.parity_surface.id_class_sel_object_pointer_type_checking_summary.return_id_spelling_sites +
              result.parity_surface.id_class_sel_object_pointer_type_checking_summary.return_class_spelling_sites +
              result.parity_surface.id_class_sel_object_pointer_type_checking_summary.return_sel_spelling_sites +
              result.parity_surface.id_class_sel_object_pointer_type_checking_summary
                  .return_instancetype_spelling_sites +
              result.parity_surface.id_class_sel_object_pointer_type_checking_summary
                  .return_object_pointer_type_sites <=
          result.parity_surface.id_class_sel_object_pointer_type_checking_summary.return_type_sites &&
      result.parity_surface.id_class_sel_object_pointer_type_checking_summary.property_id_spelling_sites +
              result.parity_surface.id_class_sel_object_pointer_type_checking_summary
                  .property_class_spelling_sites +
              result.parity_surface.id_class_sel_object_pointer_type_checking_summary.property_sel_spelling_sites +
              result.parity_surface.id_class_sel_object_pointer_type_checking_summary
                  .property_instancetype_spelling_sites +
              result.parity_surface.id_class_sel_object_pointer_type_checking_summary
                  .property_object_pointer_type_sites <=
          result.parity_surface.id_class_sel_object_pointer_type_checking_summary.property_type_sites &&
      result.parity_surface.id_class_sel_object_pointer_type_checking_summary.deterministic;
  result.parity_surface.deterministic_block_literal_capture_semantics_handoff =
      result.deterministic_block_literal_capture_semantics_handoff &&
      result.parity_surface.block_literal_capture_semantics_summary.block_literal_sites ==
          result.parity_surface.block_literal_capture_semantics_sites_total &&
      result.parity_surface.block_literal_capture_semantics_summary.block_parameter_entries ==
          result.parity_surface.block_literal_capture_semantics_parameter_entries_total &&
      result.parity_surface.block_literal_capture_semantics_summary.block_capture_entries ==
          result.parity_surface.block_literal_capture_semantics_capture_entries_total &&
      result.parity_surface.block_literal_capture_semantics_summary.block_body_statement_entries ==
          result.parity_surface.block_literal_capture_semantics_body_statement_entries_total &&
      result.parity_surface.block_literal_capture_semantics_summary.block_empty_capture_sites ==
          result.parity_surface.block_literal_capture_semantics_empty_capture_sites_total &&
      result.parity_surface.block_literal_capture_semantics_summary.block_nondeterministic_capture_sites ==
          result.parity_surface.block_literal_capture_semantics_nondeterministic_capture_sites_total &&
      result.parity_surface.block_literal_capture_semantics_summary.block_non_normalized_sites ==
          result.parity_surface.block_literal_capture_semantics_non_normalized_sites_total &&
      result.parity_surface.block_literal_capture_semantics_summary.contract_violation_sites ==
          result.parity_surface.block_literal_capture_semantics_contract_violation_sites_total &&
      result.parity_surface.block_literal_capture_semantics_summary.block_empty_capture_sites <=
          result.parity_surface.block_literal_capture_semantics_summary.block_literal_sites &&
      result.parity_surface.block_literal_capture_semantics_summary.block_nondeterministic_capture_sites <=
          result.parity_surface.block_literal_capture_semantics_summary.block_literal_sites &&
      result.parity_surface.block_literal_capture_semantics_summary.block_non_normalized_sites <=
          result.parity_surface.block_literal_capture_semantics_summary.block_literal_sites &&
      result.parity_surface.block_literal_capture_semantics_summary.contract_violation_sites <=
          result.parity_surface.block_literal_capture_semantics_summary.block_literal_sites &&
      result.parity_surface.block_literal_capture_semantics_summary.deterministic;
  result.parity_surface.deterministic_block_abi_invoke_trampoline_handoff =
      result.deterministic_block_abi_invoke_trampoline_handoff &&
      result.parity_surface.block_abi_invoke_trampoline_semantics_summary.block_literal_sites ==
          result.parity_surface.block_abi_invoke_trampoline_sites_total &&
      result.parity_surface.block_abi_invoke_trampoline_semantics_summary.invoke_argument_slots_total ==
          result.parity_surface.block_abi_invoke_trampoline_invoke_argument_slots_total &&
      result.parity_surface.block_abi_invoke_trampoline_semantics_summary.capture_word_count_total ==
          result.parity_surface.block_abi_invoke_trampoline_capture_word_count_total &&
      result.parity_surface.block_abi_invoke_trampoline_semantics_summary.parameter_entries_total ==
          result.parity_surface.block_abi_invoke_trampoline_parameter_entries_total &&
      result.parity_surface.block_abi_invoke_trampoline_semantics_summary.capture_entries_total ==
          result.parity_surface.block_abi_invoke_trampoline_capture_entries_total &&
      result.parity_surface.block_abi_invoke_trampoline_semantics_summary.body_statement_entries_total ==
          result.parity_surface.block_abi_invoke_trampoline_body_statement_entries_total &&
      result.parity_surface.block_abi_invoke_trampoline_semantics_summary.descriptor_symbolized_sites ==
          result.parity_surface.block_abi_invoke_trampoline_descriptor_symbolized_sites_total &&
      result.parity_surface.block_abi_invoke_trampoline_semantics_summary.invoke_trampoline_symbolized_sites ==
          result.parity_surface.block_abi_invoke_trampoline_invoke_symbolized_sites_total &&
      result.parity_surface.block_abi_invoke_trampoline_semantics_summary.missing_invoke_trampoline_sites ==
          result.parity_surface.block_abi_invoke_trampoline_missing_invoke_sites_total &&
      result.parity_surface.block_abi_invoke_trampoline_semantics_summary.non_normalized_layout_sites ==
          result.parity_surface.block_abi_invoke_trampoline_non_normalized_layout_sites_total &&
      result.parity_surface.block_abi_invoke_trampoline_semantics_summary.contract_violation_sites ==
          result.parity_surface.block_abi_invoke_trampoline_contract_violation_sites_total &&
      result.parity_surface.block_abi_invoke_trampoline_semantics_summary.descriptor_symbolized_sites <=
          result.parity_surface.block_abi_invoke_trampoline_semantics_summary.block_literal_sites &&
      result.parity_surface.block_abi_invoke_trampoline_semantics_summary.invoke_trampoline_symbolized_sites <=
          result.parity_surface.block_abi_invoke_trampoline_semantics_summary.block_literal_sites &&
      result.parity_surface.block_abi_invoke_trampoline_semantics_summary.missing_invoke_trampoline_sites <=
          result.parity_surface.block_abi_invoke_trampoline_semantics_summary.block_literal_sites &&
      result.parity_surface.block_abi_invoke_trampoline_semantics_summary.non_normalized_layout_sites <=
          result.parity_surface.block_abi_invoke_trampoline_semantics_summary.block_literal_sites &&
      result.parity_surface.block_abi_invoke_trampoline_semantics_summary.contract_violation_sites <=
          result.parity_surface.block_abi_invoke_trampoline_semantics_summary.block_literal_sites &&
      result.parity_surface.block_abi_invoke_trampoline_semantics_summary.invoke_trampoline_symbolized_sites +
              result.parity_surface.block_abi_invoke_trampoline_semantics_summary
                  .missing_invoke_trampoline_sites ==
          result.parity_surface.block_abi_invoke_trampoline_semantics_summary.block_literal_sites &&
      result.parity_surface.block_abi_invoke_trampoline_semantics_summary.invoke_argument_slots_total ==
          result.parity_surface.block_abi_invoke_trampoline_semantics_summary.parameter_entries_total &&
      result.parity_surface.block_abi_invoke_trampoline_semantics_summary.capture_word_count_total ==
          result.parity_surface.block_abi_invoke_trampoline_semantics_summary.capture_entries_total &&
      result.parity_surface.block_abi_invoke_trampoline_semantics_summary.deterministic;
  result.parity_surface.deterministic_block_storage_escape_handoff =
      result.deterministic_block_storage_escape_handoff &&
      result.parity_surface.block_storage_escape_semantics_summary.block_literal_sites ==
          result.parity_surface.block_storage_escape_sites_total &&
      result.parity_surface.block_storage_escape_semantics_summary.mutable_capture_count_total ==
          result.parity_surface.block_storage_escape_mutable_capture_count_total &&
      result.parity_surface.block_storage_escape_semantics_summary.byref_slot_count_total ==
          result.parity_surface.block_storage_escape_byref_slot_count_total &&
      result.parity_surface.block_storage_escape_semantics_summary.parameter_entries_total ==
          result.parity_surface.block_storage_escape_parameter_entries_total &&
      result.parity_surface.block_storage_escape_semantics_summary.capture_entries_total ==
          result.parity_surface.block_storage_escape_capture_entries_total &&
      result.parity_surface.block_storage_escape_semantics_summary.body_statement_entries_total ==
          result.parity_surface.block_storage_escape_body_statement_entries_total &&
      result.parity_surface.block_storage_escape_semantics_summary.requires_byref_cells_sites ==
          result.parity_surface.block_storage_escape_requires_byref_cells_sites_total &&
      result.parity_surface.block_storage_escape_semantics_summary.escape_analysis_enabled_sites ==
          result.parity_surface.block_storage_escape_escape_analysis_enabled_sites_total &&
      result.parity_surface.block_storage_escape_semantics_summary.escape_to_heap_sites ==
          result.parity_surface.block_storage_escape_escape_to_heap_sites_total &&
      result.parity_surface.block_storage_escape_semantics_summary.escape_profile_normalized_sites ==
          result.parity_surface.block_storage_escape_escape_profile_normalized_sites_total &&
      result.parity_surface.block_storage_escape_semantics_summary.byref_layout_symbolized_sites ==
          result.parity_surface.block_storage_escape_byref_layout_symbolized_sites_total &&
      result.parity_surface.block_storage_escape_semantics_summary.contract_violation_sites ==
          result.parity_surface.block_storage_escape_contract_violation_sites_total &&
      result.parity_surface.block_storage_escape_semantics_summary.requires_byref_cells_sites <=
          result.parity_surface.block_storage_escape_semantics_summary.block_literal_sites &&
      result.parity_surface.block_storage_escape_semantics_summary.escape_analysis_enabled_sites <=
          result.parity_surface.block_storage_escape_semantics_summary.block_literal_sites &&
      result.parity_surface.block_storage_escape_semantics_summary.escape_to_heap_sites <=
          result.parity_surface.block_storage_escape_semantics_summary.block_literal_sites &&
      result.parity_surface.block_storage_escape_semantics_summary.escape_profile_normalized_sites <=
          result.parity_surface.block_storage_escape_semantics_summary.block_literal_sites &&
      result.parity_surface.block_storage_escape_semantics_summary.byref_layout_symbolized_sites <=
          result.parity_surface.block_storage_escape_semantics_summary.block_literal_sites &&
      result.parity_surface.block_storage_escape_semantics_summary.contract_violation_sites <=
          result.parity_surface.block_storage_escape_semantics_summary.block_literal_sites &&
      result.parity_surface.block_storage_escape_semantics_summary.mutable_capture_count_total <=
          result.parity_surface.block_storage_escape_semantics_summary.capture_entries_total &&
      result.parity_surface.block_storage_escape_semantics_summary.byref_slot_count_total <=
          result.parity_surface.block_storage_escape_semantics_summary.mutable_capture_count_total &&
      result.parity_surface.block_storage_escape_semantics_summary.escape_analysis_enabled_sites ==
          result.parity_surface.block_storage_escape_semantics_summary.block_literal_sites &&
      result.parity_surface.block_storage_escape_semantics_summary.deterministic;
  result.parity_surface.deterministic_block_copy_dispose_handoff =
      result.deterministic_block_copy_dispose_handoff &&
      result.parity_surface.block_copy_dispose_semantics_summary.block_literal_sites ==
          result.parity_surface.block_copy_dispose_sites_total &&
      result.parity_surface.block_copy_dispose_semantics_summary.mutable_capture_count_total ==
          result.parity_surface.block_copy_dispose_mutable_capture_count_total &&
      result.parity_surface.block_copy_dispose_semantics_summary.byref_slot_count_total ==
          result.parity_surface.block_copy_dispose_byref_slot_count_total &&
      result.parity_surface.block_copy_dispose_semantics_summary.parameter_entries_total ==
          result.parity_surface.block_copy_dispose_parameter_entries_total &&
      result.parity_surface.block_copy_dispose_semantics_summary.capture_entries_total ==
          result.parity_surface.block_copy_dispose_capture_entries_total &&
      result.parity_surface.block_copy_dispose_semantics_summary.body_statement_entries_total ==
          result.parity_surface.block_copy_dispose_body_statement_entries_total &&
      result.parity_surface.block_copy_dispose_semantics_summary.copy_helper_required_sites ==
          result.parity_surface.block_copy_dispose_copy_helper_required_sites_total &&
      result.parity_surface.block_copy_dispose_semantics_summary.dispose_helper_required_sites ==
          result.parity_surface.block_copy_dispose_dispose_helper_required_sites_total &&
      result.parity_surface.block_copy_dispose_semantics_summary.profile_normalized_sites ==
          result.parity_surface.block_copy_dispose_profile_normalized_sites_total &&
      result.parity_surface.block_copy_dispose_semantics_summary.copy_helper_symbolized_sites ==
          result.parity_surface.block_copy_dispose_copy_helper_symbolized_sites_total &&
      result.parity_surface.block_copy_dispose_semantics_summary.dispose_helper_symbolized_sites ==
          result.parity_surface.block_copy_dispose_dispose_helper_symbolized_sites_total &&
      result.parity_surface.block_copy_dispose_semantics_summary.contract_violation_sites ==
          result.parity_surface.block_copy_dispose_contract_violation_sites_total &&
      result.parity_surface.block_copy_dispose_semantics_summary.copy_helper_required_sites <=
          result.parity_surface.block_copy_dispose_semantics_summary.block_literal_sites &&
      result.parity_surface.block_copy_dispose_semantics_summary.dispose_helper_required_sites <=
          result.parity_surface.block_copy_dispose_semantics_summary.block_literal_sites &&
      result.parity_surface.block_copy_dispose_semantics_summary.profile_normalized_sites <=
          result.parity_surface.block_copy_dispose_semantics_summary.block_literal_sites &&
      result.parity_surface.block_copy_dispose_semantics_summary.copy_helper_symbolized_sites <=
          result.parity_surface.block_copy_dispose_semantics_summary.block_literal_sites &&
      result.parity_surface.block_copy_dispose_semantics_summary.dispose_helper_symbolized_sites <=
          result.parity_surface.block_copy_dispose_semantics_summary.block_literal_sites &&
      result.parity_surface.block_copy_dispose_semantics_summary.contract_violation_sites <=
          result.parity_surface.block_copy_dispose_semantics_summary.block_literal_sites &&
      result.parity_surface.block_copy_dispose_semantics_summary.mutable_capture_count_total <=
          result.parity_surface.block_copy_dispose_semantics_summary.capture_entries_total &&
      result.parity_surface.block_copy_dispose_semantics_summary.byref_slot_count_total <=
          result.parity_surface.block_copy_dispose_semantics_summary.mutable_capture_count_total &&
      result.parity_surface.block_copy_dispose_semantics_summary.copy_helper_required_sites <=
          result.parity_surface.block_copy_dispose_semantics_summary.dispose_helper_required_sites &&
      result.parity_surface.block_copy_dispose_semantics_summary.deterministic;
  result.parity_surface.deterministic_block_determinism_perf_baseline_handoff =
      result.deterministic_block_determinism_perf_baseline_handoff &&
      result.parity_surface.block_determinism_perf_baseline_summary.block_literal_sites ==
          result.parity_surface.block_determinism_perf_baseline_sites_total &&
      result.parity_surface.block_determinism_perf_baseline_summary.baseline_weight_total ==
          result.parity_surface.block_determinism_perf_baseline_weight_total &&
      result.parity_surface.block_determinism_perf_baseline_summary.parameter_entries_total ==
          result.parity_surface.block_determinism_perf_baseline_parameter_entries_total &&
      result.parity_surface.block_determinism_perf_baseline_summary.capture_entries_total ==
          result.parity_surface.block_determinism_perf_baseline_capture_entries_total &&
      result.parity_surface.block_determinism_perf_baseline_summary.body_statement_entries_total ==
          result.parity_surface.block_determinism_perf_baseline_body_statement_entries_total &&
      result.parity_surface.block_determinism_perf_baseline_summary.deterministic_capture_sites ==
          result.parity_surface.block_determinism_perf_baseline_deterministic_capture_sites_total &&
      result.parity_surface.block_determinism_perf_baseline_summary.heavy_tier_sites ==
          result.parity_surface.block_determinism_perf_baseline_heavy_tier_sites_total &&
      result.parity_surface.block_determinism_perf_baseline_summary.normalized_profile_sites ==
          result.parity_surface.block_determinism_perf_baseline_normalized_profile_sites_total &&
      result.parity_surface.block_determinism_perf_baseline_summary.contract_violation_sites ==
          result.parity_surface.block_determinism_perf_baseline_contract_violation_sites_total &&
      result.parity_surface.block_determinism_perf_baseline_summary.deterministic_capture_sites <=
          result.parity_surface.block_determinism_perf_baseline_summary.block_literal_sites &&
      result.parity_surface.block_determinism_perf_baseline_summary.heavy_tier_sites <=
          result.parity_surface.block_determinism_perf_baseline_summary.block_literal_sites &&
      result.parity_surface.block_determinism_perf_baseline_summary.normalized_profile_sites <=
          result.parity_surface.block_determinism_perf_baseline_summary.block_literal_sites &&
      result.parity_surface.block_determinism_perf_baseline_summary.contract_violation_sites <=
          result.parity_surface.block_determinism_perf_baseline_summary.block_literal_sites &&
      result.parity_surface.block_determinism_perf_baseline_summary.deterministic;
  result.parity_surface.deterministic_message_send_selector_lowering_handoff =
      result.deterministic_message_send_selector_lowering_handoff &&
      result.parity_surface.message_send_selector_lowering_summary.message_send_sites ==
          result.parity_surface.message_send_selector_lowering_sites_total &&
      result.parity_surface.message_send_selector_lowering_summary.unary_form_sites ==
          result.parity_surface.message_send_selector_lowering_unary_form_sites_total &&
      result.parity_surface.message_send_selector_lowering_summary.keyword_form_sites ==
          result.parity_surface.message_send_selector_lowering_keyword_form_sites_total &&
      result.parity_surface.message_send_selector_lowering_summary.selector_lowering_symbol_sites ==
          result.parity_surface.message_send_selector_lowering_symbol_sites_total &&
      result.parity_surface.message_send_selector_lowering_summary.selector_lowering_piece_entries ==
          result.parity_surface.message_send_selector_lowering_piece_entries_total &&
      result.parity_surface.message_send_selector_lowering_summary.selector_lowering_argument_piece_entries ==
          result.parity_surface.message_send_selector_lowering_argument_piece_entries_total &&
      result.parity_surface.message_send_selector_lowering_summary.selector_lowering_normalized_sites ==
          result.parity_surface.message_send_selector_lowering_normalized_sites_total &&
      result.parity_surface.message_send_selector_lowering_summary.selector_lowering_form_mismatch_sites ==
          result.parity_surface.message_send_selector_lowering_form_mismatch_sites_total &&
      result.parity_surface.message_send_selector_lowering_summary.selector_lowering_arity_mismatch_sites ==
          result.parity_surface.message_send_selector_lowering_arity_mismatch_sites_total &&
      result.parity_surface.message_send_selector_lowering_summary.selector_lowering_symbol_mismatch_sites ==
          result.parity_surface.message_send_selector_lowering_symbol_mismatch_sites_total &&
      result.parity_surface.message_send_selector_lowering_summary.selector_lowering_missing_symbol_sites ==
          result.parity_surface.message_send_selector_lowering_missing_symbol_sites_total &&
      result.parity_surface.message_send_selector_lowering_summary.selector_lowering_contract_violation_sites ==
          result.parity_surface.message_send_selector_lowering_contract_violation_sites_total &&
      result.parity_surface.message_send_selector_lowering_summary.unary_form_sites +
              result.parity_surface.message_send_selector_lowering_summary.keyword_form_sites ==
          result.parity_surface.message_send_selector_lowering_summary.message_send_sites &&
      result.parity_surface.message_send_selector_lowering_summary.selector_lowering_symbol_sites <=
          result.parity_surface.message_send_selector_lowering_summary.message_send_sites &&
      result.parity_surface.message_send_selector_lowering_summary.selector_lowering_argument_piece_entries <=
          result.parity_surface.message_send_selector_lowering_summary.selector_lowering_piece_entries &&
      result.parity_surface.message_send_selector_lowering_summary.selector_lowering_normalized_sites <=
          result.parity_surface.message_send_selector_lowering_summary.selector_lowering_symbol_sites &&
      result.parity_surface.message_send_selector_lowering_summary.selector_lowering_form_mismatch_sites <=
          result.parity_surface.message_send_selector_lowering_summary.message_send_sites &&
      result.parity_surface.message_send_selector_lowering_summary.selector_lowering_arity_mismatch_sites <=
          result.parity_surface.message_send_selector_lowering_summary.message_send_sites &&
      result.parity_surface.message_send_selector_lowering_summary.selector_lowering_symbol_mismatch_sites <=
          result.parity_surface.message_send_selector_lowering_summary.message_send_sites &&
      result.parity_surface.message_send_selector_lowering_summary.selector_lowering_missing_symbol_sites <=
          result.parity_surface.message_send_selector_lowering_summary.message_send_sites &&
      result.parity_surface.message_send_selector_lowering_summary.selector_lowering_contract_violation_sites <=
          result.parity_surface.message_send_selector_lowering_summary.message_send_sites &&
      result.parity_surface.message_send_selector_lowering_summary.deterministic;
  result.parity_surface.deterministic_dispatch_abi_marshalling_handoff =
      result.deterministic_dispatch_abi_marshalling_handoff &&
      result.parity_surface.dispatch_abi_marshalling_summary.message_send_sites ==
          result.parity_surface.dispatch_abi_marshalling_sites_total &&
      result.parity_surface.dispatch_abi_marshalling_summary.receiver_slots ==
          result.parity_surface.dispatch_abi_marshalling_receiver_slots_total &&
      result.parity_surface.dispatch_abi_marshalling_summary.selector_symbol_slots ==
          result.parity_surface.dispatch_abi_marshalling_selector_symbol_slots_total &&
      result.parity_surface.dispatch_abi_marshalling_summary.argument_slots ==
          result.parity_surface.dispatch_abi_marshalling_argument_slots_total &&
      result.parity_surface.dispatch_abi_marshalling_summary.keyword_argument_slots ==
          result.parity_surface.dispatch_abi_marshalling_keyword_argument_slots_total &&
      result.parity_surface.dispatch_abi_marshalling_summary.unary_argument_slots ==
          result.parity_surface.dispatch_abi_marshalling_unary_argument_slots_total &&
      result.parity_surface.dispatch_abi_marshalling_summary.arity_mismatch_sites ==
          result.parity_surface.dispatch_abi_marshalling_arity_mismatch_sites_total &&
      result.parity_surface.dispatch_abi_marshalling_summary.missing_selector_symbol_sites ==
          result.parity_surface.dispatch_abi_marshalling_missing_selector_symbol_sites_total &&
      result.parity_surface.dispatch_abi_marshalling_summary.contract_violation_sites ==
          result.parity_surface.dispatch_abi_marshalling_contract_violation_sites_total &&
      result.parity_surface.dispatch_abi_marshalling_summary.receiver_slots ==
          result.parity_surface.dispatch_abi_marshalling_summary.message_send_sites &&
      result.parity_surface.dispatch_abi_marshalling_summary.selector_symbol_slots +
              result.parity_surface.dispatch_abi_marshalling_summary.missing_selector_symbol_sites ==
          result.parity_surface.dispatch_abi_marshalling_summary.message_send_sites &&
      result.parity_surface.dispatch_abi_marshalling_summary.keyword_argument_slots +
              result.parity_surface.dispatch_abi_marshalling_summary.unary_argument_slots ==
          result.parity_surface.dispatch_abi_marshalling_summary.argument_slots &&
      result.parity_surface.dispatch_abi_marshalling_summary.keyword_argument_slots <=
          result.parity_surface.dispatch_abi_marshalling_summary.argument_slots &&
      result.parity_surface.dispatch_abi_marshalling_summary.unary_argument_slots <=
          result.parity_surface.dispatch_abi_marshalling_summary.argument_slots &&
      result.parity_surface.dispatch_abi_marshalling_summary.selector_symbol_slots <=
          result.parity_surface.dispatch_abi_marshalling_summary.message_send_sites &&
      result.parity_surface.dispatch_abi_marshalling_summary.missing_selector_symbol_sites <=
          result.parity_surface.dispatch_abi_marshalling_summary.message_send_sites &&
      result.parity_surface.dispatch_abi_marshalling_summary.arity_mismatch_sites <=
          result.parity_surface.dispatch_abi_marshalling_summary.message_send_sites &&
      result.parity_surface.dispatch_abi_marshalling_summary.contract_violation_sites <=
          result.parity_surface.dispatch_abi_marshalling_summary.message_send_sites &&
      result.parity_surface.dispatch_abi_marshalling_summary.deterministic;
  result.parity_surface.deterministic_nil_receiver_semantics_foldability_handoff =
      result.deterministic_nil_receiver_semantics_foldability_handoff &&
      result.parity_surface.nil_receiver_semantics_foldability_summary.message_send_sites ==
          result.parity_surface.nil_receiver_semantics_foldability_sites_total &&
      result.parity_surface.nil_receiver_semantics_foldability_summary.receiver_nil_literal_sites ==
          result.parity_surface.nil_receiver_semantics_foldability_receiver_nil_literal_sites_total &&
      result.parity_surface.nil_receiver_semantics_foldability_summary.nil_receiver_semantics_enabled_sites ==
          result.parity_surface.nil_receiver_semantics_foldability_enabled_sites_total &&
      result.parity_surface.nil_receiver_semantics_foldability_summary.nil_receiver_foldable_sites ==
          result.parity_surface.nil_receiver_semantics_foldability_foldable_sites_total &&
      result.parity_surface.nil_receiver_semantics_foldability_summary.nil_receiver_runtime_dispatch_required_sites ==
          result.parity_surface.nil_receiver_semantics_foldability_runtime_dispatch_required_sites_total &&
      result.parity_surface.nil_receiver_semantics_foldability_summary.non_nil_receiver_sites ==
          result.parity_surface.nil_receiver_semantics_foldability_non_nil_receiver_sites_total &&
      result.parity_surface.nil_receiver_semantics_foldability_summary.contract_violation_sites ==
          result.parity_surface.nil_receiver_semantics_foldability_contract_violation_sites_total &&
      result.parity_surface.nil_receiver_semantics_foldability_summary.receiver_nil_literal_sites ==
          result.parity_surface.nil_receiver_semantics_foldability_summary.nil_receiver_semantics_enabled_sites &&
      result.parity_surface.nil_receiver_semantics_foldability_summary.nil_receiver_foldable_sites <=
          result.parity_surface.nil_receiver_semantics_foldability_summary.nil_receiver_semantics_enabled_sites &&
      result.parity_surface.nil_receiver_semantics_foldability_summary.nil_receiver_runtime_dispatch_required_sites +
              result.parity_surface.nil_receiver_semantics_foldability_summary.nil_receiver_foldable_sites ==
          result.parity_surface.nil_receiver_semantics_foldability_summary.message_send_sites &&
      result.parity_surface.nil_receiver_semantics_foldability_summary.nil_receiver_semantics_enabled_sites +
              result.parity_surface.nil_receiver_semantics_foldability_summary.non_nil_receiver_sites ==
          result.parity_surface.nil_receiver_semantics_foldability_summary.message_send_sites &&
      result.parity_surface.nil_receiver_semantics_foldability_summary.contract_violation_sites <=
          result.parity_surface.nil_receiver_semantics_foldability_summary.message_send_sites &&
      result.parity_surface.nil_receiver_semantics_foldability_summary.deterministic;
  result.parity_surface.deterministic_super_dispatch_method_family_handoff =
      result.deterministic_super_dispatch_method_family_handoff &&
      result.parity_surface.super_dispatch_method_family_summary.message_send_sites ==
          result.parity_surface.super_dispatch_method_family_sites_total &&
      result.parity_surface.super_dispatch_method_family_summary.receiver_super_identifier_sites ==
          result.parity_surface.super_dispatch_method_family_receiver_super_identifier_sites_total &&
      result.parity_surface.super_dispatch_method_family_summary.super_dispatch_enabled_sites ==
          result.parity_surface.super_dispatch_method_family_enabled_sites_total &&
      result.parity_surface.super_dispatch_method_family_summary.super_dispatch_requires_class_context_sites ==
          result.parity_surface.super_dispatch_method_family_requires_class_context_sites_total &&
      result.parity_surface.super_dispatch_method_family_summary.method_family_init_sites ==
          result.parity_surface.super_dispatch_method_family_init_sites_total &&
      result.parity_surface.super_dispatch_method_family_summary.method_family_copy_sites ==
          result.parity_surface.super_dispatch_method_family_copy_sites_total &&
      result.parity_surface.super_dispatch_method_family_summary.method_family_mutable_copy_sites ==
          result.parity_surface.super_dispatch_method_family_mutable_copy_sites_total &&
      result.parity_surface.super_dispatch_method_family_summary.method_family_new_sites ==
          result.parity_surface.super_dispatch_method_family_new_sites_total &&
      result.parity_surface.super_dispatch_method_family_summary.method_family_none_sites ==
          result.parity_surface.super_dispatch_method_family_none_sites_total &&
      result.parity_surface.super_dispatch_method_family_summary.method_family_returns_retained_result_sites ==
          result.parity_surface.super_dispatch_method_family_returns_retained_result_sites_total &&
      result.parity_surface.super_dispatch_method_family_summary.method_family_returns_related_result_sites ==
          result.parity_surface.super_dispatch_method_family_returns_related_result_sites_total &&
      result.parity_surface.super_dispatch_method_family_summary.contract_violation_sites ==
          result.parity_surface.super_dispatch_method_family_contract_violation_sites_total &&
      result.parity_surface.super_dispatch_method_family_summary.receiver_super_identifier_sites ==
          result.parity_surface.super_dispatch_method_family_summary.super_dispatch_enabled_sites &&
      result.parity_surface.super_dispatch_method_family_summary.super_dispatch_requires_class_context_sites ==
          result.parity_surface.super_dispatch_method_family_summary.super_dispatch_enabled_sites &&
      result.parity_surface.super_dispatch_method_family_summary.method_family_init_sites +
              result.parity_surface.super_dispatch_method_family_summary.method_family_copy_sites +
              result.parity_surface.super_dispatch_method_family_summary.method_family_mutable_copy_sites +
              result.parity_surface.super_dispatch_method_family_summary.method_family_new_sites +
              result.parity_surface.super_dispatch_method_family_summary.method_family_none_sites ==
          result.parity_surface.super_dispatch_method_family_summary.message_send_sites &&
      result.parity_surface.super_dispatch_method_family_summary.method_family_returns_related_result_sites <=
          result.parity_surface.super_dispatch_method_family_summary.method_family_init_sites &&
      result.parity_surface.super_dispatch_method_family_summary.method_family_returns_retained_result_sites <=
          result.parity_surface.super_dispatch_method_family_summary.message_send_sites &&
      result.parity_surface.super_dispatch_method_family_summary.contract_violation_sites <=
          result.parity_surface.super_dispatch_method_family_summary.message_send_sites &&
      result.parity_surface.super_dispatch_method_family_summary.deterministic;
  result.parity_surface.deterministic_runtime_link_host_link_handoff =
      result.deterministic_runtime_link_host_link_handoff &&
      result.parity_surface.runtime_link_host_link_summary.message_send_sites ==
          result.parity_surface.runtime_link_host_link_message_send_sites_total &&
      result.parity_surface.runtime_link_host_link_summary.runtime_link_required_sites ==
          result.parity_surface.runtime_link_host_link_required_sites_total &&
      result.parity_surface.runtime_link_host_link_summary.runtime_link_elided_sites ==
          result.parity_surface.runtime_link_host_link_elided_sites_total &&
      result.parity_surface.runtime_link_host_link_summary.runtime_dispatch_arg_slots ==
          result.parity_surface.runtime_link_host_link_runtime_dispatch_arg_slots_total &&
      result.parity_surface.runtime_link_host_link_summary.runtime_dispatch_declaration_parameter_count ==
          result.parity_surface.runtime_link_host_link_runtime_dispatch_declaration_parameter_count_total &&
      result.parity_surface.runtime_link_host_link_summary.contract_violation_sites ==
          result.parity_surface.runtime_link_host_link_contract_violation_sites_total &&
      result.parity_surface.runtime_link_host_link_summary.runtime_dispatch_symbol ==
          result.parity_surface.runtime_link_host_link_runtime_dispatch_symbol &&
      result.parity_surface.runtime_link_host_link_summary.default_runtime_dispatch_symbol_binding ==
          result.parity_surface.runtime_link_host_link_default_runtime_dispatch_symbol_binding &&
      result.parity_surface.runtime_link_host_link_summary.runtime_link_required_sites +
              result.parity_surface.runtime_link_host_link_summary.runtime_link_elided_sites ==
          result.parity_surface.runtime_link_host_link_summary.message_send_sites &&
      result.parity_surface.runtime_link_host_link_summary.contract_violation_sites <=
          result.parity_surface.runtime_link_host_link_summary.message_send_sites &&
      (result.parity_surface.runtime_link_host_link_summary.message_send_sites == 0 ||
       result.parity_surface.runtime_link_host_link_summary.runtime_dispatch_declaration_parameter_count ==
           result.parity_surface.runtime_link_host_link_summary.runtime_dispatch_arg_slots + 2u) &&
      (result.parity_surface.runtime_link_host_link_summary.default_runtime_dispatch_symbol_binding ==
       (result.parity_surface.runtime_link_host_link_summary.runtime_dispatch_symbol ==
        kObjc3RuntimeLinkHostLinkDefaultDispatchSymbol)) &&
      result.parity_surface.runtime_link_host_link_summary.deterministic;
  result.parity_surface.deterministic_retain_release_operation_handoff =
      result.deterministic_retain_release_operation_handoff &&
      result.parity_surface.retain_release_operation_summary.ownership_qualified_sites ==
          result.parity_surface.retain_release_operation_ownership_qualified_sites_total &&
      result.parity_surface.retain_release_operation_summary.retain_insertion_sites ==
          result.parity_surface.retain_release_operation_retain_insertion_sites_total &&
      result.parity_surface.retain_release_operation_summary.release_insertion_sites ==
          result.parity_surface.retain_release_operation_release_insertion_sites_total &&
      result.parity_surface.retain_release_operation_summary.autorelease_insertion_sites ==
          result.parity_surface.retain_release_operation_autorelease_insertion_sites_total &&
      result.parity_surface.retain_release_operation_summary.contract_violation_sites ==
          result.parity_surface.retain_release_operation_contract_violation_sites_total &&
      result.parity_surface.retain_release_operation_summary.retain_insertion_sites <=
          result.parity_surface.retain_release_operation_summary.ownership_qualified_sites +
              result.parity_surface.retain_release_operation_summary.contract_violation_sites &&
      result.parity_surface.retain_release_operation_summary.release_insertion_sites <=
          result.parity_surface.retain_release_operation_summary.ownership_qualified_sites +
              result.parity_surface.retain_release_operation_summary.contract_violation_sites &&
      result.parity_surface.retain_release_operation_summary.autorelease_insertion_sites <=
          result.parity_surface.retain_release_operation_summary.ownership_qualified_sites +
              result.parity_surface.retain_release_operation_summary.contract_violation_sites &&
      result.parity_surface.retain_release_operation_summary.deterministic;
  result.parity_surface.deterministic_weak_unowned_semantics_handoff =
      result.deterministic_weak_unowned_semantics_handoff &&
      result.parity_surface.weak_unowned_semantics_summary.ownership_candidate_sites ==
          result.parity_surface.weak_unowned_semantics_ownership_candidate_sites_total &&
      result.parity_surface.weak_unowned_semantics_summary.weak_reference_sites ==
          result.parity_surface.weak_unowned_semantics_weak_reference_sites_total &&
      result.parity_surface.weak_unowned_semantics_summary.unowned_reference_sites ==
          result.parity_surface.weak_unowned_semantics_unowned_reference_sites_total &&
      result.parity_surface.weak_unowned_semantics_summary.unowned_safe_reference_sites ==
          result.parity_surface.weak_unowned_semantics_unowned_safe_reference_sites_total &&
      result.parity_surface.weak_unowned_semantics_summary.weak_unowned_conflict_sites ==
          result.parity_surface.weak_unowned_semantics_conflict_sites_total &&
      result.parity_surface.weak_unowned_semantics_summary.contract_violation_sites ==
          result.parity_surface.weak_unowned_semantics_contract_violation_sites_total &&
      result.parity_surface.weak_unowned_semantics_summary.unowned_safe_reference_sites <=
          result.parity_surface.weak_unowned_semantics_summary.unowned_reference_sites &&
      result.parity_surface.weak_unowned_semantics_summary.weak_unowned_conflict_sites <=
          result.parity_surface.weak_unowned_semantics_summary.ownership_candidate_sites &&
      result.parity_surface.weak_unowned_semantics_summary.contract_violation_sites <=
          result.parity_surface.weak_unowned_semantics_summary.ownership_candidate_sites +
              result.parity_surface.weak_unowned_semantics_summary.weak_unowned_conflict_sites &&
      result.parity_surface.weak_unowned_semantics_summary.deterministic;
  result.parity_surface.deterministic_arc_diagnostics_fixit_handoff =
      result.deterministic_arc_diagnostics_fixit_handoff &&
      result.parity_surface.arc_diagnostics_fixit_summary.ownership_arc_diagnostic_candidate_sites ==
          result.parity_surface.ownership_arc_diagnostic_candidate_sites_total &&
      result.parity_surface.arc_diagnostics_fixit_summary.ownership_arc_fixit_available_sites ==
          result.parity_surface.ownership_arc_fixit_available_sites_total &&
      result.parity_surface.arc_diagnostics_fixit_summary.ownership_arc_profiled_sites ==
          result.parity_surface.ownership_arc_profiled_sites_total &&
      result.parity_surface.arc_diagnostics_fixit_summary.ownership_arc_weak_unowned_conflict_diagnostic_sites ==
          result.parity_surface.ownership_arc_weak_unowned_conflict_diagnostic_sites_total &&
      result.parity_surface.arc_diagnostics_fixit_summary.ownership_arc_empty_fixit_hint_sites ==
          result.parity_surface.ownership_arc_empty_fixit_hint_sites_total &&
      result.parity_surface.arc_diagnostics_fixit_summary.contract_violation_sites ==
          result.parity_surface.ownership_arc_contract_violation_sites_total &&
      result.parity_surface.arc_diagnostics_fixit_summary.ownership_arc_fixit_available_sites <=
          result.parity_surface.arc_diagnostics_fixit_summary.ownership_arc_diagnostic_candidate_sites +
              result.parity_surface.arc_diagnostics_fixit_summary.contract_violation_sites &&
      result.parity_surface.arc_diagnostics_fixit_summary.ownership_arc_profiled_sites <=
          result.parity_surface.arc_diagnostics_fixit_summary.ownership_arc_diagnostic_candidate_sites +
              result.parity_surface.arc_diagnostics_fixit_summary.contract_violation_sites &&
      result.parity_surface.arc_diagnostics_fixit_summary.ownership_arc_weak_unowned_conflict_diagnostic_sites <=
          result.parity_surface.arc_diagnostics_fixit_summary.ownership_arc_diagnostic_candidate_sites +
              result.parity_surface.arc_diagnostics_fixit_summary.contract_violation_sites &&
      result.parity_surface.arc_diagnostics_fixit_summary.ownership_arc_empty_fixit_hint_sites <=
          result.parity_surface.arc_diagnostics_fixit_summary.ownership_arc_fixit_available_sites +
              result.parity_surface.arc_diagnostics_fixit_summary.contract_violation_sites &&
      result.parity_surface.arc_diagnostics_fixit_summary.deterministic;
  result.parity_surface.deterministic_autoreleasepool_scope_handoff =
      result.deterministic_autoreleasepool_scope_handoff &&
      result.parity_surface.autoreleasepool_scope_summary.scope_sites ==
          result.parity_surface.autoreleasepool_scope_sites_total &&
      result.parity_surface.autoreleasepool_scope_summary.scope_symbolized_sites ==
          result.parity_surface.autoreleasepool_scope_symbolized_sites_total &&
      result.parity_surface.autoreleasepool_scope_summary.contract_violation_sites ==
          result.parity_surface.autoreleasepool_scope_contract_violation_sites_total &&
      result.parity_surface.autoreleasepool_scope_summary.max_scope_depth ==
          result.parity_surface.autoreleasepool_scope_max_depth_total &&
      result.parity_surface.autoreleasepool_scope_summary.scope_symbolized_sites <=
          result.parity_surface.autoreleasepool_scope_summary.scope_sites &&
      result.parity_surface.autoreleasepool_scope_summary.contract_violation_sites <=
          result.parity_surface.autoreleasepool_scope_summary.scope_sites &&
      (result.parity_surface.autoreleasepool_scope_summary.scope_sites > 0u ||
       result.parity_surface.autoreleasepool_scope_summary.max_scope_depth == 0u) &&
      result.parity_surface.autoreleasepool_scope_summary.max_scope_depth <=
          static_cast<unsigned>(result.parity_surface.autoreleasepool_scope_summary.scope_sites) &&
      result.parity_surface.autoreleasepool_scope_summary.deterministic;
  result.parity_surface.atomic_memory_order_mapping = result.atomic_memory_order_mapping;
  result.parity_surface.deterministic_atomic_memory_order_mapping = result.deterministic_atomic_memory_order_mapping;
  result.parity_surface.vector_type_lowering = result.vector_type_lowering;
  result.parity_surface.deterministic_vector_type_lowering = result.deterministic_vector_type_lowering;
  result.type_metadata_mapping_readiness_record =
      BuildObjc3SemaTypeMetadataMappingReadinessRecord(
          input, result.parity_surface);
  result.deterministic_type_metadata_mapping_readiness_record =
      IsReadyObjc3SemaTypeMetadataMappingReadinessRecord(
          result.type_metadata_mapping_readiness_record);
  result.parity_surface.type_metadata_mapping_readiness_record =
      result.type_metadata_mapping_readiness_record;
  result.parity_surface.deterministic_type_metadata_mapping_readiness_record =
      result.deterministic_type_metadata_mapping_readiness_record;
  result.typed_semantic_handoff_record =
      BuildObjc3SemaTypedSemanticHandoffRecord(input, result.parity_surface);
  result.deterministic_typed_semantic_handoff_record =
      IsReadyObjc3SemaTypedSemanticHandoffRecord(
          result.typed_semantic_handoff_record);
  result.parity_surface.typed_semantic_handoff_record =
      result.typed_semantic_handoff_record;
  result.parity_surface.deterministic_typed_semantic_handoff_record =
      result.deterministic_typed_semantic_handoff_record;
  const bool parity_parser_sema_contract_ready =
      result.parity_surface.deterministic_parser_sema_conformance_matrix &&
      result.parity_surface.deterministic_parser_sema_conformance_corpus &&
      result.parity_surface.deterministic_parser_sema_performance_quality_guardrails &&
      result.parity_surface.deterministic_parser_sema_cross_lane_integration_sync &&
      result.parity_surface.deterministic_parser_sema_docs_runbook_sync &&
      result.parity_surface.deterministic_parser_sema_release_candidate_replay_dry_run &&
      result.parity_surface.deterministic_parser_sema_advanced_core_shard1 &&
      result.parity_surface.deterministic_parser_sema_advanced_contract_rejection_shard1 &&
      result.parity_surface.deterministic_parser_sema_advanced_diagnostics_shard1 &&
      result.parity_surface.deterministic_parser_sema_advanced_conformance_shard1 &&
      result.parity_surface.deterministic_parser_sema_advanced_integration_shard1 &&
      result.parity_surface.deterministic_parser_sema_advanced_performance_shard1 &&
      result.parity_surface.deterministic_parser_sema_advanced_core_shard2 &&
      result.parity_surface.deterministic_parser_sema_advanced_contract_rejection_shard2 &&
      result.parity_surface.deterministic_parser_sema_advanced_diagnostics_shard2 &&
      result.parity_surface.deterministic_parser_sema_integration_closeout_signoff &&
      result.parity_surface.parser_sema_conformance_matrix.deterministic &&
      result.parity_surface.parser_sema_conformance_corpus.deterministic &&
      result.parity_surface.parser_sema_performance_quality_guardrails
          .deterministic &&
      result.parity_surface.parser_sema_cross_lane_integration_sync
          .deterministic &&
      result.parity_surface.parser_sema_docs_runbook_sync.deterministic &&
      result.parity_surface.parser_sema_release_candidate_replay_dry_run
          .deterministic &&
      result.parity_surface.parser_sema_advanced_core_shard1.deterministic &&
      result.parity_surface.parser_sema_advanced_contract_rejection_shard1
          .deterministic &&
      result.parity_surface.parser_sema_advanced_diagnostics_shard1
          .deterministic &&
      result.parity_surface.parser_sema_advanced_conformance_shard1
          .deterministic &&
      result.parity_surface.parser_sema_advanced_integration_shard1
          .deterministic &&
      result.parity_surface.parser_sema_advanced_performance_shard1
          .deterministic &&
      result.parity_surface.parser_sema_advanced_core_shard2.deterministic &&
      result.parity_surface.parser_sema_advanced_contract_rejection_shard2
          .deterministic &&
      result.parity_surface.parser_sema_advanced_diagnostics_shard2
          .deterministic &&
      result.parity_surface.parser_sema_integration_closeout_signoff
          .deterministic;
  const bool parity_publication_records_ready =
      result.parity_surface.deterministic_pass_manager_publication_record &&
      IsReadyObjc3SemaPassManagerPublicationRecord(
          result.parity_surface.pass_manager_publication_record) &&
      result.parity_surface.deterministic_type_metadata_publication_record &&
      IsReadyObjc3SemaTypeMetadataPublicationRecord(
          result.parity_surface.type_metadata_publication_record);
  const bool parity_diagnostics_publication_ready =
      result.parity_surface.deterministic_diagnostics_publication_record &&
      IsReadyObjc3SemaDiagnosticsPublicationRecord(
          result.parity_surface.diagnostics_publication_record);
  const bool parity_pass_flow_recovery_ready =
      result.parity_surface.deterministic_pass_flow_recovery_record &&
      IsReadyObjc3SemaPassFlowRecoveryRecord(
          result.parity_surface.pass_flow_recovery_record);
  const bool parity_type_metadata_cardinality_ready =
      result.parity_surface
          .deterministic_type_metadata_mapping_readiness_record &&
      result.parity_surface.type_metadata_mapping_readiness_record
          .type_metadata_handoff_ready &&
      result.parity_surface.type_metadata_mapping_readiness_record
          .cardinality_consistent;
  const bool parity_typed_semantic_handoffs_ready =
      result.parity_surface.deterministic_typed_semantic_handoff_record &&
      IsReadyObjc3SemaTypedSemanticHandoffRecord(
          result.parity_surface.typed_semantic_handoff_record);
  const bool parity_mapping_summaries_ready =
      result.parity_surface
          .deterministic_type_metadata_mapping_readiness_record &&
      IsReadyObjc3SemaTypeMetadataMappingReadinessRecord(
          result.parity_surface.type_metadata_mapping_readiness_record);
  result.parity_validation_record = BuildObjc3SemaParityValidationRecord(
      input,
      result.executed,
      parity_parser_sema_contract_ready,
      IsReadyObjc3SemaPassFlowSummary(
          result.parity_surface.sema_pass_flow_summary),
      parity_publication_records_ready,
      parity_diagnostics_publication_ready,
      parity_pass_flow_recovery_ready,
      parity_type_metadata_cardinality_ready,
      parity_typed_semantic_handoffs_ready,
      parity_mapping_summaries_ready);
  result.deterministic_parity_validation_record =
      IsReadyObjc3SemaParityValidationRecord(result.parity_validation_record);
  result.parity_surface.parity_validation_record =
      result.parity_validation_record;
  result.parity_surface.deterministic_parity_validation_record =
      result.deterministic_parity_validation_record;
  result.parity_surface.ready =
      result.executed &&
      result.parity_surface.deterministic_parity_validation_record &&
      IsReadyObjc3SemaParityValidationRecord(
          result.parity_surface.parity_validation_record) &&
      result.parity_surface.deterministic_diagnostics_publication_record &&
      IsReadyObjc3SemaDiagnosticsPublicationRecord(
          result.parity_surface.diagnostics_publication_record) &&
      result.parity_surface.deterministic_pass_flow_recovery_record &&
      IsReadyObjc3SemaPassFlowRecoveryRecord(
          result.parity_surface.pass_flow_recovery_record) &&
      result.parity_surface.deterministic_parser_sema_conformance_matrix &&
      result.parity_surface.deterministic_parser_sema_conformance_corpus &&
      result.parity_surface.deterministic_parser_sema_performance_quality_guardrails &&
      result.parity_surface.deterministic_parser_sema_cross_lane_integration_sync &&
      result.parity_surface.deterministic_parser_sema_docs_runbook_sync &&
      result.parity_surface.deterministic_parser_sema_release_candidate_replay_dry_run &&
      result.parity_surface.deterministic_parser_sema_advanced_core_shard1 &&
      result.parity_surface.deterministic_parser_sema_advanced_contract_rejection_shard1 &&
      result.parity_surface.deterministic_parser_sema_advanced_diagnostics_shard1 &&
      result.parity_surface.deterministic_parser_sema_advanced_conformance_shard1 &&
      result.parity_surface.deterministic_parser_sema_advanced_integration_shard1 &&
      result.parity_surface.deterministic_parser_sema_advanced_performance_shard1 &&
      result.parity_surface.deterministic_parser_sema_advanced_core_shard2 &&
      result.parity_surface.deterministic_parser_sema_advanced_contract_rejection_shard2 &&
      result.parity_surface.deterministic_parser_sema_advanced_diagnostics_shard2 &&
      result.parity_surface.deterministic_parser_sema_integration_closeout_signoff &&
      result.parity_surface.deterministic_pass_manager_publication_record &&
      IsReadyObjc3SemaPassFlowSummary(result.parity_surface.sema_pass_flow_summary) &&
      IsReadyObjc3SemaPassManagerPublicationRecord(
          result.parity_surface.pass_manager_publication_record) &&
      result.parity_surface.deterministic_type_metadata_publication_record &&
      IsReadyObjc3SemaTypeMetadataPublicationRecord(
          result.parity_surface.type_metadata_publication_record) &&
      result.parity_surface.parser_sema_conformance_matrix.deterministic &&
      result.parity_surface.parser_sema_conformance_corpus.deterministic &&
      result.parity_surface.parser_sema_performance_quality_guardrails
          .deterministic &&
      result.parity_surface.parser_sema_cross_lane_integration_sync
          .deterministic &&
      result.parity_surface.parser_sema_docs_runbook_sync
          .deterministic &&
      result.parity_surface.parser_sema_release_candidate_replay_dry_run
          .deterministic &&
      result.parity_surface.parser_sema_advanced_core_shard1
          .deterministic &&
      result.parity_surface.parser_sema_advanced_contract_rejection_shard1
          .deterministic &&
      result.parity_surface.parser_sema_advanced_diagnostics_shard1
          .deterministic &&
      result.parity_surface.parser_sema_advanced_conformance_shard1
          .deterministic &&
      result.parity_surface.parser_sema_advanced_integration_shard1
          .deterministic &&
      result.parity_surface.parser_sema_advanced_performance_shard1
          .deterministic &&
      result.parity_surface.parser_sema_advanced_core_shard2
          .deterministic &&
      result.parity_surface.parser_sema_advanced_contract_rejection_shard2
          .deterministic &&
      result.parity_surface.parser_sema_advanced_diagnostics_shard2
          .deterministic &&
      result.parity_surface.parser_sema_integration_closeout_signoff
          .deterministic &&
      result.parity_surface.parser_sema_performance_quality_guardrails
          .required_guardrail_count == 7u &&
      result.parity_surface.parser_sema_performance_quality_guardrails
          .passed_guardrail_count ==
          result.parity_surface.parser_sema_performance_quality_guardrails
              .required_guardrail_count &&
      result.parity_surface.parser_sema_performance_quality_guardrails
          .failed_guardrail_count == 0u &&
      result.parity_surface.parser_sema_performance_quality_guardrails
          .conformance_matrix_builder_budget_guarded &&
      result.parity_surface.parser_sema_performance_quality_guardrails
          .conformance_corpus_builder_budget_guarded &&
      result.parity_surface.parser_sema_performance_quality_guardrails
          .handoff_scaffold_builder_budget_guarded &&
      result.parity_surface.parser_sema_performance_quality_guardrails
          .matrix_diagnostic_budget_consistent &&
      result.parity_surface.parser_sema_performance_quality_guardrails
          .matrix_token_top_level_budget_consistent &&
      result.parity_surface.parser_sema_performance_quality_guardrails
          .matrix_subset_budget_consistent &&
      result.parity_surface.parser_sema_performance_quality_guardrails
          .corpus_case_budget_consistent &&
      result.parity_surface.parser_sema_cross_lane_integration_sync
          .required_sync_count == 4u &&
      result.parity_surface.parser_sema_cross_lane_integration_sync
          .passed_sync_count ==
          result.parity_surface.parser_sema_cross_lane_integration_sync
              .required_sync_count &&
      result.parity_surface.parser_sema_cross_lane_integration_sync
          .failed_sync_count == 0u &&
      result.parity_surface.parser_sema_cross_lane_integration_sync
          .matrix_consistent &&
      result.parity_surface.parser_sema_cross_lane_integration_sync
          .corpus_consistent &&
      result.parity_surface.parser_sema_cross_lane_integration_sync
          .performance_quality_guardrails_consistent &&
      result.parity_surface.parser_sema_cross_lane_integration_sync
          .pass_manager_contract_surface_sync &&
      result.parity_surface.parser_sema_docs_runbook_sync
          .required_sync_count == 3u &&
      result.parity_surface.parser_sema_docs_runbook_sync
          .passed_sync_count ==
          result.parity_surface.parser_sema_docs_runbook_sync
              .required_sync_count &&
      result.parity_surface.parser_sema_docs_runbook_sync
          .failed_sync_count == 0u &&
      result.parity_surface.parser_sema_docs_runbook_sync
          .cross_lane_integration_sync_ready &&
      result.parity_surface.parser_sema_docs_runbook_sync
          .pass_manager_contract_surface_sync &&
      result.parity_surface.parser_sema_docs_runbook_sync
          .parity_surface_sync &&
      result.parity_surface.parser_sema_release_candidate_replay_dry_run
          .required_sync_count == 3u &&
      result.parity_surface.parser_sema_release_candidate_replay_dry_run
          .passed_sync_count ==
          result.parity_surface.parser_sema_release_candidate_replay_dry_run
              .required_sync_count &&
      result.parity_surface.parser_sema_release_candidate_replay_dry_run
          .failed_sync_count == 0u &&
      result.parity_surface.parser_sema_release_candidate_replay_dry_run
          .docs_runbook_sync_ready &&
      result.parity_surface.parser_sema_release_candidate_replay_dry_run
          .pass_manager_contract_surface_sync &&
      result.parity_surface.parser_sema_release_candidate_replay_dry_run
          .replay_surface_sync &&
      result.parity_surface.parser_sema_advanced_core_shard1
          .required_sync_count == 3u &&
      result.parity_surface.parser_sema_advanced_core_shard1
          .passed_sync_count ==
          result.parity_surface.parser_sema_advanced_core_shard1
              .required_sync_count &&
      result.parity_surface.parser_sema_advanced_core_shard1
          .failed_sync_count == 0u &&
      result.parity_surface.parser_sema_advanced_core_shard1
          .release_candidate_replay_dry_run_ready &&
      result.parity_surface.parser_sema_advanced_core_shard1
          .pass_manager_contract_surface_sync &&
      result.parity_surface.parser_sema_advanced_core_shard1
          .shard_surface_sync &&
      result.parity_surface.parser_sema_advanced_contract_rejection_shard1
          .required_sync_count == 3u &&
      result.parity_surface.parser_sema_advanced_contract_rejection_shard1
          .passed_sync_count ==
          result.parity_surface.parser_sema_advanced_contract_rejection_shard1
              .required_sync_count &&
      result.parity_surface.parser_sema_advanced_contract_rejection_shard1
          .failed_sync_count == 0u &&
      result.parity_surface.parser_sema_advanced_contract_rejection_shard1
          .advanced_core_shard1_ready &&
      result.parity_surface.parser_sema_advanced_contract_rejection_shard1
          .pass_manager_contract_surface_sync &&
      result.parity_surface.parser_sema_advanced_contract_rejection_shard1
          .shard_surface_sync &&
      result.parity_surface.parser_sema_advanced_diagnostics_shard1
          .required_sync_count == 3u &&
      result.parity_surface.parser_sema_advanced_diagnostics_shard1
          .passed_sync_count ==
          result.parity_surface.parser_sema_advanced_diagnostics_shard1
              .required_sync_count &&
      result.parity_surface.parser_sema_advanced_diagnostics_shard1
          .failed_sync_count == 0u &&
      result.parity_surface.parser_sema_advanced_diagnostics_shard1
          .advanced_contract_rejection_shard1_ready &&
      result.parity_surface.parser_sema_advanced_diagnostics_shard1
          .pass_manager_contract_surface_sync &&
      result.parity_surface.parser_sema_advanced_diagnostics_shard1
          .shard_surface_sync &&
      result.parity_surface.parser_sema_advanced_conformance_shard1
          .required_sync_count == 3u &&
      result.parity_surface.parser_sema_advanced_conformance_shard1
          .passed_sync_count ==
          result.parity_surface.parser_sema_advanced_conformance_shard1
              .required_sync_count &&
      result.parity_surface.parser_sema_advanced_conformance_shard1
          .failed_sync_count == 0u &&
      result.parity_surface.parser_sema_advanced_conformance_shard1
          .advanced_diagnostics_shard1_ready &&
      result.parity_surface.parser_sema_advanced_conformance_shard1
          .pass_manager_contract_surface_sync &&
      result.parity_surface.parser_sema_advanced_conformance_shard1
          .shard_surface_sync &&
      result.parity_surface.parser_sema_advanced_integration_shard1
          .required_sync_count == 3u &&
      result.parity_surface.parser_sema_advanced_integration_shard1
          .passed_sync_count ==
          result.parity_surface.parser_sema_advanced_integration_shard1
              .required_sync_count &&
      result.parity_surface.parser_sema_advanced_integration_shard1
          .failed_sync_count == 0u &&
      result.parity_surface.parser_sema_advanced_integration_shard1
          .advanced_conformance_shard1_ready &&
      result.parity_surface.parser_sema_advanced_integration_shard1
          .pass_manager_contract_surface_sync &&
      result.parity_surface.parser_sema_advanced_integration_shard1
          .shard_surface_sync &&
      result.parity_surface.parser_sema_advanced_performance_shard1
          .required_sync_count == 3u &&
      result.parity_surface.parser_sema_advanced_performance_shard1
          .passed_sync_count ==
          result.parity_surface.parser_sema_advanced_performance_shard1
              .required_sync_count &&
      result.parity_surface.parser_sema_advanced_performance_shard1
          .failed_sync_count == 0u &&
      result.parity_surface.parser_sema_advanced_performance_shard1
          .advanced_integration_shard1_ready &&
      result.parity_surface.parser_sema_advanced_performance_shard1
          .pass_manager_contract_surface_sync &&
      result.parity_surface.parser_sema_advanced_performance_shard1
          .shard_surface_sync &&
      result.parity_surface.parser_sema_advanced_core_shard2
          .required_sync_count == 3u &&
      result.parity_surface.parser_sema_advanced_core_shard2
          .passed_sync_count ==
          result.parity_surface.parser_sema_advanced_core_shard2
              .required_sync_count &&
      result.parity_surface.parser_sema_advanced_core_shard2
          .failed_sync_count == 0u &&
      result.parity_surface.parser_sema_advanced_core_shard2
          .advanced_performance_shard1_ready &&
      result.parity_surface.parser_sema_advanced_core_shard2
          .pass_manager_contract_surface_sync &&
      result.parity_surface.parser_sema_advanced_core_shard2
          .shard_surface_sync &&
      result.parity_surface.parser_sema_advanced_contract_rejection_shard2
          .required_sync_count == 3u &&
      result.parity_surface.parser_sema_advanced_contract_rejection_shard2
          .passed_sync_count ==
          result.parity_surface.parser_sema_advanced_contract_rejection_shard2
              .required_sync_count &&
      result.parity_surface.parser_sema_advanced_contract_rejection_shard2
          .failed_sync_count == 0u &&
      result.parity_surface.parser_sema_advanced_contract_rejection_shard2
          .advanced_core_shard2_ready &&
      result.parity_surface.parser_sema_advanced_contract_rejection_shard2
          .pass_manager_contract_surface_sync &&
      result.parity_surface.parser_sema_advanced_contract_rejection_shard2
          .shard_surface_sync &&
      result.parity_surface.parser_sema_advanced_diagnostics_shard2
          .required_sync_count == 3u &&
      result.parity_surface.parser_sema_advanced_diagnostics_shard2
          .passed_sync_count ==
          result.parity_surface.parser_sema_advanced_diagnostics_shard2
              .required_sync_count &&
