std::string BuildConcurrencyAsyncDiagnosticsCompatibilitySummaryJson(
    const Objc3ConcurrencyAsyncDiagnosticsCompatibilitySummary &summary) {
  std::ostringstream out;
  out << '{'
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id) << "\""
      << ",\"dependency_contract_id\":\""
      << EscapeJsonString(summary.dependency_contract_id) << "\""
      << ",\"surface_path\":\"" << EscapeJsonString(summary.surface_path) << "\""
      << ",\"semantic_model\":\"" << EscapeJsonString(summary.semantic_model) << "\""
      << ",\"deferred_model\":\"" << EscapeJsonString(summary.deferred_model) << "\""
      << ",\"async_callable_sites\":" << summary.async_callable_sites
      << ",\"executor_affinity_sites\":" << summary.executor_affinity_sites
      << ",\"illegal_non_async_executor_sites\":"
      << summary.illegal_non_async_executor_sites
      << ",\"illegal_async_function_prototype_sites\":"
      << summary.illegal_async_function_prototype_sites
      << ",\"illegal_async_throws_sites\":"
      << summary.illegal_async_throws_sites
      << ",\"compatibility_diagnostic_sites\":"
      << summary.compatibility_diagnostic_sites
      << ",\"supported_async_callable_sites\":"
      << summary.supported_async_callable_sites
      << ",\"dependency_required\":"
      << (summary.dependency_required ? "true" : "false")
      << ",\"executor_affinity_requires_async_enforced\":"
      << (summary.executor_affinity_requires_async_enforced ? "true" : "false")
      << ",\"async_function_prototypes_fail_closed\":"
      << (summary.async_function_prototypes_fail_closed ? "true" : "false")
      << ",\"async_throws_fail_closed\":"
      << (summary.async_throws_fail_closed ? "true" : "false")
      << ",\"unsupported_topology_fail_closed\":"
      << (summary.unsupported_topology_fail_closed ? "true" : "false")
      << ",\"deterministic\":" << (summary.deterministic ? "true" : "false")
      << ",\"ready_for_lowering_and_runtime\":"
      << (summary.ready_for_lowering_and_runtime ? "true" : "false")
      << ",\"failure_reason\":\"" << EscapeJsonString(summary.failure_reason) << "\""
      << ",\"replay_key\":\"" << EscapeJsonString(summary.replay_key) << "\""
      << '}';
  return out.str();
}
