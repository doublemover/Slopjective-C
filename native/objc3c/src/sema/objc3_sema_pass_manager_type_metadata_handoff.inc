              .cancellation_check_sites <=
          result.type_metadata_handoff.task_runtime_cancellation_summary
              .task_runtime_interop_sites &&
      result.type_metadata_handoff.task_runtime_cancellation_summary
              .cancellation_handler_sites <=
          result.type_metadata_handoff.task_runtime_cancellation_summary
              .task_runtime_interop_sites &&
      result.type_metadata_handoff.task_runtime_cancellation_summary
              .suspension_point_sites <=
          result.type_metadata_handoff.task_runtime_cancellation_summary
              .task_runtime_interop_sites &&
      result.type_metadata_handoff.task_runtime_cancellation_summary
              .cancellation_propagation_sites <=
          result.type_metadata_handoff.task_runtime_cancellation_summary
              .cancellation_check_sites &&
      result.type_metadata_handoff.task_runtime_cancellation_summary
              .cancellation_propagation_sites <=
          result.type_metadata_handoff.task_runtime_cancellation_summary
              .cancellation_handler_sites &&
      result.type_metadata_handoff.task_runtime_cancellation_summary
              .normalized_sites <=
          result.type_metadata_handoff.task_runtime_cancellation_summary
              .task_runtime_interop_sites &&
      result.type_metadata_handoff.task_runtime_cancellation_summary
              .gate_blocked_sites <=
          result.type_metadata_handoff.task_runtime_cancellation_summary
              .task_runtime_interop_sites &&
      result.type_metadata_handoff.task_runtime_cancellation_summary
              .gate_blocked_sites <=
          result.type_metadata_handoff.task_runtime_cancellation_summary
              .cancellation_propagation_sites &&
      result.type_metadata_handoff.task_runtime_cancellation_summary
              .contract_violation_sites <=
          result.type_metadata_handoff.task_runtime_cancellation_summary
              .task_runtime_interop_sites &&
      result.type_metadata_handoff.task_runtime_cancellation_summary
              .normalized_sites +
              result.type_metadata_handoff.task_runtime_cancellation_summary
                  .gate_blocked_sites ==
          result.type_metadata_handoff.task_runtime_cancellation_summary
              .task_runtime_interop_sites;
  result.concurrency_replay_race_guard_summary =
      result.integration_surface.concurrency_replay_race_guard_summary;
  result.deterministic_concurrency_replay_race_guard_handoff =
      result.type_metadata_handoff.concurrency_replay_race_guard_summary
              .deterministic &&
      result.integration_surface.concurrency_replay_race_guard_summary
          .deterministic &&
      IsEquivalentConcurrencyReplayRaceGuardSummary(
          result.integration_surface.concurrency_replay_race_guard_summary,
          result.type_metadata_handoff
              .concurrency_replay_race_guard_summary) &&
      result.type_metadata_handoff.concurrency_replay_race_guard_summary
              .concurrency_replay_sites <=
          result.type_metadata_handoff.concurrency_replay_race_guard_summary
              .concurrency_replay_race_guard_sites &&
      result.type_metadata_handoff.concurrency_replay_race_guard_summary
              .replay_proof_sites <=
          result.type_metadata_handoff.concurrency_replay_race_guard_summary
              .concurrency_replay_race_guard_sites &&
      result.type_metadata_handoff.concurrency_replay_race_guard_summary
              .race_guard_sites <=
          result.type_metadata_handoff.concurrency_replay_race_guard_summary
              .concurrency_replay_race_guard_sites &&
      result.type_metadata_handoff.concurrency_replay_race_guard_summary
              .task_handoff_sites <=
          result.type_metadata_handoff.concurrency_replay_race_guard_summary
              .concurrency_replay_race_guard_sites &&
      result.type_metadata_handoff.concurrency_replay_race_guard_summary
              .actor_isolation_sites <=
          result.type_metadata_handoff.concurrency_replay_race_guard_summary
              .concurrency_replay_race_guard_sites &&
      result.type_metadata_handoff.concurrency_replay_race_guard_summary
              .deterministic_schedule_sites <=
          result.type_metadata_handoff.concurrency_replay_race_guard_summary
              .concurrency_replay_sites &&
      result.type_metadata_handoff.concurrency_replay_race_guard_summary
              .guard_blocked_sites <=
          result.type_metadata_handoff.concurrency_replay_race_guard_summary
              .concurrency_replay_sites &&
      result.type_metadata_handoff.concurrency_replay_race_guard_summary
              .contract_violation_sites <=
          result.type_metadata_handoff.concurrency_replay_race_guard_summary
              .concurrency_replay_race_guard_sites &&
      result.type_metadata_handoff.concurrency_replay_race_guard_summary
              .deterministic_schedule_sites +
              result.type_metadata_handoff
                  .concurrency_replay_race_guard_summary
                  .guard_blocked_sites ==
          result.type_metadata_handoff.concurrency_replay_race_guard_summary
              .concurrency_replay_sites;
  result.unsafe_pointer_extension_summary =
      result.integration_surface.unsafe_pointer_extension_summary;
  result.deterministic_unsafe_pointer_extension_handoff =
      result.type_metadata_handoff.unsafe_pointer_extension_summary
              .deterministic &&
      result.integration_surface.unsafe_pointer_extension_summary
          .deterministic &&
      IsEquivalentUnsafePointerExtensionSummary(
          result.integration_surface.unsafe_pointer_extension_summary,
          result.type_metadata_handoff.unsafe_pointer_extension_summary) &&
      result.type_metadata_handoff.unsafe_pointer_extension_summary
              .unsafe_keyword_sites <=
          result.type_metadata_handoff.unsafe_pointer_extension_summary
              .unsafe_pointer_extension_sites &&
      result.type_metadata_handoff.unsafe_pointer_extension_summary
              .pointer_arithmetic_sites <=
          result.type_metadata_handoff.unsafe_pointer_extension_summary
              .unsafe_pointer_extension_sites &&
      result.type_metadata_handoff.unsafe_pointer_extension_summary
              .raw_pointer_type_sites <=
          result.type_metadata_handoff.unsafe_pointer_extension_summary
              .unsafe_pointer_extension_sites &&
      result.type_metadata_handoff.unsafe_pointer_extension_summary
              .unsafe_operation_sites <=
          result.type_metadata_handoff.unsafe_pointer_extension_summary
              .unsafe_pointer_extension_sites &&
      result.type_metadata_handoff.unsafe_pointer_extension_summary
              .normalized_sites <=
          result.type_metadata_handoff.unsafe_pointer_extension_summary
              .unsafe_pointer_extension_sites &&
      result.type_metadata_handoff.unsafe_pointer_extension_summary
              .gate_blocked_sites <=
          result.type_metadata_handoff.unsafe_pointer_extension_summary
              .unsafe_pointer_extension_sites &&
      result.type_metadata_handoff.unsafe_pointer_extension_summary
              .contract_violation_sites <=
          result.type_metadata_handoff.unsafe_pointer_extension_summary
              .unsafe_pointer_extension_sites &&
      result.type_metadata_handoff.unsafe_pointer_extension_summary
              .normalized_sites +
              result.type_metadata_handoff.unsafe_pointer_extension_summary
                  .gate_blocked_sites ==
          result.type_metadata_handoff.unsafe_pointer_extension_summary
              .unsafe_pointer_extension_sites;
  result.inline_asm_intrinsic_governance_summary =
      result.integration_surface.inline_asm_intrinsic_governance_summary;
  result.deterministic_inline_asm_intrinsic_governance_handoff =
      result.type_metadata_handoff.inline_asm_intrinsic_governance_summary
              .deterministic &&
      result.integration_surface.inline_asm_intrinsic_governance_summary
          .deterministic &&
      IsEquivalentInlineAsmIntrinsicGovernanceSummary(
          result.integration_surface.inline_asm_intrinsic_governance_summary,
          result.type_metadata_handoff
              .inline_asm_intrinsic_governance_summary) &&
      result.type_metadata_handoff.inline_asm_intrinsic_governance_summary
              .inline_asm_sites <=
          result.type_metadata_handoff.inline_asm_intrinsic_governance_summary
              .inline_asm_intrinsic_sites &&
      result.type_metadata_handoff.inline_asm_intrinsic_governance_summary
              .intrinsic_sites <=
          result.type_metadata_handoff.inline_asm_intrinsic_governance_summary
              .inline_asm_intrinsic_sites &&
      result.type_metadata_handoff.inline_asm_intrinsic_governance_summary
              .governed_intrinsic_sites <=
          result.type_metadata_handoff.inline_asm_intrinsic_governance_summary
              .intrinsic_sites &&
      result.type_metadata_handoff.inline_asm_intrinsic_governance_summary
              .privileged_intrinsic_sites <=
          result.type_metadata_handoff.inline_asm_intrinsic_governance_summary
              .governed_intrinsic_sites &&
      result.type_metadata_handoff.inline_asm_intrinsic_governance_summary
              .normalized_sites <=
          result.type_metadata_handoff.inline_asm_intrinsic_governance_summary
              .inline_asm_intrinsic_sites &&
      result.type_metadata_handoff.inline_asm_intrinsic_governance_summary
              .gate_blocked_sites <=
          result.type_metadata_handoff.inline_asm_intrinsic_governance_summary
              .inline_asm_intrinsic_sites &&
      result.type_metadata_handoff.inline_asm_intrinsic_governance_summary
              .contract_violation_sites <=
          result.type_metadata_handoff.inline_asm_intrinsic_governance_summary
              .inline_asm_intrinsic_sites &&
      result.type_metadata_handoff.inline_asm_intrinsic_governance_summary
              .inline_asm_sites ==
          result.type_metadata_handoff.throws_propagation_summary
              .cache_invalidation_candidate_sites &&
      result.type_metadata_handoff.inline_asm_intrinsic_governance_summary
              .intrinsic_sites ==
          result.type_metadata_handoff.unsafe_pointer_extension_summary
              .unsafe_operation_sites &&
      result.type_metadata_handoff.inline_asm_intrinsic_governance_summary
              .governed_intrinsic_sites <=
          result.type_metadata_handoff.throws_propagation_summary
              .normalized_sites &&
      result.type_metadata_handoff.inline_asm_intrinsic_governance_summary
              .privileged_intrinsic_sites <=
          result.type_metadata_handoff.unsafe_pointer_extension_summary
              .normalized_sites &&
      result.type_metadata_handoff.inline_asm_intrinsic_governance_summary
              .normalized_sites >=
          result.type_metadata_handoff.inline_asm_intrinsic_governance_summary
              .inline_asm_sites &&
      result.type_metadata_handoff.inline_asm_intrinsic_governance_summary
              .normalized_sites -
              result.type_metadata_handoff
                  .inline_asm_intrinsic_governance_summary
                  .inline_asm_sites ==
          result.type_metadata_handoff.inline_asm_intrinsic_governance_summary
              .governed_intrinsic_sites &&
      result.type_metadata_handoff.inline_asm_intrinsic_governance_summary
              .gate_blocked_sites ==
          result.type_metadata_handoff.inline_asm_intrinsic_governance_summary
                  .intrinsic_sites -
              result.type_metadata_handoff
                  .inline_asm_intrinsic_governance_summary
                  .governed_intrinsic_sites &&
      result.type_metadata_handoff.inline_asm_intrinsic_governance_summary
              .normalized_sites +
              result.type_metadata_handoff
                  .inline_asm_intrinsic_governance_summary
                  .gate_blocked_sites ==
          result.type_metadata_handoff.inline_asm_intrinsic_governance_summary
              .inline_asm_intrinsic_sites;
  result.ns_error_bridging_summary =
      result.integration_surface.ns_error_bridging_summary;
  result.deterministic_ns_error_bridging_handoff =
      result.type_metadata_handoff.ns_error_bridging_summary.deterministic &&
      result.integration_surface.ns_error_bridging_summary.deterministic &&
      IsEquivalentNSErrorBridgingSummary(
          result.integration_surface.ns_error_bridging_summary,
          result.type_metadata_handoff.ns_error_bridging_summary) &&
      result.type_metadata_handoff.ns_error_bridging_summary
              .ns_error_parameter_sites <=
          result.type_metadata_handoff.ns_error_bridging_summary
              .ns_error_bridging_sites &&
      result.type_metadata_handoff.ns_error_bridging_summary
              .ns_error_out_parameter_sites <=
          result.type_metadata_handoff.ns_error_bridging_summary
              .ns_error_bridging_sites &&
      result.type_metadata_handoff.ns_error_bridging_summary
              .ns_error_bridge_path_sites <=
          result.type_metadata_handoff.ns_error_bridging_summary
              .ns_error_bridging_sites &&
      result.type_metadata_handoff.ns_error_bridging_summary.failable_call_sites <=
          result.type_metadata_handoff.ns_error_bridging_summary
              .ns_error_bridging_sites &&
      result.type_metadata_handoff.ns_error_bridging_summary.normalized_sites <=
          result.type_metadata_handoff.ns_error_bridging_summary
              .ns_error_bridging_sites &&
      result.type_metadata_handoff.ns_error_bridging_summary
              .bridge_boundary_sites <=
          result.type_metadata_handoff.ns_error_bridging_summary
              .ns_error_bridging_sites &&
      result.type_metadata_handoff.ns_error_bridging_summary.normalized_sites +
              result.type_metadata_handoff.ns_error_bridging_summary
                  .bridge_boundary_sites ==
          result.type_metadata_handoff.ns_error_bridging_summary
              .ns_error_bridging_sites &&
      result.type_metadata_handoff.ns_error_bridging_summary
              .contract_violation_sites <=
          result.type_metadata_handoff.ns_error_bridging_summary
              .ns_error_bridging_sites;
  result.error_diagnostics_recovery_summary =
      result.integration_surface.error_diagnostics_recovery_summary;
  result.deterministic_error_diagnostics_recovery_handoff =
      result.type_metadata_handoff.error_diagnostics_recovery_summary
              .deterministic &&
      result.integration_surface.error_diagnostics_recovery_summary
          .deterministic &&
      IsEquivalentErrorDiagnosticsRecoverySummary(
          result.integration_surface.error_diagnostics_recovery_summary,
          result.type_metadata_handoff.error_diagnostics_recovery_summary) &&
      result.type_metadata_handoff.error_diagnostics_recovery_summary
              .diagnostic_emit_sites <=
          result.type_metadata_handoff.error_diagnostics_recovery_summary
              .error_diagnostics_recovery_sites &&
      result.type_metadata_handoff.error_diagnostics_recovery_summary
              .recovery_anchor_sites <=
          result.type_metadata_handoff.error_diagnostics_recovery_summary
              .error_diagnostics_recovery_sites &&
      result.type_metadata_handoff.error_diagnostics_recovery_summary
              .recovery_boundary_sites <=
          result.type_metadata_handoff.error_diagnostics_recovery_summary
              .error_diagnostics_recovery_sites &&
      result.type_metadata_handoff.error_diagnostics_recovery_summary
              .fail_closed_diagnostic_sites <=
          result.type_metadata_handoff.error_diagnostics_recovery_summary
              .error_diagnostics_recovery_sites &&
      result.type_metadata_handoff.error_diagnostics_recovery_summary
              .fail_closed_diagnostic_sites <=
          result.type_metadata_handoff.error_diagnostics_recovery_summary
              .diagnostic_emit_sites &&
      result.type_metadata_handoff.error_diagnostics_recovery_summary
              .normalized_sites <=
          result.type_metadata_handoff.error_diagnostics_recovery_summary
              .error_diagnostics_recovery_sites &&
      result.type_metadata_handoff.error_diagnostics_recovery_summary
              .gate_blocked_sites <=
          result.type_metadata_handoff.error_diagnostics_recovery_summary
              .error_diagnostics_recovery_sites &&
      result.type_metadata_handoff.error_diagnostics_recovery_summary
              .contract_violation_sites <=
          result.type_metadata_handoff.error_diagnostics_recovery_summary
              .error_diagnostics_recovery_sites &&
      result.type_metadata_handoff.error_diagnostics_recovery_summary
              .normalized_sites +
              result.type_metadata_handoff.error_diagnostics_recovery_summary
                  .gate_blocked_sites ==
          result.type_metadata_handoff.error_diagnostics_recovery_summary
              .error_diagnostics_recovery_sites;
  result.result_like_lowering_summary =
      result.integration_surface.result_like_lowering_summary;
  result.deterministic_result_like_lowering_handoff =
      result.type_metadata_handoff.result_like_lowering_summary.deterministic &&
      result.integration_surface.result_like_lowering_summary.deterministic &&
      IsEquivalentResultLikeLoweringSummary(
          result.integration_surface.result_like_lowering_summary,
          result.type_metadata_handoff.result_like_lowering_summary) &&
      result.type_metadata_handoff.result_like_lowering_summary
              .result_success_sites <=
          result.type_metadata_handoff.result_like_lowering_summary
              .result_like_sites &&
      result.type_metadata_handoff.result_like_lowering_summary
              .result_failure_sites <=
          result.type_metadata_handoff.result_like_lowering_summary
              .result_like_sites &&
      result.type_metadata_handoff.result_like_lowering_summary
              .result_branch_sites <=
          result.type_metadata_handoff.result_like_lowering_summary
              .result_like_sites &&
      result.type_metadata_handoff.result_like_lowering_summary
              .result_payload_sites <=
          result.type_metadata_handoff.result_like_lowering_summary
              .result_like_sites &&
      result.type_metadata_handoff.result_like_lowering_summary.normalized_sites <=
          result.type_metadata_handoff.result_like_lowering_summary
              .result_like_sites &&
      result.type_metadata_handoff.result_like_lowering_summary
              .branch_merge_sites <=
          result.type_metadata_handoff.result_like_lowering_summary
              .result_like_sites &&
      result.type_metadata_handoff.result_like_lowering_summary
              .contract_violation_sites <=
          result.type_metadata_handoff.result_like_lowering_summary
              .result_like_sites &&
      result.type_metadata_handoff.result_like_lowering_summary
              .result_success_sites +
              result.type_metadata_handoff.result_like_lowering_summary
                  .result_failure_sites ==
          result.type_metadata_handoff.result_like_lowering_summary
              .normalized_sites &&
      result.type_metadata_handoff.result_like_lowering_summary
              .normalized_sites +
              result.type_metadata_handoff.result_like_lowering_summary
                  .branch_merge_sites ==
          result.type_metadata_handoff.result_like_lowering_summary
              .result_like_sites;
  result.unwind_cleanup_summary = result.integration_surface.unwind_cleanup_summary;
  result.deterministic_unwind_cleanup_handoff =
      result.type_metadata_handoff.unwind_cleanup_summary.deterministic &&
      result.integration_surface.unwind_cleanup_summary.deterministic &&
      IsEquivalentUnwindCleanupSummary(
          result.integration_surface.unwind_cleanup_summary,
          result.type_metadata_handoff.unwind_cleanup_summary) &&
      result.type_metadata_handoff.unwind_cleanup_summary.exceptional_exit_sites <=
          result.type_metadata_handoff.unwind_cleanup_summary.unwind_cleanup_sites &&
      result.type_metadata_handoff.unwind_cleanup_summary.cleanup_action_sites <=
          result.type_metadata_handoff.unwind_cleanup_summary.unwind_cleanup_sites &&
      result.type_metadata_handoff.unwind_cleanup_summary.cleanup_scope_sites <=
          result.type_metadata_handoff.unwind_cleanup_summary.unwind_cleanup_sites &&
      result.type_metadata_handoff.unwind_cleanup_summary.cleanup_resume_sites <=
          result.type_metadata_handoff.unwind_cleanup_summary.unwind_cleanup_sites &&
      result.type_metadata_handoff.unwind_cleanup_summary.normalized_sites <=
          result.type_metadata_handoff.unwind_cleanup_summary.unwind_cleanup_sites &&
      result.type_metadata_handoff.unwind_cleanup_summary.fail_closed_sites <=
          result.type_metadata_handoff.unwind_cleanup_summary.unwind_cleanup_sites &&
      result.type_metadata_handoff.unwind_cleanup_summary
              .contract_violation_sites <=
          result.type_metadata_handoff.unwind_cleanup_summary.unwind_cleanup_sites &&
      result.type_metadata_handoff.unwind_cleanup_summary.normalized_sites +
              result.type_metadata_handoff.unwind_cleanup_summary
                  .fail_closed_sites ==
          result.type_metadata_handoff.unwind_cleanup_summary.unwind_cleanup_sites;
  result.async_continuation_summary =
      result.integration_surface.async_continuation_summary;
  result.deterministic_async_continuation_handoff =
      result.type_metadata_handoff.async_continuation_summary.deterministic &&
      result.integration_surface.async_continuation_summary.deterministic &&
      IsEquivalentAsyncContinuationSummary(
          result.integration_surface.async_continuation_summary,
          result.type_metadata_handoff.async_continuation_summary) &&
      result.type_metadata_handoff.async_continuation_summary
              .async_keyword_sites <=
          result.type_metadata_handoff.async_continuation_summary
              .async_continuation_sites &&
      result.type_metadata_handoff.async_continuation_summary
              .async_function_sites <=
          result.type_metadata_handoff.async_continuation_summary
              .async_continuation_sites &&
      result.type_metadata_handoff.async_continuation_summary
              .continuation_allocation_sites <=
          result.type_metadata_handoff.async_continuation_summary
              .async_continuation_sites &&
      result.type_metadata_handoff.async_continuation_summary
              .continuation_resume_sites <=
          result.type_metadata_handoff.async_continuation_summary
              .async_continuation_sites &&
      result.type_metadata_handoff.async_continuation_summary
              .continuation_suspend_sites <=
          result.type_metadata_handoff.async_continuation_summary
              .async_continuation_sites &&
      result.type_metadata_handoff.async_continuation_summary
              .async_state_machine_sites <=
          result.type_metadata_handoff.async_continuation_summary
              .async_continuation_sites &&
      result.type_metadata_handoff.async_continuation_summary.normalized_sites <=
          result.type_metadata_handoff.async_continuation_summary
              .async_continuation_sites &&
      result.type_metadata_handoff.async_continuation_summary
              .gate_blocked_sites <=
          result.type_metadata_handoff.async_continuation_summary
              .async_continuation_sites &&
      result.type_metadata_handoff.async_continuation_summary
              .contract_violation_sites <=
          result.type_metadata_handoff.async_continuation_summary
              .async_continuation_sites &&
      result.type_metadata_handoff.async_continuation_summary.normalized_sites +
              result.type_metadata_handoff.async_continuation_summary
                  .gate_blocked_sites ==
          result.type_metadata_handoff.async_continuation_summary
              .async_continuation_sites;
  result.await_lowering_suspension_state_lowering_summary =
      result.integration_surface.await_lowering_suspension_state_lowering_summary;
  result.deterministic_await_lowering_suspension_state_lowering_handoff =
      result.type_metadata_handoff
              .await_lowering_suspension_state_lowering_summary.deterministic &&
      result.integration_surface
              .await_lowering_suspension_state_lowering_summary.deterministic &&
      IsEquivalentAwaitLoweringSuspensionStateSummary(
          result.integration_surface
              .await_lowering_suspension_state_lowering_summary,
          result.type_metadata_handoff
              .await_lowering_suspension_state_lowering_summary) &&
      result.type_metadata_handoff
              .await_lowering_suspension_state_lowering_summary
              .await_keyword_sites <=
          result.type_metadata_handoff
              .await_lowering_suspension_state_lowering_summary
              .await_suspension_sites &&
      result.type_metadata_handoff
              .await_lowering_suspension_state_lowering_summary
              .await_suspension_point_sites <=
          result.type_metadata_handoff
              .await_lowering_suspension_state_lowering_summary
              .await_suspension_sites &&
      result.type_metadata_handoff
              .await_lowering_suspension_state_lowering_summary.await_resume_sites <=
          result.type_metadata_handoff
              .await_lowering_suspension_state_lowering_summary
              .await_suspension_sites &&
      result.type_metadata_handoff
              .await_lowering_suspension_state_lowering_summary
              .await_state_machine_sites <=
          result.type_metadata_handoff
              .await_lowering_suspension_state_lowering_summary
              .await_suspension_point_sites &&
      result.type_metadata_handoff
              .await_lowering_suspension_state_lowering_summary
              .await_continuation_sites <=
          result.type_metadata_handoff
              .await_lowering_suspension_state_lowering_summary
              .await_suspension_point_sites &&
      result.type_metadata_handoff
              .await_lowering_suspension_state_lowering_summary.normalized_sites <=
          result.type_metadata_handoff
              .await_lowering_suspension_state_lowering_summary
              .await_suspension_sites &&
      result.type_metadata_handoff
              .await_lowering_suspension_state_lowering_summary
              .gate_blocked_sites <=
          result.type_metadata_handoff
              .await_lowering_suspension_state_lowering_summary
              .await_suspension_sites &&
      result.type_metadata_handoff
              .await_lowering_suspension_state_lowering_summary
              .contract_violation_sites <=
          result.type_metadata_handoff
              .await_lowering_suspension_state_lowering_summary
              .await_suspension_sites &&
      result.type_metadata_handoff
              .await_lowering_suspension_state_lowering_summary.normalized_sites +
              result.type_metadata_handoff
                  .await_lowering_suspension_state_lowering_summary
                  .gate_blocked_sites ==
          result.type_metadata_handoff
              .await_lowering_suspension_state_lowering_summary
              .await_suspension_sites;
#include "sema/objc3_sema_pass_manager_type_metadata_handoff_source_graph.inc"
#include "sema/objc3_sema_pass_manager_type_metadata_handoff_blocks.inc"
  result.message_send_selector_lowering_summary =
      result.integration_surface.message_send_selector_lowering_summary;
  result.deterministic_message_send_selector_lowering_handoff =
      result.type_metadata_handoff.message_send_selector_lowering_summary.deterministic &&
      result.integration_surface.message_send_selector_lowering_summary.deterministic &&
      IsEquivalentMessageSendSelectorLoweringSummary(
          result.integration_surface.message_send_selector_lowering_summary,
          result.type_metadata_handoff.message_send_selector_lowering_summary) &&
      result.type_metadata_handoff.message_send_selector_lowering_summary.unary_form_sites +
              result.type_metadata_handoff.message_send_selector_lowering_summary.keyword_form_sites ==
          result.type_metadata_handoff.message_send_selector_lowering_summary.message_send_sites &&
      result.type_metadata_handoff.message_send_selector_lowering_summary.selector_lowering_symbol_sites <=
          result.type_metadata_handoff.message_send_selector_lowering_summary.message_send_sites &&
      result.type_metadata_handoff.message_send_selector_lowering_summary.selector_lowering_argument_piece_entries <=
          result.type_metadata_handoff.message_send_selector_lowering_summary.selector_lowering_piece_entries &&
      result.type_metadata_handoff.message_send_selector_lowering_summary.selector_lowering_normalized_sites <=
          result.type_metadata_handoff.message_send_selector_lowering_summary.selector_lowering_symbol_sites &&
      result.type_metadata_handoff.message_send_selector_lowering_summary.selector_lowering_form_mismatch_sites <=
          result.type_metadata_handoff.message_send_selector_lowering_summary.message_send_sites &&
      result.type_metadata_handoff.message_send_selector_lowering_summary.selector_lowering_arity_mismatch_sites <=
          result.type_metadata_handoff.message_send_selector_lowering_summary.message_send_sites &&
      result.type_metadata_handoff.message_send_selector_lowering_summary.selector_lowering_symbol_mismatch_sites <=
          result.type_metadata_handoff.message_send_selector_lowering_summary.message_send_sites &&
      result.type_metadata_handoff.message_send_selector_lowering_summary.selector_lowering_missing_symbol_sites <=
          result.type_metadata_handoff.message_send_selector_lowering_summary.message_send_sites &&
      result.type_metadata_handoff.message_send_selector_lowering_summary.selector_lowering_contract_violation_sites <=
          result.type_metadata_handoff.message_send_selector_lowering_summary.message_send_sites;
  result.dispatch_abi_marshalling_summary = result.integration_surface.dispatch_abi_marshalling_summary;
  result.deterministic_dispatch_abi_marshalling_handoff =
      result.type_metadata_handoff.dispatch_abi_marshalling_summary.deterministic &&
      result.integration_surface.dispatch_abi_marshalling_summary.deterministic &&
      IsEquivalentDispatchAbiMarshallingSummary(
          result.integration_surface.dispatch_abi_marshalling_summary,
          result.type_metadata_handoff.dispatch_abi_marshalling_summary) &&
      result.type_metadata_handoff.dispatch_abi_marshalling_summary.receiver_slots ==
          result.type_metadata_handoff.dispatch_abi_marshalling_summary.message_send_sites &&
      result.type_metadata_handoff.dispatch_abi_marshalling_summary.selector_symbol_slots +
              result.type_metadata_handoff.dispatch_abi_marshalling_summary.missing_selector_symbol_sites ==
          result.type_metadata_handoff.dispatch_abi_marshalling_summary.message_send_sites &&
      result.type_metadata_handoff.dispatch_abi_marshalling_summary.keyword_argument_slots +
              result.type_metadata_handoff.dispatch_abi_marshalling_summary.unary_argument_slots ==
          result.type_metadata_handoff.dispatch_abi_marshalling_summary.argument_slots &&
      result.type_metadata_handoff.dispatch_abi_marshalling_summary.keyword_argument_slots <=
          result.type_metadata_handoff.dispatch_abi_marshalling_summary.argument_slots &&
      result.type_metadata_handoff.dispatch_abi_marshalling_summary.unary_argument_slots <=
          result.type_metadata_handoff.dispatch_abi_marshalling_summary.argument_slots &&
      result.type_metadata_handoff.dispatch_abi_marshalling_summary.selector_symbol_slots <=
          result.type_metadata_handoff.dispatch_abi_marshalling_summary.message_send_sites &&
      result.type_metadata_handoff.dispatch_abi_marshalling_summary.missing_selector_symbol_sites <=
          result.type_metadata_handoff.dispatch_abi_marshalling_summary.message_send_sites &&
      result.type_metadata_handoff.dispatch_abi_marshalling_summary.arity_mismatch_sites <=
          result.type_metadata_handoff.dispatch_abi_marshalling_summary.message_send_sites &&
      result.type_metadata_handoff.dispatch_abi_marshalling_summary.contract_violation_sites <=
          result.type_metadata_handoff.dispatch_abi_marshalling_summary.message_send_sites;
  result.nil_receiver_semantics_foldability_summary =
      result.integration_surface.nil_receiver_semantics_foldability_summary;
  result.deterministic_nil_receiver_semantics_foldability_handoff =
      result.type_metadata_handoff.nil_receiver_semantics_foldability_summary.deterministic &&
      result.integration_surface.nil_receiver_semantics_foldability_summary.deterministic &&
      IsEquivalentNilReceiverSemanticsFoldabilitySummary(
          result.integration_surface.nil_receiver_semantics_foldability_summary,
          result.type_metadata_handoff.nil_receiver_semantics_foldability_summary) &&
      result.type_metadata_handoff.nil_receiver_semantics_foldability_summary.receiver_nil_literal_sites ==
          result.type_metadata_handoff.nil_receiver_semantics_foldability_summary.nil_receiver_semantics_enabled_sites &&
      result.type_metadata_handoff.nil_receiver_semantics_foldability_summary.nil_receiver_foldable_sites <=
          result.type_metadata_handoff.nil_receiver_semantics_foldability_summary.nil_receiver_semantics_enabled_sites &&
      result.type_metadata_handoff.nil_receiver_semantics_foldability_summary
              .nil_receiver_runtime_dispatch_required_sites +
              result.type_metadata_handoff.nil_receiver_semantics_foldability_summary.nil_receiver_foldable_sites ==
          result.type_metadata_handoff.nil_receiver_semantics_foldability_summary.message_send_sites &&
      result.type_metadata_handoff.nil_receiver_semantics_foldability_summary.nil_receiver_semantics_enabled_sites +
              result.type_metadata_handoff.nil_receiver_semantics_foldability_summary.non_nil_receiver_sites ==
          result.type_metadata_handoff.nil_receiver_semantics_foldability_summary.message_send_sites &&
      result.type_metadata_handoff.nil_receiver_semantics_foldability_summary.contract_violation_sites <=
          result.type_metadata_handoff.nil_receiver_semantics_foldability_summary.message_send_sites;
  result.super_dispatch_method_family_summary =
      result.integration_surface.super_dispatch_method_family_summary;
  result.deterministic_super_dispatch_method_family_handoff =
      result.type_metadata_handoff.super_dispatch_method_family_summary.deterministic &&
      result.integration_surface.super_dispatch_method_family_summary.deterministic &&
      IsEquivalentSuperDispatchMethodFamilySummary(
          result.integration_surface.super_dispatch_method_family_summary,
          result.type_metadata_handoff.super_dispatch_method_family_summary) &&
      result.type_metadata_handoff.super_dispatch_method_family_summary.receiver_super_identifier_sites ==
          result.type_metadata_handoff.super_dispatch_method_family_summary.super_dispatch_enabled_sites &&
      result.type_metadata_handoff.super_dispatch_method_family_summary.super_dispatch_requires_class_context_sites ==
          result.type_metadata_handoff.super_dispatch_method_family_summary.super_dispatch_enabled_sites &&
      result.type_metadata_handoff.super_dispatch_method_family_summary.method_family_init_sites +
              result.type_metadata_handoff.super_dispatch_method_family_summary.method_family_copy_sites +
              result.type_metadata_handoff.super_dispatch_method_family_summary.method_family_mutable_copy_sites +
              result.type_metadata_handoff.super_dispatch_method_family_summary.method_family_new_sites +
              result.type_metadata_handoff.super_dispatch_method_family_summary.method_family_none_sites ==
          result.type_metadata_handoff.super_dispatch_method_family_summary.message_send_sites &&
      result.type_metadata_handoff.super_dispatch_method_family_summary.method_family_returns_related_result_sites <=
          result.type_metadata_handoff.super_dispatch_method_family_summary.method_family_init_sites &&
      result.type_metadata_handoff.super_dispatch_method_family_summary.method_family_returns_retained_result_sites <=
          result.type_metadata_handoff.super_dispatch_method_family_summary.message_send_sites &&
      result.type_metadata_handoff.super_dispatch_method_family_summary.contract_violation_sites <=
          result.type_metadata_handoff.super_dispatch_method_family_summary.message_send_sites;
  result.runtime_link_host_link_summary = result.integration_surface.runtime_link_host_link_summary;
  result.deterministic_runtime_link_host_link_handoff =
      result.type_metadata_handoff.runtime_link_host_link_summary.deterministic &&
      result.integration_surface.runtime_link_host_link_summary.deterministic &&
      IsEquivalentRuntimeLinkHostLinkSummary(
          result.integration_surface.runtime_link_host_link_summary,
          result.type_metadata_handoff.runtime_link_host_link_summary) &&
      result.type_metadata_handoff.runtime_link_host_link_summary.runtime_link_required_sites +
              result.type_metadata_handoff.runtime_link_host_link_summary.runtime_link_elided_sites ==
          result.type_metadata_handoff.runtime_link_host_link_summary.message_send_sites &&
      result.type_metadata_handoff.runtime_link_host_link_summary.contract_violation_sites <=
          result.type_metadata_handoff.runtime_link_host_link_summary.message_send_sites &&
      (result.type_metadata_handoff.runtime_link_host_link_summary.message_send_sites == 0 ||
       result.type_metadata_handoff.runtime_link_host_link_summary.runtime_dispatch_declaration_parameter_count ==
           result.type_metadata_handoff.runtime_link_host_link_summary.runtime_dispatch_arg_slots + 2u) &&
      (result.type_metadata_handoff.runtime_link_host_link_summary.default_runtime_dispatch_symbol_binding ==
       (result.type_metadata_handoff.runtime_link_host_link_summary.runtime_dispatch_symbol ==
        kObjc3RuntimeLinkHostLinkDefaultDispatchSymbol));
  result.retain_release_operation_summary = result.integration_surface.retain_release_operation_summary;
  result.deterministic_retain_release_operation_handoff =
      result.type_metadata_handoff.retain_release_operation_summary.deterministic &&
      result.integration_surface.retain_release_operation_summary.deterministic &&
      IsEquivalentRetainReleaseOperationSummary(
          result.integration_surface.retain_release_operation_summary,
          result.type_metadata_handoff.retain_release_operation_summary) &&
      // runtime ARC helper API surface anchor: sema continues to
      // publish ownership and helper-need summaries only; the concrete helper
      // ABI itself remains frozen in the runtime/lowering boundary.
      // runtime ARC helper implementation anchor: sema continues to
      // publish ownership and helper-need summaries only, while the supported
      // property/weak/autorelease-return ARC slice now reaches a live linked
      // helper/runtime boundary rather than a semantic promise here.
      // ownership-debug/runtime-validation anchor: sema still does
      // not own ARC debug counters or helper-traffic probes; it only preserves
      // the packets that the private runtime/testing surface consumes.
      result.type_metadata_handoff.retain_release_operation_summary.retain_insertion_sites <=
          result.type_metadata_handoff.retain_release_operation_summary.ownership_qualified_sites +
              result.type_metadata_handoff.retain_release_operation_summary.contract_violation_sites &&
      result.type_metadata_handoff.retain_release_operation_summary.release_insertion_sites <=
          result.type_metadata_handoff.retain_release_operation_summary.ownership_qualified_sites +
              result.type_metadata_handoff.retain_release_operation_summary.contract_violation_sites &&
      result.type_metadata_handoff.retain_release_operation_summary.autorelease_insertion_sites <=
          result.type_metadata_handoff.retain_release_operation_summary.ownership_qualified_sites +
              result.type_metadata_handoff.retain_release_operation_summary.contract_violation_sites;
  result.weak_unowned_semantics_summary = result.integration_surface.weak_unowned_semantics_summary;
  result.deterministic_weak_unowned_semantics_handoff =
      result.type_metadata_handoff.weak_unowned_semantics_summary.deterministic &&
      result.integration_surface.weak_unowned_semantics_summary.deterministic &&
      IsEquivalentWeakUnownedSemanticsSummary(
          result.integration_surface.weak_unowned_semantics_summary,
          result.type_metadata_handoff.weak_unowned_semantics_summary) &&
      result.type_metadata_handoff.weak_unowned_semantics_summary.unowned_safe_reference_sites <=
          result.type_metadata_handoff.weak_unowned_semantics_summary.unowned_reference_sites &&
      result.type_metadata_handoff.weak_unowned_semantics_summary.weak_unowned_conflict_sites <=
          result.type_metadata_handoff.weak_unowned_semantics_summary.ownership_candidate_sites &&
      result.type_metadata_handoff.weak_unowned_semantics_summary.contract_violation_sites <=
          result.type_metadata_handoff.weak_unowned_semantics_summary.ownership_candidate_sites +
              result.type_metadata_handoff.weak_unowned_semantics_summary.weak_unowned_conflict_sites;
  // ARC mode-handling anchor: the handoff keeps ownership
  // qualifiers, weak/unowned summaries, ARC fix-it summaries, and
  // autoreleasepool scope accounting live and deterministic while explicit
  // ARC mode now widens executable ownership-qualified
  // function/method
  // signatures without claiming full ARC automation.
  // ARC semantic-rule freeze anchor: the pass-manager handoff still
  // treats ARC semantic legality as explicit-mode-only, preserving deterministic
  // weak/unowned and ARC-fixit surfaces while broader inference remains
  // deferred.
  // ARC inference/lifetime implementation anchor: the handoff now
  // treats explicit ARC mode as permission to infer strong-owned retain/release
  // activity for the supported unqualified object slice, while non-ARC still
  // preserves the zero-inference baseline and later ARC cleanup work remains
  // deferred.
  // ARC interaction-semantics expansion anchor: the handoff now also
  // preserves the semantic packets that distinguish weak/non-owning storage,
  // explicit autorelease returns, synthesized accessor ownership packets, and
  // block-capture interactions inside the supported ARC slice.
  // ARC lowering ABI/cleanup freeze anchor: this handoff remains
  // semantic-only and must not silently claim cleanup scheduling, helper-call
  // placement, or runtime ABI widening before the lane-C lowering issues land.
  // ARC automatic-insertion implementation anchor: the handoff now
  // also preserves the supported ARC insertion flags that lowering consumes for
  // helper placement, without moving retain/release/autorelease emission into
  // sema itself.
  // ARC cleanup/weak/lifetime implementation anchor: the handoff
  // continues to be semantic-only while preserving the packets lowering now
  // consumes for scope-exit cleanup ordering, weak current-property helper
  // usage, and deterministic block-capture lifetime cleanup.
  // ARC/block autorelease-return implementation anchor: the handoff
  // also preserves the explicit block-escape and return-autorelease packets,
  // while lane-C owns the branch-stable cleanup ordering needed to combine
  // them truthfully.
  // runnable-arc-runtime gate anchor: lane-E consumes this existing
  // source, semantic, lowering, and runtime proof chain and must not regress
  // into parser-only or metadata-only evidence.
  // runnable-arc-closeout anchor: the closeout matrix consumes this
  // unchanged proof chain plus integrated smoke instead of broadening parser
  // or semantic acceptance in lane E.
  result.arc_diagnostics_fixit_summary = result.integration_surface.arc_diagnostics_fixit_summary;
  result.deterministic_arc_diagnostics_fixit_handoff =
      result.type_metadata_handoff.arc_diagnostics_fixit_summary.deterministic &&
      result.integration_surface.arc_diagnostics_fixit_summary.deterministic &&
      IsEquivalentArcDiagnosticsFixitSummary(
          result.integration_surface.arc_diagnostics_fixit_summary,
          result.type_metadata_handoff.arc_diagnostics_fixit_summary) &&
      result.type_metadata_handoff.arc_diagnostics_fixit_summary.ownership_arc_fixit_available_sites <=
          result.type_metadata_handoff.arc_diagnostics_fixit_summary.ownership_arc_diagnostic_candidate_sites +
              result.type_metadata_handoff.arc_diagnostics_fixit_summary.contract_violation_sites &&
      result.type_metadata_handoff.arc_diagnostics_fixit_summary.ownership_arc_profiled_sites <=
          result.type_metadata_handoff.arc_diagnostics_fixit_summary.ownership_arc_diagnostic_candidate_sites +
              result.type_metadata_handoff.arc_diagnostics_fixit_summary.contract_violation_sites &&
      result.type_metadata_handoff.arc_diagnostics_fixit_summary
              .ownership_arc_weak_unowned_conflict_diagnostic_sites <=
          result.type_metadata_handoff.arc_diagnostics_fixit_summary.ownership_arc_diagnostic_candidate_sites +
              result.type_metadata_handoff.arc_diagnostics_fixit_summary.contract_violation_sites &&
      result.type_metadata_handoff.arc_diagnostics_fixit_summary.ownership_arc_empty_fixit_hint_sites <=
          result.type_metadata_handoff.arc_diagnostics_fixit_summary.ownership_arc_fixit_available_sites +
              result.type_metadata_handoff.arc_diagnostics_fixit_summary.contract_violation_sites;
  result.autoreleasepool_scope_summary = result.integration_surface.autoreleasepool_scope_summary;
  result.deterministic_autoreleasepool_scope_handoff =
      result.type_metadata_handoff.autoreleasepool_scope_summary.deterministic &&
      result.integration_surface.autoreleasepool_scope_summary.deterministic &&
      IsEquivalentAutoreleasePoolScopeSummary(
          result.integration_surface.autoreleasepool_scope_summary,
          result.type_metadata_handoff.autoreleasepool_scope_summary) &&
      result.type_metadata_handoff.autoreleasepool_scope_summary.scope_symbolized_sites <=
          result.type_metadata_handoff.autoreleasepool_scope_summary.scope_sites &&
      result.type_metadata_handoff.autoreleasepool_scope_summary.contract_violation_sites <=
          result.type_metadata_handoff.autoreleasepool_scope_summary.scope_sites &&
      (result.type_metadata_handoff.autoreleasepool_scope_summary.scope_sites > 0u ||
       result.type_metadata_handoff.autoreleasepool_scope_summary.max_scope_depth == 0u) &&
      result.type_metadata_handoff.autoreleasepool_scope_summary.max_scope_depth <=
          static_cast<unsigned>(result.type_metadata_handoff.autoreleasepool_scope_summary.scope_sites);
  result.atomic_memory_order_mapping = BuildAtomicMemoryOrderMappingSummary(*input.program);
  result.deterministic_atomic_memory_order_mapping = result.atomic_memory_order_mapping.deterministic;
  result.vector_type_lowering = BuildVectorTypeLoweringSummary(result.integration_surface);
  result.deterministic_vector_type_lowering = result.vector_type_lowering.deterministic;
  result.type_metadata_publication_record =
      BuildObjc3SemaTypeMetadataPublicationRecord(
          input,
          result.integration_surface,
          result.type_metadata_handoff,
          result.deterministic_type_metadata_handoff,
          result.deterministic_interface_implementation_handoff,
          result.deterministic_protocol_category_composition_handoff,
          result.deterministic_class_protocol_category_linking_handoff,
          result.deterministic_selector_normalization_handoff);
  result.deterministic_type_metadata_publication_record =
      IsReadyObjc3SemaTypeMetadataPublicationRecord(
          result.type_metadata_publication_record);
  PublishObjc3SemaPassFlowSummary(
      handoff_publication,
      diagnostics_run,
      result);

  result.parity_surface.parser_sema_conformance_matrix =
      result.parser_sema_conformance_matrix;
  result.parity_surface.parser_sema_conformance_corpus =
      result.parser_sema_conformance_corpus;
  result.parity_surface.parser_sema_performance_quality_guardrails =
      result.parser_sema_performance_quality_guardrails;
  result.parity_surface.parser_sema_cross_lane_integration_sync =
      result.parser_sema_cross_lane_integration_sync;
  result.parity_surface.parser_sema_docs_runbook_sync =
      result.parser_sema_docs_runbook_sync;
  result.parity_surface.parser_sema_release_candidate_replay_dry_run =
      result.parser_sema_release_candidate_replay_dry_run;
  result.parity_surface.parser_sema_advanced_core_shard1 =
      result.parser_sema_advanced_core_shard1;
  result.parity_surface.parser_sema_advanced_contract_rejection_shard1 =
      result.parser_sema_advanced_contract_rejection_shard1;
  result.parity_surface.parser_sema_advanced_diagnostics_shard1 =
      result.parser_sema_advanced_diagnostics_shard1;
  result.parity_surface.parser_sema_advanced_conformance_shard1 =
      result.parser_sema_advanced_conformance_shard1;
  result.parity_surface.parser_sema_advanced_integration_shard1 =
      result.parser_sema_advanced_integration_shard1;
  result.parity_surface.parser_sema_advanced_performance_shard1 =
      result.parser_sema_advanced_performance_shard1;
  result.parity_surface.parser_sema_advanced_core_shard2 =
      result.parser_sema_advanced_core_shard2;
  result.parity_surface.parser_sema_advanced_contract_rejection_shard2 =
      result.parser_sema_advanced_contract_rejection_shard2;
  result.parity_surface.parser_sema_advanced_diagnostics_shard2 =
      result.parser_sema_advanced_diagnostics_shard2;
  result.parity_surface.parser_sema_integration_closeout_signoff =
      result.parser_sema_integration_closeout_signoff;
  result.parity_surface.sema_pass_flow_summary = result.sema_pass_flow_summary;
  result.parity_surface.diagnostics_publication_record =
      result.diagnostics_publication_record;
  result.parity_surface.pass_flow_recovery_record =
      result.pass_flow_recovery_record;
  result.parity_surface.pass_manager_publication_record =
      result.pass_manager_publication_record;
  result.parity_surface.type_metadata_publication_record =
      result.type_metadata_publication_record;
  result.parity_surface.diagnostics_after_pass = result.diagnostics_after_pass;
  result.parity_surface.diagnostics_emitted_by_pass = result.diagnostics_emitted_by_pass;
  result.parity_surface.diagnostics_total = result.diagnostics.size();
  result.parity_surface.globals_total = result.integration_surface.globals.size();
  result.parity_surface.functions_total = result.integration_surface.functions.size();
  result.parity_surface.interfaces_total = result.integration_surface.interfaces.size();
  result.parity_surface.implementations_total = result.integration_surface.implementations.size();
  result.parity_surface.type_metadata_global_entries = result.type_metadata_handoff.global_names_lexicographic.size();
  result.parity_surface.type_metadata_function_entries = result.type_metadata_handoff.functions_lexicographic.size();
  result.parity_surface.type_metadata_interface_entries = result.type_metadata_handoff.interfaces_lexicographic.size();
  result.parity_surface.type_metadata_implementation_entries =
      result.type_metadata_handoff.implementations_lexicographic.size();
  result.parity_surface.interface_implementation_summary = result.type_metadata_handoff.interface_implementation_summary;
  result.parity_surface.bootstrap_legality_failure_contract_summary =
      result.integration_surface.bootstrap_legality_failure_contract_summary;
  result.parity_surface.bootstrap_legality_semantics_summary =
      result.integration_surface.bootstrap_legality_semantics_summary;
  result.parity_surface.bootstrap_failure_restart_semantics_summary =
      result.integration_surface.bootstrap_failure_restart_semantics_summary;
  result.parity_surface.compatibility_strictness_claim_semantics_summary =
      result.integration_surface.compatibility_strictness_claim_semantics_summary;
  result.parity_surface.interface_method_symbols_total =
      result.parity_surface.interface_implementation_summary.interface_method_symbols;
  result.parity_surface.implementation_method_symbols_total =
      result.parity_surface.interface_implementation_summary.implementation_method_symbols;
  result.parity_surface.linked_implementation_symbols_total =
      result.parity_surface.interface_implementation_summary.linked_implementation_symbols;
  result.parity_surface.protocol_category_composition_summary =
      result.type_metadata_handoff.protocol_category_composition_summary;
  result.parity_surface.protocol_composition_sites_total =
      result.parity_surface.protocol_category_composition_summary.protocol_composition_sites;
  result.parity_surface.protocol_composition_symbols_total =
      result.parity_surface.protocol_category_composition_summary.protocol_composition_symbols;
  result.parity_surface.category_composition_sites_total =
      result.parity_surface.protocol_category_composition_summary.category_composition_sites;
  result.parity_surface.category_composition_symbols_total =
      result.parity_surface.protocol_category_composition_summary.category_composition_symbols;
  result.parity_surface.invalid_protocol_composition_sites_total =
      result.parity_surface.protocol_category_composition_summary.invalid_protocol_composition_sites;
  result.parity_surface.class_protocol_category_linking_summary =
      result.type_metadata_handoff.class_protocol_category_linking_summary;
  result.parity_surface.selector_normalization_summary = result.type_metadata_handoff.selector_normalization_summary;
  result.parity_surface.selector_normalization_methods_total =
      result.parity_surface.selector_normalization_summary.methods_total;
  result.parity_surface.selector_normalization_normalized_methods_total =
      result.parity_surface.selector_normalization_summary.normalized_methods;
  result.parity_surface.selector_normalization_piece_entries_total =
      result.parity_surface.selector_normalization_summary.selector_piece_entries;
  result.parity_surface.selector_normalization_parameter_piece_entries_total =
      result.parity_surface.selector_normalization_summary.selector_parameter_piece_entries;
  result.parity_surface.selector_normalization_pieceless_methods_total =
      result.parity_surface.selector_normalization_summary.selector_pieceless_methods;
  result.parity_surface.selector_normalization_spelling_mismatches_total =
      result.parity_surface.selector_normalization_summary.selector_spelling_mismatches;
  result.parity_surface.selector_normalization_arity_mismatches_total =
      result.parity_surface.selector_normalization_summary.selector_arity_mismatches;
  result.parity_surface.selector_normalization_parameter_linkage_mismatches_total =
      result.parity_surface.selector_normalization_summary.selector_parameter_linkage_mismatches;
  result.parity_surface.selector_normalization_flag_mismatches_total =
      result.parity_surface.selector_normalization_summary.selector_normalization_flag_mismatches;
  result.parity_surface.selector_normalization_missing_keyword_pieces_total =
      result.parity_surface.selector_normalization_summary.selector_missing_keyword_pieces;
  result.parity_surface.property_attribute_summary = result.type_metadata_handoff.property_attribute_summary;
  result.parity_surface.property_attribute_properties_total =
      result.parity_surface.property_attribute_summary.properties_total;
  result.parity_surface.property_attribute_entries_total =
      result.parity_surface.property_attribute_summary.attribute_entries;
  result.parity_surface.property_attribute_readonly_modifiers_total =
      result.parity_surface.property_attribute_summary.readonly_modifiers;
  result.parity_surface.property_attribute_readwrite_modifiers_total =
      result.parity_surface.property_attribute_summary.readwrite_modifiers;
  result.parity_surface.property_attribute_atomic_modifiers_total =
      result.parity_surface.property_attribute_summary.atomic_modifiers;
  result.parity_surface.property_attribute_nonatomic_modifiers_total =
      result.parity_surface.property_attribute_summary.nonatomic_modifiers;
  result.parity_surface.property_attribute_copy_modifiers_total =
      result.parity_surface.property_attribute_summary.copy_modifiers;
  result.parity_surface.property_attribute_strong_modifiers_total =
      result.parity_surface.property_attribute_summary.strong_modifiers;
  result.parity_surface.property_attribute_weak_modifiers_total =
      result.parity_surface.property_attribute_summary.weak_modifiers;
  result.parity_surface.property_attribute_assign_modifiers_total =
      result.parity_surface.property_attribute_summary.assign_modifiers;
  result.parity_surface.property_attribute_getter_modifiers_total =
      result.parity_surface.property_attribute_summary.getter_modifiers;
  result.parity_surface.property_attribute_setter_modifiers_total =
      result.parity_surface.property_attribute_summary.setter_modifiers;
  result.parity_surface.property_attribute_invalid_attribute_entries_total =
      result.parity_surface.property_attribute_summary.invalid_attribute_entries;
  result.parity_surface.property_attribute_contract_violations_total =
      result.parity_surface.property_attribute_summary.property_contract_violations;
  result.parity_surface.type_annotation_surface_summary = result.type_metadata_handoff.type_annotation_surface_summary;
  result.parity_surface.type_annotation_generic_suffix_sites_total =
      result.parity_surface.type_annotation_surface_summary.generic_suffix_sites;
  result.parity_surface.type_annotation_pointer_declarator_sites_total =
      result.parity_surface.type_annotation_surface_summary.pointer_declarator_sites;
  result.parity_surface.type_annotation_nullability_suffix_sites_total =
      result.parity_surface.type_annotation_surface_summary.nullability_suffix_sites;
  result.parity_surface.type_annotation_ownership_qualifier_sites_total =
      result.parity_surface.type_annotation_surface_summary.ownership_qualifier_sites;
  result.parity_surface.type_annotation_object_pointer_type_sites_total =
      result.parity_surface.type_annotation_surface_summary.object_pointer_type_sites;
  result.parity_surface.type_annotation_invalid_generic_suffix_sites_total =
      result.parity_surface.type_annotation_surface_summary.invalid_generic_suffix_sites;
  result.parity_surface.type_annotation_invalid_pointer_declarator_sites_total =
      result.parity_surface.type_annotation_surface_summary.invalid_pointer_declarator_sites;
  result.parity_surface.type_annotation_invalid_nullability_suffix_sites_total =
      result.parity_surface.type_annotation_surface_summary.invalid_nullability_suffix_sites;
  result.parity_surface.type_annotation_invalid_ownership_qualifier_sites_total =
      result.parity_surface.type_annotation_surface_summary.invalid_ownership_qualifier_sites;
  result.parity_surface.lightweight_generic_constraint_summary =
      result.type_metadata_handoff.lightweight_generic_constraint_summary;
  result.parity_surface.lightweight_generic_constraint_sites_total =
      result.parity_surface.lightweight_generic_constraint_summary.generic_constraint_sites;
  result.parity_surface.lightweight_generic_constraint_generic_suffix_sites_total =
      result.parity_surface.lightweight_generic_constraint_summary.generic_suffix_sites;
  result.parity_surface.lightweight_generic_constraint_object_pointer_type_sites_total =
      result.parity_surface.lightweight_generic_constraint_summary.object_pointer_type_sites;
  result.parity_surface.lightweight_generic_constraint_terminated_generic_suffix_sites_total =
      result.parity_surface.lightweight_generic_constraint_summary.terminated_generic_suffix_sites;
  result.parity_surface.lightweight_generic_constraint_pointer_declarator_sites_total =
      result.parity_surface.lightweight_generic_constraint_summary.pointer_declarator_sites;
  result.parity_surface.lightweight_generic_constraint_normalized_sites_total =
      result.parity_surface.lightweight_generic_constraint_summary.normalized_constraint_sites;
  result.parity_surface.lightweight_generic_constraint_contract_violation_sites_total =
      result.parity_surface.lightweight_generic_constraint_summary.contract_violation_sites;
  result.parity_surface.nullability_flow_warning_precision_summary =
      result.type_metadata_handoff.nullability_flow_warning_precision_summary;
  result.parity_surface.nullability_flow_sites_total =
      result.parity_surface.nullability_flow_warning_precision_summary.nullability_flow_sites;
  result.parity_surface.nullability_flow_object_pointer_type_sites_total =
      result.parity_surface.nullability_flow_warning_precision_summary.object_pointer_type_sites;
  result.parity_surface.nullability_flow_nullability_suffix_sites_total =
      result.parity_surface.nullability_flow_warning_precision_summary.nullability_suffix_sites;
  result.parity_surface.nullability_flow_nullable_suffix_sites_total =
      result.parity_surface.nullability_flow_warning_precision_summary.nullable_suffix_sites;
  result.parity_surface.nullability_flow_nonnull_suffix_sites_total =
      result.parity_surface.nullability_flow_warning_precision_summary.nonnull_suffix_sites;
  result.parity_surface.nullability_flow_normalized_sites_total =
      result.parity_surface.nullability_flow_warning_precision_summary.normalized_sites;
  result.parity_surface.nullability_flow_contract_violation_sites_total =
      result.parity_surface.nullability_flow_warning_precision_summary.contract_violation_sites;
  result.parity_surface.protocol_qualified_object_type_summary =
      result.type_metadata_handoff.protocol_qualified_object_type_summary;
  result.parity_surface.protocol_qualified_object_type_sites_total =
      result.parity_surface.protocol_qualified_object_type_summary.protocol_qualified_object_type_sites;
  result.parity_surface.protocol_qualified_object_type_protocol_composition_sites_total =
      result.parity_surface.protocol_qualified_object_type_summary.protocol_composition_sites;
  result.parity_surface.protocol_qualified_object_type_object_pointer_type_sites_total =
      result.parity_surface.protocol_qualified_object_type_summary.object_pointer_type_sites;
  result.parity_surface.protocol_qualified_object_type_terminated_protocol_composition_sites_total =
      result.parity_surface.protocol_qualified_object_type_summary.terminated_protocol_composition_sites;
  result.parity_surface.protocol_qualified_object_type_pointer_declarator_sites_total =
      result.parity_surface.protocol_qualified_object_type_summary.pointer_declarator_sites;
  result.parity_surface.protocol_qualified_object_type_normalized_protocol_composition_sites_total =
      result.parity_surface.protocol_qualified_object_type_summary.normalized_protocol_composition_sites;
  result.parity_surface.protocol_qualified_object_type_contract_violation_sites_total =
      result.parity_surface.protocol_qualified_object_type_summary.contract_violation_sites;
  result.parity_surface.variance_bridge_cast_summary = result.type_metadata_handoff.variance_bridge_cast_summary;
  result.parity_surface.variance_bridge_cast_sites_total =
      result.parity_surface.variance_bridge_cast_summary.variance_bridge_cast_sites;
  result.parity_surface.variance_bridge_cast_protocol_composition_sites_total =
      result.parity_surface.variance_bridge_cast_summary.protocol_composition_sites;
  result.parity_surface.variance_bridge_cast_ownership_qualifier_sites_total =
      result.parity_surface.variance_bridge_cast_summary.ownership_qualifier_sites;
  result.parity_surface.variance_bridge_cast_object_pointer_type_sites_total =
      result.parity_surface.variance_bridge_cast_summary.object_pointer_type_sites;
  result.parity_surface.variance_bridge_cast_pointer_declarator_sites_total =
      result.parity_surface.variance_bridge_cast_summary.pointer_declarator_sites;
  result.parity_surface.variance_bridge_cast_normalized_sites_total =
      result.parity_surface.variance_bridge_cast_summary.normalized_sites;
  result.parity_surface.variance_bridge_cast_contract_violation_sites_total =
      result.parity_surface.variance_bridge_cast_summary.contract_violation_sites;
  result.parity_surface.generic_metadata_abi_summary = result.type_metadata_handoff.generic_metadata_abi_summary;
  result.parity_surface.generic_metadata_abi_sites_total =
      result.parity_surface.generic_metadata_abi_summary.generic_metadata_abi_sites;
  result.parity_surface.generic_metadata_abi_generic_suffix_sites_total =
      result.parity_surface.generic_metadata_abi_summary.generic_suffix_sites;
  result.parity_surface.generic_metadata_abi_protocol_composition_sites_total =
      result.parity_surface.generic_metadata_abi_summary.protocol_composition_sites;
  result.parity_surface.generic_metadata_abi_ownership_qualifier_sites_total =
      result.parity_surface.generic_metadata_abi_summary.ownership_qualifier_sites;
  result.parity_surface.generic_metadata_abi_object_pointer_type_sites_total =
      result.parity_surface.generic_metadata_abi_summary.object_pointer_type_sites;
  result.parity_surface.generic_metadata_abi_pointer_declarator_sites_total =
      result.parity_surface.generic_metadata_abi_summary.pointer_declarator_sites;
  result.parity_surface.generic_metadata_abi_normalized_sites_total =
      result.parity_surface.generic_metadata_abi_summary.normalized_sites;
  result.parity_surface.generic_metadata_abi_contract_violation_sites_total =
      result.parity_surface.generic_metadata_abi_summary.contract_violation_sites;
  result.parity_surface.module_import_graph_summary =
      result.type_metadata_handoff.module_import_graph_summary;
  result.parity_surface.module_import_graph_sites_total =
      result.parity_surface.module_import_graph_summary.module_import_graph_sites;
  result.parity_surface.module_import_graph_import_edge_candidate_sites_total =
      result.parity_surface.module_import_graph_summary.import_edge_candidate_sites;
  result.parity_surface.module_import_graph_namespace_segment_sites_total =
      result.parity_surface.module_import_graph_summary.namespace_segment_sites;
  result.parity_surface.module_import_graph_object_pointer_type_sites_total =
      result.parity_surface.module_import_graph_summary.object_pointer_type_sites;
  result.parity_surface.module_import_graph_pointer_declarator_sites_total =
      result.parity_surface.module_import_graph_summary.pointer_declarator_sites;
  result.parity_surface.module_import_graph_normalized_sites_total =
      result.parity_surface.module_import_graph_summary.normalized_sites;
  result.parity_surface.module_import_graph_contract_violation_sites_total =
      result.parity_surface.module_import_graph_summary.contract_violation_sites;
  result.parity_surface.namespace_collision_shadowing_summary =
      result.type_metadata_handoff.namespace_collision_shadowing_summary;
  result.parity_surface.namespace_collision_shadowing_sites_total =
      result.parity_surface.namespace_collision_shadowing_summary
          .namespace_collision_shadowing_sites;
  result.parity_surface.namespace_collision_shadowing_namespace_segment_sites_total =
      result.parity_surface.namespace_collision_shadowing_summary.namespace_segment_sites;
  result.parity_surface
      .namespace_collision_shadowing_import_edge_candidate_sites_total =
      result.parity_surface.namespace_collision_shadowing_summary
          .import_edge_candidate_sites;
  result.parity_surface.namespace_collision_shadowing_object_pointer_type_sites_total =
      result.parity_surface.namespace_collision_shadowing_summary
          .object_pointer_type_sites;
  result.parity_surface.namespace_collision_shadowing_pointer_declarator_sites_total =
      result.parity_surface.namespace_collision_shadowing_summary
          .pointer_declarator_sites;
  result.parity_surface.namespace_collision_shadowing_normalized_sites_total =
      result.parity_surface.namespace_collision_shadowing_summary.normalized_sites;
  result.parity_surface
      .namespace_collision_shadowing_contract_violation_sites_total =
      result.parity_surface.namespace_collision_shadowing_summary
          .contract_violation_sites;
  result.parity_surface.public_private_api_partition_summary =
      result.type_metadata_handoff.public_private_api_partition_summary;
  result.parity_surface.public_private_api_partition_sites_total =
      result.parity_surface.public_private_api_partition_summary
          .public_private_api_partition_sites;
  result.parity_surface.public_private_api_partition_namespace_segment_sites_total =
      result.parity_surface.public_private_api_partition_summary
          .namespace_segment_sites;
  result.parity_surface
      .public_private_api_partition_import_edge_candidate_sites_total =
      result.parity_surface.public_private_api_partition_summary
          .import_edge_candidate_sites;
  result.parity_surface.public_private_api_partition_object_pointer_type_sites_total =
      result.parity_surface.public_private_api_partition_summary
          .object_pointer_type_sites;
  result.parity_surface.public_private_api_partition_pointer_declarator_sites_total =
      result.parity_surface.public_private_api_partition_summary
          .pointer_declarator_sites;
  result.parity_surface.public_private_api_partition_normalized_sites_total =
      result.parity_surface.public_private_api_partition_summary.normalized_sites;
  result.parity_surface.public_private_api_partition_contract_violation_sites_total =
      result.parity_surface.public_private_api_partition_summary
          .contract_violation_sites;
  result.parity_surface.incremental_module_cache_invalidation_summary =
      result.type_metadata_handoff.incremental_module_cache_invalidation_summary;
  result.parity_surface.incremental_module_cache_invalidation_sites_total =
      result.parity_surface.incremental_module_cache_invalidation_summary
          .incremental_module_cache_invalidation_sites;
  result.parity_surface
      .incremental_module_cache_invalidation_namespace_segment_sites_total =
      result.parity_surface.incremental_module_cache_invalidation_summary
          .namespace_segment_sites;
  result.parity_surface
      .incremental_module_cache_invalidation_import_edge_candidate_sites_total =
      result.parity_surface.incremental_module_cache_invalidation_summary
          .import_edge_candidate_sites;
  result.parity_surface
      .incremental_module_cache_invalidation_object_pointer_type_sites_total =
      result.parity_surface.incremental_module_cache_invalidation_summary
          .object_pointer_type_sites;
  result.parity_surface
      .incremental_module_cache_invalidation_pointer_declarator_sites_total =
      result.parity_surface.incremental_module_cache_invalidation_summary
          .pointer_declarator_sites;
  result.parity_surface.incremental_module_cache_invalidation_normalized_sites_total =
      result.parity_surface.incremental_module_cache_invalidation_summary
          .normalized_sites;
  result.parity_surface
      .incremental_module_cache_invalidation_cache_invalidation_candidate_sites_total =
      result.parity_surface.incremental_module_cache_invalidation_summary
          .cache_invalidation_candidate_sites;
  result.parity_surface
      .incremental_module_cache_invalidation_contract_violation_sites_total =
      result.parity_surface.incremental_module_cache_invalidation_summary
          .contract_violation_sites;
  result.parity_surface.cross_module_conformance_summary =
      result.type_metadata_handoff.cross_module_conformance_summary;
  result.parity_surface.cross_module_conformance_sites_total =
      result.parity_surface.cross_module_conformance_summary.cross_module_conformance_sites;
  result.parity_surface.cross_module_conformance_namespace_segment_sites_total =
      result.parity_surface.cross_module_conformance_summary.namespace_segment_sites;
  result.parity_surface.cross_module_conformance_import_edge_candidate_sites_total =
      result.parity_surface.cross_module_conformance_summary.import_edge_candidate_sites;
  result.parity_surface.cross_module_conformance_object_pointer_type_sites_total =
      result.parity_surface.cross_module_conformance_summary.object_pointer_type_sites;
  result.parity_surface.cross_module_conformance_pointer_declarator_sites_total =
      result.parity_surface.cross_module_conformance_summary.pointer_declarator_sites;
  result.parity_surface.cross_module_conformance_normalized_sites_total =
      result.parity_surface.cross_module_conformance_summary.normalized_sites;
  result.parity_surface.cross_module_conformance_cache_invalidation_candidate_sites_total =
      result.parity_surface.cross_module_conformance_summary
          .cache_invalidation_candidate_sites;
  result.parity_surface.cross_module_conformance_contract_violation_sites_total =
      result.parity_surface.cross_module_conformance_summary.contract_violation_sites;
  result.parity_surface.throws_propagation_summary =
      result.type_metadata_handoff.throws_propagation_summary;
  result.parity_surface.throws_propagation_sites_total =
      result.parity_surface.throws_propagation_summary.throws_propagation_sites;
  result.parity_surface.throws_propagation_namespace_segment_sites_total =
      result.parity_surface.throws_propagation_summary.namespace_segment_sites;
  result.parity_surface.throws_propagation_import_edge_candidate_sites_total =
      result.parity_surface.throws_propagation_summary.import_edge_candidate_sites;
  result.parity_surface.throws_propagation_object_pointer_type_sites_total =
      result.parity_surface.throws_propagation_summary.object_pointer_type_sites;
  result.parity_surface.throws_propagation_pointer_declarator_sites_total =
      result.parity_surface.throws_propagation_summary.pointer_declarator_sites;
  result.parity_surface.throws_propagation_normalized_sites_total =
      result.parity_surface.throws_propagation_summary.normalized_sites;
  result.parity_surface.throws_propagation_cache_invalidation_candidate_sites_total =
      result.parity_surface.throws_propagation_summary
          .cache_invalidation_candidate_sites;
  result.parity_surface.throws_propagation_contract_violation_sites_total =
      result.parity_surface.throws_propagation_summary.contract_violation_sites;
  result.parity_surface.async_continuation_summary =
      result.type_metadata_handoff.async_continuation_summary;
  result.parity_surface.async_continuation_sites_total =
      result.parity_surface.async_continuation_summary.async_continuation_sites;
  result.parity_surface.async_continuation_async_keyword_sites_total =
      result.parity_surface.async_continuation_summary.async_keyword_sites;
  result.parity_surface.async_continuation_async_function_sites_total =
      result.parity_surface.async_continuation_summary.async_function_sites;
  result.parity_surface.async_continuation_allocation_sites_total =
      result.parity_surface.async_continuation_summary
          .continuation_allocation_sites;
  result.parity_surface.async_continuation_resume_sites_total =
      result.parity_surface.async_continuation_summary.continuation_resume_sites;
  result.parity_surface.async_continuation_suspend_sites_total =
      result.parity_surface.async_continuation_summary
          .continuation_suspend_sites;
  result.parity_surface.async_continuation_state_machine_sites_total =
      result.parity_surface.async_continuation_summary.async_state_machine_sites;
  result.parity_surface.async_continuation_normalized_sites_total =
      result.parity_surface.async_continuation_summary.normalized_sites;
  result.parity_surface.async_continuation_gate_blocked_sites_total =
      result.parity_surface.async_continuation_summary.gate_blocked_sites;
  result.parity_surface.async_continuation_contract_violation_sites_total =
      result.parity_surface.async_continuation_summary.contract_violation_sites;
  result.parity_surface.actor_isolation_sendability_summary =
      result.type_metadata_handoff.actor_isolation_sendability_summary;
  result.parity_surface.actor_isolation_sendability_sites_total =
      result.parity_surface.actor_isolation_sendability_summary
          .actor_isolation_sendability_sites;
  result.parity_surface.actor_isolation_decl_sites_total =
      result.parity_surface.actor_isolation_sendability_summary
          .actor_isolation_decl_sites;
  result.parity_surface.actor_hop_sites_total =
      result.parity_surface.actor_isolation_sendability_summary.actor_hop_sites;
  result.parity_surface.sendable_annotation_sites_total =
      result.parity_surface.actor_isolation_sendability_summary
          .sendable_annotation_sites;
  result.parity_surface.non_sendable_crossing_sites_total =
      result.parity_surface.actor_isolation_sendability_summary
          .non_sendable_crossing_sites;
  result.parity_surface.actor_isolation_sendability_isolation_boundary_sites_total =
      result.parity_surface.actor_isolation_sendability_summary
          .isolation_boundary_sites;
  result.parity_surface.actor_isolation_sendability_normalized_sites_total =
      result.parity_surface.actor_isolation_sendability_summary.normalized_sites;
  result.parity_surface.actor_isolation_sendability_gate_blocked_sites_total =
      result.parity_surface.actor_isolation_sendability_summary
          .gate_blocked_sites;
  result.parity_surface.actor_isolation_sendability_contract_violation_sites_total =
      result.parity_surface.actor_isolation_sendability_summary
          .contract_violation_sites;
  result.parity_surface.task_runtime_cancellation_summary =
      result.type_metadata_handoff.task_runtime_cancellation_summary;
  result.parity_surface.task_runtime_cancellation_sites_total =
      result.parity_surface.task_runtime_cancellation_summary
          .task_runtime_interop_sites;
  result.parity_surface.task_runtime_cancellation_runtime_hook_sites_total =
      result.parity_surface.task_runtime_cancellation_summary
