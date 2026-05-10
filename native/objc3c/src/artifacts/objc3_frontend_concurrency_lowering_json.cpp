#include "artifacts/objc3_frontend_concurrency_semantic_artifacts.h"

#include <sstream>
#include <string>

#include "io/objc3_json.h"

namespace objc3::artifacts::frontend {
namespace {

using objc3::io::EscapeJsonString;

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
inline constexpr const char
    *kObjc3ConcurrencyContinuationAbiAsyncLoweringContractId =
        "objc3c.concurrency.continuation.abi.async.lowering.contract.v1";
inline constexpr const char
    *kObjc3ConcurrencyContinuationAbiAsyncLoweringSurfacePath =
        "frontend.pipeline.semantic_surface."
        "objc_concurrency_continuation_abi_and_async_lowering_contract";
inline constexpr const char
    *kObjc3ConcurrencyContinuationAbiAsyncLoweringContinuationModel =
        "async-entry-points-carry-deterministic-continuation-lowering-replay-keys-and-counts-into-emitted-ir";
inline constexpr const char
    *kObjc3ConcurrencyContinuationAbiAsyncLoweringAwaitModel =
        "await-suspension-state-lowering-replay-keys-and-counts-are-published-alongside-continuation-lowering";
inline constexpr const char
    *kObjc3ConcurrencyContinuationAbiAsyncLoweringDeferredModel =
        "runnable-async-frame-layout-resume-cleanup-and-executor-runtime-execution-remain-later-runtime-work";
inline constexpr const char
    *kObjc3ConcurrencyAsyncDirectCallLoweringContractId =
        "objc3c.concurrency.async.direct.call.lowering.v1";
inline constexpr const char
    *kObjc3ConcurrencyAsyncDirectCallLoweringSurfacePath =
        "frontend.pipeline.semantic_surface."
        "objc_concurrency_async_function_await_and_continuation_lowering";
inline constexpr const char
    *kObjc3ConcurrencyAsyncDirectCallLoweringImplementationModel =
        "supported-async-functions-and-await-lower-through-direct-calls-in-native-ir-and-object-emission-for-the-current-non-suspending-slice";
inline constexpr const char *kObjc3ConcurrencyAsyncDirectCallLoweringAwaitModel =
    "await-marked-expressions-currently-reuse-the-operand-direct-call-lowering-path-without-materializing-a-suspension-state-machine";
inline constexpr const char
    *kObjc3ConcurrencyAsyncDirectCallLoweringDeferredModel =
        "continuation-allocation-resume-suspend-state-machine-cleanup-and-executor-runtime-scheduling-remain-later-runtime-work";
inline constexpr const char
    *kObjc3ConcurrencySuspensionCleanupIntegrationContractId =
        "objc3c.concurrency.suspension.autorelease.cleanup.integration.v1";
inline constexpr const char
    *kObjc3ConcurrencySuspensionCleanupIntegrationSurfacePath =
        "frontend.pipeline.semantic_surface."
        "objc_concurrency_suspension_autorelease_and_cleanup_integration";
inline constexpr const char *kObjc3ConcurrencySuspensionCleanupIntegrationModel =
    "supported-non-suspending-async-lowering-reuses-existing-autoreleasepool-scope-and-defer-cleanup-lowering-through-real-ir-and-object-emission";
inline constexpr const char
    *kObjc3ConcurrencySuspensionCleanupIntegrationOrderingModel =
        "current-proof-fixtures-show-terminal-return-paths-compose-await-direct-call-lowering-with-autoreleasepool-pop-and-defer-cleanup-without-a-separate-suspension-runtime";
inline constexpr const char
    *kObjc3ConcurrencySuspensionCleanupIntegrationDeferredModel =
        "continuation-resume-cleanup-suspension-state-frames-and-executor-runtime-execution-remain-later-runtime-work";

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
      << EscapeJsonString(
             kObjc3ConcurrencyContinuationAbiAsyncLoweringDeferredModel)
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
      << ",\"await_keyword_sites\":" << await_contract.await_keyword_sites
      << ",\"await_suspension_point_sites\":"
      << await_contract.await_suspension_point_sites
      << ",\"await_resume_sites\":" << await_contract.await_resume_sites
      << ",\"await_state_machine_sites\":"
      << await_contract.await_state_machine_sites
      << ",\"await_continuation_sites\":"
      << await_contract.await_continuation_sites
      << ",\"await_normalized_sites\":" << await_contract.normalized_sites
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
      << EscapeJsonString(
             kObjc3ConcurrencyAsyncDirectCallLoweringImplementationModel)
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
      << "\",\"async_function_sites\":" << source_summary.async_function_sites
      << ",\"async_method_sites\":" << source_summary.async_method_sites
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
      << ",\"await_resume_sites\":" << await_contract.await_resume_sites
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
      << "\",\"await_replay_key\":\"" << EscapeJsonString(await_replay_key)
      << "\",\"deterministic\":" << (deterministic ? "true" : "false")
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
      << EscapeJsonString(
             kObjc3ConcurrencySuspensionCleanupIntegrationOrderingModel)
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
      << ",\"await_resume_sites\":" << await_contract.await_resume_sites
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
      << "\",\"await_replay_key\":\"" << EscapeJsonString(await_replay_key)
      << "\",\"autoreleasepool_scope_supported\":"
      << (!autoreleasepool_replay_key.empty() ? "true" : "false")
      << ",\"defer_cleanup_supported\":"
      << (control_flow_contract.live_defer_cleanup_sites > 0 ? "true" : "false")
      << ",\"direct_call_lowering_supported\":"
      << (deterministic ? "true" : "false")
      << ",\"suspension_runtime_required\":false"
      << ",\"state_machine_emission_present\":false"
      << ",\"deterministic\":" << (deterministic ? "true" : "false")
      << ",\"ready_for_ir_object_emission\":"
      << (deterministic ? "true" : "false")
      << "}";
  return out.str();
}

}  // namespace objc3::artifacts::frontend
