#include "sema/objc3_semantic_passes.h"

#include <algorithm>
#include <sstream>

Objc3ConcurrencyAsyncEffectSuspensionSemanticModelSummary
BuildConcurrencyAsyncEffectSuspensionSemanticModelSummary(
    const Objc3FrontendConcurrencyAsyncSourceClosureSummary &source_summary,
    const Objc3SemanticIntegrationSurface &surface) {
  Objc3ConcurrencyAsyncEffectSuspensionSemanticModelSummary summary;
  const Objc3AsyncContinuationSummary &async_summary =
      surface.async_continuation_summary;
  const Objc3AwaitLoweringSuspensionStateSummary &await_summary =
      surface.await_lowering_suspension_state_lowering_summary;
  const Objc3ActorIsolationSendabilitySummary &actor_summary =
      surface.actor_isolation_sendability_summary;
  const Objc3TaskRuntimeCancellationSummary &task_summary =
      surface.task_runtime_cancellation_summary;
  const Objc3ConcurrencyReplayRaceGuardSummary &replay_summary =
      surface.concurrency_replay_race_guard_summary;

  summary.async_continuation_sites = async_summary.async_continuation_sites;
  summary.async_keyword_sites = async_summary.async_keyword_sites;
  summary.async_function_sites = source_summary.async_function_sites;
  summary.async_method_sites = source_summary.async_method_sites;
  summary.executor_attribute_sites = source_summary.executor_attribute_sites;
  summary.executor_main_sites = source_summary.executor_main_sites;
  summary.executor_global_sites = source_summary.executor_global_sites;
  summary.executor_named_sites = source_summary.executor_named_sites;
  summary.continuation_allocation_sites =
      async_summary.continuation_allocation_sites;
  summary.continuation_resume_sites = async_summary.continuation_resume_sites;
  summary.continuation_suspend_sites = async_summary.continuation_suspend_sites;
  summary.async_state_machine_sites = async_summary.async_state_machine_sites;
  summary.await_suspension_sites = await_summary.await_suspension_sites;
  summary.await_keyword_sites = await_summary.await_keyword_sites;
  summary.await_expression_sites = source_summary.await_expression_sites;
  summary.await_suspension_point_sites =
      await_summary.await_suspension_point_sites;
  summary.await_resume_sites = await_summary.await_resume_sites;
  summary.await_state_machine_sites = await_summary.await_state_machine_sites;
  summary.await_continuation_sites = await_summary.await_continuation_sites;
  summary.actor_isolation_sendability_sites =
      actor_summary.actor_isolation_sendability_sites;
  summary.actor_isolation_decl_sites =
      actor_summary.actor_isolation_decl_sites;
  summary.actor_hop_sites = actor_summary.actor_hop_sites;
  summary.sendable_annotation_sites = actor_summary.sendable_annotation_sites;
  summary.non_sendable_crossing_sites =
      actor_summary.non_sendable_crossing_sites;
  summary.isolation_boundary_sites = actor_summary.isolation_boundary_sites;
  summary.task_runtime_interop_sites = task_summary.task_runtime_interop_sites;
  summary.runtime_hook_sites = task_summary.runtime_hook_sites;
  summary.cancellation_check_sites = task_summary.cancellation_check_sites;
  summary.cancellation_handler_sites = task_summary.cancellation_handler_sites;
  summary.suspension_point_sites = task_summary.suspension_point_sites;
  summary.cancellation_propagation_sites =
      task_summary.cancellation_propagation_sites;
  summary.concurrency_replay_race_guard_sites =
      replay_summary.concurrency_replay_race_guard_sites;
  summary.concurrency_replay_sites = replay_summary.concurrency_replay_sites;
  summary.replay_proof_sites = replay_summary.replay_proof_sites;
  summary.race_guard_sites = replay_summary.race_guard_sites;
  summary.task_handoff_sites = replay_summary.task_handoff_sites;
  summary.actor_isolation_sites = replay_summary.actor_isolation_sites;
  summary.deterministic_schedule_sites =
      replay_summary.deterministic_schedule_sites;

  summary.source_dependency_required = true;
  summary.async_declaration_semantics_landed =
      source_summary.async_function_source_supported &&
      source_summary.async_method_source_supported &&
      async_summary.deterministic &&
      summary.async_function_sites == source_summary.async_function_sites &&
      summary.async_method_sites == source_summary.async_method_sites &&
      summary.async_keyword_sites <= source_summary.async_keyword_sites &&
      ((summary.async_keyword_sites > 0u) ==
       (source_summary.async_keyword_sites > 0u));
  summary.executor_affinity_semantics_landed =
      source_summary.executor_attribute_source_supported &&
      summary.executor_attribute_sites == source_summary.executor_attribute_sites &&
      summary.executor_main_sites == source_summary.executor_main_sites &&
      summary.executor_global_sites == source_summary.executor_global_sites &&
      summary.executor_named_sites == source_summary.executor_named_sites;
  summary.await_legality_semantics_landed =
      source_summary.await_expression_source_supported &&
      await_summary.deterministic &&
      summary.await_expression_sites == source_summary.await_expression_sites &&
      summary.await_keyword_sites <= source_summary.await_keyword_sites &&
      ((summary.await_keyword_sites > 0u) ==
       (source_summary.await_keyword_sites > 0u));
  summary.continuation_profile_semantics_landed =
      async_summary.deterministic &&
      summary.async_keyword_sites <= summary.async_continuation_sites &&
      summary.continuation_allocation_sites <= summary.async_continuation_sites &&
      summary.continuation_resume_sites <= summary.async_continuation_sites &&
      summary.continuation_suspend_sites <= summary.async_continuation_sites &&
      summary.async_state_machine_sites <= summary.async_continuation_sites;
  summary.await_suspension_profile_semantics_landed =
      await_summary.deterministic &&
      summary.await_keyword_sites <= summary.await_suspension_sites &&
      summary.await_suspension_point_sites <= summary.await_suspension_sites &&
      summary.await_resume_sites <= summary.await_suspension_sites &&
      summary.await_state_machine_sites <= summary.await_suspension_sites &&
      summary.await_continuation_sites <= summary.await_suspension_sites;
  summary.actor_isolation_sendability_semantics_landed =
      actor_summary.deterministic;
  summary.task_runtime_cancellation_semantics_landed =
      task_summary.deterministic;
  summary.concurrency_replay_race_guard_semantics_landed =
      replay_summary.deterministic;
  summary.runnable_lowering_deferred = true;
  summary.executor_runtime_deferred = true;
  // Async/await semantic readiness must not be blocked on the later task
  // runtime cancellation lane. That runtime lane is surfaced here for
  // normalization closure, but B002 needs the continuation and suspension
  // semantics to stand on their own once the async/await profiles are sound.
  summary.deterministic =
      source_summary.deterministic_handoff && async_summary.deterministic &&
      await_summary.deterministic && actor_summary.deterministic &&
      replay_summary.deterministic &&
      summary.async_declaration_semantics_landed &&
      summary.executor_affinity_semantics_landed &&
      summary.await_legality_semantics_landed &&
      summary.continuation_profile_semantics_landed &&
      summary.await_suspension_profile_semantics_landed &&
      summary.actor_isolation_sendability_semantics_landed &&
      summary.concurrency_replay_race_guard_semantics_landed;
  summary.ready_for_lowering_and_runtime = summary.deterministic;

  std::ostringstream out;
  out << summary.contract_id
      << ";source-dependency=" << summary.frontend_dependency_contract_id
      << ";async-sites=" << summary.async_continuation_sites << ":"
      << summary.async_keyword_sites << ":" << summary.async_function_sites
      << ":" << summary.async_method_sites
      << ";executor-sites=" << summary.executor_attribute_sites << ":"
      << summary.executor_main_sites << ":" << summary.executor_global_sites
      << ":" << summary.executor_named_sites
      << ";continuation-sites=" << summary.continuation_allocation_sites << ":"
      << summary.continuation_resume_sites << ":"
      << summary.continuation_suspend_sites << ":"
      << summary.async_state_machine_sites
      << ";await-sites=" << summary.await_suspension_sites << ":"
      << summary.await_keyword_sites << ":" << summary.await_expression_sites
      << ":" << summary.await_suspension_point_sites << ":"
      << summary.await_resume_sites << ":"
      << summary.await_state_machine_sites << ":"
      << summary.await_continuation_sites
      << ";actor-sites=" << summary.actor_isolation_sendability_sites << ":"
      << summary.actor_isolation_decl_sites << ":" << summary.actor_hop_sites
      << ":" << summary.sendable_annotation_sites << ":"
      << summary.non_sendable_crossing_sites << ":"
      << summary.isolation_boundary_sites
      << ";task-sites=" << summary.task_runtime_interop_sites << ":"
      << summary.runtime_hook_sites << ":" << summary.cancellation_check_sites
      << ":" << summary.cancellation_handler_sites << ":"
      << summary.suspension_point_sites << ":"
      << summary.cancellation_propagation_sites
      << ";replay-sites=" << summary.concurrency_replay_race_guard_sites << ":"
      << summary.concurrency_replay_sites << ":" << summary.replay_proof_sites
      << ":" << summary.race_guard_sites << ":" << summary.task_handoff_sites
      << ":" << summary.actor_isolation_sites << ":"
      << summary.deterministic_schedule_sites
      << ";deterministic=" << (summary.deterministic ? "true" : "false");
  summary.replay_key = out.str();
  return summary;
}

Objc3ConcurrencyTaskExecutorCancellationSemanticModelSummary
BuildConcurrencyTaskExecutorCancellationSemanticModelSummary(
    const Objc3FrontendConcurrencyTaskGroupCancellationSourceClosureSummary
        &source_summary,
    const Objc3SemanticIntegrationSurface &surface) {
  Objc3ConcurrencyTaskExecutorCancellationSemanticModelSummary summary;
  const Objc3TaskRuntimeCancellationSummary &task_summary =
      surface.task_runtime_cancellation_summary;

  summary.async_callable_sites = source_summary.async_callable_sites;
  summary.executor_attribute_sites = source_summary.executor_attribute_sites;
  summary.task_creation_sites = source_summary.task_creation_sites;
  summary.task_group_scope_sites = source_summary.task_group_scope_sites;
  summary.task_group_add_task_sites = source_summary.task_group_add_task_sites;
  summary.task_group_wait_next_sites = source_summary.task_group_wait_next_sites;
  summary.task_group_cancel_all_sites =
      source_summary.task_group_cancel_all_sites;
  summary.task_runtime_interop_sites = task_summary.task_runtime_interop_sites;
  summary.runtime_hook_sites = task_summary.runtime_hook_sites;
  summary.cancellation_check_sites = task_summary.cancellation_check_sites;
  summary.cancellation_handler_sites = task_summary.cancellation_handler_sites;
  summary.suspension_point_sites = task_summary.suspension_point_sites;
  summary.cancellation_propagation_sites =
      task_summary.cancellation_propagation_sites;

  summary.source_dependency_required = true;
  summary.task_lifetime_semantics_landed =
      source_summary.task_creation_source_supported &&
      summary.task_creation_sites > 0u &&
      summary.task_creation_sites <= summary.runtime_hook_sites &&
      summary.task_creation_sites <= summary.task_runtime_interop_sites;
  summary.executor_affinity_semantics_landed =
      summary.executor_attribute_sites == source_summary.executor_attribute_sites &&
      summary.executor_attribute_sites <= summary.async_callable_sites;
  summary.cancellation_observation_semantics_landed =
      source_summary.cancellation_source_supported &&
      summary.cancellation_check_sites >= source_summary.cancellation_check_sites &&
      summary.cancellation_handler_sites >=
          source_summary.cancellation_handler_sites &&
      summary.cancellation_propagation_sites <=
          summary.cancellation_check_sites;
  summary.structured_task_legality_semantics_landed =
      source_summary.task_group_source_supported &&
      source_summary.deterministic_handoff &&
      summary.task_group_scope_sites > 0u &&
      summary.task_group_add_task_sites <=
          summary.task_group_scope_sites + summary.task_group_add_task_sites +
              summary.task_group_wait_next_sites +
              summary.task_group_cancel_all_sites &&
      summary.task_group_wait_next_sites <=
          std::max<std::size_t>(summary.suspension_point_sites, 1u) &&
      summary.task_group_cancel_all_sites <=
          summary.cancellation_check_sites;
  summary.runnable_lowering_deferred = true;
  summary.executor_runtime_deferred = true;
  summary.scheduler_runtime_deferred = true;
  summary.deterministic = source_summary.deterministic_handoff &&
                          summary.task_lifetime_semantics_landed &&
                          summary.executor_affinity_semantics_landed &&
                          summary.cancellation_observation_semantics_landed &&
                          summary.structured_task_legality_semantics_landed;
  summary.ready_for_lowering_and_runtime = summary.deterministic;

  std::ostringstream out;
  out << summary.contract_id
      << ";source-dependency=" << summary.frontend_dependency_contract_id
      << ";async-callables=" << summary.async_callable_sites
      << ";executor-sites=" << summary.executor_attribute_sites
      << ";task-creation-sites=" << summary.task_creation_sites
      << ";task-group-sites=" << summary.task_group_scope_sites << ":"
      << summary.task_group_add_task_sites << ":"
      << summary.task_group_wait_next_sites << ":"
      << summary.task_group_cancel_all_sites
      << ";task-runtime-sites=" << summary.task_runtime_interop_sites << ":"
      << summary.runtime_hook_sites << ":"
      << summary.cancellation_check_sites << ":"
      << summary.cancellation_handler_sites << ":"
      << summary.suspension_point_sites << ":"
      << summary.cancellation_propagation_sites
      << ";deterministic=" << (summary.deterministic ? "true" : "false");
  summary.replay_key = out.str();
  return summary;
}

Objc3ConcurrencyActorIsolationSendableSemanticModelSummary
BuildConcurrencyActorIsolationSendableSemanticModelSummary(
    const Objc3FrontendConcurrencyActorMemberIsolationSourceClosureSummary
        &source_summary,
    const Objc3SemanticIntegrationSurface &surface) {
  Objc3ConcurrencyActorIsolationSendableSemanticModelSummary summary;
  const Objc3ActorIsolationSendabilitySummary &actor_summary =
      surface.actor_isolation_sendability_summary;

  summary.actor_interface_sites = source_summary.actor_interface_sites;
  summary.actor_method_sites = source_summary.actor_method_sites;
  summary.actor_property_sites = source_summary.actor_property_sites;
  summary.objc_nonisolated_annotation_sites =
      source_summary.objc_nonisolated_annotation_sites;
  summary.actor_member_executor_annotation_sites =
      source_summary.actor_member_executor_annotation_sites;
  summary.actor_async_method_sites = source_summary.actor_async_method_sites;
  summary.actor_member_metadata_sites =
      source_summary.actor_member_metadata_sites;
  summary.actor_isolation_sendability_sites =
      actor_summary.actor_isolation_sendability_sites;
  summary.actor_isolation_decl_sites = actor_summary.actor_isolation_decl_sites;
  summary.actor_hop_sites = actor_summary.actor_hop_sites;
  summary.sendable_annotation_sites = actor_summary.sendable_annotation_sites;
  summary.non_sendable_crossing_sites =
      actor_summary.non_sendable_crossing_sites;
  summary.isolation_boundary_sites = actor_summary.isolation_boundary_sites;
  summary.normalized_sites = actor_summary.normalized_sites;
  summary.gate_blocked_sites = actor_summary.gate_blocked_sites;
  summary.contract_violation_sites = actor_summary.contract_violation_sites;

  summary.source_dependency_required = true;
  summary.actor_member_source_supported =
      source_summary.actor_declaration_source_supported &&
      source_summary.actor_member_source_supported &&
      source_summary.isolation_annotation_source_supported &&
      source_summary.actor_metadata_surface_supported &&
      source_summary.deterministic_handoff &&
      summary.actor_member_metadata_sites ==
          summary.actor_method_sites + summary.actor_property_sites;
  summary.actor_isolation_sendability_profile_normalized =
      actor_summary.deterministic &&
      summary.normalized_sites + summary.gate_blocked_sites ==
          summary.actor_isolation_sendability_sites &&
      summary.gate_blocked_sites <= summary.non_sendable_crossing_sites &&
      summary.contract_violation_sites == 0u;
  summary.strict_concurrency_selection_fail_closed = true;
  summary.actor_runtime_deferred = true;
  summary.executor_runtime_deferred = true;
  summary.cross_actor_enforcement_deferred = true;
  summary.deterministic =
      summary.actor_member_source_supported &&
      summary.actor_isolation_sendability_profile_normalized;
  summary.ready_for_semantic_expansion = summary.deterministic;
  if (!summary.deterministic) {
    summary.failure_reason =
        "actor-member source closure and actor/sendability profile must remain deterministic";
  }

  std::ostringstream out;
  out << summary.contract_id
      << ";source-dependency=" << summary.frontend_dependency_contract_id
      << ";actor-interfaces=" << summary.actor_interface_sites
      << ";actor-methods=" << summary.actor_method_sites
      << ";actor-properties=" << summary.actor_property_sites
      << ";nonisolated-sites=" << summary.objc_nonisolated_annotation_sites
      << ";actor-member-executor-sites="
      << summary.actor_member_executor_annotation_sites
      << ";actor-async-method-sites=" << summary.actor_async_method_sites
      << ";actor-metadata-sites=" << summary.actor_member_metadata_sites
      << ";actor-sendability-sites="
      << summary.actor_isolation_sendability_sites
      << ";actor-decl-sites=" << summary.actor_isolation_decl_sites
      << ";actor-hop-sites=" << summary.actor_hop_sites
      << ";sendable-sites=" << summary.sendable_annotation_sites
      << ";non-sendable-sites=" << summary.non_sendable_crossing_sites
      << ";isolation-boundary-sites=" << summary.isolation_boundary_sites
      << ";normalized-sites=" << summary.normalized_sites
      << ";gate-blocked-sites=" << summary.gate_blocked_sites
      << ";contract-violation-sites=" << summary.contract_violation_sites
      << ";deterministic=" << (summary.deterministic ? "true" : "false");
  summary.replay_key = out.str();
  return summary;
}
