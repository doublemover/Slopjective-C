#pragma once

struct Objc3ActorIsolationSendabilityLoweringContract;
struct Objc3ConcurrencyExecutorHopAffinityCompatibilitySummary;
struct Objc3ConcurrencyReplayRaceGuardLoweringContract;
struct Objc3ConcurrencyStructuredTaskCancellationSemanticSummary;
struct Objc3ConcurrencyTaskExecutorCancellationSemanticModelSummary;
struct Objc3DispatchDispatchControlLoweringContract;
struct Objc3DispatchDispatchIntentCompatibilitySummary;
struct Objc3DispatchDispatchIntentLegalitySummary;
struct Objc3DispatchDispatchIntentSemanticModelSummary;
struct Objc3OwnershipBorrowedPointerEscapeAnalysisSummary;
struct Objc3OwnershipCaptureListRetainableFamilyLegalityCompletionSummary;
struct Objc3OwnershipResourceMoveUseAfterMoveSemanticsSummary;
struct Objc3OwnershipSystemExtensionLoweringContract;
struct Objc3OwnershipSystemExtensionSemanticModelSummary;
struct Objc3TaskRuntimeInteropCancellationLoweringContract;

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
