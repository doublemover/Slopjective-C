std::string BuildConcurrencyAsyncEffectSuspensionSemanticModelSummaryJson(
    const Objc3ConcurrencyAsyncEffectSuspensionSemanticModelSummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"frontend_dependency_contract_id\":\""
      << EscapeJsonString(summary.frontend_dependency_contract_id)
      << "\",\"surface_path\":\"" << EscapeJsonString(summary.surface_path)
      << "\",\"semantic_model\":\""
      << EscapeJsonString(summary.semantic_model)
      << "\",\"deferred_model\":\""
      << EscapeJsonString(summary.deferred_model)
      << "\",\"async_continuation_sites\":"
      << summary.async_continuation_sites
      << ",\"async_keyword_sites\":" << summary.async_keyword_sites
      << ",\"async_function_sites\":" << summary.async_function_sites
      << ",\"async_method_sites\":" << summary.async_method_sites
      << ",\"executor_attribute_sites\":"
      << summary.executor_attribute_sites
      << ",\"executor_main_sites\":" << summary.executor_main_sites
      << ",\"executor_global_sites\":" << summary.executor_global_sites
      << ",\"executor_named_sites\":" << summary.executor_named_sites
      << ",\"continuation_allocation_sites\":"
      << summary.continuation_allocation_sites
      << ",\"continuation_resume_sites\":"
      << summary.continuation_resume_sites
      << ",\"continuation_suspend_sites\":"
      << summary.continuation_suspend_sites
      << ",\"async_state_machine_sites\":" << summary.async_state_machine_sites
      << ",\"await_suspension_sites\":" << summary.await_suspension_sites
      << ",\"await_keyword_sites\":" << summary.await_keyword_sites
      << ",\"await_expression_sites\":" << summary.await_expression_sites
      << ",\"await_suspension_point_sites\":"
      << summary.await_suspension_point_sites
      << ",\"await_resume_sites\":" << summary.await_resume_sites
      << ",\"await_state_machine_sites\":"
      << summary.await_state_machine_sites
      << ",\"await_continuation_sites\":"
      << summary.await_continuation_sites
      << ",\"actor_isolation_sendability_sites\":"
      << summary.actor_isolation_sendability_sites
      << ",\"actor_isolation_decl_sites\":"
      << summary.actor_isolation_decl_sites
      << ",\"actor_hop_sites\":" << summary.actor_hop_sites
      << ",\"sendable_annotation_sites\":"
      << summary.sendable_annotation_sites
      << ",\"non_sendable_crossing_sites\":"
      << summary.non_sendable_crossing_sites
      << ",\"isolation_boundary_sites\":"
      << summary.isolation_boundary_sites
      << ",\"task_runtime_interop_sites\":"
      << summary.task_runtime_interop_sites
      << ",\"runtime_hook_sites\":" << summary.runtime_hook_sites
      << ",\"cancellation_check_sites\":"
      << summary.cancellation_check_sites
      << ",\"cancellation_handler_sites\":"
      << summary.cancellation_handler_sites
      << ",\"suspension_point_sites\":" << summary.suspension_point_sites
      << ",\"cancellation_propagation_sites\":"
      << summary.cancellation_propagation_sites
      << ",\"concurrency_replay_race_guard_sites\":"
      << summary.concurrency_replay_race_guard_sites
      << ",\"concurrency_replay_sites\":" << summary.concurrency_replay_sites
      << ",\"replay_proof_sites\":" << summary.replay_proof_sites
      << ",\"race_guard_sites\":" << summary.race_guard_sites
      << ",\"task_handoff_sites\":" << summary.task_handoff_sites
      << ",\"actor_isolation_sites\":" << summary.actor_isolation_sites
      << ",\"deterministic_schedule_sites\":"
      << summary.deterministic_schedule_sites
      << ",\"source_dependency_required\":"
      << (summary.source_dependency_required ? "true" : "false")
      << ",\"async_declaration_semantics_landed\":"
      << (summary.async_declaration_semantics_landed ? "true" : "false")
      << ",\"executor_affinity_semantics_landed\":"
      << (summary.executor_affinity_semantics_landed ? "true" : "false")
      << ",\"await_legality_semantics_landed\":"
      << (summary.await_legality_semantics_landed ? "true" : "false")
      << ",\"continuation_profile_semantics_landed\":"
      << (summary.continuation_profile_semantics_landed ? "true" : "false")
      << ",\"await_suspension_profile_semantics_landed\":"
      << (summary.await_suspension_profile_semantics_landed ? "true" : "false")
      << ",\"actor_isolation_sendability_semantics_landed\":"
      << (summary.actor_isolation_sendability_semantics_landed ? "true" : "false")
      << ",\"task_runtime_cancellation_semantics_landed\":"
      << (summary.task_runtime_cancellation_semantics_landed ? "true" : "false")
      << ",\"concurrency_replay_race_guard_semantics_landed\":"
      << (summary.concurrency_replay_race_guard_semantics_landed ? "true" : "false")
      << ",\"runnable_lowering_deferred\":"
      << (summary.runnable_lowering_deferred ? "true" : "false")
      << ",\"executor_runtime_deferred\":"
      << (summary.executor_runtime_deferred ? "true" : "false")
      << ",\"deterministic\":" << (summary.deterministic ? "true" : "false")
      << ",\"ready_for_lowering_and_runtime\":"
      << (summary.ready_for_lowering_and_runtime ? "true" : "false")
      << ",\"failure_reason\":\"" << EscapeJsonString(summary.failure_reason)
      << "\",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}
