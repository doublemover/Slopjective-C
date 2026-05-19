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
      summary.executor_attribute_sites ==
          source_summary.executor_attribute_sites &&
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
      << summary.await_resume_sites << ":" << summary.await_state_machine_sites
      << ":" << summary.await_continuation_sites
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
