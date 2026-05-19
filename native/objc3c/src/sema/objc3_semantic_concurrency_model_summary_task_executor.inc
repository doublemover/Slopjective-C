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
      summary.executor_attribute_sites ==
          source_summary.executor_attribute_sites &&
      summary.executor_attribute_sites <= summary.async_callable_sites;
  summary.cancellation_observation_semantics_landed =
      source_summary.cancellation_source_supported &&
      summary.cancellation_check_sites >=
          source_summary.cancellation_check_sites &&
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
      << summary.runtime_hook_sites << ":" << summary.cancellation_check_sites
      << ":" << summary.cancellation_handler_sites << ":"
      << summary.suspension_point_sites << ":"
      << summary.cancellation_propagation_sites
      << ";deterministic=" << (summary.deterministic ? "true" : "false");
  summary.replay_key = out.str();
  return summary;
}
