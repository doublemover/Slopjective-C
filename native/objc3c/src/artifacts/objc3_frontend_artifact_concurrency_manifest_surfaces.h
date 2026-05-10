#pragma once

#include <iosfwd>
#include <string>

struct Objc3ActorLoweringMetadataContract;
struct Objc3ConcurrencyActorIsolationSendabilityEnforcementSummary;
struct Objc3ConcurrencyActorIsolationSendableSemanticModelSummary;
struct Objc3ConcurrencyActorRaceHazardEscapeDiagnosticsSummary;
struct Objc3ConcurrencyAsyncEffectSuspensionSemanticModelSummary;
struct Objc3ConcurrencyTaskExecutorCancellationSemanticModelSummary;
struct Objc3FrontendConcurrencyActorMemberIsolationSourceClosureSummary;
struct Objc3FrontendConcurrencyTaskGroupCancellationSourceClosureSummary;

namespace objc3::artifacts::frontend {

void WriteConcurrencyManifestSurfaces(
    std::ostream &manifest,
    const Objc3FrontendConcurrencyActorMemberIsolationSourceClosureSummary
        &concurrency_actor_member_isolation_source_closure_summary,
    const Objc3ConcurrencyActorIsolationSendableSemanticModelSummary
        &concurrency_actor_isolation_sendable_semantic_model_summary,
    const Objc3ConcurrencyActorIsolationSendabilityEnforcementSummary
        &concurrency_actor_isolation_sendability_enforcement_summary,
    const Objc3ConcurrencyActorRaceHazardEscapeDiagnosticsSummary
        &concurrency_actor_race_hazard_escape_diagnostics_summary,
    const Objc3ActorLoweringMetadataContract
        &concurrency_actor_lowering_metadata_contract,
    const std::string &concurrency_actor_lowering_metadata_replay_key,
    const Objc3FrontendConcurrencyTaskGroupCancellationSourceClosureSummary
        &concurrency_task_group_cancellation_source_closure_summary,
    const Objc3ConcurrencyAsyncEffectSuspensionSemanticModelSummary
        &concurrency_async_effect_suspension_semantic_model_summary,
    const Objc3ConcurrencyTaskExecutorCancellationSemanticModelSummary
        &concurrency_task_executor_cancellation_semantic_model_summary);

}  // namespace objc3::artifacts::frontend
