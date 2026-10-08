#include "artifacts/objc3_frontend_actor_semantic_artifacts.h"

#include <sstream>
#include <string>

#include "io/objc3_json.h"
#include "lower/contracts/concurrency_actor_contracts.h"
#include "sema/model/semantic_symbol_core_source_closures.h"
#include "sema/objc3_sema_contract_actor_concurrency_surfaces.h"

namespace objc3::artifacts::frontend {
namespace {

using objc3::io::EscapeJsonString;

inline constexpr const char *kObjc3ConcurrencyActorLoweringMetadataContractId =
    "objc3c.concurrency.actor.lowering.and.metadata.contract.v1";
inline constexpr const char *kObjc3ConcurrencyActorLoweringMetadataSurfacePath =
    "frontend.pipeline.semantic_surface."
    "objc_concurrency_actor_lowering_and_metadata_contract";
inline constexpr const char *kObjc3ConcurrencyActorLoweringMetadataModel =
    "actor-member-semantic-and-hazard-packets-now-lower-through-one-deterministic-actor-metadata-isolation-thunk-and-hop-artifact-contract";
inline constexpr const char *kObjc3ConcurrencyActorLoweringMetadataDeferredModel =
    "live-actor-thunk-bodies-mailbox-runtime-entrypoints-and-runnable-cross-actor-scheduling-remain-later-runtime-work";

}  // namespace

std::string BuildConcurrencyActorIsolationSendableSemanticModelSummaryJson(
    const Objc3ConcurrencyActorIsolationSendableSemanticModelSummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"frontend_dependency_contract_id\":\""
      << EscapeJsonString(summary.frontend_dependency_contract_id)
      << "\",\"surface_path\":\"" << EscapeJsonString(summary.surface_path)
      << "\",\"semantic_model\":\"" << EscapeJsonString(summary.semantic_model)
      << "\",\"deferred_model\":\"" << EscapeJsonString(summary.deferred_model)
      << "\",\"actor_interface_sites\":" << summary.actor_interface_sites
      << ",\"actor_method_sites\":" << summary.actor_method_sites
      << ",\"actor_property_sites\":" << summary.actor_property_sites
      << ",\"objc_nonisolated_annotation_sites\":"
      << summary.objc_nonisolated_annotation_sites
      << ",\"actor_member_executor_annotation_sites\":"
      << summary.actor_member_executor_annotation_sites
      << ",\"actor_async_method_sites\":"
      << summary.actor_async_method_sites
      << ",\"actor_member_metadata_sites\":"
      << summary.actor_member_metadata_sites
      << ",\"actor_isolation_sendability_sites\":"
      << summary.actor_isolation_sendability_sites
      << ",\"actor_isolation_decl_sites\":"
      << summary.actor_isolation_decl_sites
      << ",\"actor_hop_sites\":" << summary.actor_hop_sites
      << ",\"sendable_annotation_sites\":"
      << summary.sendable_annotation_sites
      << ",\"non_sendable_crossing_sites\":"
      << summary.non_sendable_crossing_sites
      << ",\"isolation_boundary_sites\":" << summary.isolation_boundary_sites
      << ",\"normalized_sites\":" << summary.normalized_sites
      << ",\"gate_blocked_sites\":" << summary.gate_blocked_sites
      << ",\"contract_violation_sites\":"
      << summary.contract_violation_sites
      << ",\"source_dependency_required\":"
      << (summary.source_dependency_required ? "true" : "false")
      << ",\"actor_member_source_supported\":"
      << (summary.actor_member_source_supported ? "true" : "false")
      << ",\"actor_isolation_sendability_profile_normalized\":"
      << (summary.actor_isolation_sendability_profile_normalized ? "true"
                                                                 : "false")
      << ",\"strict_concurrency_selection_supported\":"
      << (summary.strict_concurrency_selection_supported ? "true" : "false")
      << ",\"actor_runtime_deferred\":"
      << (summary.actor_runtime_deferred ? "true" : "false")
      << ",\"executor_runtime_deferred\":"
      << (summary.executor_runtime_deferred ? "true" : "false")
      << ",\"cross_actor_enforcement_deferred\":"
      << (summary.cross_actor_enforcement_deferred ? "true" : "false")
      << ",\"deterministic\":" << (summary.deterministic ? "true" : "false")
      << ",\"ready_for_semantic_expansion\":"
      << (summary.ready_for_semantic_expansion ? "true" : "false")
      << ",\"failure_reason\":\""
      << EscapeJsonString(summary.failure_reason)
      << "\",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

std::string BuildConcurrencyActorIsolationSendabilityEnforcementSummaryJson(
    const Objc3ConcurrencyActorIsolationSendabilityEnforcementSummary
        &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"dependency_contract_id\":\""
      << EscapeJsonString(summary.dependency_contract_id)
      << "\",\"surface_path\":\"" << EscapeJsonString(summary.surface_path)
      << "\",\"semantic_model\":\"" << EscapeJsonString(summary.semantic_model)
      << "\",\"deferred_model\":\"" << EscapeJsonString(summary.deferred_model)
      << "\",\"actor_interface_sites\":" << summary.actor_interface_sites
      << ",\"actor_method_sites\":" << summary.actor_method_sites
      << ",\"objc_nonisolated_annotation_sites\":"
      << summary.objc_nonisolated_annotation_sites
      << ",\"actor_member_executor_annotation_sites\":"
      << summary.actor_member_executor_annotation_sites
      << ",\"actor_async_method_sites\":"
      << summary.actor_async_method_sites
      << ",\"actor_hop_sites\":" << summary.actor_hop_sites
      << ",\"non_sendable_crossing_sites\":"
      << summary.non_sendable_crossing_sites
      << ",\"total_nonisolated_method_sites\":"
      << summary.total_nonisolated_method_sites
      << ",\"illegal_non_actor_nonisolated_sites\":"
      << summary.illegal_non_actor_nonisolated_sites
      << ",\"illegal_nonisolated_async_sites\":"
      << summary.illegal_nonisolated_async_sites
      << ",\"illegal_nonisolated_executor_sites\":"
      << summary.illegal_nonisolated_executor_sites
      << ",\"illegal_actor_hop_without_async_sites\":"
      << summary.illegal_actor_hop_without_async_sites
      << ",\"illegal_non_sendable_crossing_sites\":"
      << summary.illegal_non_sendable_crossing_sites
      << ",\"dependency_required\":"
      << (summary.dependency_required ? "true" : "false")
      << ",\"non_actor_nonisolated_fail_closed\":"
      << (summary.non_actor_nonisolated_fail_closed ? "true" : "false")
      << ",\"nonisolated_combination_fail_closed\":"
      << (summary.nonisolated_combination_fail_closed ? "true" : "false")
      << ",\"actor_hop_async_boundary_enforced\":"
      << (summary.actor_hop_async_boundary_enforced ? "true" : "false")
      << ",\"non_sendable_crossing_fail_closed\":"
      << (summary.non_sendable_crossing_fail_closed ? "true" : "false")
      << ",\"runnable_lowering_deferred\":"
      << (summary.runnable_lowering_deferred ? "true" : "false")
      << ",\"actor_runtime_deferred\":"
      << (summary.actor_runtime_deferred ? "true" : "false")
      << ",\"executor_runtime_deferred\":"
      << (summary.executor_runtime_deferred ? "true" : "false")
      << ",\"deterministic\":" << (summary.deterministic ? "true" : "false")
      << ",\"ready_for_lowering_and_runtime\":"
      << (summary.ready_for_lowering_and_runtime ? "true" : "false")
      << ",\"failure_reason\":\""
      << EscapeJsonString(summary.failure_reason)
      << "\",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

std::string BuildConcurrencyActorRaceHazardEscapeDiagnosticsSummaryJson(
    const Objc3ConcurrencyActorRaceHazardEscapeDiagnosticsSummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"dependency_contract_id\":\""
      << EscapeJsonString(summary.dependency_contract_id)
      << "\",\"surface_path\":\"" << EscapeJsonString(summary.surface_path)
      << "\",\"semantic_model\":\"" << EscapeJsonString(summary.semantic_model)
      << "\",\"deferred_model\":\"" << EscapeJsonString(summary.deferred_model)
      << "\",\"actor_method_sites\":" << summary.actor_method_sites
      << ",\"replay_proof_sites\":" << summary.replay_proof_sites
      << ",\"race_guard_sites\":" << summary.race_guard_sites
      << ",\"task_handoff_sites\":" << summary.task_handoff_sites
      << ",\"actor_isolation_sites\":" << summary.actor_isolation_sites
      << ",\"escaping_block_literal_sites\":"
      << summary.escaping_block_literal_sites
      << ",\"illegal_missing_race_guard_sites\":"
      << summary.illegal_missing_race_guard_sites
      << ",\"illegal_missing_replay_proof_sites\":"
      << summary.illegal_missing_replay_proof_sites
      << ",\"illegal_missing_actor_isolation_sites\":"
      << summary.illegal_missing_actor_isolation_sites
      << ",\"illegal_escaping_block_literal_sites\":"
      << summary.illegal_escaping_block_literal_sites
      << ",\"dependency_required\":"
      << (summary.dependency_required ? "true" : "false")
      << ",\"race_guard_fail_closed\":"
      << (summary.race_guard_fail_closed ? "true" : "false")
      << ",\"replay_proof_fail_closed\":"
      << (summary.replay_proof_fail_closed ? "true" : "false")
      << ",\"actor_isolation_boundary_fail_closed\":"
      << (summary.actor_isolation_boundary_fail_closed ? "true" : "false")
      << ",\"escaping_block_fail_closed\":"
      << (summary.escaping_block_fail_closed ? "true" : "false")
      << ",\"runnable_lowering_deferred\":"
      << (summary.runnable_lowering_deferred ? "true" : "false")
      << ",\"actor_runtime_deferred\":"
      << (summary.actor_runtime_deferred ? "true" : "false")
      << ",\"deterministic\":" << (summary.deterministic ? "true" : "false")
      << ",\"ready_for_lowering_and_runtime\":"
      << (summary.ready_for_lowering_and_runtime ? "true" : "false")
      << ",\"failure_reason\":\""
      << EscapeJsonString(summary.failure_reason)
      << "\",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

std::string BuildConcurrencyActorLoweringMetadataContractJson(
    const Objc3FrontendConcurrencyActorMemberIsolationSourceClosureSummary
        &source_summary,
    const Objc3ConcurrencyActorIsolationSendabilityEnforcementSummary
        &enforcement_summary,
    const Objc3ConcurrencyActorRaceHazardEscapeDiagnosticsSummary &hazard_summary,
    const Objc3ActorLoweringMetadataContract &contract,
    const std::string &replay_key) {
  const bool ready_for_ir_emission =
      contract.deterministic &&
      IsValidObjc3ActorLoweringMetadataContract(contract);
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\""
      << EscapeJsonString(kObjc3ConcurrencyActorLoweringMetadataContractId)
      << "\",\"surface_path\":\""
      << EscapeJsonString(kObjc3ConcurrencyActorLoweringMetadataSurfacePath)
      << "\",\"source_contract_id\":\""
      << EscapeJsonString(source_summary.contract_id)
      << "\",\"semantic_contract_id\":\""
      << EscapeJsonString(enforcement_summary.contract_id)
      << "\",\"hazard_contract_id\":\""
      << EscapeJsonString(hazard_summary.contract_id)
      << "\",\"lane_contract_id\":\""
      << EscapeJsonString(kObjc3ConcurrencyActorLoweringMetadataLaneContract)
      << "\",\"metadata_model\":\""
      << EscapeJsonString(kObjc3ConcurrencyActorLoweringMetadataModel)
      << "\",\"deferred_model\":\""
      << EscapeJsonString(kObjc3ConcurrencyActorLoweringMetadataDeferredModel)
      << "\",\"replay_key\":\"" << EscapeJsonString(replay_key)
      << "\",\"actor_interface_sites\":" << contract.actor_interface_sites
      << ",\"actor_method_sites\":" << contract.actor_method_sites
      << ",\"actor_metadata_record_sites\":"
      << contract.actor_metadata_record_sites
      << ",\"nonisolated_entry_sites\":" << contract.nonisolated_entry_sites
      << ",\"executor_affinity_sites\":" << contract.executor_affinity_sites
      << ",\"actor_hop_artifact_sites\":"
      << contract.actor_hop_artifact_sites
      << ",\"actor_isolation_thunk_sites\":"
      << contract.actor_isolation_thunk_sites
      << ",\"replay_proof_dependency_sites\":"
      << contract.replay_proof_dependency_sites
      << ",\"race_guard_dependency_sites\":"
      << contract.race_guard_dependency_sites
      << ",\"task_handoff_sites\":" << contract.task_handoff_sites
      << ",\"guard_blocked_sites\":" << contract.guard_blocked_sites
      << ",\"contract_violation_sites\":"
      << contract.contract_violation_sites
      << ",\"deterministic_handoff\":"
      << (contract.deterministic ? "true" : "false")
      << ",\"ready_for_ir_emission\":"
      << (ready_for_ir_emission ? "true" : "false")
      << "}";
  return out.str();
}

}  // namespace objc3::artifacts::frontend
