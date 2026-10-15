#include "artifacts/objc3_frontend_runtime_import_artifacts.h"

#include <sstream>

#include "artifacts/objc3_frontend_actor_semantic_artifacts.h"
#include "io/objc3_json.h"

namespace objc3::artifacts::frontend {
namespace {

using objc3::io::EscapeJsonString;

constexpr char kObjc3ConcurrencyActorMailboxRuntimeImportContractId[] =
    "objc3c.concurrency.actor.mailbox.isolation.import.surface.v1";
constexpr char kObjc3ConcurrencyActorMailboxRuntimeImportSurfacePath[] =
    "frontend.pipeline.semantic_surface."
    "objc_concurrency_actor_mailbox_and_isolation_runtime_import_surface";
constexpr char kObjc3ConcurrencyActorMailboxRuntimeImportSourceModel[] =
    "runtime-import-surface-preserves-actor-lowering-and-isolation-replay-facts-for-cross-module-runtime-link-planning";
constexpr char kObjc3ConcurrencyActorMailboxRuntimeImportFailClosedModel[] =
    "missing-or-drifted-actor-mailbox-runtime-import-packets-disable-cross-module-actor-runtime-preservation-claims";

}  // namespace

Objc3ConcurrencyActorMailboxRuntimeImportSummary
BuildConcurrencyActorMailboxRuntimeImportSummary(
    const Objc3ActorLoweringMetadataContract &actor_contract,
    const std::string &actor_lowering_replay_key,
    const std::string &actor_isolation_lowering_replay_key) {
  Objc3ConcurrencyActorMailboxRuntimeImportSummary summary;
  summary.contract_id = kObjc3ConcurrencyActorMailboxRuntimeImportContractId;
  summary.source_contract_id = kObjc3ConcurrencyActorLoweringMetadataContractId;
  summary.surface_path = kObjc3ConcurrencyActorMailboxRuntimeImportSurfacePath;
  summary.source_model = kObjc3ConcurrencyActorMailboxRuntimeImportSourceModel;
  summary.fail_closed_model =
      kObjc3ConcurrencyActorMailboxRuntimeImportFailClosedModel;
  summary.actor_interface_sites = actor_contract.actor_interface_sites;
  summary.actor_method_sites = actor_contract.actor_method_sites;
  summary.actor_metadata_record_sites =
      actor_contract.actor_metadata_record_sites;
  summary.nonisolated_entry_sites = actor_contract.nonisolated_entry_sites;
  summary.executor_affinity_sites = actor_contract.executor_affinity_sites;
  summary.actor_hop_artifact_sites = actor_contract.actor_hop_artifact_sites;
  summary.actor_isolation_thunk_sites =
      actor_contract.actor_isolation_thunk_sites;
  summary.replay_proof_dependency_sites =
      actor_contract.replay_proof_dependency_sites;
  summary.race_guard_dependency_sites =
      actor_contract.race_guard_dependency_sites;
  summary.task_handoff_sites = actor_contract.task_handoff_sites;
  summary.guard_blocked_sites = actor_contract.guard_blocked_sites;
  summary.contract_violation_sites = actor_contract.contract_violation_sites;
  summary.actor_lowering_replay_key = actor_lowering_replay_key;
  summary.actor_isolation_lowering_replay_key =
      actor_isolation_lowering_replay_key;
  const bool actor_sites_present = actor_contract.actor_interface_sites != 0u ||
                                   actor_contract.actor_method_sites != 0u;
  summary.actor_mailbox_runtime_ready =
      actor_sites_present &&
      IsValidObjc3ActorLoweringMetadataContract(actor_contract) &&
      !actor_lowering_replay_key.empty() &&
      !actor_isolation_lowering_replay_key.empty();
  summary.deterministic = actor_contract.deterministic;
  if (summary.actor_mailbox_runtime_ready) {
    summary.actor_mailbox_message_identity_field_count = 2u;
    summary.actor_mailbox_fifo_ordering_field_count = 3u;
    summary.actor_mailbox_drain_operation_field_count = 2u;
    summary.actor_mailbox_cancel_operation_field_count = 2u;
    summary.actor_mailbox_error_operation_field_count = 2u;
    summary.actor_mailbox_shutdown_operation_field_count = 2u;
  }
  std::ostringstream replay_key;
  replay_key << summary.contract_id
             << ";source_contract_id=" << summary.source_contract_id
             << ";actor_interface_sites=" << summary.actor_interface_sites
             << ";actor_method_sites=" << summary.actor_method_sites
             << ";actor_metadata_record_sites="
             << summary.actor_metadata_record_sites
             << ";nonisolated_entry_sites=" << summary.nonisolated_entry_sites
             << ";executor_affinity_sites=" << summary.executor_affinity_sites
             << ";actor_hop_artifact_sites="
             << summary.actor_hop_artifact_sites
             << ";actor_isolation_thunk_sites="
             << summary.actor_isolation_thunk_sites
             << ";replay_proof_dependency_sites="
             << summary.replay_proof_dependency_sites
             << ";race_guard_dependency_sites="
             << summary.race_guard_dependency_sites
             << ";task_handoff_sites=" << summary.task_handoff_sites
             << ";actor_mailbox_message_identity_field_count="
             << summary.actor_mailbox_message_identity_field_count
             << ";actor_mailbox_fifo_ordering_field_count="
             << summary.actor_mailbox_fifo_ordering_field_count
             << ";actor_mailbox_drain_operation_field_count="
             << summary.actor_mailbox_drain_operation_field_count
             << ";actor_mailbox_cancel_operation_field_count="
             << summary.actor_mailbox_cancel_operation_field_count
             << ";actor_mailbox_error_operation_field_count="
             << summary.actor_mailbox_error_operation_field_count
             << ";actor_mailbox_shutdown_operation_field_count="
             << summary.actor_mailbox_shutdown_operation_field_count
             << ";distributed_actor_transport_evidence_sites="
             << summary.distributed_actor_transport_evidence_sites
             << ";guard_blocked_sites=" << summary.guard_blocked_sites
             << ";contract_violation_sites="
             << summary.contract_violation_sites
             << ";actor_mailbox_runtime_ready="
             << (summary.actor_mailbox_runtime_ready ? "true" : "false")
             << ";deterministic=" << (summary.deterministic ? "true" : "false")
             << ";actor_lowering_replay_key=" << actor_lowering_replay_key
             << ";actor_isolation_lowering_replay_key="
             << actor_isolation_lowering_replay_key;
  summary.replay_key = replay_key.str();
  return summary;
}

std::string BuildConcurrencyActorMailboxRuntimeImportSummaryJson(
    const Objc3ConcurrencyActorMailboxRuntimeImportSummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"source_contract_id\":\""
      << EscapeJsonString(summary.source_contract_id)
      << "\",\"surface_path\":\"" << EscapeJsonString(summary.surface_path)
      << "\",\"source_model\":\"" << EscapeJsonString(summary.source_model)
      << "\",\"fail_closed_model\":\""
      << EscapeJsonString(summary.fail_closed_model)
      << "\",\"actor_interface_sites\":" << summary.actor_interface_sites
      << ",\"actor_method_sites\":" << summary.actor_method_sites
      << ",\"actor_metadata_record_sites\":"
      << summary.actor_metadata_record_sites
      << ",\"nonisolated_entry_sites\":" << summary.nonisolated_entry_sites
      << ",\"executor_affinity_sites\":" << summary.executor_affinity_sites
      << ",\"actor_hop_artifact_sites\":" << summary.actor_hop_artifact_sites
      << ",\"actor_isolation_thunk_sites\":"
      << summary.actor_isolation_thunk_sites
      << ",\"replay_proof_dependency_sites\":"
      << summary.replay_proof_dependency_sites
      << ",\"race_guard_dependency_sites\":"
      << summary.race_guard_dependency_sites
      << ",\"task_handoff_sites\":" << summary.task_handoff_sites
      << ",\"actor_mailbox_message_identity_field_count\":"
      << summary.actor_mailbox_message_identity_field_count
      << ",\"actor_mailbox_fifo_ordering_field_count\":"
      << summary.actor_mailbox_fifo_ordering_field_count
      << ",\"actor_mailbox_drain_operation_field_count\":"
      << summary.actor_mailbox_drain_operation_field_count
      << ",\"actor_mailbox_cancel_operation_field_count\":"
      << summary.actor_mailbox_cancel_operation_field_count
      << ",\"actor_mailbox_error_operation_field_count\":"
      << summary.actor_mailbox_error_operation_field_count
      << ",\"actor_mailbox_shutdown_operation_field_count\":"
      << summary.actor_mailbox_shutdown_operation_field_count
      << ",\"distributed_actor_transport_evidence_sites\":"
      << summary.distributed_actor_transport_evidence_sites
      << ",\"guard_blocked_sites\":" << summary.guard_blocked_sites
      << ",\"contract_violation_sites\":"
      << summary.contract_violation_sites
      << ",\"actor_mailbox_runtime_ready\":"
      << (summary.actor_mailbox_runtime_ready ? "true" : "false")
      << ",\"deterministic\":" << (summary.deterministic ? "true" : "false")
      << ",\"actor_lowering_replay_key\":\""
      << EscapeJsonString(summary.actor_lowering_replay_key)
      << "\",\"actor_isolation_lowering_replay_key\":\""
      << EscapeJsonString(summary.actor_isolation_lowering_replay_key)
      << "\",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

}  // namespace objc3::artifacts::frontend
