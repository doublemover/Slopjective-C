          handoff.async_continuation_summary.deterministic &&
          handoff.async_continuation_summary.async_continuation_sites ==
              async_continuation_summary.async_continuation_sites &&
          handoff.async_continuation_summary.async_keyword_sites ==
              async_continuation_summary.async_keyword_sites &&
          handoff.async_continuation_summary.async_function_sites ==
              async_continuation_summary.async_function_sites &&
          handoff.async_continuation_summary.continuation_allocation_sites ==
              async_continuation_summary.continuation_allocation_sites &&
          handoff.async_continuation_summary.continuation_resume_sites ==
              async_continuation_summary.continuation_resume_sites &&
          handoff.async_continuation_summary.continuation_suspend_sites ==
              async_continuation_summary.continuation_suspend_sites &&
          handoff.async_continuation_summary.async_state_machine_sites ==
              async_continuation_summary.async_state_machine_sites &&
          handoff.async_continuation_summary.normalized_sites ==
              async_continuation_summary.normalized_sites &&
          handoff.async_continuation_summary.gate_blocked_sites ==
              async_continuation_summary.gate_blocked_sites &&
          handoff.async_continuation_summary.contract_violation_sites ==
              async_continuation_summary.contract_violation_sites &&
          handoff.async_continuation_summary.async_keyword_sites <=
              handoff.async_continuation_summary.async_continuation_sites &&
          handoff.async_continuation_summary.async_function_sites <=
              handoff.async_continuation_summary.async_continuation_sites &&
          handoff.async_continuation_summary.continuation_allocation_sites <=
              handoff.async_continuation_summary.async_continuation_sites &&
          handoff.async_continuation_summary.continuation_resume_sites <=
              handoff.async_continuation_summary.async_continuation_sites &&
          handoff.async_continuation_summary.continuation_suspend_sites <=
              handoff.async_continuation_summary.async_continuation_sites &&
          handoff.async_continuation_summary.async_state_machine_sites <=
              handoff.async_continuation_summary.async_continuation_sites &&
          handoff.async_continuation_summary.normalized_sites <=
              handoff.async_continuation_summary.async_continuation_sites &&
          handoff.async_continuation_summary.gate_blocked_sites <=
              handoff.async_continuation_summary.async_continuation_sites &&
          handoff.async_continuation_summary.contract_violation_sites <=
              handoff.async_continuation_summary.async_continuation_sites &&
          handoff.async_continuation_summary.normalized_sites +
                  handoff.async_continuation_summary.gate_blocked_sites ==
              handoff.async_continuation_summary.async_continuation_sites &&
          handoff.await_lowering_suspension_state_lowering_summary.deterministic &&
          handoff.await_lowering_suspension_state_lowering_summary
                  .await_suspension_sites ==
              await_lowering_suspension_state_lowering_summary
                  .await_suspension_sites &&
          handoff.await_lowering_suspension_state_lowering_summary
                  .await_keyword_sites ==
              await_lowering_suspension_state_lowering_summary
                  .await_keyword_sites &&
          handoff.await_lowering_suspension_state_lowering_summary
                  .await_suspension_point_sites ==
              await_lowering_suspension_state_lowering_summary
                  .await_suspension_point_sites &&
          handoff.await_lowering_suspension_state_lowering_summary
                  .await_resume_sites ==
              await_lowering_suspension_state_lowering_summary
                  .await_resume_sites &&
          handoff.await_lowering_suspension_state_lowering_summary
                  .await_state_machine_sites ==
              await_lowering_suspension_state_lowering_summary
                  .await_state_machine_sites &&
          handoff.await_lowering_suspension_state_lowering_summary
                  .await_continuation_sites ==
              await_lowering_suspension_state_lowering_summary
                  .await_continuation_sites &&
          handoff.await_lowering_suspension_state_lowering_summary
                  .normalized_sites ==
              await_lowering_suspension_state_lowering_summary.normalized_sites &&
          handoff.await_lowering_suspension_state_lowering_summary
                  .gate_blocked_sites ==
              await_lowering_suspension_state_lowering_summary
                  .gate_blocked_sites &&
          handoff.await_lowering_suspension_state_lowering_summary
                  .contract_violation_sites ==
              await_lowering_suspension_state_lowering_summary
                  .contract_violation_sites &&
          handoff.await_lowering_suspension_state_lowering_summary
                  .await_keyword_sites <=
              handoff.await_lowering_suspension_state_lowering_summary
                  .await_suspension_sites &&
          handoff.await_lowering_suspension_state_lowering_summary
                  .await_suspension_point_sites <=
              handoff.await_lowering_suspension_state_lowering_summary
                  .await_suspension_sites &&
          handoff.await_lowering_suspension_state_lowering_summary
                  .await_resume_sites <=
              handoff.await_lowering_suspension_state_lowering_summary
                  .await_suspension_sites &&
          handoff.await_lowering_suspension_state_lowering_summary
                  .await_state_machine_sites <=
              handoff.await_lowering_suspension_state_lowering_summary
                  .await_suspension_point_sites &&
          handoff.await_lowering_suspension_state_lowering_summary
                  .await_continuation_sites <=
              handoff.await_lowering_suspension_state_lowering_summary
                  .await_suspension_point_sites &&
          handoff.await_lowering_suspension_state_lowering_summary
                  .normalized_sites <=
              handoff.await_lowering_suspension_state_lowering_summary
                  .await_suspension_sites &&
          handoff.await_lowering_suspension_state_lowering_summary
                  .gate_blocked_sites <=
              handoff.await_lowering_suspension_state_lowering_summary
                  .await_suspension_sites &&
          handoff.await_lowering_suspension_state_lowering_summary
                  .contract_violation_sites <=
              handoff.await_lowering_suspension_state_lowering_summary
                  .await_suspension_sites &&
          handoff.await_lowering_suspension_state_lowering_summary
                  .normalized_sites +
                  handoff.await_lowering_suspension_state_lowering_summary
                      .gate_blocked_sites ==
              handoff.await_lowering_suspension_state_lowering_summary
                  .await_suspension_sites &&
          handoff.actor_isolation_sendability_summary.deterministic &&
          handoff.actor_isolation_sendability_summary
                  .actor_isolation_sendability_sites ==
              actor_isolation_sendability_summary
                  .actor_isolation_sendability_sites &&
          handoff.actor_isolation_sendability_summary
                  .actor_isolation_decl_sites ==
              actor_isolation_sendability_summary.actor_isolation_decl_sites &&
          handoff.actor_isolation_sendability_summary.actor_hop_sites ==
              actor_isolation_sendability_summary.actor_hop_sites &&
          handoff.actor_isolation_sendability_summary
                  .sendable_annotation_sites ==
              actor_isolation_sendability_summary.sendable_annotation_sites &&
          handoff.actor_isolation_sendability_summary
                  .non_sendable_crossing_sites ==
              actor_isolation_sendability_summary.non_sendable_crossing_sites &&
          handoff.actor_isolation_sendability_summary
                  .isolation_boundary_sites ==
              actor_isolation_sendability_summary.isolation_boundary_sites &&
          handoff.actor_isolation_sendability_summary.normalized_sites ==
              actor_isolation_sendability_summary.normalized_sites &&
          handoff.actor_isolation_sendability_summary.gate_blocked_sites ==
              actor_isolation_sendability_summary.gate_blocked_sites &&
          handoff.actor_isolation_sendability_summary
                  .contract_violation_sites ==
              actor_isolation_sendability_summary.contract_violation_sites &&
          handoff.actor_isolation_sendability_summary
                  .actor_isolation_decl_sites <=
              handoff.actor_isolation_sendability_summary
                  .actor_isolation_sendability_sites &&
          handoff.actor_isolation_sendability_summary.actor_hop_sites <=
              handoff.actor_isolation_sendability_summary
                  .actor_isolation_sendability_sites &&
          handoff.actor_isolation_sendability_summary
                  .sendable_annotation_sites <=
              handoff.actor_isolation_sendability_summary
                  .actor_isolation_sendability_sites &&
          handoff.actor_isolation_sendability_summary
                  .non_sendable_crossing_sites <=
              handoff.actor_isolation_sendability_summary
                  .actor_isolation_sendability_sites &&
          handoff.actor_isolation_sendability_summary
                  .isolation_boundary_sites <=
              handoff.actor_isolation_sendability_summary
                  .actor_isolation_sendability_sites &&
          handoff.actor_isolation_sendability_summary.normalized_sites <=
              handoff.actor_isolation_sendability_summary
                  .actor_isolation_sendability_sites &&
          handoff.actor_isolation_sendability_summary.gate_blocked_sites <=
              handoff.actor_isolation_sendability_summary
                  .actor_isolation_sendability_sites &&
          handoff.actor_isolation_sendability_summary.gate_blocked_sites <=
              handoff.actor_isolation_sendability_summary
                  .non_sendable_crossing_sites &&
          handoff.actor_isolation_sendability_summary
                  .contract_violation_sites <=
              handoff.actor_isolation_sendability_summary
                  .actor_isolation_sendability_sites &&
          handoff.actor_isolation_sendability_summary.normalized_sites +
                  handoff.actor_isolation_sendability_summary.gate_blocked_sites ==
              handoff.actor_isolation_sendability_summary
                  .actor_isolation_sendability_sites &&
          handoff.task_runtime_cancellation_summary.deterministic &&
          handoff.task_runtime_cancellation_summary.task_runtime_interop_sites ==
              task_runtime_cancellation_summary.task_runtime_interop_sites &&
          handoff.task_runtime_cancellation_summary.runtime_hook_sites ==
              task_runtime_cancellation_summary.runtime_hook_sites &&
          handoff.task_runtime_cancellation_summary.cancellation_check_sites ==
              task_runtime_cancellation_summary.cancellation_check_sites &&
          handoff.task_runtime_cancellation_summary.cancellation_handler_sites ==
              task_runtime_cancellation_summary.cancellation_handler_sites &&
          handoff.task_runtime_cancellation_summary.suspension_point_sites ==
              task_runtime_cancellation_summary.suspension_point_sites &&
          handoff.task_runtime_cancellation_summary
                  .cancellation_propagation_sites ==
              task_runtime_cancellation_summary
                  .cancellation_propagation_sites &&
          handoff.task_runtime_cancellation_summary.normalized_sites ==
              task_runtime_cancellation_summary.normalized_sites &&
          handoff.task_runtime_cancellation_summary.gate_blocked_sites ==
              task_runtime_cancellation_summary.gate_blocked_sites &&
          handoff.task_runtime_cancellation_summary.contract_violation_sites ==
              task_runtime_cancellation_summary.contract_violation_sites &&
          handoff.task_runtime_cancellation_summary.runtime_hook_sites <=
              handoff.task_runtime_cancellation_summary
                  .task_runtime_interop_sites &&
          handoff.task_runtime_cancellation_summary.cancellation_check_sites <=
              handoff.task_runtime_cancellation_summary
                  .task_runtime_interop_sites &&
          handoff.task_runtime_cancellation_summary.cancellation_handler_sites <=
              handoff.task_runtime_cancellation_summary
                  .task_runtime_interop_sites &&
          handoff.task_runtime_cancellation_summary.suspension_point_sites <=
              handoff.task_runtime_cancellation_summary
                  .task_runtime_interop_sites &&
          handoff.task_runtime_cancellation_summary
                  .cancellation_propagation_sites <=
              handoff.task_runtime_cancellation_summary
                  .cancellation_check_sites &&
          handoff.task_runtime_cancellation_summary
                  .cancellation_propagation_sites <=
              handoff.task_runtime_cancellation_summary
                  .cancellation_handler_sites &&
          handoff.task_runtime_cancellation_summary.normalized_sites <=
              handoff.task_runtime_cancellation_summary
                  .task_runtime_interop_sites &&
          handoff.task_runtime_cancellation_summary.gate_blocked_sites <=
              handoff.task_runtime_cancellation_summary
                  .task_runtime_interop_sites &&
          handoff.task_runtime_cancellation_summary.gate_blocked_sites <=
              handoff.task_runtime_cancellation_summary
                  .cancellation_propagation_sites &&
          handoff.task_runtime_cancellation_summary.contract_violation_sites <=
              handoff.task_runtime_cancellation_summary
                  .task_runtime_interop_sites &&
          handoff.task_runtime_cancellation_summary.normalized_sites +
                  handoff.task_runtime_cancellation_summary.gate_blocked_sites ==
              handoff.task_runtime_cancellation_summary
                  .task_runtime_interop_sites &&
          handoff.concurrency_replay_race_guard_summary.deterministic &&
          handoff.concurrency_replay_race_guard_summary
                  .concurrency_replay_race_guard_sites ==
              concurrency_replay_race_guard_summary
                  .concurrency_replay_race_guard_sites &&
          handoff.concurrency_replay_race_guard_summary
                  .concurrency_replay_sites ==
              concurrency_replay_race_guard_summary.concurrency_replay_sites &&
          handoff.concurrency_replay_race_guard_summary.replay_proof_sites ==
              concurrency_replay_race_guard_summary.replay_proof_sites &&
          handoff.concurrency_replay_race_guard_summary.race_guard_sites ==
              concurrency_replay_race_guard_summary.race_guard_sites &&
          handoff.concurrency_replay_race_guard_summary.task_handoff_sites ==
              concurrency_replay_race_guard_summary.task_handoff_sites &&
          handoff.concurrency_replay_race_guard_summary.actor_isolation_sites ==
              concurrency_replay_race_guard_summary.actor_isolation_sites &&
          handoff.concurrency_replay_race_guard_summary
                  .deterministic_schedule_sites ==
              concurrency_replay_race_guard_summary
                  .deterministic_schedule_sites &&
          handoff.concurrency_replay_race_guard_summary.guard_blocked_sites ==
              concurrency_replay_race_guard_summary.guard_blocked_sites &&
          handoff.concurrency_replay_race_guard_summary
                  .contract_violation_sites ==
              concurrency_replay_race_guard_summary.contract_violation_sites &&
          handoff.concurrency_replay_race_guard_summary.concurrency_replay_sites <=
              handoff.concurrency_replay_race_guard_summary
                  .concurrency_replay_race_guard_sites &&
          handoff.concurrency_replay_race_guard_summary.replay_proof_sites <=
              handoff.concurrency_replay_race_guard_summary
                  .concurrency_replay_race_guard_sites &&
          handoff.concurrency_replay_race_guard_summary.race_guard_sites <=
              handoff.concurrency_replay_race_guard_summary
                  .concurrency_replay_race_guard_sites &&
          handoff.concurrency_replay_race_guard_summary.task_handoff_sites <=
              handoff.concurrency_replay_race_guard_summary
                  .concurrency_replay_race_guard_sites &&
          handoff.concurrency_replay_race_guard_summary.actor_isolation_sites <=
              handoff.concurrency_replay_race_guard_summary
                  .concurrency_replay_race_guard_sites &&
          handoff.concurrency_replay_race_guard_summary
                  .deterministic_schedule_sites <=
              handoff.concurrency_replay_race_guard_summary
                  .concurrency_replay_sites &&
          handoff.concurrency_replay_race_guard_summary.guard_blocked_sites <=
              handoff.concurrency_replay_race_guard_summary
                  .concurrency_replay_sites &&
          handoff.concurrency_replay_race_guard_summary
                  .contract_violation_sites <=
              handoff.concurrency_replay_race_guard_summary
                  .concurrency_replay_race_guard_sites &&
          handoff.concurrency_replay_race_guard_summary
                  .deterministic_schedule_sites +
                  handoff.concurrency_replay_race_guard_summary
                      .guard_blocked_sites ==
              handoff.concurrency_replay_race_guard_summary
                  .concurrency_replay_sites &&
