std::string BuildConcurrencyTaskExecutorCancellationSemanticModelSummaryJson(
    const Objc3ConcurrencyTaskExecutorCancellationSemanticModelSummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"frontend_dependency_contract_id\":\""
      << EscapeJsonString(summary.frontend_dependency_contract_id)
      << "\",\"surface_path\":\"" << EscapeJsonString(summary.surface_path)
      << "\",\"semantic_model\":\"" << EscapeJsonString(summary.semantic_model)
      << "\",\"deferred_model\":\"" << EscapeJsonString(summary.deferred_model)
      << "\",\"async_callable_sites\":" << summary.async_callable_sites
      << ",\"executor_attribute_sites\":" << summary.executor_attribute_sites
      << ",\"task_creation_sites\":" << summary.task_creation_sites
      << ",\"task_group_scope_sites\":" << summary.task_group_scope_sites
      << ",\"task_group_add_task_sites\":" << summary.task_group_add_task_sites
      << ",\"task_group_wait_next_sites\":" << summary.task_group_wait_next_sites
      << ",\"task_group_cancel_all_sites\":" << summary.task_group_cancel_all_sites
      << ",\"task_runtime_interop_sites\":" << summary.task_runtime_interop_sites
      << ",\"runtime_hook_sites\":" << summary.runtime_hook_sites
      << ",\"cancellation_check_sites\":" << summary.cancellation_check_sites
      << ",\"cancellation_handler_sites\":" << summary.cancellation_handler_sites
      << ",\"suspension_point_sites\":" << summary.suspension_point_sites
      << ",\"cancellation_propagation_sites\":"
      << summary.cancellation_propagation_sites
      << ",\"source_dependency_required\":"
      << (summary.source_dependency_required ? "true" : "false")
      << ",\"task_lifetime_semantics_landed\":"
      << (summary.task_lifetime_semantics_landed ? "true" : "false")
      << ",\"executor_affinity_semantics_landed\":"
      << (summary.executor_affinity_semantics_landed ? "true" : "false")
      << ",\"cancellation_observation_semantics_landed\":"
      << (summary.cancellation_observation_semantics_landed ? "true" : "false")
      << ",\"structured_task_legality_semantics_landed\":"
      << (summary.structured_task_legality_semantics_landed ? "true" : "false")
      << ",\"runnable_lowering_deferred\":"
      << (summary.runnable_lowering_deferred ? "true" : "false")
      << ",\"executor_runtime_deferred\":"
      << (summary.executor_runtime_deferred ? "true" : "false")
      << ",\"scheduler_runtime_deferred\":"
      << (summary.scheduler_runtime_deferred ? "true" : "false")
      << ",\"deterministic\":" << (summary.deterministic ? "true" : "false")
      << ",\"ready_for_lowering_and_runtime\":"
      << (summary.ready_for_lowering_and_runtime ? "true" : "false")
      << ",\"failure_reason\":\"" << EscapeJsonString(summary.failure_reason)
      << "\",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}
