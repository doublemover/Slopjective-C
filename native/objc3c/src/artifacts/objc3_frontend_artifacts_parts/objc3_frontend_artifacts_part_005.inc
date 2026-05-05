      << "\",\"borrowed_semantic_contract_id\":\""
      << EscapeJsonString(borrowed_summary.contract_id)
      << "\",\"family_semantic_contract_id\":\""
      << EscapeJsonString(family_summary.contract_id)
      << "\",\"lane_contract_id\":\""
      << EscapeJsonString(kObjc3OwnershipSystemExtensionLoweringLaneContract)
      << "\",\"lowering_model\":\""
      << EscapeJsonString(kObjc3OwnershipSystemExtensionLoweringModel)
      << "\",\"deferred_model\":\""
      << EscapeJsonString(kObjc3OwnershipSystemExtensionLoweringDeferredModel)
      << "\",\"replay_key\":\"" << EscapeJsonString(replay_key)
      << "\",\"cleanup_hook_sites\":" << contract.cleanup_hook_sites
      << ",\"resource_local_sites\":" << contract.resource_local_sites
      << ",\"cleanup_owned_local_sites\":"
      << contract.cleanup_owned_local_sites
      << ",\"resource_move_capture_sites\":"
      << contract.resource_move_capture_sites
      << ",\"borrowed_parameter_sites\":"
      << contract.borrowed_parameter_sites
      << ",\"borrowed_return_callable_sites\":"
      << contract.borrowed_return_callable_sites
      << ",\"borrowed_escape_candidate_sites\":"
      << contract.borrowed_escape_candidate_sites
      << ",\"explicit_capture_item_sites\":"
      << contract.explicit_capture_item_sites
      << ",\"retainable_family_callable_sites\":"
      << contract.retainable_family_callable_sites
      << ",\"retainable_family_operation_callable_sites\":"
      << contract.retainable_family_operation_callable_sites
      << ",\"retainable_family_alias_callable_sites\":"
      << contract.retainable_family_alias_callable_sites
      << ",\"guard_blocked_sites\":" << contract.guard_blocked_sites
      << ",\"contract_violation_sites\":"
      << contract.contract_violation_sites
      << ",\"deterministic_handoff\":"
      << (contract.deterministic ? "true" : "false")
      << ",\"ready_for_ir_emission\":"
        << (ready_for_ir_emission ? "true" : "false")
        << "}";
  return out.str();
}

std::string BuildOwnershipBorrowedRetainableAbiCompletionReplayKey(
    const Objc3OwnershipSystemExtensionLoweringContract &contract,
    const Objc3FrontendOwnershipSystemExtensionSourceClosureSummary &source_summary,
    const Objc3FrontendOwnershipRetainableCFamilySourceCompletionSummary
        &retainable_summary) {
  std::ostringstream out;
  out << "lowering_replay_key="
      << Objc3OwnershipSystemExtensionLoweringReplayKey(contract)
      << ";returns_borrowed_attribute_sites="
      << source_summary.returns_borrowed_attribute_sites
      << ";family_retain_sites=" << retainable_summary.family_retain_sites
      << ";family_release_sites=" << retainable_summary.family_release_sites
      << ";family_autorelease_sites="
      << retainable_summary.family_autorelease_sites
      << ";compatibility_returns_retained_sites="
      << retainable_summary.compatibility_returns_retained_sites
      << ";compatibility_returns_not_retained_sites="
      << retainable_summary.compatibility_returns_not_retained_sites
      << ";compatibility_consumed_sites="
      << retainable_summary.compatibility_consumed_sites
      << ";deterministic=true;lane_contract="
      << kObjc3OwnershipBorrowedRetainableAbiCompletionLaneContract;
  return out.str();
}

std::string BuildOwnershipBorrowedRetainableAbiCompletionJson(
    const Objc3OwnershipSystemExtensionLoweringContract &contract,
    const Objc3FrontendOwnershipSystemExtensionSourceClosureSummary &source_summary,
    const Objc3FrontendOwnershipRetainableCFamilySourceCompletionSummary
        &retainable_summary,
    const std::string &lowering_replay_key,
    const std::string &abi_completion_replay_key) {
  const bool ready_for_ir_emission =
      contract.deterministic &&
      IsValidObjc3OwnershipSystemExtensionLoweringContract(contract) &&
      source_summary.returns_borrowed_attribute_sites <=
          contract.borrowed_return_callable_sites &&
      retainable_summary.family_retain_sites +
              retainable_summary.family_release_sites +
              retainable_summary.family_autorelease_sites ==
          contract.retainable_family_operation_callable_sites &&
      retainable_summary.compatibility_returns_retained_sites +
              retainable_summary.compatibility_returns_not_retained_sites +
              retainable_summary.compatibility_consumed_sites ==
          contract.retainable_family_alias_callable_sites;
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\""
      << EscapeJsonString(kObjc3OwnershipBorrowedRetainableAbiCompletionContractId)
      << "\",\"surface_path\":\""
      << EscapeJsonString(kObjc3OwnershipBorrowedRetainableAbiCompletionSurfacePath)
      << "\",\"lowering_contract_id\":\""
      << EscapeJsonString(kObjc3OwnershipSystemExtensionLoweringContractId)
      << "\",\"artifact_model\":\""
      << EscapeJsonString(kObjc3OwnershipBorrowedRetainableAbiCompletionArtifactModel)
      << "\",\"proof_model\":\""
      << EscapeJsonString(kObjc3OwnershipBorrowedRetainableAbiCompletionProofModel)
      << "\",\"lowering_replay_key\":\""
      << EscapeJsonString(lowering_replay_key)
      << "\",\"replay_key\":\""
      << EscapeJsonString(abi_completion_replay_key)
      << "\",\"borrowed_parameter_sites\":"
      << contract.borrowed_parameter_sites
      << ",\"borrowed_return_callable_sites\":"
      << contract.borrowed_return_callable_sites
      << ",\"returns_borrowed_attribute_sites\":"
      << source_summary.returns_borrowed_attribute_sites
      << ",\"retainable_family_callable_sites\":"
      << contract.retainable_family_callable_sites
      << ",\"retainable_family_operation_callable_sites\":"
      << contract.retainable_family_operation_callable_sites
      << ",\"retainable_family_alias_callable_sites\":"
      << contract.retainable_family_alias_callable_sites
      << ",\"family_retain_sites\":"
      << retainable_summary.family_retain_sites
      << ",\"family_release_sites\":"
      << retainable_summary.family_release_sites
      << ",\"family_autorelease_sites\":"
      << retainable_summary.family_autorelease_sites
      << ",\"compatibility_returns_retained_sites\":"
      << retainable_summary.compatibility_returns_retained_sites
      << ",\"compatibility_returns_not_retained_sites\":"
      << retainable_summary.compatibility_returns_not_retained_sites
      << ",\"compatibility_consumed_sites\":"
      << retainable_summary.compatibility_consumed_sites
      << ",\"deterministic_handoff\":true"
      << ",\"ready_for_ir_emission\":"
      << (ready_for_ir_emission ? "true" : "false")
      << "}";
  return out.str();
}

Objc3TaskRuntimeInteropCancellationLoweringContract
BuildConcurrencyTaskRuntimeInteropCancellationLoweringContract(
    const Objc3ConcurrencyTaskExecutorCancellationSemanticModelSummary
        &semantic_summary,
    const Objc3ConcurrencyStructuredTaskCancellationSemanticSummary
        &structured_summary,
    const Objc3ConcurrencyExecutorHopAffinityCompatibilitySummary &executor_summary) {
  Objc3TaskRuntimeInteropCancellationLoweringContract contract;
  contract.task_runtime_sites =
      semantic_summary.task_runtime_interop_sites +
      structured_summary.illegal_non_async_task_sites +
      structured_summary.illegal_task_group_scope_sites +
      structured_summary.illegal_task_hierarchy_sites +
      structured_summary.illegal_cancellation_usage_sites +
      executor_summary.illegal_missing_executor_affinity_sites +
      executor_summary.illegal_main_executor_detached_sites;
  contract.task_runtime_interop_sites =
      semantic_summary.task_runtime_interop_sites;
  contract.cancellation_probe_sites =
      semantic_summary.cancellation_check_sites;
  contract.cancellation_handler_sites =
      semantic_summary.cancellation_handler_sites;
  contract.runtime_resume_sites =
      structured_summary.task_group_wait_next_sites;
  contract.runtime_cancel_sites =
      structured_summary.task_group_cancel_all_sites;
  contract.guard_blocked_sites =
      structured_summary.illegal_non_async_task_sites +
      structured_summary.illegal_task_group_scope_sites +
      structured_summary.illegal_task_hierarchy_sites +
      structured_summary.illegal_cancellation_usage_sites +
      executor_summary.illegal_missing_executor_affinity_sites +
      executor_summary.illegal_main_executor_detached_sites;
  contract.normalized_sites =
      contract.task_runtime_sites - contract.guard_blocked_sites;
  contract.contract_violation_sites = 0;
  contract.deterministic =
      semantic_summary.deterministic && structured_summary.deterministic &&
      executor_summary.deterministic &&
      semantic_summary.ready_for_lowering_and_runtime &&
      structured_summary.ready_for_lowering_and_runtime &&
      executor_summary.ready_for_lowering_and_runtime &&
      contract.task_runtime_interop_sites <= contract.task_runtime_sites;
  return contract;
}

Objc3ConcurrencyReplayRaceGuardLoweringContract
BuildConcurrencyConcurrencyReplayRaceGuardLoweringContract(
    const Objc3ConcurrencyTaskExecutorCancellationSemanticModelSummary
        &semantic_summary,
    const Objc3ConcurrencyStructuredTaskCancellationSemanticSummary
        &structured_summary,
    const Objc3ConcurrencyExecutorHopAffinityCompatibilitySummary &executor_summary,
    const Objc3ActorIsolationSendabilityLoweringContract &actor_contract) {
  Objc3ConcurrencyReplayRaceGuardLoweringContract contract;
  contract.replay_proof_sites =
      semantic_summary.task_creation_sites +
      structured_summary.task_group_wait_next_sites;
  contract.race_guard_sites =
      semantic_summary.cancellation_check_sites +
      semantic_summary.cancellation_handler_sites;
  contract.task_handoff_sites =
      semantic_summary.task_creation_sites +
      executor_summary.detached_task_creation_sites +
      structured_summary.task_group_wait_next_sites;
  contract.actor_isolation_sites = actor_contract.actor_isolation_sites;
  contract.guard_blocked_sites =
      structured_summary.illegal_non_async_task_sites +
      structured_summary.illegal_task_group_scope_sites +
      structured_summary.illegal_task_hierarchy_sites +
      structured_summary.illegal_cancellation_usage_sites +
      executor_summary.illegal_missing_executor_affinity_sites +
      executor_summary.illegal_main_executor_detached_sites;
  contract.deterministic_schedule_sites =
      contract.replay_proof_sites + contract.race_guard_sites +
      contract.task_handoff_sites + contract.actor_isolation_sites;
  contract.concurrency_replay_sites =
      contract.deterministic_schedule_sites + contract.guard_blocked_sites;
  contract.contract_violation_sites = 0;
  contract.deterministic =
      semantic_summary.deterministic && structured_summary.deterministic &&
      executor_summary.deterministic && actor_contract.deterministic &&
      semantic_summary.ready_for_lowering_and_runtime &&
      structured_summary.ready_for_lowering_and_runtime &&
      executor_summary.ready_for_lowering_and_runtime;
  return contract;
}

std::string BuildConcurrencyTaskRuntimeLoweringContractJson(
    const Objc3ConcurrencyTaskExecutorCancellationSemanticModelSummary
        &semantic_summary,
    const Objc3ConcurrencyStructuredTaskCancellationSemanticSummary
        &structured_summary,
    const Objc3ConcurrencyExecutorHopAffinityCompatibilitySummary &executor_summary,
    const Objc3ActorIsolationSendabilityLoweringContract &actor_contract,
    const std::string &actor_replay_key,
    const Objc3TaskRuntimeInteropCancellationLoweringContract &task_contract,
    const std::string &task_replay_key,
    const Objc3ConcurrencyReplayRaceGuardLoweringContract &race_contract,
    const std::string &race_replay_key) {
  const bool deterministic_handoff =
      actor_contract.deterministic && task_contract.deterministic &&
      race_contract.deterministic;
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\""
      << EscapeJsonString(kObjc3ConcurrencyTaskRuntimeLoweringContractId)
      << "\",\"surface_path\":\""
      << EscapeJsonString(kObjc3ConcurrencyTaskRuntimeLoweringSurfacePath)
      << "\",\"semantic_contract_id\":\""
      << EscapeJsonString(
             kObjc3ConcurrencyTaskExecutorCancellationSemanticModelContractId)
      << "\",\"structured_semantic_contract_id\":\""
      << EscapeJsonString(
             kObjc3ConcurrencyStructuredTaskCancellationSemanticSummaryContractId)
      << "\",\"executor_semantic_contract_id\":\""
      << EscapeJsonString(
             kObjc3ConcurrencyExecutorHopAffinityCompatibilitySummaryContractId)
      << "\",\"actor_lane_contract_id\":\""
      << EscapeJsonString(kObjc3ActorIsolationSendabilityLoweringLaneContract)
      << "\",\"task_runtime_lane_contract_id\":\""
      << EscapeJsonString(
             kObjc3TaskRuntimeInteropCancellationLoweringLaneContract)
      << "\",\"concurrency_lane_contract_id\":\""
      << EscapeJsonString(kObjc3ConcurrencyReplayRaceGuardLoweringLaneContract)
      << "\",\"task_model\":\""
      << EscapeJsonString(kObjc3ConcurrencyTaskRuntimeLoweringTaskModel)
      << "\",\"executor_model\":\""
      << EscapeJsonString(kObjc3ConcurrencyTaskRuntimeLoweringExecutorModel)
      << "\",\"concurrency_model\":\""
      << EscapeJsonString(kObjc3ConcurrencyTaskRuntimeLoweringConcurrencyModel)
      << "\",\"deferred_model\":\""
      << EscapeJsonString(kObjc3ConcurrencyTaskRuntimeLoweringDeferredModel)
      << "\",\"actor_replay_key\":\"" << EscapeJsonString(actor_replay_key)
      << "\",\"task_runtime_replay_key\":\""
      << EscapeJsonString(task_replay_key)
      << "\",\"concurrency_replay_key\":\""
      << EscapeJsonString(race_replay_key)
      << "\",\"task_creation_sites\":" << semantic_summary.task_creation_sites
      << ",\"task_group_scope_sites\":"
      << semantic_summary.task_group_scope_sites
      << ",\"task_group_add_task_sites\":"
      << semantic_summary.task_group_add_task_sites
      << ",\"task_group_wait_next_sites\":"
      << semantic_summary.task_group_wait_next_sites
      << ",\"task_group_cancel_all_sites\":"
      << semantic_summary.task_group_cancel_all_sites
      << ",\"detached_task_creation_sites\":"
      << executor_summary.detached_task_creation_sites
      << ",\"actor_isolation_sites\":" << actor_contract.actor_isolation_sites
      << ",\"cross_actor_hop_sites\":"
      << actor_contract.cross_actor_hop_sites
      << ",\"sendability_check_sites\":"
      << actor_contract.sendability_check_sites
      << ",\"task_runtime_sites\":" << task_contract.task_runtime_sites
      << ",\"task_runtime_interop_sites\":"
      << task_contract.task_runtime_interop_sites
      << ",\"cancellation_probe_sites\":"
      << task_contract.cancellation_probe_sites
      << ",\"cancellation_handler_sites\":"
      << task_contract.cancellation_handler_sites
      << ",\"runtime_resume_sites\":"
      << task_contract.runtime_resume_sites
      << ",\"runtime_cancel_sites\":" << task_contract.runtime_cancel_sites
      << ",\"task_runtime_normalized_sites\":"
      << task_contract.normalized_sites
      << ",\"task_runtime_guard_blocked_sites\":"
      << task_contract.guard_blocked_sites
      << ",\"concurrency_replay_sites\":"
      << race_contract.concurrency_replay_sites
      << ",\"replay_proof_sites\":" << race_contract.replay_proof_sites
      << ",\"race_guard_sites\":" << race_contract.race_guard_sites
      << ",\"task_handoff_sites\":" << race_contract.task_handoff_sites
      << ",\"deterministic_schedule_sites\":"
      << race_contract.deterministic_schedule_sites
      << ",\"concurrency_guard_blocked_sites\":"
      << race_contract.guard_blocked_sites
      << ",\"deterministic_handoff\":"
      << (deterministic_handoff ? "true" : "false")
      << ",\"ready_for_ir_emission\":"
      << (deterministic_handoff ? "true" : "false")
      << "}";
  return out.str();
}

std::string BuildConcurrencyTaskRuntimeAbiCompletionJson(
    const std::string &task_runtime_replay_key,
    const std::string &concurrency_replay_key) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\""
      << EscapeJsonString(kObjc3ConcurrencyTaskRuntimeAbiCompletionContractId)
      << "\",\"surface_path\":\""
      << EscapeJsonString(kObjc3ConcurrencyTaskRuntimeAbiCompletionSurfacePath)
      << "\",\"lowering_contract_id\":\""
      << EscapeJsonString(kObjc3ConcurrencyTaskRuntimeLoweringContractId)
      << "\",\"artifact_model\":\""
      << EscapeJsonString(kObjc3ConcurrencyTaskRuntimeAbiCompletionArtifactModel)
      << "\",\"runtime_proof_model\":\""
      << EscapeJsonString(kObjc3ConcurrencyTaskRuntimeAbiCompletionProofModel)
      << "\",\"task_runtime_replay_key\":\""
      << EscapeJsonString(task_runtime_replay_key)
      << "\",\"concurrency_replay_key\":\""
      << EscapeJsonString(concurrency_replay_key)
      << "\",\"helper_symbol_count\":8"
      << ",\"task_group_helper_count\":4"
      << ",\"scheduler_visible_runtime_proof\":true"
      << ",\"runtime_snapshot_symbol\":\""
      << EscapeJsonString("objc3_runtime_copy_task_runtime_state_for_testing")
      << "\",\"helper_symbols\":["
      << "\"" << EscapeJsonString(kObjc3RuntimeSpawnTaskI32Symbol) << "\","
      << "\"" << EscapeJsonString(kObjc3RuntimeEnterTaskGroupScopeI32Symbol)
      << "\","
      << "\"" << EscapeJsonString(kObjc3RuntimeAddTaskGroupTaskI32Symbol)
      << "\","
      << "\"" << EscapeJsonString(kObjc3RuntimeWaitTaskGroupNextI32Symbol)
      << "\","
      << "\"" << EscapeJsonString(kObjc3RuntimeCancelTaskGroupI32Symbol)
      << "\","
      << "\"" << EscapeJsonString(kObjc3RuntimeTaskIsCancelledI32Symbol)
      << "\","
      << "\"" << EscapeJsonString(kObjc3RuntimeTaskOnCancelI32Symbol)
      << "\","
      << "\"" << EscapeJsonString(kObjc3RuntimeExecutorHopI32Symbol) << "\""
      << "]}";
  return out.str();
}

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

Objc3AsyncContinuationLoweringContract BuildConcurrencyAsyncContinuationLoweringContract(
    const Objc3ConcurrencyAsyncEffectSuspensionSemanticModelSummary &summary,
    const Objc3ConcurrencyAsyncDiagnosticsCompatibilitySummary
        &compatibility_summary) {
  Objc3AsyncContinuationLoweringContract contract;
  contract.async_continuation_sites = summary.async_continuation_sites;
  contract.async_keyword_sites = summary.async_keyword_sites;
  contract.async_function_sites = summary.async_function_sites;
  contract.continuation_allocation_sites = summary.continuation_allocation_sites;
  contract.continuation_resume_sites = summary.continuation_resume_sites;
  contract.continuation_suspend_sites = summary.continuation_suspend_sites;
  contract.async_state_machine_sites = summary.async_state_machine_sites;
  contract.gate_blocked_sites =
      compatibility_summary.illegal_non_async_executor_sites +
      compatibility_summary.illegal_async_function_prototype_sites +
      compatibility_summary.illegal_async_throws_sites;
  if (contract.gate_blocked_sites > contract.async_continuation_sites) {
    contract.contract_violation_sites =
        contract.gate_blocked_sites - contract.async_continuation_sites;
    contract.gate_blocked_sites = contract.async_continuation_sites;
  }
  contract.normalized_sites =
      contract.async_continuation_sites - contract.gate_blocked_sites;
  contract.deterministic =
      summary.deterministic && compatibility_summary.deterministic &&
      summary.ready_for_lowering_and_runtime &&
      compatibility_summary.ready_for_lowering_and_runtime &&
      contract.contract_violation_sites == 0;
  return contract;
}

Objc3AwaitLoweringSuspensionStateLoweringContract
BuildConcurrencyAwaitLoweringSuspensionStateLoweringContract(
    const Objc3ConcurrencyAwaitSuspensionResumeSemanticSummary &summary) {
  Objc3AwaitLoweringSuspensionStateLoweringContract contract;
  contract.await_suspension_sites = summary.await_expression_sites +
                                    summary.await_suspension_point_sites;
  contract.await_keyword_sites =
      summary.await_suspension_point_sites > 0 ? 1 : 0;
  contract.await_suspension_point_sites = summary.await_suspension_point_sites;
  contract.await_resume_sites = summary.await_resume_sites;
  contract.await_state_machine_sites = summary.await_resume_sites;
  contract.await_continuation_sites =
      summary.await_suspension_point_sites > 0
          ? std::min(summary.await_suspension_point_sites,
                     std::max(summary.continuation_resume_sites,
                              summary.continuation_suspend_sites))
          : 0;
  contract.gate_blocked_sites = summary.illegal_await_sites;
  if (contract.gate_blocked_sites > contract.await_suspension_sites) {
    contract.contract_violation_sites =
        contract.gate_blocked_sites - contract.await_suspension_sites;
    contract.gate_blocked_sites = contract.await_suspension_sites;
  }
  contract.normalized_sites =
      contract.await_suspension_sites - contract.gate_blocked_sites;
  contract.deterministic =
      summary.deterministic && summary.ready_for_lowering_and_runtime &&
      contract.contract_violation_sites == 0;
  return contract;
}

std::string BuildConcurrencyContinuationAbiAsyncLoweringContractJson(
    const Objc3AsyncContinuationLoweringContract &continuation_contract,
    const Objc3AwaitLoweringSuspensionStateLoweringContract &await_contract,
    const std::string &continuation_replay_key,
    const std::string &await_replay_key) {
  const bool deterministic_handoff =
      continuation_contract.deterministic && await_contract.deterministic;
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\""
      << EscapeJsonString(kObjc3ConcurrencyContinuationAbiAsyncLoweringContractId)
      << "\",\"surface_path\":\""
      << EscapeJsonString(kObjc3ConcurrencyContinuationAbiAsyncLoweringSurfacePath)
      << "\",\"continuation_lane_contract_id\":\""
      << EscapeJsonString(kObjc3AsyncContinuationLoweringLaneContract)
      << "\",\"await_lane_contract_id\":\""
      << EscapeJsonString(kObjc3AwaitLoweringSuspensionStateLoweringLaneContract)
      << "\",\"continuation_model\":\""
      << EscapeJsonString(
             kObjc3ConcurrencyContinuationAbiAsyncLoweringContinuationModel)
      << "\",\"await_model\":\""
      << EscapeJsonString(kObjc3ConcurrencyContinuationAbiAsyncLoweringAwaitModel)
      << "\",\"deferred_model\":\""
      << EscapeJsonString(kObjc3ConcurrencyContinuationAbiAsyncLoweringDeferredModel)
      << "\",\"async_continuation_replay_key\":\""
      << EscapeJsonString(continuation_replay_key)
      << "\",\"await_suspension_replay_key\":\""
      << EscapeJsonString(await_replay_key)
      << "\",\"async_continuation_sites\":"
      << continuation_contract.async_continuation_sites
      << ",\"async_keyword_sites\":"
      << continuation_contract.async_keyword_sites
      << ",\"async_function_sites\":"
      << continuation_contract.async_function_sites
      << ",\"continuation_allocation_sites\":"
      << continuation_contract.continuation_allocation_sites
      << ",\"continuation_resume_sites\":"
      << continuation_contract.continuation_resume_sites
      << ",\"continuation_suspend_sites\":"
      << continuation_contract.continuation_suspend_sites
      << ",\"async_state_machine_sites\":"
      << continuation_contract.async_state_machine_sites
      << ",\"async_normalized_sites\":"
      << continuation_contract.normalized_sites
      << ",\"async_gate_blocked_sites\":"
      << continuation_contract.gate_blocked_sites
      << ",\"async_contract_violation_sites\":"
      << continuation_contract.contract_violation_sites
      << ",\"await_suspension_sites\":"
      << await_contract.await_suspension_sites
      << ",\"await_keyword_sites\":"
      << await_contract.await_keyword_sites
      << ",\"await_suspension_point_sites\":"
      << await_contract.await_suspension_point_sites
      << ",\"await_resume_sites\":"
      << await_contract.await_resume_sites
      << ",\"await_state_machine_sites\":"
      << await_contract.await_state_machine_sites
      << ",\"await_continuation_sites\":"
      << await_contract.await_continuation_sites
      << ",\"await_normalized_sites\":"
      << await_contract.normalized_sites
      << ",\"await_gate_blocked_sites\":"
      << await_contract.gate_blocked_sites
      << ",\"await_contract_violation_sites\":"
      << await_contract.contract_violation_sites
      << ",\"deterministic_handoff\":"
      << (deterministic_handoff ? "true" : "false")
      << ",\"ready_for_ir_emission\":"
      << (deterministic_handoff ? "true" : "false")
      << "}";
  return out.str();
}

std::string BuildConcurrencyAsyncDirectCallLoweringJson(
    const Objc3FrontendConcurrencyAsyncSourceClosureSummary &source_summary,
    const Objc3AsyncContinuationLoweringContract &continuation_contract,
    const Objc3AwaitLoweringSuspensionStateLoweringContract &await_contract,
    const std::string &continuation_replay_key,
    const std::string &await_replay_key) {
  const bool deterministic = source_summary.deterministic_handoff &&
                             continuation_contract.deterministic &&
                             await_contract.deterministic;
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\""
      << EscapeJsonString(kObjc3ConcurrencyAsyncDirectCallLoweringContractId)
      << "\",\"surface_path\":\""
      << EscapeJsonString(kObjc3ConcurrencyAsyncDirectCallLoweringSurfacePath)
      << "\",\"implementation_model\":\""
      << EscapeJsonString(kObjc3ConcurrencyAsyncDirectCallLoweringImplementationModel)
      << "\",\"await_lowering_model\":\""
      << EscapeJsonString(kObjc3ConcurrencyAsyncDirectCallLoweringAwaitModel)
      << "\",\"deferred_model\":\""
      << EscapeJsonString(kObjc3ConcurrencyAsyncDirectCallLoweringDeferredModel)
      << "\",\"source_closure_contract_id\":\""
      << EscapeJsonString(kObjc3ConcurrencyAsyncSourceClosureContractId)
      << "\",\"continuation_lane_contract_id\":\""
      << EscapeJsonString(kObjc3AsyncContinuationLoweringLaneContract)
      << "\",\"await_lane_contract_id\":\""
      << EscapeJsonString(kObjc3AwaitLoweringSuspensionStateLoweringLaneContract)
      << "\",\"async_function_sites\":"
      << source_summary.async_function_sites
      << ",\"async_method_sites\":"
      << source_summary.async_method_sites
      << ",\"await_expression_sites\":"
      << source_summary.await_expression_sites
      << ",\"continuation_allocation_sites\":"
      << continuation_contract.continuation_allocation_sites
      << ",\"continuation_resume_sites\":"
      << continuation_contract.continuation_resume_sites
      << ",\"continuation_suspend_sites\":"
      << continuation_contract.continuation_suspend_sites
      << ",\"async_state_machine_sites\":"
      << continuation_contract.async_state_machine_sites
      << ",\"await_resume_sites\":"
      << await_contract.await_resume_sites
      << ",\"await_state_machine_sites\":"
      << await_contract.await_state_machine_sites
      << ",\"await_continuation_sites\":"
      << await_contract.await_continuation_sites
      << ",\"direct_call_lowering_supported\":"
      << (deterministic ? "true" : "false")
      << ",\"non_suspending_happy_path_only\":true"
      << ",\"object_emission_supported\":"
      << (deterministic ? "true" : "false")
      << ",\"runtime_scheduler_required\":false"
      << ",\"continuation_replay_key\":\""
      << EscapeJsonString(continuation_replay_key)
      << "\",\"await_replay_key\":\""
      << EscapeJsonString(await_replay_key)
      << "\",\"deterministic\":"
      << (deterministic ? "true" : "false")
      << ",\"ready_for_ir_object_emission\":"
      << (deterministic ? "true" : "false")
      << "}";
  return out.str();
}

std::string BuildConcurrencySuspensionCleanupIntegrationJson(
    const Objc3ControlFlowControlFlowSafetyLoweringContract &control_flow_contract,
    const std::string &control_flow_replay_key,
    const Objc3AutoreleasePoolScopeLoweringContract &autoreleasepool_contract,
    const std::string &autoreleasepool_replay_key,
    const Objc3AsyncContinuationLoweringContract &continuation_contract,
    const Objc3AwaitLoweringSuspensionStateLoweringContract &await_contract,
    const std::string &continuation_replay_key,
    const std::string &await_replay_key) {
  const bool deterministic = control_flow_contract.deterministic &&
                             autoreleasepool_contract.deterministic &&
                             continuation_contract.deterministic &&
                             await_contract.deterministic;
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\""
      << EscapeJsonString(kObjc3ConcurrencySuspensionCleanupIntegrationContractId)
      << "\",\"surface_path\":\""
      << EscapeJsonString(kObjc3ConcurrencySuspensionCleanupIntegrationSurfacePath)
      << "\",\"integration_model\":\""
      << EscapeJsonString(kObjc3ConcurrencySuspensionCleanupIntegrationModel)
      << "\",\"cleanup_ordering_model\":\""
      << EscapeJsonString(kObjc3ConcurrencySuspensionCleanupIntegrationOrderingModel)
      << "\",\"deferred_model\":\""
      << EscapeJsonString(kObjc3ConcurrencySuspensionCleanupIntegrationDeferredModel)
      << "\",\"async_lowering_contract_id\":\""
      << EscapeJsonString(kObjc3ConcurrencyAsyncDirectCallLoweringContractId)
      << "\",\"control_flow_contract_id\":\""
      << EscapeJsonString(kObjc3ControlFlowControlFlowSafetyLoweringContractId)
      << "\",\"autoreleasepool_lane_contract_id\":\""
      << EscapeJsonString(kObjc3AutoreleasePoolScopeLoweringLaneContract)
      << "\",\"continuation_lane_contract_id\":\""
      << EscapeJsonString(kObjc3AsyncContinuationLoweringLaneContract)
      << "\",\"await_lane_contract_id\":\""
      << EscapeJsonString(kObjc3AwaitLoweringSuspensionStateLoweringLaneContract)
      << "\",\"defer_statement_sites\":"
      << control_flow_contract.defer_statement_sites
      << ",\"live_defer_cleanup_sites\":"
      << control_flow_contract.live_defer_cleanup_sites
      << ",\"continuation_allocation_sites\":"
      << continuation_contract.continuation_allocation_sites
      << ",\"continuation_resume_sites\":"
      << continuation_contract.continuation_resume_sites
      << ",\"continuation_suspend_sites\":"
      << continuation_contract.continuation_suspend_sites
      << ",\"await_resume_sites\":"
      << await_contract.await_resume_sites
      << ",\"await_state_machine_sites\":"
      << await_contract.await_state_machine_sites
      << ",\"await_continuation_sites\":"
      << await_contract.await_continuation_sites
      << ",\"control_flow_replay_key\":\""
      << EscapeJsonString(control_flow_replay_key)
      << "\",\"autoreleasepool_replay_key\":\""
      << EscapeJsonString(autoreleasepool_replay_key)
      << "\",\"continuation_replay_key\":\""
      << EscapeJsonString(continuation_replay_key)
      << "\",\"await_replay_key\":\""
      << EscapeJsonString(await_replay_key)
      << "\",\"autoreleasepool_scope_supported\":"
      << (!autoreleasepool_replay_key.empty() ? "true" : "false")
      << ",\"defer_cleanup_supported\":"
      << (control_flow_contract.live_defer_cleanup_sites > 0 ? "true" : "false")
      << ",\"direct_call_lowering_supported\":"
      << (deterministic ? "true" : "false")
      << ",\"suspension_runtime_required\":false"
      << ",\"state_machine_emission_present\":false"
      << ",\"deterministic\":"
      << (deterministic ? "true" : "false")
      << ",\"ready_for_ir_object_emission\":"
      << (deterministic ? "true" : "false")
      << "}";
  return out.str();
}

std::string BuildErrorHandlingTryDoCatchSemanticSummaryJson(
    const Objc3ErrorHandlingTryDoCatchSemanticSummary &summary) {
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
      << "\",\"try_expression_sites\":" << summary.try_expression_sites
      << ",\"try_propagating_sites\":" << summary.try_propagating_sites
      << ",\"try_optional_sites\":" << summary.try_optional_sites
      << ",\"try_forced_sites\":" << summary.try_forced_sites
      << ",\"throw_statement_sites\":" << summary.throw_statement_sites
      << ",\"do_catch_sites\":" << summary.do_catch_sites
      << ",\"catch_clause_sites\":" << summary.catch_clause_sites
      << ",\"catch_binding_sites\":" << summary.catch_binding_sites
      << ",\"catch_all_sites\":" << summary.catch_all_sites
      << ",\"throwing_callable_try_sites\":"
      << summary.throwing_callable_try_sites
      << ",\"bridged_callable_try_sites\":" << summary.bridged_callable_try_sites
      << ",\"caller_propagation_sites\":" << summary.caller_propagation_sites
      << ",\"local_handler_sites\":" << summary.local_handler_sites
      << ",\"rethrow_sites\":" << summary.rethrow_sites
      << ",\"contract_violation_sites\":" << summary.contract_violation_sites
      << ",\"source_dependency_required\":"
      << (summary.source_dependency_required ? "true" : "false")
      << ",\"try_surface_landed\":"
      << (summary.try_surface_landed ? "true" : "false")
      << ",\"throw_surface_landed\":"
      << (summary.throw_surface_landed ? "true" : "false")
      << ",\"do_catch_surface_landed\":"
      << (summary.do_catch_surface_landed ? "true" : "false")
      << ",\"throwing_context_legality_enforced\":"
      << (summary.throwing_context_legality_enforced ? "true" : "false")
      << ",\"native_emit_remains_fail_closed\":"
      << (summary.native_emit_remains_fail_closed ? "true" : "false")
      << ",\"deterministic\":" << (summary.deterministic ? "true" : "false")
      << ",\"ready_for_lowering_and_runtime\":"
      << (summary.ready_for_lowering_and_runtime ? "true" : "false")
      << ",\"failure_reason\":\"" << EscapeJsonString(summary.failure_reason)
      << "\",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

std::string BuildErrorHandlingErrorBridgeLegalitySummaryJson(
    const Objc3ErrorHandlingErrorBridgeLegalitySummary &summary) {
  // Semantic-surface key literal: "objc_error_handling_error_bridge_legality":
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
      << "\",\"bridge_callable_sites\":" << summary.bridge_callable_sites
      << ",\"objc_nserror_callable_sites\":"
      << summary.objc_nserror_callable_sites
      << ",\"objc_status_code_callable_sites\":"
      << summary.objc_status_code_callable_sites
      << ",\"semantically_valid_bridge_callable_sites\":"
      << summary.semantically_valid_bridge_callable_sites
      << ",\"try_eligible_bridge_callable_sites\":"
      << summary.try_eligible_bridge_callable_sites
      << ",\"missing_error_out_parameter_sites\":"
      << summary.missing_error_out_parameter_sites
      << ",\"invalid_nserror_return_sites\":"
      << summary.invalid_nserror_return_sites
      << ",\"invalid_status_return_sites\":"
      << summary.invalid_status_return_sites
      << ",\"invalid_error_type_sites\":"
      << summary.invalid_error_type_sites
      << ",\"missing_mapping_symbol_sites\":"
      << summary.missing_mapping_symbol_sites
      << ",\"invalid_mapping_signature_sites\":"
      << summary.invalid_mapping_signature_sites
      << ",\"throws_bridge_conflict_sites\":"
      << summary.throws_bridge_conflict_sites
      << ",\"marker_conflict_sites\":" << summary.marker_conflict_sites
      << ",\"unsupported_combination_sites\":"
      << summary.unsupported_combination_sites
      << ",\"contract_violation_sites\":"
      << summary.contract_violation_sites
      << ",\"source_dependency_required\":"
      << (summary.source_dependency_required ? "true" : "false")
      << ",\"bridge_legality_landed\":"
      << (summary.bridge_legality_landed ? "true" : "false")
      << ",\"try_bridge_filter_landed\":"
      << (summary.try_bridge_filter_landed ? "true" : "false")
      << ",\"unsupported_combinations_fail_closed\":"
      << (summary.unsupported_combinations_fail_closed ? "true" : "false")
      << ",\"native_emit_remains_fail_closed\":"
      << (summary.native_emit_remains_fail_closed ? "true" : "false")
      << ",\"deterministic\":" << (summary.deterministic ? "true" : "false")
      << ",\"ready_for_lowering_and_runtime\":"
      << (summary.ready_for_lowering_and_runtime ? "true" : "false")
      << ",\"failure_reason\":\"" << EscapeJsonString(summary.failure_reason)
      << "\",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

std::string BuildControlFlowControlFlowSafetyLoweringContractJson(
    const Objc3ControlFlowControlFlowSafetyLoweringContract &contract,
    const Objc3ControlFlowControlFlowSemanticModelSummary &semantic_summary,
    const std::string &semantic_summary_replay_key,
    const std::string &replay_key) {
  std::ostringstream out;
  const bool source_semantic_model_ready =
      semantic_summary.deterministic &&
      semantic_summary.ready_for_lowering_and_runtime;
  out << "{"
      << "\"contract_id\":\""
      << EscapeJsonString(kObjc3ControlFlowControlFlowSafetyLoweringContractId)
      << "\",\"surface_path\":\""
      << EscapeJsonString(kObjc3ControlFlowControlFlowSafetyLoweringSurfacePath)
      << "\",\"source_semantic_contract_id\":\""
      << EscapeJsonString(semantic_summary.contract_id)
      << "\",\"guard_model\":\""
      << EscapeJsonString(kObjc3ControlFlowControlFlowSafetyLoweringGuardModel)
      << "\",\"match_model\":\""
      << EscapeJsonString(kObjc3ControlFlowControlFlowSafetyLoweringMatchModel)
      << "\",\"defer_model\":\""
      << EscapeJsonString(kObjc3ControlFlowControlFlowSafetyLoweringDeferModel)
      << "\",\"authority_model\":\""
      << EscapeJsonString(kObjc3ControlFlowControlFlowSafetyLoweringAuthorityModel)
      << "\",\"fail_closed_model\":\""
      << EscapeJsonString(kObjc3ControlFlowControlFlowSafetyLoweringFailClosedModel)
      << "\",\"guard_statement_sites\":"
      << contract.guard_statement_sites
      << ",\"guard_clause_sites\":" << contract.guard_clause_sites
      << ",\"match_statement_sites\":" << contract.match_statement_sites
      << ",\"defer_statement_sites\":" << contract.defer_statement_sites
      << ",\"live_guard_short_circuit_sites\":"
      << contract.live_guard_short_circuit_sites
      << ",\"live_match_dispatch_sites\":"
      << contract.live_match_dispatch_sites
      << ",\"live_defer_cleanup_sites\":"
      << contract.live_defer_cleanup_sites
      << ",\"fail_closed_guard_short_circuit_sites\":"
      << contract.fail_closed_guard_short_circuit_sites
      << ",\"fail_closed_match_dispatch_sites\":"
      << contract.fail_closed_match_dispatch_sites
      << ",\"fail_closed_defer_cleanup_sites\":"
      << contract.fail_closed_defer_cleanup_sites
      << ",\"deterministic_fail_closed_sites\":"
      << contract.deterministic_fail_closed_sites
      << ",\"contract_violation_sites\":"
      << contract.contract_violation_sites
      << ",\"deterministic\":"
      << (contract.deterministic ? "true" : "false")
      << ",\"fail_closed\":"
      << (contract.deterministic_fail_closed_sites > 0u ? "true" : "false")
      << ",\"source_semantic_model_ready\":"
      << (source_semantic_model_ready ? "true" : "false")
      << ",\"ready_for_native_guard_lowering\":true"
      << ",\"ready_for_native_match_lowering\":"
      << (contract.fail_closed_match_dispatch_sites == 0u ? "true" : "false")
      << ",\"ready_for_native_defer_lowering\":true"
      << ",\"semantic_summary_replay_key\":\""
      << EscapeJsonString(semantic_summary_replay_key)
      << "\",\"replay_key\":\"" << EscapeJsonString(replay_key)
      << "\"}";
  return out.str();
}

std::string BuildTypeSystemOptionalKeypathLoweringContractJson(
    const Objc3TypeSystemOptionalKeypathLoweringContract &contract,
    const Objc3TypeSystemTypeSemanticModelSummary &semantic_summary,
    const std::string &semantic_summary_replay_key,
    const std::string &message_send_selector_lowering_replay_key,
    const std::string &dispatch_abi_marshalling_replay_key,
    const std::string &nil_receiver_semantics_foldability_replay_key,
    const std::string &replay_key) {
  std::ostringstream out;
  const bool source_semantic_model_ready = semantic_summary.deterministic;
  const bool typed_keypath_artifact_emission_deferred =
      contract.deferred_typed_keypath_sites != 0;
  const bool ready_for_native_optional_lowering =
      contract.deterministic && contract.contract_violation_sites == 0;
  const bool ready_for_typed_keypath_artifact_emission =
      contract.deterministic && contract.contract_violation_sites == 0 &&
      contract.live_typed_keypath_artifact_sites ==
          contract.typed_keypath_literal_sites;
  out << "{"
      << "\"contract_id\":\""
      << EscapeJsonString(kObjc3TypeSystemOptionalKeypathLoweringContractId)
      << "\",\"surface_path\":\""
      << EscapeJsonString(kObjc3TypeSystemOptionalKeypathLoweringSurfacePath)
      << "\",\"source_semantic_contract_id\":\""
      << EscapeJsonString(semantic_summary.contract_id)
      << "\",\"optional_model\":\""
      << EscapeJsonString(kObjc3TypeSystemOptionalKeypathLoweringOptionalModel)
      << "\",\"typed_keypath_model\":\""
      << EscapeJsonString(kObjc3TypeSystemOptionalKeypathLoweringTypedKeypathModel)
      << "\",\"authority_model\":\""
      << EscapeJsonString(kObjc3TypeSystemOptionalKeypathLoweringAuthorityModel)
      << "\",\"fail_closed_model\":\""
      << EscapeJsonString(kObjc3TypeSystemOptionalKeypathLoweringFailClosedModel)
      << "\",\"optional_binding_sites\":"
      << contract.optional_binding_sites
      << ",\"optional_binding_clause_sites\":"
      << contract.optional_binding_clause_sites
      << ",\"optional_send_sites\":" << contract.optional_send_sites
      << ",\"nil_coalescing_sites\":" << contract.nil_coalescing_sites
      << ",\"typed_keypath_literal_sites\":"
      << contract.typed_keypath_literal_sites
      << ",\"typed_keypath_self_root_sites\":"
      << contract.typed_keypath_self_root_sites
      << ",\"typed_keypath_class_root_sites\":"
      << contract.typed_keypath_class_root_sites
      << ",\"live_optional_lowering_sites\":"
      << contract.live_optional_lowering_sites
      << ",\"single_evaluation_nil_short_circuit_sites\":"
      << contract.single_evaluation_nil_short_circuit_sites
      << ",\"live_typed_keypath_artifact_sites\":"
      << contract.live_typed_keypath_artifact_sites
      << ",\"deferred_typed_keypath_sites\":"
      << contract.deferred_typed_keypath_sites
      << ",\"contract_violation_sites\":"
      << contract.contract_violation_sites
      << ",\"deterministic\":"
      << (contract.deterministic ? "true" : "false")
      << ",\"fail_closed\":true"
      << ",\"source_semantic_model_ready\":"
      << (source_semantic_model_ready ? "true" : "false")
      << ",\"message_send_selector_lowering_ready\":true"
      << ",\"dispatch_abi_marshalling_ready\":true"
      << ",\"nil_receiver_semantics_foldability_ready\":true"
      << ",\"live_optional_binding_lowering_landed\":true"
      << ",\"live_optional_send_lowering_landed\":true"
      << ",\"live_nil_coalescing_lowering_landed\":true"
      << ",\"single_evaluation_nil_short_circuit_landed\":true"
      << ",\"typed_keypath_artifact_emission_deferred\":"
      << (typed_keypath_artifact_emission_deferred ? "true" : "false")
      << ",\"ready_for_native_optional_lowering\":"
      << (ready_for_native_optional_lowering ? "true" : "false")
      << ",\"ready_for_typed_keypath_artifact_emission\":"
      << (ready_for_typed_keypath_artifact_emission ? "true" : "false")
      << ",\"semantic_summary_replay_key\":\""
      << EscapeJsonString(semantic_summary_replay_key)
      << "\",\"message_send_selector_lowering_replay_key\":\""
      << EscapeJsonString(message_send_selector_lowering_replay_key)
      << "\",\"dispatch_abi_marshalling_replay_key\":\""
      << EscapeJsonString(dispatch_abi_marshalling_replay_key)
      << "\",\"nil_receiver_semantics_foldability_replay_key\":\""
      << EscapeJsonString(nil_receiver_semantics_foldability_replay_key)
      << "\",\"replay_key\":\"" << EscapeJsonString(replay_key) << "\"}";
  return out.str();
}

std::string BuildTypeSystemOptionalKeypathRuntimeHelperContractJson(
    const Objc3TypeSystemOptionalKeypathLoweringContract &contract,
    const Objc3RuntimeSupportLibraryLinkWiringSummary &runtime_link_wiring,
    const std::string &lowering_replay_key) {
  const bool optional_send_runtime_ready =
      runtime_link_wiring.ready_for_runtime_library_consumption &&
      contract.live_optional_lowering_sites >= contract.optional_send_sites;
  const bool typed_keypath_descriptor_handles_ready =
      contract.live_typed_keypath_artifact_sites ==
          contract.typed_keypath_literal_sites &&
      contract.deferred_typed_keypath_sites == 0;
  const bool typed_keypath_runtime_execution_helper_landed = true;
  const bool diagnostic_fail_closed_ready = true;
  std::ostringstream replay_key;
  replay_key << "contract="
             << kObjc3TypeSystemOptionalKeypathRuntimeHelperContractId
             << ";lowering_replay=" << lowering_replay_key
             << ";runtime_link_ready="
             << (runtime_link_wiring.ready_for_runtime_library_consumption
                     ? "true"
                     : "false")
             << ";optional_send_sites=" << contract.optional_send_sites
             << ";live_optional_lowering_sites="
             << contract.live_optional_lowering_sites
             << ";typed_keypath_literal_sites="
             << contract.typed_keypath_literal_sites
             << ";live_typed_keypath_artifact_sites="
             << contract.live_typed_keypath_artifact_sites;
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\""
      << EscapeJsonString(kObjc3TypeSystemOptionalKeypathRuntimeHelperContractId)
      << "\",\"source_lowering_contract_id\":\""
      << EscapeJsonString(kObjc3TypeSystemOptionalKeypathLoweringContractId)
      << "\",\"runtime_link_wiring_contract_id\":\""
      << EscapeJsonString(kObjc3RuntimeSupportLibraryLinkWiringContractId)
      << "\",\"frontend_surface_path\":\""
      << EscapeJsonString(kObjc3TypeSystemOptionalKeypathRuntimeHelperSurfacePath)
      << "\",\"optional_send_helper_model\":\""
      << EscapeJsonString(kObjc3TypeSystemOptionalKeypathRuntimeHelperOptionalModel)
      << "\",\"typed_keypath_helper_model\":\""
      << EscapeJsonString(
             kObjc3TypeSystemOptionalKeypathRuntimeHelperTypedKeypathModel)
      << "\",\"diagnostic_fail_closed_model\":\""
      << EscapeJsonString(
             kObjc3TypeSystemOptionalKeypathRuntimeHelperDiagnosticModel)
      << "\",\"public_lookup_selector_symbol\":\""
      << EscapeJsonString(kObjc3RuntimeSupportLibraryLookupSelectorSymbol)
      << "\",\"public_dispatch_i32_symbol\":\""
      << EscapeJsonString(kObjc3RuntimeSupportLibraryDispatchI32Symbol)
      << "\",\"keypath_descriptor_section\":\""
      << EscapeJsonString(kObjc3RuntimeKeypathDescriptorLogicalSection)
      << "\",\"keypath_descriptor_aggregate_symbol\":\"__objc3_sec_keypath_descriptors\""
      << ",\"runtime_library_archive_available\":"
      << (runtime_link_wiring.runtime_library_archive_available ? "true"
                                                                : "false")
      << ",\"runtime_library_consumption_ready\":"
      << (runtime_link_wiring.ready_for_runtime_library_consumption ? "true"
                                                                    : "false")
      << ",\"optional_send_runtime_ready\":"
      << (optional_send_runtime_ready ? "true" : "false")
      << ",\"typed_keypath_descriptor_handles_ready\":"
      << (typed_keypath_descriptor_handles_ready ? "true" : "false")
      << ",\"typed_keypath_runtime_execution_helper_landed\":"
      << (typed_keypath_runtime_execution_helper_landed ? "true" : "false")
      << ",\"diagnostic_fail_closed_ready\":"
      << (diagnostic_fail_closed_ready ? "true" : "false")
      << ",\"fail_closed\":true"
      << ",\"replay_key\":\"" << EscapeJsonString(replay_key.str()) << "\"}";
  return out.str();
}

std::string BuildRuntimeAwareImportModuleSurfaceReplayKey(
    const Objc3Program &program,
    const Objc3ParserContractSnapshot &parser_contract_snapshot,
    const Objc3ModuleImportGraphLoweringContract
        &module_import_graph_lowering_contract) {
  std::ostringstream out;
  out << kObjc3RuntimeAwareImportModuleSurfaceContractId
      << ";module_name=" << program.module_name
      << ";protocol_decl_count=" << parser_contract_snapshot.protocol_decl_count
      << ";interface_decl_count="
      << parser_contract_snapshot.interface_decl_count
      << ";implementation_decl_count="
      << parser_contract_snapshot.implementation_decl_count
      << ";interface_category_decl_count="
      << parser_contract_snapshot.interface_category_decl_count
      << ";implementation_category_decl_count="
      << parser_contract_snapshot.implementation_category_decl_count
      << ";function_decl_count=" << parser_contract_snapshot.function_decl_count
      << ";module_import_graph_sites="
      << module_import_graph_lowering_contract.module_import_graph_sites
      << ";import_edge_candidate_sites="
      << module_import_graph_lowering_contract.import_edge_candidate_sites
      << ";object_pointer_type_sites="
      << module_import_graph_lowering_contract.object_pointer_type_sites
      << ";normalized_sites="
      << module_import_graph_lowering_contract.normalized_sites
      << ";runtime_aware_import_declarations_landed=false"
      << ";module_metadata_import_surface_landed=false"
      << ";runtime_owned_declaration_import_landed=false"
      << ";runtime_metadata_reference_import_landed=false"
      << ";public_frontend_api_module_surface_landed=false"
      << ";fail_closed=true"
      << ";deterministic="
      << (module_import_graph_lowering_contract.deterministic ? "true" : "false");
  return out.str();
}

std::string BuildRuntimeAwareImportModuleSurfaceSummaryJson(
    const Objc3Program &program,
    const Objc3ParserContractSnapshot &parser_contract_snapshot,
    const Objc3ModuleImportGraphLoweringContract
        &module_import_graph_lowering_contract) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\""
      << kObjc3RuntimeAwareImportModuleSurfaceContractId
      << "\",\"surface_path\":\""
      << kObjc3RuntimeAwareImportModuleSurfacePath
      << "\",\"source_model\":\""
      << kObjc3RuntimeAwareImportModuleSurfaceSourceModel
      << "\",\"non_goal_model\":\""
      << kObjc3RuntimeAwareImportModuleSurfaceNonGoalModel
      << "\",\"failure_model\":\""
      << kObjc3RuntimeAwareImportModuleSurfaceFailureModel
      << "\",\"module_name\":\"" << EscapeJsonString(program.module_name)
      << "\",\"protocol_decl_count\":"
      << parser_contract_snapshot.protocol_decl_count
      << ",\"interface_decl_count\":"
      << parser_contract_snapshot.interface_decl_count
      << ",\"implementation_decl_count\":"
      << parser_contract_snapshot.implementation_decl_count
      << ",\"interface_category_decl_count\":"
      << parser_contract_snapshot.interface_category_decl_count
      << ",\"implementation_category_decl_count\":"
      << parser_contract_snapshot.implementation_category_decl_count
      << ",\"function_decl_count\":"
      << parser_contract_snapshot.function_decl_count
      << ",\"module_import_graph_sites\":"
      << module_import_graph_lowering_contract.module_import_graph_sites
      << ",\"import_edge_candidate_sites\":"
      << module_import_graph_lowering_contract.import_edge_candidate_sites
      << ",\"namespace_segment_sites\":"
      << module_import_graph_lowering_contract.namespace_segment_sites
      << ",\"object_pointer_type_sites\":"
      << module_import_graph_lowering_contract.object_pointer_type_sites
      << ",\"pointer_declarator_sites\":"
      << module_import_graph_lowering_contract.pointer_declarator_sites
      << ",\"normalized_sites\":"
      << module_import_graph_lowering_contract.normalized_sites
      << ",\"contract_violation_sites\":"
      << module_import_graph_lowering_contract.contract_violation_sites
      << ",\"runtime_aware_import_declarations_landed\":false"
      << ",\"module_metadata_import_surface_landed\":false"
      << ",\"runtime_owned_declaration_import_landed\":false"
      << ",\"runtime_metadata_reference_import_landed\":false"
      << ",\"public_frontend_api_module_surface_landed\":false"
      << ",\"fail_closed\":true"
      << ",\"deterministic\":"
      << (module_import_graph_lowering_contract.deterministic ? "true" : "false")
      << ",\"ready_for_core_feature_implementation\":true"
      << ",\"next_issue\":\"objc3c.importmodule.runtimeaware.corefeature.v1\""
      << ",\"replay_key\":\""
      << EscapeJsonString(BuildRuntimeAwareImportModuleSurfaceReplayKey(
             program,
             parser_contract_snapshot,
             module_import_graph_lowering_contract))
      << "\"}";
  return out.str();
}

std::size_t CountRuntimeAwareImportProtocolReferences(
    const Objc3RuntimeMetadataSourceRecordSet &records) {
  std::size_t total = 0;
  for (const auto &class_record : records.classes_lexicographic) {
    total += class_record.adopted_protocols_lexicographic.size();
  }
  for (const auto &protocol_record : records.protocols_lexicographic) {
    total += protocol_record.inherited_protocols_lexicographic.size();
  }
  for (const auto &category_record : records.categories_lexicographic) {
    total += category_record.adopted_protocols_lexicographic.size();
  }
  return total;
}

std::size_t CountRuntimeAwareImportPropertyAccessorReferences(
    const Objc3RuntimeMetadataSourceRecordSet &records) {
  std::size_t total = 0;
  for (const auto &property_record : records.properties_lexicographic) {
    if (!property_record.effective_getter_selector.empty()) {
      ++total;
    }
    if (property_record.effective_setter_available &&
        !property_record.effective_setter_selector.empty()) {
      ++total;
    }
  }
  return total;
}

std::size_t CountRuntimeAwareImportPropertyIvarBindingReferences(
    const Objc3RuntimeMetadataSourceRecordSet &records) {
  std::size_t total = 0;
  for (const auto &property_record : records.properties_lexicographic) {
    if (!property_record.ivar_binding_symbol.empty()) {
      ++total;
    }
  }
  return total;
}

std::string BuildRuntimeAwareImportModuleFrontendClosureReplayKey(
    const Objc3RuntimeAwareImportModuleFrontendClosureSummary &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";source_surface_contract_id=" << summary.source_surface_contract_id
      << ";frontend_surface_path=" << summary.frontend_surface_path
      << ";payload_model=" << summary.payload_model
      << ";artifact_relative_path=" << summary.artifact_relative_path
      << ";authority_model=" << summary.authority_model
      << ";payload_ownership_model=" << summary.payload_ownership_model
      << ";module_name=" << summary.module_name
      << ";protocol_decl_count=" << summary.protocol_decl_count
      << ";interface_decl_count=" << summary.interface_decl_count
      << ";implementation_decl_count=" << summary.implementation_decl_count
      << ";interface_category_decl_count="
      << summary.interface_category_decl_count
      << ";implementation_category_decl_count="
      << summary.implementation_category_decl_count
      << ";function_decl_count=" << summary.function_decl_count
      << ";module_import_graph_sites=" << summary.module_import_graph_sites
      << ";import_edge_candidate_sites="
      << summary.import_edge_candidate_sites
      << ";namespace_segment_sites=" << summary.namespace_segment_sites
      << ";object_pointer_type_sites=" << summary.object_pointer_type_sites
      << ";pointer_declarator_sites=" << summary.pointer_declarator_sites
      << ";normalized_sites=" << summary.normalized_sites
      << ";contract_violation_sites=" << summary.contract_violation_sites
      << ";class_record_count=" << summary.class_record_count
      << ";protocol_record_count=" << summary.protocol_record_count
      << ";category_record_count=" << summary.category_record_count
      << ";property_record_count=" << summary.property_record_count
      << ";method_record_count=" << summary.method_record_count
      << ";ivar_record_count=" << summary.ivar_record_count
      << ";runtime_owned_declaration_count="
      << summary.runtime_owned_declaration_count
      << ";superclass_reference_count=" << summary.superclass_reference_count
      << ";protocol_reference_count=" << summary.protocol_reference_count
      << ";property_accessor_reference_count="
      << summary.property_accessor_reference_count
      << ";property_ivar_binding_reference_count="
      << summary.property_ivar_binding_reference_count
      << ";method_selector_reference_count="
      << summary.method_selector_reference_count
      << ";metadata_reference_count=" << summary.metadata_reference_count
      << ";source_surface_replay_key=" << summary.source_surface_replay_key;
  return out.str();
}

Objc3RuntimeAwareImportModuleFrontendClosureSummary
BuildRuntimeAwareImportModuleFrontendClosureSummary(
    const Objc3Program &program,
    const Objc3ParserContractSnapshot &parser_contract_snapshot,
    const Objc3ModuleImportGraphLoweringContract
        &module_import_graph_lowering_contract,
    const Objc3RuntimeMetadataSourceRecordSet &runtime_metadata_source_records) {
  Objc3RuntimeAwareImportModuleFrontendClosureSummary summary;
  summary.fail_closed = true;
  summary.source_surface_contract_ready =
      !program.module_name.empty() &&
      IsValidObjc3ModuleImportGraphLoweringContract(
          module_import_graph_lowering_contract);
  summary.runtime_metadata_source_records_ready =
      IsReadyObjc3RuntimeMetadataSourceRecordSet(runtime_metadata_source_records);
  summary.frontend_surface_published = true;
  summary.import_artifact_template_published = true;
  summary.runtime_aware_import_declarations_landed = true;
  summary.module_metadata_import_surface_landed = true;
  summary.runtime_owned_declaration_import_landed = true;
  summary.runtime_metadata_reference_import_landed = true;
  summary.public_frontend_api_module_surface_landed = true;
  summary.module_name = program.module_name;
  summary.protocol_decl_count = parser_contract_snapshot.protocol_decl_count;
  summary.interface_decl_count = parser_contract_snapshot.interface_decl_count;
  summary.implementation_decl_count =
      parser_contract_snapshot.implementation_decl_count;
  summary.interface_category_decl_count =
      parser_contract_snapshot.interface_category_decl_count;
  summary.implementation_category_decl_count =
      parser_contract_snapshot.implementation_category_decl_count;
  summary.function_decl_count = parser_contract_snapshot.function_decl_count;
  summary.module_import_graph_sites =
      module_import_graph_lowering_contract.module_import_graph_sites;
  summary.import_edge_candidate_sites =
      module_import_graph_lowering_contract.import_edge_candidate_sites;
  summary.namespace_segment_sites =
      module_import_graph_lowering_contract.namespace_segment_sites;
  summary.object_pointer_type_sites =
      module_import_graph_lowering_contract.object_pointer_type_sites;
  summary.pointer_declarator_sites =
      module_import_graph_lowering_contract.pointer_declarator_sites;
  summary.normalized_sites = module_import_graph_lowering_contract.normalized_sites;
  summary.contract_violation_sites =
      module_import_graph_lowering_contract.contract_violation_sites;
  summary.class_record_count =
      runtime_metadata_source_records.classes_lexicographic.size();
  summary.protocol_record_count =
      runtime_metadata_source_records.protocols_lexicographic.size();
  summary.category_record_count =
      runtime_metadata_source_records.categories_lexicographic.size();
  summary.property_record_count =
      runtime_metadata_source_records.properties_lexicographic.size();
  summary.method_record_count =
      runtime_metadata_source_records.methods_lexicographic.size();
  summary.ivar_record_count =
      runtime_metadata_source_records.ivars_lexicographic.size();
  summary.runtime_owned_declaration_count =
      summary.class_record_count + summary.protocol_record_count +
