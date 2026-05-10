#include "artifacts/objc3_frontend_artifact_concurrency_runtime_manifest_surfaces.h"

#include <ostream>

#include "artifacts/objc3_frontend_concurrency_semantic_artifacts.h"

namespace objc3::artifacts::frontend {

void WriteConcurrencyRuntimeManifestSurfaces(
    std::ostream &manifest,
    const Objc3ConcurrencyStructuredTaskCancellationSemanticSummary
        &concurrency_structured_task_cancellation_semantic_summary,
    const Objc3ConcurrencyExecutorHopAffinityCompatibilitySummary
        &concurrency_executor_hop_affinity_compatibility_summary,
    const Objc3ConcurrencyAwaitSuspensionResumeSemanticSummary
        &concurrency_await_suspension_resume_semantic_summary,
    const Objc3ConcurrencyAsyncDiagnosticsCompatibilitySummary
        &concurrency_async_diagnostics_compatibility_summary,
    const Objc3AsyncContinuationLoweringContract
        &concurrency_async_continuation_lowering_contract,
    const Objc3AwaitLoweringSuspensionStateLoweringContract
        &concurrency_await_lowering_suspension_state_lowering_contract,
    const std::string &concurrency_async_continuation_lowering_replay_key,
    const std::string
        &concurrency_await_lowering_suspension_state_lowering_replay_key,
    const Objc3ConcurrencyTaskExecutorCancellationSemanticModelSummary
        &concurrency_task_executor_cancellation_semantic_model_summary,
    const Objc3ActorIsolationSendabilityLoweringContract
        &concurrency_actor_isolation_sendability_lowering_contract,
    const std::string &concurrency_actor_isolation_sendability_lowering_replay_key,
    const Objc3TaskRuntimeInteropCancellationLoweringContract
        &concurrency_task_runtime_interop_cancellation_lowering_contract,
    const std::string
        &concurrency_task_runtime_interop_cancellation_lowering_replay_key,
    const Objc3ConcurrencyReplayRaceGuardLoweringContract
        &concurrency_concurrency_replay_race_guard_lowering_contract,
    const std::string
        &concurrency_concurrency_replay_race_guard_lowering_replay_key,
    const Objc3FrontendConcurrencyAsyncSourceClosureSummary
        &concurrency_async_source_closure_summary,
    const Objc3ControlFlowControlFlowSafetyLoweringContract
        &control_flow_control_flow_safety_lowering_contract,
    const std::string &control_flow_control_flow_safety_lowering_replay_key,
    const Objc3AutoreleasePoolScopeLoweringContract
        &autoreleasepool_scope_lowering_contract,
    const std::string &autoreleasepool_scope_lowering_replay_key) {
  manifest
      << ",\"objc_concurrency_structured_task_and_cancellation_semantics\":"
      << BuildConcurrencyStructuredTaskCancellationSemanticSummaryJson(
             concurrency_structured_task_cancellation_semantic_summary)
      << ",\"objc_concurrency_executor_hop_and_affinity_compatibility_completion\":"
      << BuildConcurrencyExecutorHopAffinityCompatibilitySummaryJson(
             concurrency_executor_hop_affinity_compatibility_summary)
      << ",\"objc_concurrency_await_suspension_and_resume_semantics\":"
      << BuildConcurrencyAwaitSuspensionResumeSemanticSummaryJson(
             concurrency_await_suspension_resume_semantic_summary)
      << ",\"objc_concurrency_async_diagnostics_and_compatibility_completion\":"
      << BuildConcurrencyAsyncDiagnosticsCompatibilitySummaryJson(
             concurrency_async_diagnostics_compatibility_summary)
      << ",\"objc_concurrency_continuation_abi_and_async_lowering_contract\":"
      << BuildConcurrencyContinuationAbiAsyncLoweringContractJson(
             concurrency_async_continuation_lowering_contract,
             concurrency_await_lowering_suspension_state_lowering_contract,
             concurrency_async_continuation_lowering_replay_key,
             concurrency_await_lowering_suspension_state_lowering_replay_key)
      << ",\"objc_concurrency_task_runtime_lowering_contract\":"
      << BuildConcurrencyTaskRuntimeLoweringContractJson(
             concurrency_task_executor_cancellation_semantic_model_summary,
             concurrency_structured_task_cancellation_semantic_summary,
             concurrency_executor_hop_affinity_compatibility_summary,
             concurrency_actor_isolation_sendability_lowering_contract,
             concurrency_actor_isolation_sendability_lowering_replay_key,
             concurrency_task_runtime_interop_cancellation_lowering_contract,
             concurrency_task_runtime_interop_cancellation_lowering_replay_key,
             concurrency_concurrency_replay_race_guard_lowering_contract,
             concurrency_concurrency_replay_race_guard_lowering_replay_key)
      << ",\"objc_concurrency_task_group_and_runtime_abi_completion\":"
      << BuildConcurrencyTaskRuntimeAbiCompletionJson(
             concurrency_task_runtime_interop_cancellation_lowering_replay_key,
             concurrency_concurrency_replay_race_guard_lowering_replay_key)
      << ",\"objc_concurrency_async_function_await_and_continuation_lowering\":"
      << BuildConcurrencyAsyncDirectCallLoweringJson(
             concurrency_async_source_closure_summary,
             concurrency_async_continuation_lowering_contract,
             concurrency_await_lowering_suspension_state_lowering_contract,
             concurrency_async_continuation_lowering_replay_key,
             concurrency_await_lowering_suspension_state_lowering_replay_key)
      << ",\"objc_concurrency_suspension_autorelease_and_cleanup_integration\":"
      << BuildConcurrencySuspensionCleanupIntegrationJson(
             control_flow_control_flow_safety_lowering_contract,
             control_flow_control_flow_safety_lowering_replay_key,
             autoreleasepool_scope_lowering_contract,
             autoreleasepool_scope_lowering_replay_key,
             concurrency_async_continuation_lowering_contract,
             concurrency_await_lowering_suspension_state_lowering_contract,
             concurrency_async_continuation_lowering_replay_key,
             concurrency_await_lowering_suspension_state_lowering_replay_key);
}

}  // namespace objc3::artifacts::frontend
