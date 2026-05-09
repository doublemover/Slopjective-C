#pragma once

#include <string>

#include "lower/contracts/concurrency_actor_contracts.h"
#include "lower/contracts/concurrency_task_lowering_contracts.h"
#include "pipeline/objc3_frontend_types.h"

namespace objc3::artifacts::frontend {

[[nodiscard]] std::string
BuildConcurrencyTaskExecutorCancellationSemanticModelSummaryJson(
    const Objc3ConcurrencyTaskExecutorCancellationSemanticModelSummary &summary);

[[nodiscard]] std::string
BuildConcurrencyAsyncEffectSuspensionSemanticModelSummaryJson(
    const Objc3ConcurrencyAsyncEffectSuspensionSemanticModelSummary &summary);

[[nodiscard]] std::string
BuildConcurrencyAwaitSuspensionResumeSemanticSummaryJson(
    const Objc3ConcurrencyAwaitSuspensionResumeSemanticSummary &summary);

[[nodiscard]] std::string
BuildConcurrencyAsyncDiagnosticsCompatibilitySummaryJson(
    const Objc3ConcurrencyAsyncDiagnosticsCompatibilitySummary &summary);

[[nodiscard]] std::string
BuildConcurrencyStructuredTaskCancellationSemanticSummaryJson(
    const Objc3ConcurrencyStructuredTaskCancellationSemanticSummary &summary);

[[nodiscard]] std::string
BuildConcurrencyExecutorHopAffinityCompatibilitySummaryJson(
    const Objc3ConcurrencyExecutorHopAffinityCompatibilitySummary &summary);

[[nodiscard]] std::string BuildConcurrencyTaskRuntimeLoweringContractJson(
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
    const std::string &race_replay_key);

[[nodiscard]] std::string BuildConcurrencyTaskRuntimeAbiCompletionJson(
    const std::string &task_runtime_replay_key,
    const std::string &concurrency_replay_key);

}  // namespace objc3::artifacts::frontend
