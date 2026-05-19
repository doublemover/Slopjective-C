#include "artifacts/objc3_frontend_artifact_concurrency_manifest_surfaces.h"

#include <ostream>

#include "artifacts/objc3_frontend_actor_semantic_artifacts.h"
#include "artifacts/objc3_frontend_concurrency_semantic_artifacts.h"
#include "artifacts/objc3_frontend_source_closure_artifacts.h"

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
        &concurrency_task_executor_cancellation_semantic_model_summary) {
  manifest
      << ",\"objc_concurrency_actor_member_and_isolation_source_closure\":"
      << BuildConcurrencyActorMemberIsolationSourceClosureSummaryJson(
             concurrency_actor_member_isolation_source_closure_summary)
      << ",\"objc_concurrency_actor_isolation_and_sendable_semantic_model\":"
      << BuildConcurrencyActorIsolationSendableSemanticModelSummaryJson(
             concurrency_actor_isolation_sendable_semantic_model_summary)
      << ",\"objc_concurrency_actor_isolation_and_sendability_enforcement\":"
      << BuildConcurrencyActorIsolationSendabilityEnforcementSummaryJson(
             concurrency_actor_isolation_sendability_enforcement_summary)
      << ",\"objc_concurrency_actor_race_hazard_and_escape_diagnostics\":"
      << BuildConcurrencyActorRaceHazardEscapeDiagnosticsSummaryJson(
             concurrency_actor_race_hazard_escape_diagnostics_summary)
      << ",\"objc_concurrency_actor_lowering_and_metadata_contract\":"
      << BuildConcurrencyActorLoweringMetadataContractJson(
             concurrency_actor_member_isolation_source_closure_summary,
             concurrency_actor_isolation_sendability_enforcement_summary,
             concurrency_actor_race_hazard_escape_diagnostics_summary,
             concurrency_actor_lowering_metadata_contract,
             concurrency_actor_lowering_metadata_replay_key)
      << ",\"objc_concurrency_task_group_and_cancellation_source_closure\":"
      << BuildConcurrencyTaskGroupCancellationSourceClosureSummaryJson(
             concurrency_task_group_cancellation_source_closure_summary)
      << ",\"objc_concurrency_async_effect_and_suspension_semantic_model\":"
      << BuildConcurrencyAsyncEffectSuspensionSemanticModelSummaryJson(
             concurrency_async_effect_suspension_semantic_model_summary)
      << ",\"objc_concurrency_task_executor_and_cancellation_semantic_model\":"
      << BuildConcurrencyTaskExecutorCancellationSemanticModelSummaryJson(
             concurrency_task_executor_cancellation_semantic_model_summary);
}

}  // namespace objc3::artifacts::frontend
