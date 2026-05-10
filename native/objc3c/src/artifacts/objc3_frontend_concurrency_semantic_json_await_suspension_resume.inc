std::string BuildConcurrencyAwaitSuspensionResumeSemanticSummaryJson(
    const Objc3ConcurrencyAwaitSuspensionResumeSemanticSummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"dependency_contract_id\":\""
      << EscapeJsonString(summary.dependency_contract_id)
      << "\",\"surface_path\":\"" << EscapeJsonString(summary.surface_path)
      << "\",\"semantic_model\":\""
      << EscapeJsonString(summary.semantic_model)
      << "\",\"deferred_model\":\""
      << EscapeJsonString(summary.deferred_model)
      << "\",\"async_callable_sites\":" << summary.async_callable_sites
      << ",\"await_expression_sites\":" << summary.await_expression_sites
      << ",\"await_in_async_callable_sites\":"
      << summary.await_in_async_callable_sites
      << ",\"illegal_await_sites\":" << summary.illegal_await_sites
      << ",\"await_suspension_point_sites\":"
      << summary.await_suspension_point_sites
      << ",\"await_resume_sites\":" << summary.await_resume_sites
      << ",\"continuation_resume_sites\":"
      << summary.continuation_resume_sites
      << ",\"continuation_suspend_sites\":"
      << summary.continuation_suspend_sites
      << ",\"source_dependency_required\":"
      << (summary.source_dependency_required ? "true" : "false")
      << ",\"await_placement_enforced\":"
      << (summary.await_placement_enforced ? "true" : "false")
      << ",\"suspension_profile_enforced\":"
      << (summary.suspension_profile_enforced ? "true" : "false")
      << ",\"resume_profile_enforced\":"
      << (summary.resume_profile_enforced ? "true" : "false")
      << ",\"non_async_await_fail_closed\":"
      << (summary.non_async_await_fail_closed ? "true" : "false")
      << ",\"deterministic\":" << (summary.deterministic ? "true" : "false")
      << ",\"ready_for_lowering_and_runtime\":"
      << (summary.ready_for_lowering_and_runtime ? "true" : "false")
      << ",\"failure_reason\":\"" << EscapeJsonString(summary.failure_reason)
      << "\",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}
