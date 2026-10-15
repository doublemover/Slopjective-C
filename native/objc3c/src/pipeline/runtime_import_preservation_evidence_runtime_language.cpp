#include "pipeline/runtime_import_preservation_owners.h"

#include <string>
#include <utility>

#include "lower/objc3_lowering_contract.h"

namespace objc3c::pipeline::runtime_import_preservation {
namespace {

bool PopulateImportedErrorHandlingResultAndBridgingArtifactReplay(
    const RuntimeImportJsonValue::Object &root,
    Objc3ImportedRuntimeModuleSurface &surface,
    std::string &error) {
  const RuntimeImportJsonValue *replay_value =
      FindMember(root, "objc_error_handling_result_and_bridging_artifact_replay");
  if (replay_value == nullptr) {
    return true;
  }

  const RuntimeImportJsonValue::Object *replay_object =
      AsObject(*replay_value);
  if (replay_object == nullptr) {
    error =
        "error_handling result/bridging imported replay contract must be a JSON object";
    return false;
  }

  std::string contract_id;
  std::string source_contract_id;
  if (!ReadStringMember(*replay_object, "contract_id", contract_id, error) ||
      !ReadStringMember(*replay_object, "source_contract_id",
                        source_contract_id, error) ||
      !ReadBoolMember(*replay_object, "binary_artifact_replay_ready",
                      surface.error_handling_binary_artifact_replay_ready,
                      error) ||
      !ReadBoolMember(*replay_object, "runtime_import_artifact_ready",
                      surface.error_handling_runtime_import_artifact_ready,
                      error) ||
      !ReadBoolMember(*replay_object, "separate_compilation_replay_ready",
                      surface.error_handling_separate_compilation_replay_ready,
                      error) ||
      !ReadBoolMember(*replay_object, "deterministic",
                      surface.error_handling_deterministic, error) ||
      !ReadStringMember(
          *replay_object, "replay_key",
          surface.error_handling_result_and_bridging_artifact_replay_key,
          error) ||
      !ReadStringMember(*replay_object, "error_handling_replay_key",
                        surface.error_handling_error_handling_replay_key,
                        error) ||
      !ReadStringMember(*replay_object, "throws_replay_key",
                        surface.error_handling_throws_replay_key, error) ||
      !ReadStringMember(*replay_object, "result_like_replay_key",
                        surface.error_handling_result_like_replay_key,
                        error) ||
      !ReadStringMember(*replay_object, "ns_error_replay_key",
                        surface.error_handling_ns_error_replay_key, error) ||
      !ReadStringMember(*replay_object, "unwind_replay_key",
                        surface.error_handling_unwind_replay_key, error)) {
    return false;
  }

  if (contract_id !=
      kObjc3ErrorHandlingResultAndBridgingArtifactReplayContractId) {
    error =
        "unexpected Part 6 result/bridging artifact replay contract id in import surface";
    return false;
  }
  if (source_contract_id !=
      kObjc3ErrorHandlingThrowsAbiPropagationLoweringContractId) {
    error =
        "unexpected Part 6 throws ABI propagation source contract id in import surface";
    return false;
  }

  surface.error_handling_result_and_bridging_artifact_replay_present = true;
  surface.error_handling_contract_id = std::move(contract_id);
  surface.error_handling_source_contract_id = std::move(source_contract_id);
  return true;
}

bool PopulateImportedConcurrencyActorMailboxRuntimeImport(
    const RuntimeImportJsonValue::Object &root,
    Objc3ImportedRuntimeModuleSurface &surface,
    std::string &error) {
  const RuntimeImportJsonValue *runtime_value = FindMember(
      root, "objc_concurrency_actor_mailbox_and_isolation_runtime_import_surface");
  if (runtime_value == nullptr) {
    error =
        "missing Part 7 actor mailbox runtime import surface in import artifact";
    return false;
  }

  const RuntimeImportJsonValue::Object *runtime_object =
      AsObject(*runtime_value);
  if (runtime_object == nullptr) {
    error = "concurrency actor mailbox runtime import surface must be a JSON object";
    return false;
  }

  std::string contract_id;
  std::string source_contract_id;
  if (!ReadStringMember(*runtime_object, "contract_id", contract_id, error) ||
      !ReadStringMember(*runtime_object, "source_contract_id",
                        source_contract_id, error) ||
      !ReadSizeMember(*runtime_object, "actor_interface_sites",
                      surface.concurrency_actor_interface_sites, error) ||
      !ReadSizeMember(*runtime_object, "actor_method_sites",
                      surface.concurrency_actor_method_sites, error) ||
      !ReadSizeMember(*runtime_object, "actor_metadata_record_sites",
                      surface.concurrency_actor_metadata_record_sites, error) ||
      !ReadSizeMember(*runtime_object, "nonisolated_entry_sites",
                      surface.concurrency_actor_nonisolated_entry_sites,
                      error) ||
      !ReadSizeMember(*runtime_object, "executor_affinity_sites",
                      surface.concurrency_actor_executor_affinity_sites,
                      error) ||
      !ReadSizeMember(*runtime_object, "actor_hop_artifact_sites",
                      surface.concurrency_actor_hop_artifact_sites, error) ||
      !ReadSizeMember(*runtime_object, "actor_isolation_thunk_sites",
                      surface.concurrency_actor_isolation_thunk_sites,
                      error) ||
      !ReadSizeMember(*runtime_object, "replay_proof_dependency_sites",
                      surface.concurrency_actor_replay_proof_dependency_sites,
                      error) ||
      !ReadSizeMember(*runtime_object, "race_guard_dependency_sites",
                      surface.concurrency_actor_race_guard_dependency_sites,
                      error) ||
      !ReadSizeMember(*runtime_object, "task_handoff_sites",
                      surface.concurrency_actor_task_handoff_sites, error) ||
      !ReadSizeMember(
          *runtime_object, "actor_mailbox_message_identity_field_count",
          surface.concurrency_actor_mailbox_message_identity_field_count,
          error) ||
      !ReadSizeMember(
          *runtime_object, "actor_mailbox_fifo_ordering_field_count",
          surface.concurrency_actor_mailbox_fifo_ordering_field_count, error) ||
      !ReadSizeMember(
          *runtime_object, "actor_mailbox_drain_operation_field_count",
          surface.concurrency_actor_mailbox_drain_operation_field_count,
          error) ||
      !ReadSizeMember(
          *runtime_object, "actor_mailbox_cancel_operation_field_count",
          surface.concurrency_actor_mailbox_cancel_operation_field_count,
          error) ||
      !ReadSizeMember(
          *runtime_object, "actor_mailbox_error_operation_field_count",
          surface.concurrency_actor_mailbox_error_operation_field_count,
          error) ||
      !ReadSizeMember(
          *runtime_object, "actor_mailbox_shutdown_operation_field_count",
          surface.concurrency_actor_mailbox_shutdown_operation_field_count,
          error) ||
      !ReadSizeMember(
          *runtime_object, "distributed_actor_transport_evidence_sites",
          surface.concurrency_actor_distributed_transport_evidence_sites,
          error) ||
      !ReadSizeMember(*runtime_object, "guard_blocked_sites",
                      surface.concurrency_actor_guard_blocked_sites, error) ||
      !ReadSizeMember(*runtime_object, "contract_violation_sites",
                      surface.concurrency_actor_contract_violation_sites,
                      error) ||
      !ReadBoolMember(*runtime_object, "actor_mailbox_runtime_ready",
                      surface.concurrency_actor_mailbox_runtime_ready,
                      error) ||
      !ReadBoolMember(*runtime_object, "deterministic",
                      surface.concurrency_actor_mailbox_runtime_deterministic,
                      error) ||
      !ReadStringMember(*runtime_object, "replay_key",
                        surface.concurrency_actor_mailbox_runtime_replay_key,
                        error) ||
      !ReadStringMember(*runtime_object, "actor_lowering_replay_key",
                        surface.concurrency_actor_lowering_replay_key, error) ||
      !ReadStringMember(*runtime_object, "actor_isolation_lowering_replay_key",
                        surface.concurrency_actor_isolation_lowering_replay_key,
                        error)) {
    return false;
  }

  if (contract_id !=
      "objc3c.concurrency.actor.mailbox.isolation.import.surface.v1") {
    error = "unexpected Part 7 actor mailbox runtime import contract id in import surface";
    return false;
  }
  if (source_contract_id !=
      "objc3c.concurrency.actor.lowering.and.metadata.contract.v1") {
    error = "unexpected Part 7 actor mailbox runtime source contract id in import surface";
    return false;
  }

  const bool has_actor_metadata =
      HasObjc3ImportedConcurrencyActorRuntimeMetadata(surface);
  if (!has_actor_metadata) {
    if (surface.concurrency_actor_mailbox_runtime_ready ||
        surface.concurrency_actor_mailbox_message_identity_field_count != 0u ||
        surface.concurrency_actor_mailbox_fifo_ordering_field_count != 0u ||
        surface.concurrency_actor_mailbox_drain_operation_field_count != 0u ||
        surface.concurrency_actor_mailbox_cancel_operation_field_count != 0u ||
        surface.concurrency_actor_mailbox_error_operation_field_count != 0u ||
        surface.concurrency_actor_mailbox_shutdown_operation_field_count !=
            0u ||
        surface.concurrency_actor_distributed_transport_evidence_sites != 0u) {
      error =
          "Part 7 actor mailbox runtime import surface carries actor mailbox evidence without actor metadata";
      return false;
    }
    return true;
  }

  surface.concurrency_actor_mailbox_runtime_import_present = true;
  surface.concurrency_actor_mailbox_runtime_contract_id =
      std::move(contract_id);
  surface.concurrency_actor_mailbox_runtime_source_contract_id =
      std::move(source_contract_id);
  if (!IsReadyObjc3ImportedConcurrencyActorMailboxRuntimeImportSurface(
          surface)) {
    error =
        "Part 7 actor mailbox runtime import surface is incomplete for actor metadata";
    return false;
  }
  return true;
}

bool PopulateImportedConcurrencySchedulerTaskRuntimeImport(
    const RuntimeImportJsonValue::Object &root,
    Objc3ImportedRuntimeModuleSurface &surface,
    std::string &error) {
  const RuntimeImportJsonValue *runtime_value =
      FindMember(root, "objc_concurrency_scheduler_task_runtime_import_surface");
  if (runtime_value == nullptr) {
    return true;
  }

  const RuntimeImportJsonValue::Object *runtime_object =
      AsObject(*runtime_value);
  if (runtime_object == nullptr) {
    error =
        "concurrency scheduler/task runtime import surface must be a JSON object";
    return false;
  }

  if (!ReadStringMember(
          *runtime_object, "contract_id",
          surface.concurrency_scheduler_task_runtime_contract_id, error) ||
      !ReadStringMember(
          *runtime_object, "source_contract_id",
          surface.concurrency_scheduler_task_runtime_source_contract_id,
          error) ||
      !ReadBoolMember(
          *runtime_object, "task_runtime_ready",
          surface.concurrency_scheduler_task_runtime_ready, error) ||
      !ReadBoolMember(
          *runtime_object, "deterministic",
          surface.concurrency_scheduler_task_runtime_deterministic, error) ||
      !ReadBoolMember(
          *runtime_object, "shutdown_drain_ready",
          surface.concurrency_scheduler_shutdown_drain_ready, error) ||
      !ReadBoolMember(
          *runtime_object, "cancellation_error_cleanup_ready",
          surface.concurrency_scheduler_cancellation_error_cleanup_ready,
          error) ||
      !ReadStringMember(
          *runtime_object, "replay_key",
          surface.concurrency_scheduler_task_runtime_replay_key, error) ||
      !ReadStringMember(
          *runtime_object, "task_lifecycle_replay_key",
          surface.concurrency_scheduler_task_lifecycle_replay_key, error) ||
      !ReadStringMember(
          *runtime_object, "task_cancellation_replay_key",
          surface.concurrency_scheduler_task_cancellation_replay_key, error) ||
      !ReadStringMember(
          *runtime_object, "shutdown_replay_key",
          surface.concurrency_scheduler_task_shutdown_replay_key, error) ||
      !ReadSizeMember(*runtime_object, "task_record_sites",
                      surface.concurrency_scheduler_task_record_sites, error) ||
      !ReadSizeMember(
          *runtime_object, "continuation_record_sites",
          surface.concurrency_scheduler_continuation_record_sites, error) ||
      !ReadSizeMember(
          *runtime_object, "executor_hop_record_sites",
          surface.concurrency_scheduler_executor_hop_record_sites, error) ||
      !ReadSizeMember(
          *runtime_object, "queue_lifecycle_record_sites",
          surface.concurrency_scheduler_queue_lifecycle_record_sites, error) ||
      !ReadSizeMember(
          *runtime_object, "cancellation_checkpoint_sites",
          surface.concurrency_scheduler_cancellation_checkpoint_sites, error) ||
      !ReadSizeMember(*runtime_object, "error_cleanup_sites",
                      surface.concurrency_scheduler_error_cleanup_sites,
                      error) ||
      !ReadSizeMember(*runtime_object, "shutdown_drain_sites",
                      surface.concurrency_scheduler_shutdown_drain_sites,
                      error) ||
      !ReadSizeMember(
          *runtime_object, "unsupported_policy_sites",
          surface.concurrency_scheduler_unsupported_policy_sites, error)) {
    return false;
  }

  if (surface.concurrency_scheduler_task_runtime_contract_id !=
      "objc3c.concurrency.scheduler.task.runtime.import.surface.v1") {
    error =
        "unexpected Part 7 scheduler/task runtime import contract id in import surface";
    return false;
  }
  if (surface.concurrency_scheduler_task_runtime_source_contract_id !=
      "objc3c.concurrency.task.runtime.lowering.contract.v1") {
    error =
        "unexpected Part 7 scheduler/task runtime import source contract id in import surface";
    return false;
  }

  surface.concurrency_scheduler_task_runtime_import_present = true;
  if (!IsReadyObjc3ImportedConcurrencySchedulerTaskRuntimeImportSurface(
          surface)) {
    error =
        "Part 7 scheduler/task runtime import surface is incomplete or overclaims unsupported scheduler policy";
    return false;
  }
  return true;
}

}  // namespace

bool PopulateImportedRuntimeLanguageEvidence(
    const RuntimeImportJsonValue::Object &root,
    Objc3ImportedRuntimeModuleSurface &surface,
    std::string &error) {
  if (!PopulateImportedErrorHandlingResultAndBridgingArtifactReplay(root, surface,
                                                                    error)) {
    return false;
  }
  if (!PopulateImportedConcurrencyActorMailboxRuntimeImport(root, surface,
                                                            error)) {
    return false;
  }
  return PopulateImportedConcurrencySchedulerTaskRuntimeImport(root, surface,
                                                              error);
}

}  // namespace objc3c::pipeline::runtime_import_preservation
