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
    return true;
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

  if (!surface.concurrency_actor_mailbox_runtime_ready) {
    return true;
  }

  surface.concurrency_actor_mailbox_runtime_import_present = true;
  surface.concurrency_actor_mailbox_runtime_contract_id =
      std::move(contract_id);
  surface.concurrency_actor_mailbox_runtime_source_contract_id =
      std::move(source_contract_id);
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
  return PopulateImportedConcurrencyActorMailboxRuntimeImport(root, surface,
                                                             error);
}

}  // namespace objc3c::pipeline::runtime_import_preservation
