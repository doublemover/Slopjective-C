#include "artifacts/objc3_frontend_concurrency_semantic_artifacts.h"

#include <sstream>
#include <string>

#include "io/objc3_json.h"
#include "lower/contracts/concurrency_task_runtime_helper_contracts.h"

namespace objc3::artifacts::frontend {
namespace {

using objc3::io::EscapeJsonString;

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

std::string BuildConcurrencyExecutorHopAffinityCompatibilitySummaryJson(
    const Objc3ConcurrencyExecutorHopAffinityCompatibilitySummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"dependency_contract_id\":\""
      << EscapeJsonString(summary.dependency_contract_id)
      << "\",\"surface_path\":\"" << EscapeJsonString(summary.surface_path)
      << "\",\"semantic_model\":\"" << EscapeJsonString(summary.semantic_model)
      << "\",\"deferred_model\":\"" << EscapeJsonString(summary.deferred_model)
      << "\",\"async_callable_sites\":" << summary.async_callable_sites
      << ",\"executor_affinity_sites\":" << summary.executor_affinity_sites
      << ",\"executor_main_sites\":" << summary.executor_main_sites
      << ",\"executor_global_sites\":" << summary.executor_global_sites
      << ",\"executor_named_sites\":" << summary.executor_named_sites
      << ",\"task_creation_sites\":" << summary.task_creation_sites
      << ",\"detached_task_creation_sites\":"
      << summary.detached_task_creation_sites
      << ",\"illegal_missing_executor_affinity_sites\":"
      << summary.illegal_missing_executor_affinity_sites
      << ",\"illegal_main_executor_detached_sites\":"
      << summary.illegal_main_executor_detached_sites
      << ",\"dependency_required\":"
      << (summary.dependency_required ? "true" : "false")
      << ",\"executor_affinity_required_for_task_callables_enforced\":"
      << (summary.executor_affinity_required_for_task_callables_enforced
              ? "true"
              : "false")
      << ",\"detached_task_hop_boundary_enforced\":"
      << (summary.detached_task_hop_boundary_enforced ? "true" : "false")
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
