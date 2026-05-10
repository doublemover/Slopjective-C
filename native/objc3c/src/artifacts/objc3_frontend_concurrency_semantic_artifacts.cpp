#include "artifacts/objc3_frontend_concurrency_semantic_artifacts.h"

#include <algorithm>

namespace objc3::artifacts::frontend {

Objc3ActorLoweringMetadataContract BuildConcurrencyActorLoweringMetadataContract(
    const Objc3FrontendConcurrencyActorMemberIsolationSourceClosureSummary
        &source_summary,
    const Objc3ConcurrencyActorIsolationSendabilityEnforcementSummary
        &enforcement_summary,
    const Objc3ConcurrencyActorRaceHazardEscapeDiagnosticsSummary
        &hazard_summary) {
  Objc3ActorLoweringMetadataContract contract;
  contract.actor_interface_sites = source_summary.actor_interface_sites;
  contract.actor_method_sites = enforcement_summary.actor_method_sites;
  contract.actor_metadata_record_sites =
      source_summary.actor_interface_sites +
      source_summary.actor_member_metadata_sites;
  contract.nonisolated_entry_sites =
      enforcement_summary.total_nonisolated_method_sites;
  contract.executor_affinity_sites =
      enforcement_summary.actor_member_executor_annotation_sites;
  contract.actor_hop_artifact_sites = enforcement_summary.actor_hop_sites;
  contract.actor_isolation_thunk_sites = hazard_summary.task_handoff_sites;
  contract.replay_proof_dependency_sites = hazard_summary.replay_proof_sites;
  contract.race_guard_dependency_sites = hazard_summary.race_guard_sites;
  contract.task_handoff_sites = hazard_summary.task_handoff_sites;
  contract.guard_blocked_sites =
      enforcement_summary.illegal_non_actor_nonisolated_sites +
      enforcement_summary.illegal_nonisolated_async_sites +
      enforcement_summary.illegal_nonisolated_executor_sites +
      enforcement_summary.illegal_actor_hop_without_async_sites +
      enforcement_summary.illegal_non_sendable_crossing_sites +
      hazard_summary.illegal_missing_race_guard_sites +
      hazard_summary.illegal_missing_replay_proof_sites +
      hazard_summary.illegal_missing_actor_isolation_sites +
      hazard_summary.illegal_escaping_block_literal_sites;
  contract.contract_violation_sites = 0;
  contract.deterministic =
      source_summary.deterministic_handoff &&
      source_summary.ready_for_semantic_expansion &&
      enforcement_summary.deterministic &&
      enforcement_summary.ready_for_lowering_and_runtime &&
      hazard_summary.deterministic &&
      hazard_summary.ready_for_lowering_and_runtime;
  return contract;
}

Objc3AsyncContinuationLoweringContract
BuildConcurrencyAsyncContinuationLoweringContract(
    const Objc3ConcurrencyAsyncEffectSuspensionSemanticModelSummary &summary,
    const Objc3ConcurrencyAsyncDiagnosticsCompatibilitySummary
        &compatibility_summary) {
  Objc3AsyncContinuationLoweringContract contract;
  contract.async_continuation_sites = summary.async_continuation_sites;
  contract.async_keyword_sites = summary.async_keyword_sites;
  contract.async_function_sites = summary.async_function_sites;
  contract.continuation_allocation_sites = summary.continuation_allocation_sites;
  contract.continuation_resume_sites = summary.continuation_resume_sites;
  contract.continuation_suspend_sites = summary.continuation_suspend_sites;
  contract.async_state_machine_sites = summary.async_state_machine_sites;
  contract.gate_blocked_sites =
      compatibility_summary.illegal_non_async_executor_sites +
      compatibility_summary.illegal_async_function_prototype_sites +
      compatibility_summary.illegal_async_throws_sites;
  if (contract.gate_blocked_sites > contract.async_continuation_sites) {
    contract.contract_violation_sites =
        contract.gate_blocked_sites - contract.async_continuation_sites;
    contract.gate_blocked_sites = contract.async_continuation_sites;
  }
  contract.normalized_sites =
      contract.async_continuation_sites - contract.gate_blocked_sites;
  contract.deterministic =
      summary.deterministic && compatibility_summary.deterministic &&
      summary.ready_for_lowering_and_runtime &&
      compatibility_summary.ready_for_lowering_and_runtime &&
      contract.contract_violation_sites == 0;
  return contract;
}

Objc3AwaitLoweringSuspensionStateLoweringContract
BuildConcurrencyAwaitLoweringSuspensionStateLoweringContract(
    const Objc3ConcurrencyAwaitSuspensionResumeSemanticSummary &summary) {
  Objc3AwaitLoweringSuspensionStateLoweringContract contract;
  contract.await_suspension_sites = summary.await_expression_sites +
                                    summary.await_suspension_point_sites;
  contract.await_keyword_sites =
      summary.await_suspension_point_sites > 0 ? 1 : 0;
  contract.await_suspension_point_sites = summary.await_suspension_point_sites;
  contract.await_resume_sites = summary.await_resume_sites;
  contract.await_state_machine_sites = summary.await_resume_sites;
  contract.await_continuation_sites =
      summary.await_suspension_point_sites > 0
          ? std::min(summary.await_suspension_point_sites,
                     std::max(summary.continuation_resume_sites,
                              summary.continuation_suspend_sites))
          : 0;
  contract.gate_blocked_sites = summary.illegal_await_sites;
  if (contract.gate_blocked_sites > contract.await_suspension_sites) {
    contract.contract_violation_sites =
        contract.gate_blocked_sites - contract.await_suspension_sites;
    contract.gate_blocked_sites = contract.await_suspension_sites;
  }
  contract.normalized_sites =
      contract.await_suspension_sites - contract.gate_blocked_sites;
  contract.deterministic =
      summary.deterministic && summary.ready_for_lowering_and_runtime &&
      contract.contract_violation_sites == 0;
  return contract;
}

}  // namespace objc3::artifacts::frontend
