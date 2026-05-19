#include "parser/frontend/objc3_parser_type_projection_internal.h"

namespace objc3c::parse {

void CopyObjc3MethodReturnTypeConcurrencyProjection(const FunctionDecl &source,
                                                    Objc3MethodDecl &target) {
  target.async_continuation_profile_is_normalized =
      source.async_continuation_profile_is_normalized;
  target.deterministic_async_continuation_handoff =
      source.deterministic_async_continuation_handoff;
  target.async_continuation_sites = source.async_continuation_sites;
  target.async_keyword_sites = source.async_keyword_sites;
  target.async_function_sites = source.async_function_sites;
  target.continuation_allocation_sites = source.continuation_allocation_sites;
  target.continuation_resume_sites = source.continuation_resume_sites;
  target.continuation_suspend_sites = source.continuation_suspend_sites;
  target.async_state_machine_sites = source.async_state_machine_sites;
  target.async_continuation_normalized_sites =
      source.async_continuation_normalized_sites;
  target.async_continuation_gate_blocked_sites =
      source.async_continuation_gate_blocked_sites;
  target.async_continuation_contract_violation_sites =
      source.async_continuation_contract_violation_sites;
  target.async_continuation_profile = source.async_continuation_profile;
  target.await_suspension_profile_is_normalized =
      source.await_suspension_profile_is_normalized;
  target.deterministic_await_suspension_handoff =
      source.deterministic_await_suspension_handoff;
  target.await_suspension_sites = source.await_suspension_sites;
  target.await_keyword_sites = source.await_keyword_sites;
  target.await_suspension_point_sites = source.await_suspension_point_sites;
  target.await_resume_sites = source.await_resume_sites;
  target.await_state_machine_sites = source.await_state_machine_sites;
  target.await_continuation_sites = source.await_continuation_sites;
  target.await_suspension_normalized_sites =
      source.await_suspension_normalized_sites;
  target.await_suspension_gate_blocked_sites =
      source.await_suspension_gate_blocked_sites;
  target.await_suspension_contract_violation_sites =
      source.await_suspension_contract_violation_sites;
  target.await_suspension_profile = source.await_suspension_profile;
  target.actor_isolation_sendability_profile_is_normalized =
      source.actor_isolation_sendability_profile_is_normalized;
  target.deterministic_actor_isolation_sendability_handoff =
      source.deterministic_actor_isolation_sendability_handoff;
  target.actor_isolation_sendability_sites =
      source.actor_isolation_sendability_sites;
  target.actor_isolation_decl_sites = source.actor_isolation_decl_sites;
  target.actor_hop_sites = source.actor_hop_sites;
  target.sendable_annotation_sites = source.sendable_annotation_sites;
  target.non_sendable_crossing_sites = source.non_sendable_crossing_sites;
  target.isolation_boundary_sites = source.isolation_boundary_sites;
  target.actor_isolation_sendability_normalized_sites =
      source.actor_isolation_sendability_normalized_sites;
  target.actor_isolation_sendability_gate_blocked_sites =
      source.actor_isolation_sendability_gate_blocked_sites;
  target.actor_isolation_sendability_contract_violation_sites =
      source.actor_isolation_sendability_contract_violation_sites;
  target.actor_isolation_sendability_profile =
      source.actor_isolation_sendability_profile;
  target.task_runtime_cancellation_profile_is_normalized =
      source.task_runtime_cancellation_profile_is_normalized;
  target.deterministic_task_runtime_cancellation_handoff =
      source.deterministic_task_runtime_cancellation_handoff;
  target.task_runtime_interop_sites = source.task_runtime_interop_sites;
  target.runtime_hook_sites = source.runtime_hook_sites;
  target.cancellation_check_sites = source.cancellation_check_sites;
  target.cancellation_handler_sites = source.cancellation_handler_sites;
  target.suspension_point_sites = source.suspension_point_sites;
  target.cancellation_propagation_sites =
      source.cancellation_propagation_sites;
  target.task_runtime_normalized_sites = source.task_runtime_normalized_sites;
  target.task_runtime_gate_blocked_sites = source.task_runtime_gate_blocked_sites;
  target.task_runtime_contract_violation_sites =
      source.task_runtime_contract_violation_sites;
  target.task_runtime_cancellation_normalized_sites =
      source.task_runtime_cancellation_normalized_sites;
  target.task_runtime_cancellation_gate_blocked_sites =
      source.task_runtime_cancellation_gate_blocked_sites;
  target.task_runtime_cancellation_contract_violation_sites =
      source.task_runtime_cancellation_contract_violation_sites;
  target.task_runtime_cancellation_profile =
      source.task_runtime_cancellation_profile;
  target.concurrency_replay_race_guard_profile_is_normalized =
      source.concurrency_replay_race_guard_profile_is_normalized;
  target.deterministic_concurrency_replay_race_guard_handoff =
      source.deterministic_concurrency_replay_race_guard_handoff;
  target.concurrency_replay_race_guard_sites =
      source.concurrency_replay_race_guard_sites;
  target.concurrency_replay_sites = source.concurrency_replay_sites;
  target.replay_proof_sites = source.replay_proof_sites;
  target.race_guard_sites = source.race_guard_sites;
  target.task_handoff_sites = source.task_handoff_sites;
  target.actor_isolation_sites = source.actor_isolation_sites;
  target.deterministic_schedule_sites = source.deterministic_schedule_sites;
  target.concurrency_replay_guard_blocked_sites =
      source.concurrency_replay_guard_blocked_sites;
  target.concurrency_replay_contract_violation_sites =
      source.concurrency_replay_contract_violation_sites;
  target.concurrency_replay_race_guard_profile =
      source.concurrency_replay_race_guard_profile;
}

}  // namespace objc3c::parse
