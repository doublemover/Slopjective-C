namespace {

inline constexpr const char
    *kArtifactConcurrencyTaskRuntimeLoweringContractId =
        "objc3c.concurrency.task.runtime.lowering.contract.v1";
inline constexpr const char
    *kArtifactConcurrencyTaskRuntimeLoweringSurfacePath =
        "frontend.pipeline.semantic_surface."
        "objc_concurrency_task_runtime_lowering_contract";
inline constexpr const char
    *kArtifactConcurrencyTaskRuntimeAbiCompletionContractId =
        "objc3c.concurrency.task.runtime.abi.completion.v1";
inline constexpr const char
    *kArtifactConcurrencyTaskRuntimeAbiCompletionSurfacePath =
        "frontend.pipeline.semantic_surface."
        "objc_concurrency_task_group_and_runtime_abi_completion";
inline constexpr const char *kArtifactRuntimeSpawnTaskI32Symbol =
    "objc3_runtime_spawn_task_i32";
inline constexpr const char *kArtifactRuntimeEnterTaskGroupScopeI32Symbol =
    "objc3_runtime_enter_task_group_scope_i32";
inline constexpr const char *kArtifactRuntimeAddTaskGroupTaskI32Symbol =
    "objc3_runtime_add_task_group_task_i32";
inline constexpr const char *kArtifactRuntimeWaitTaskGroupNextI32Symbol =
    "objc3_runtime_wait_task_group_next_i32";
inline constexpr const char *kArtifactRuntimeCancelTaskGroupI32Symbol =
    "objc3_runtime_cancel_task_group_i32";
inline constexpr const char *kArtifactRuntimeTaskIsCancelledI32Symbol =
    "objc3_runtime_task_is_cancelled_i32";
inline constexpr const char *kArtifactRuntimeTaskOnCancelI32Symbol =
    "objc3_runtime_task_on_cancel_i32";
inline constexpr const char *kArtifactRuntimeExecutorHopI32Symbol =
    "objc3_runtime_executor_hop_i32";
inline constexpr const char *kObjc3ConcurrencyTaskRuntimeLoweringTaskModel =
    "task-creation-cancellation-polls-and-task-group-artifacts-now-lower-through-explicit-replay-stable-lane-contracts";
inline constexpr const char *kObjc3ConcurrencyTaskRuntimeLoweringExecutorModel =
    "executor-affinity-and-detached-hop-boundaries-now-lower-through-explicit-actor-and-task-runtime-profile-handoffs";
inline constexpr const char
    *kObjc3ConcurrencyTaskRuntimeLoweringConcurrencyModel =
        "scheduler-visible-task-handoff-and-cancellation-guard-proof-points-now-lower-through-deterministic-concurrency-replay-profiles";
inline constexpr const char *kObjc3ConcurrencyTaskRuntimeLoweringDeferredModel =
    "native-task-spawn-executor-hop-cancellation-runtime-entrypoints-and-task-group-abi-completion-remain-later-runtime-work";
inline constexpr const char
    *kObjc3ConcurrencyTaskRuntimeAbiCompletionArtifactModel =
        "helper-backed-task-runtime-lowering-now-publishes-a-dedicated-abi-and-runtime-proof-packet";
inline constexpr const char *kObjc3ConcurrencyTaskRuntimeAbiCompletionProofModel =
    "scheduler-visible-runtime-proof-remains-private-and-snapshotted-through-objc3_runtime_copy_task_runtime_state_for_testing";

}  // namespace

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
      << EscapeJsonString(kArtifactConcurrencyTaskRuntimeLoweringContractId)
      << "\",\"surface_path\":\""
      << EscapeJsonString(kArtifactConcurrencyTaskRuntimeLoweringSurfacePath)
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
      << EscapeJsonString(kArtifactConcurrencyTaskRuntimeAbiCompletionContractId)
      << "\",\"surface_path\":\""
      << EscapeJsonString(
             kArtifactConcurrencyTaskRuntimeAbiCompletionSurfacePath)
      << "\",\"lowering_contract_id\":\""
      << EscapeJsonString(kArtifactConcurrencyTaskRuntimeLoweringContractId)
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
      << "\"" << EscapeJsonString(kArtifactRuntimeSpawnTaskI32Symbol) << "\","
      << "\""
      << EscapeJsonString(kArtifactRuntimeEnterTaskGroupScopeI32Symbol)
      << "\","
      << "\"" << EscapeJsonString(kArtifactRuntimeAddTaskGroupTaskI32Symbol)
      << "\","
      << "\"" << EscapeJsonString(kArtifactRuntimeWaitTaskGroupNextI32Symbol)
      << "\","
      << "\"" << EscapeJsonString(kArtifactRuntimeCancelTaskGroupI32Symbol)
      << "\","
      << "\"" << EscapeJsonString(kArtifactRuntimeTaskIsCancelledI32Symbol)
      << "\","
      << "\"" << EscapeJsonString(kArtifactRuntimeTaskOnCancelI32Symbol)
      << "\","
      << "\"" << EscapeJsonString(kArtifactRuntimeExecutorHopI32Symbol)
      << "\""
      << "]}";
  return out.str();
}
