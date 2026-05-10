#include "sema/objc3_semantic_passes_body_validation_core_concurrency_async_await.inc"
#include "sema/objc3_semantic_passes_body_validation_core_concurrency_actor_sendability.inc"
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
