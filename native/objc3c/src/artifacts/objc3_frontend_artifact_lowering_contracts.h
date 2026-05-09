#pragma once

#include "lower/contracts/concurrency_actor_contracts.h"
#include "lower/contracts/concurrency_task_lowering_contracts.h"
#include "lower/contracts/dispatch_control_lowering_contracts.h"
#include "lower/contracts/ownership_system_extension_contracts.h"
#include "pipeline/objc3_frontend_types.h"

namespace objc3::artifacts::frontend {

[[nodiscard]] Objc3ActorIsolationSendabilityLoweringContract
BuildConcurrencyActorIsolationSendabilityLoweringContract(
    const Objc3ConcurrencyExecutorHopAffinityCompatibilitySummary &summary);

[[nodiscard]] Objc3OwnershipSystemExtensionLoweringContract
BuildOwnershipSystemExtensionLoweringContract(
    const Objc3OwnershipSystemExtensionSemanticModelSummary &semantic_summary,
    const Objc3OwnershipResourceMoveUseAfterMoveSemanticsSummary
        &resource_summary,
    const Objc3OwnershipBorrowedPointerEscapeAnalysisSummary &borrowed_summary,
    const Objc3OwnershipCaptureListRetainableFamilyLegalityCompletionSummary
        &family_summary);

[[nodiscard]] Objc3DispatchDispatchControlLoweringContract
BuildDispatchDispatchControlLoweringContract(
    const Objc3DispatchDispatchIntentSemanticModelSummary &semantic_summary,
    const Objc3DispatchDispatchIntentLegalitySummary &legality_summary,
    const Objc3DispatchDispatchIntentCompatibilitySummary
        &compatibility_summary);

[[nodiscard]] Objc3TaskRuntimeInteropCancellationLoweringContract
BuildConcurrencyTaskRuntimeInteropCancellationLoweringContract(
    const Objc3ConcurrencyTaskExecutorCancellationSemanticModelSummary
        &semantic_summary,
    const Objc3ConcurrencyStructuredTaskCancellationSemanticSummary
        &structured_summary,
    const Objc3ConcurrencyExecutorHopAffinityCompatibilitySummary
        &executor_summary);

[[nodiscard]] Objc3ConcurrencyReplayRaceGuardLoweringContract
BuildConcurrencyConcurrencyReplayRaceGuardLoweringContract(
    const Objc3ConcurrencyTaskExecutorCancellationSemanticModelSummary
        &semantic_summary,
    const Objc3ConcurrencyStructuredTaskCancellationSemanticSummary
        &structured_summary,
    const Objc3ConcurrencyExecutorHopAffinityCompatibilitySummary
        &executor_summary,
    const Objc3ActorIsolationSendabilityLoweringContract &actor_contract);

}  // namespace objc3::artifacts::frontend
