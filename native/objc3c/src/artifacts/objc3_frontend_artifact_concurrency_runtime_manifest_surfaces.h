#pragma once

#include <iosfwd>
#include <string>

struct Objc3ActorIsolationSendabilityLoweringContract;
struct Objc3AsyncContinuationLoweringContract;
struct Objc3AutoreleasePoolScopeLoweringContract;
struct Objc3AwaitLoweringSuspensionStateLoweringContract;
struct Objc3ConcurrencyAsyncDiagnosticsCompatibilitySummary;
struct Objc3ConcurrencyAwaitSuspensionResumeSemanticSummary;
struct Objc3ConcurrencyExecutorHopAffinityCompatibilitySummary;
struct Objc3ConcurrencyReplayRaceGuardLoweringContract;
struct Objc3ConcurrencyStructuredTaskCancellationSemanticSummary;
struct Objc3ConcurrencyTaskExecutorCancellationSemanticModelSummary;
struct Objc3ControlFlowControlFlowSafetyLoweringContract;
struct Objc3FrontendConcurrencyAsyncSourceClosureSummary;
struct Objc3TaskRuntimeInteropCancellationLoweringContract;

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
    const std::string &autoreleasepool_scope_lowering_replay_key);

}  // namespace objc3::artifacts::frontend
