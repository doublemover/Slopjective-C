std::string BuildConcurrencyStructuredTaskCancellationSemanticSummaryJson(
    const Objc3ConcurrencyStructuredTaskCancellationSemanticSummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"dependency_contract_id\":\""
      << EscapeJsonString(summary.dependency_contract_id)
      << "\",\"surface_path\":\"" << EscapeJsonString(summary.surface_path)
      << "\",\"semantic_model\":\"" << EscapeJsonString(summary.semantic_model)
      << "\",\"deferred_model\":\"" << EscapeJsonString(summary.deferred_model)
      << "\",\"async_callable_sites\":" << summary.async_callable_sites
      << ",\"task_creation_sites\":" << summary.task_creation_sites
      << ",\"task_group_scope_sites\":" << summary.task_group_scope_sites
      << ",\"task_group_add_task_sites\":" << summary.task_group_add_task_sites
      << ",\"task_group_wait_next_sites\":"
      << summary.task_group_wait_next_sites
      << ",\"task_group_cancel_all_sites\":"
      << summary.task_group_cancel_all_sites
      << ",\"cancellation_check_sites\":"
      << summary.cancellation_check_sites
      << ",\"cancellation_handler_sites\":"
      << summary.cancellation_handler_sites
      << ",\"illegal_non_async_task_sites\":"
      << summary.illegal_non_async_task_sites
      << ",\"illegal_task_group_scope_sites\":"
      << summary.illegal_task_group_scope_sites
      << ",\"illegal_task_hierarchy_sites\":"
      << summary.illegal_task_hierarchy_sites
      << ",\"illegal_cancellation_usage_sites\":"
      << summary.illegal_cancellation_usage_sites
      << ",\"source_dependency_required\":"
      << (summary.source_dependency_required ? "true" : "false")
      << ",\"async_task_boundary_enforced\":"
      << (summary.async_task_boundary_enforced ? "true" : "false")
      << ",\"structured_task_scope_enforced\":"
      << (summary.structured_task_scope_enforced ? "true" : "false")
      << ",\"task_hierarchy_enforced\":"
      << (summary.task_hierarchy_enforced ? "true" : "false")
      << ",\"cancellation_usage_enforced\":"
      << (summary.cancellation_usage_enforced ? "true" : "false")
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
