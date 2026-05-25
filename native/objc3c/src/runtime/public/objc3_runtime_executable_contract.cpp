#include "runtime/public/objc3_runtime_executable_contract.h"

#include <cstddef>
#include <cstdint>

namespace objc3c::runtime {
namespace {

struct RuntimeExecutableContractSurfaceRecord {
  int surface_kind;
  int issue_ref;
  int dependency_issue_ref;
  int support_status;
  bool typed_dispatch_required;
  bool native_registration_required;
  bool executable_runtime_required;
  bool fail_closed_required;
  bool public_abi_widened;
  bool compatibility_shim_allowed;
  bool task_record_required;
  bool continuation_record_required;
  bool executor_hop_record_required;
  bool actor_mailbox_event_required;
  bool deterministic_replay_required;
  bool scheduler_queue_state_required;
  bool task_lifecycle_state_required;
  bool cancellation_checkpoint_required;
  bool task_error_cleanup_required;
  bool shutdown_drain_required;
  bool imported_runtime_replay_required;
  bool scheduler_fairness_supported;
  bool scheduler_priority_supported;
  bool distributed_scheduler_supported;
  bool foreign_c_abi_supported;
  bool foreign_cxx_abi_supported;
  bool swift_runtime_abi_supported;
  bool foreign_metadata_preserved;
  bool foreign_runtime_mirroring_supported;
  bool foreign_surface_classification_required;
  bool foreign_typed_dispatch_expectation_required;
  bool foreign_package_runtime_identity_required;
  bool foreign_bridge_ownership_required;
  bool swift_metadata_boundary_preserved;
  bool cpp_metadata_boundary_preserved;
  bool unsafe_mixed_image_rejected;
  bool unsupported_runtime_fallback_rejected;
  bool actor_mailbox_message_identity_required;
  bool actor_mailbox_fifo_drain_required;
  bool actor_mailbox_cancel_evidence_required;
  bool actor_mailbox_error_evidence_required;
  bool actor_mailbox_shutdown_evidence_required;
  bool distributed_actor_transport_evidence_required;
  bool distributed_actor_without_transport_rejected;
  bool stale_actor_identity_rejected;
  bool cancelled_mailbox_delivery_rejected;
  bool macro_package_lock_identity_required;
  bool macro_package_trust_identity_required;
  bool macro_host_identity_required;
  bool macro_cache_identity_required;
  bool macro_input_output_identity_required;
  bool macro_runtime_consumption_identity_required;
  bool macro_stale_cache_rejected;
  bool macro_tampered_metadata_rejected;
  bool macro_missing_replay_metadata_rejected;
  int rejected_surface_count;
  const char *contract_id;
  const char *support_claim;
  const char *surface_name;
  const char *supported_boundary;
  const char *reserved_boundary;
  const char *primary_fixture;
  const char *negative_fixture;
  const char *strict_failure_diagnostic;
  const char *typed_dispatch_contract;
  const char *registration_contract;
  const char *runtime_contract;
  const char *schema_path;
  const char *docs_path;
  const char *public_command;
  const char *replay_key;
};

constexpr const char *kExecutableContractSchemaPath =
    "schemas/objc3c-advanced-runtime-executable-contract-v1.schema.json";
constexpr const char *kExecutableContractDocsPath =
    "docs/runbooks/objc3c_advanced_runtime_executable_contract.md";
constexpr const char *kAdvancedRuntimeClosureCommand =
    "npm run objc3c -- validate-advanced-runtime-closure";
constexpr const char *kTypedDispatchContract =
    "objc3_runtime_dispatch_typed_result";
constexpr const char *kNativeRegistrationContract =
    "objc3_runtime_registration_state_snapshot";

constexpr RuntimeExecutableContractSurfaceRecord
    kRuntimeExecutableContractSurfaceRecords[] = {
        {OBJC3_RUNTIME_EXECUTABLE_CONTRACT_SURFACE_SCHEDULER_TASK_RUNTIME,
         8214,
         8213,
         OBJC3_RUNTIME_EXECUTABLE_CONTRACT_SUPPORT_SUPPORTED,
         true,
         true,
         true,
         true,
         false,
         false,
         true,
         true,
         true,
         true,
         true,
         true,
         true,
         true,
         true,
         true,
         true,
         false,
         false,
         false,
         false,
         false,
         false,
         false,
         false,
         false,
         false,
         false,
         false,
         false,
         false,
         false,
         false,
         false,
         false,
         false,
         false,
         false,
         false,
         false,
         false,
         false,
         false,
         false,
         false,
         false,
         false,
         false,
         false,
         false,
         false,
         8,
         "objc3c.advanced-runtime.executable-contract.scheduler-task.v1",
         "objc3c.behavior.runtime.advanced.scheduler-task-contract",
         "scheduler-task-runtime-contract",
         "single-process deterministic task creation, start, completion, "
         "cooperative cancellation checkpoints, continuation lifecycle, "
         "executor-hop ordering, per-actor FIFO mailbox enqueue/dequeue, and "
         "bounded runtime snapshots",
         "fairness, priority inheritance, work stealing, distributed "
         "scheduling, cross-process actor transport, and external executor "
         "integration remain reserved or rejected",
         "tests/native/runtime/advanced_closure/combined_positive.objc3",
         "tests/native/runtime/advanced_closure/"
         "negative_scheduler_priority_fairness_overclaim.objc3",
         "advanced-runtime.scheduler-unsupported-policy",
         kTypedDispatchContract,
         kNativeRegistrationContract,
         "objc3_runtime_task_runtime_state_snapshot+"
         "objc3_runtime_actor_runtime_state_snapshot",
         kExecutableContractSchemaPath,
         kExecutableContractDocsPath,
         kAdvancedRuntimeClosureCommand,
         "advanced-runtime:#8214:scheduler-task-runtime-contract:v1"},
        {OBJC3_RUNTIME_EXECUTABLE_CONTRACT_SURFACE_FOREIGN_ABI_RUNTIME,
         8215,
         8213,
         OBJC3_RUNTIME_EXECUTABLE_CONTRACT_SUPPORT_SUPPORTED,
         true,
         true,
         true,
         true,
         false,
         false,
         false,
         false,
         false,
         false,
         true,
         false,
         false,
         false,
         false,
         false,
         true,
         false,
         false,
         false,
         true,
         false,
         false,
         true,
         false,
         true,
         true,
         true,
         true,
         true,
         true,
         true,
         true,
         false,
         false,
         false,
         false,
         false,
         false,
         false,
         false,
         false,
         false,
         false,
         false,
         false,
         false,
         false,
         false,
         false,
         false,
         7,
         "objc3c.advanced-runtime.executable-contract.foreign-abi.v1",
         "objc3c.behavior.runtime.advanced.foreign-abi-contract",
         "foreign-abi-runtime-contract",
         "narrow C ABI package/replay boundary with stable ownership, "
         "nullability, error, and async metadata normalized before lowering",
         "Swift runtime ABI, C++ exception propagation, template/object-layout "
         "mirroring, actor-owned Swift callables, async foreign callbacks, "
         "and arbitrary foreign runtime mirroring remain rejected",
         "tests/native/runtime/advanced_closure/combined_positive.objc3",
         "tests/native/runtime/advanced_closure/"
         "negative_foreign_abi_runtime_mirroring.objc3",
         "advanced-runtime.foreign-abi-unsupported-runtime",
         kTypedDispatchContract,
         kNativeRegistrationContract,
         "objc3_runtime_dispatch_typed_result+"
         "Objc3CrossModuleRuntimeLinkPlanImportedInput",
         kExecutableContractSchemaPath,
         kExecutableContractDocsPath,
         kAdvancedRuntimeClosureCommand,
         "advanced-runtime:#8215:foreign-abi-runtime-contract:v1"},
        {OBJC3_RUNTIME_EXECUTABLE_CONTRACT_SURFACE_ACTOR_MAILBOX_RUNTIME,
         8216,
         8213,
         OBJC3_RUNTIME_EXECUTABLE_CONTRACT_SUPPORT_SUPPORTED,
         true,
         true,
         true,
         true,
         false,
         false,
         false,
         false,
         true,
         true,
         true,
         false,
         false,
         false,
         false,
         false,
         true,
         false,
         false,
         false,
         false,
         false,
         false,
         false,
         false,
         false,
         false,
         false,
         false,
         false,
         false,
         false,
         false,
         true,
         true,
         true,
         true,
         true,
         true,
         true,
         true,
         true,
         false,
         false,
         false,
         false,
         false,
         false,
         false,
         false,
         false,
         5,
         "objc3c.advanced-runtime.executable-contract.actor-mailbox.v1",
         "objc3c.behavior.runtime.advanced.actor-mailbox-expansion",
         "actor-mailbox-runtime-expansion-contract",
         "local actor mailbox helpers preserve stable message identity, FIFO "
         "enqueue/drain order, cancellation, error, shutdown, executor binding, "
         "and replay evidence through the private actor runtime snapshot",
         "distributed actor networking, cross-process transport, remote "
         "serialization, remote identity, and Swift actor ABI compatibility "
         "remain rejected without explicit transport evidence",
         "tests/native/runtime/advanced_closure/combined_positive.objc3",
         "tests/native/runtime/advanced_closure/"
         "negative_distributed_actor_missing_transport.objc3",
         "advanced-runtime.distributed-actor-missing-transport",
         kTypedDispatchContract,
         kNativeRegistrationContract,
         "objc3_runtime_actor_runtime_state_snapshot+"
         "objc3_runtime_actor_mailbox_enqueue_i32+"
         "objc3_runtime_actor_mailbox_drain_next_i32+"
         "objc3_runtime_actor_mailbox_cancel_i32+"
         "objc3_runtime_actor_mailbox_record_error_i32+"
         "objc3_runtime_actor_mailbox_shutdown_i32",
         kExecutableContractSchemaPath,
         kExecutableContractDocsPath,
         kAdvancedRuntimeClosureCommand,
         "advanced-runtime:#8216:actor-mailbox-runtime-expansion-contract:v1"},
        {OBJC3_RUNTIME_EXECUTABLE_CONTRACT_SURFACE_MACRO_PACKAGE_REPLAY_RUNTIME,
         8217,
         8213,
         OBJC3_RUNTIME_EXECUTABLE_CONTRACT_SUPPORT_SUPPORTED,
         true,
         true,
         true,
         true,
         false,
         false,
         false,
         false,
         false,
         false,
         true,
         false,
         false,
         false,
         false,
         false,
         true,
         false,
         false,
         false,
         false,
         false,
         false,
         false,
         false,
         false,
         false,
         false,
         false,
         false,
         false,
         false,
         false,
         false,
         false,
         false,
         false,
         false,
         false,
         false,
         false,
         false,
         true,
         true,
         true,
         true,
         true,
         true,
         true,
         true,
         true,
         5,
         "objc3c.advanced-runtime.executable-contract.macro-package-replay.v1",
         "objc3c.behavior.runtime.advanced.macro-package-replay",
         "macro-package-replay-runtime-contract",
         "macro host cache runtime-import packets preserve package identity, "
         "package lock identity, trust identity, host identity, cache identity, "
         "input/output content identity, replay generation, and runtime "
         "consumption identity before cross-module replay consumption",
         "arbitrary macro host execution, unsigned package replay, stale cache "
         "reuse, host identity mismatch, missing replay metadata, and tampered "
         "runtime metadata remain rejected",
         "tests/native/runtime/advanced_closure/combined_positive.objc3",
         "tests/native/runtime/advanced_closure/"
         "negative_macro_package_replay_stale_cache.objc3",
         "advanced-runtime.macro-package-replay-stale-cache",
         kTypedDispatchContract,
         kNativeRegistrationContract,
         "objc_metaprogramming_macro_host_process_and_cache_runtime_integration+"
         "Objc3CrossModuleRuntimeLinkPlanImportedInput",
         kExecutableContractSchemaPath,
         kExecutableContractDocsPath,
         kAdvancedRuntimeClosureCommand,
         "advanced-runtime:#8217:macro-package-replay-runtime-contract:v1"},
};

constexpr std::uint64_t RuntimeExecutableContractSurfaceCount() {
  return sizeof(kRuntimeExecutableContractSurfaceRecords) /
         sizeof(kRuntimeExecutableContractSurfaceRecords[0]);
}

void InitializeRuntimeExecutableContractSurfaceSnapshot(
    objc3_runtime_executable_contract_surface_snapshot &snapshot) {
  snapshot = {};
  snapshot.abi_version = OBJC3_RUNTIME_EXECUTABLE_CONTRACT_ABI_VERSION;
  snapshot.snapshot_size =
      sizeof(objc3_runtime_executable_contract_surface_snapshot);
  snapshot.query_status = OBJC3_RUNTIME_EXECUTABLE_CONTRACT_STATUS_NOT_FOUND;
  snapshot.surface_kind = OBJC3_RUNTIME_EXECUTABLE_CONTRACT_SURFACE_INVALID;
  snapshot.support_status = OBJC3_RUNTIME_EXECUTABLE_CONTRACT_SUPPORT_REJECTED;
  snapshot.fail_closed_required = 1;
  snapshot.compatibility_shim_allowed = 0;
}

void PopulateRuntimeExecutableContractSurfaceSnapshot(
    const RuntimeExecutableContractSurfaceRecord &record,
    objc3_runtime_executable_contract_surface_snapshot &snapshot) {
  snapshot.query_status = OBJC3_RUNTIME_EXECUTABLE_CONTRACT_STATUS_OK;
  snapshot.surface_kind = record.surface_kind;
  snapshot.issue_ref = record.issue_ref;
  snapshot.dependency_issue_ref = record.dependency_issue_ref;
  snapshot.support_status = record.support_status;
  snapshot.typed_dispatch_required = record.typed_dispatch_required ? 1 : 0;
  snapshot.typed_dispatch_result_abi_version =
      OBJC3_RUNTIME_DISPATCH_TYPED_RESULT_ABI_VERSION;
  snapshot.native_registration_required =
      record.native_registration_required ? 1 : 0;
  snapshot.registration_snapshot_abi_version =
      OBJC3_RUNTIME_REGISTRATION_STATE_SNAPSHOT_ABI_VERSION;
  snapshot.executable_runtime_required =
      record.executable_runtime_required ? 1 : 0;
  snapshot.fail_closed_required = record.fail_closed_required ? 1 : 0;
  snapshot.public_abi_widened = record.public_abi_widened ? 1 : 0;
  snapshot.compatibility_shim_allowed =
      record.compatibility_shim_allowed ? 1 : 0;
  snapshot.task_record_required = record.task_record_required ? 1 : 0;
  snapshot.continuation_record_required =
      record.continuation_record_required ? 1 : 0;
  snapshot.executor_hop_record_required =
      record.executor_hop_record_required ? 1 : 0;
  snapshot.actor_mailbox_event_required =
      record.actor_mailbox_event_required ? 1 : 0;
  snapshot.deterministic_replay_required =
      record.deterministic_replay_required ? 1 : 0;
  snapshot.scheduler_queue_state_required =
      record.scheduler_queue_state_required ? 1 : 0;
  snapshot.task_lifecycle_state_required =
      record.task_lifecycle_state_required ? 1 : 0;
  snapshot.cancellation_checkpoint_required =
      record.cancellation_checkpoint_required ? 1 : 0;
  snapshot.task_error_cleanup_required =
      record.task_error_cleanup_required ? 1 : 0;
  snapshot.shutdown_drain_required = record.shutdown_drain_required ? 1 : 0;
  snapshot.imported_runtime_replay_required =
      record.imported_runtime_replay_required ? 1 : 0;
  snapshot.scheduler_fairness_supported =
      record.scheduler_fairness_supported ? 1 : 0;
  snapshot.scheduler_priority_supported =
      record.scheduler_priority_supported ? 1 : 0;
  snapshot.distributed_scheduler_supported =
      record.distributed_scheduler_supported ? 1 : 0;
  snapshot.foreign_c_abi_supported = record.foreign_c_abi_supported ? 1 : 0;
  snapshot.foreign_cxx_abi_supported =
      record.foreign_cxx_abi_supported ? 1 : 0;
  snapshot.swift_runtime_abi_supported =
      record.swift_runtime_abi_supported ? 1 : 0;
  snapshot.foreign_metadata_preserved =
      record.foreign_metadata_preserved ? 1 : 0;
  snapshot.foreign_runtime_mirroring_supported =
      record.foreign_runtime_mirroring_supported ? 1 : 0;
  snapshot.foreign_surface_classification_required =
      record.foreign_surface_classification_required ? 1 : 0;
  snapshot.foreign_typed_dispatch_expectation_required =
      record.foreign_typed_dispatch_expectation_required ? 1 : 0;
  snapshot.foreign_package_runtime_identity_required =
      record.foreign_package_runtime_identity_required ? 1 : 0;
  snapshot.foreign_bridge_ownership_required =
      record.foreign_bridge_ownership_required ? 1 : 0;
  snapshot.swift_metadata_boundary_preserved =
      record.swift_metadata_boundary_preserved ? 1 : 0;
  snapshot.cpp_metadata_boundary_preserved =
      record.cpp_metadata_boundary_preserved ? 1 : 0;
  snapshot.unsafe_mixed_image_rejected =
      record.unsafe_mixed_image_rejected ? 1 : 0;
  snapshot.unsupported_runtime_fallback_rejected =
      record.unsupported_runtime_fallback_rejected ? 1 : 0;
  snapshot.actor_mailbox_message_identity_required =
      record.actor_mailbox_message_identity_required ? 1 : 0;
  snapshot.actor_mailbox_fifo_drain_required =
      record.actor_mailbox_fifo_drain_required ? 1 : 0;
  snapshot.actor_mailbox_cancel_evidence_required =
      record.actor_mailbox_cancel_evidence_required ? 1 : 0;
  snapshot.actor_mailbox_error_evidence_required =
      record.actor_mailbox_error_evidence_required ? 1 : 0;
  snapshot.actor_mailbox_shutdown_evidence_required =
      record.actor_mailbox_shutdown_evidence_required ? 1 : 0;
  snapshot.distributed_actor_transport_evidence_required =
      record.distributed_actor_transport_evidence_required ? 1 : 0;
  snapshot.distributed_actor_without_transport_rejected =
      record.distributed_actor_without_transport_rejected ? 1 : 0;
  snapshot.stale_actor_identity_rejected =
      record.stale_actor_identity_rejected ? 1 : 0;
  snapshot.cancelled_mailbox_delivery_rejected =
      record.cancelled_mailbox_delivery_rejected ? 1 : 0;
  snapshot.macro_package_lock_identity_required =
      record.macro_package_lock_identity_required ? 1 : 0;
  snapshot.macro_package_trust_identity_required =
      record.macro_package_trust_identity_required ? 1 : 0;
  snapshot.macro_host_identity_required =
      record.macro_host_identity_required ? 1 : 0;
  snapshot.macro_cache_identity_required =
      record.macro_cache_identity_required ? 1 : 0;
  snapshot.macro_input_output_identity_required =
      record.macro_input_output_identity_required ? 1 : 0;
  snapshot.macro_runtime_consumption_identity_required =
      record.macro_runtime_consumption_identity_required ? 1 : 0;
  snapshot.macro_stale_cache_rejected =
      record.macro_stale_cache_rejected ? 1 : 0;
  snapshot.macro_tampered_metadata_rejected =
      record.macro_tampered_metadata_rejected ? 1 : 0;
  snapshot.macro_missing_replay_metadata_rejected =
      record.macro_missing_replay_metadata_rejected ? 1 : 0;
  snapshot.rejected_surface_count = record.rejected_surface_count;
  snapshot.contract_id = record.contract_id;
  snapshot.support_claim = record.support_claim;
  snapshot.surface_name = record.surface_name;
  snapshot.supported_boundary = record.supported_boundary;
  snapshot.reserved_boundary = record.reserved_boundary;
  snapshot.primary_fixture = record.primary_fixture;
  snapshot.negative_fixture = record.negative_fixture;
  snapshot.strict_failure_diagnostic = record.strict_failure_diagnostic;
  snapshot.typed_dispatch_contract = record.typed_dispatch_contract;
  snapshot.registration_contract = record.registration_contract;
  snapshot.runtime_contract = record.runtime_contract;
  snapshot.schema_path = record.schema_path;
  snapshot.docs_path = record.docs_path;
  snapshot.public_command = record.public_command;
  snapshot.replay_key = record.replay_key;
}

const RuntimeExecutableContractSurfaceRecord *
FindRuntimeExecutableContractSurfaceByKind(int surface_kind) {
  if (surface_kind <= OBJC3_RUNTIME_EXECUTABLE_CONTRACT_SURFACE_INVALID) {
    return nullptr;
  }
  for (const auto &record : kRuntimeExecutableContractSurfaceRecords) {
    if (record.surface_kind == surface_kind) {
      return &record;
    }
  }
  return nullptr;
}

}  // namespace
}  // namespace objc3c::runtime

extern "C" uint32_t objc3_runtime_executable_contract_api_abi_version(void) {
  return OBJC3_RUNTIME_EXECUTABLE_CONTRACT_ABI_VERSION;
}

extern "C" uint64_t objc3_runtime_executable_contract_surface_count(void) {
  return objc3c::runtime::RuntimeExecutableContractSurfaceCount();
}

extern "C" int objc3_runtime_copy_executable_contract_surface(
    uint64_t index,
    objc3_runtime_executable_contract_surface_snapshot *snapshot) {
  if (snapshot == nullptr) {
    return OBJC3_RUNTIME_EXECUTABLE_CONTRACT_STATUS_INVALID_OUTPUT;
  }
  objc3c::runtime::InitializeRuntimeExecutableContractSurfaceSnapshot(
      *snapshot);
  if (index >= objc3c::runtime::RuntimeExecutableContractSurfaceCount()) {
    return OBJC3_RUNTIME_EXECUTABLE_CONTRACT_STATUS_NOT_FOUND;
  }
  objc3c::runtime::PopulateRuntimeExecutableContractSurfaceSnapshot(
      objc3c::runtime::kRuntimeExecutableContractSurfaceRecords[index],
      *snapshot);
  return OBJC3_RUNTIME_EXECUTABLE_CONTRACT_STATUS_OK;
}

extern "C" int objc3_runtime_copy_executable_contract_surface_by_kind(
    int surface_kind,
    objc3_runtime_executable_contract_surface_snapshot *snapshot) {
  if (snapshot == nullptr) {
    return OBJC3_RUNTIME_EXECUTABLE_CONTRACT_STATUS_INVALID_OUTPUT;
  }
  objc3c::runtime::InitializeRuntimeExecutableContractSurfaceSnapshot(
      *snapshot);
  if (surface_kind <= OBJC3_RUNTIME_EXECUTABLE_CONTRACT_SURFACE_INVALID) {
    return OBJC3_RUNTIME_EXECUTABLE_CONTRACT_STATUS_INVALID_QUERY;
  }
  const objc3c::runtime::RuntimeExecutableContractSurfaceRecord *const record =
      objc3c::runtime::FindRuntimeExecutableContractSurfaceByKind(
          surface_kind);
  if (record == nullptr) {
    return OBJC3_RUNTIME_EXECUTABLE_CONTRACT_STATUS_NOT_FOUND;
  }
  objc3c::runtime::PopulateRuntimeExecutableContractSurfaceSnapshot(*record,
                                                                    *snapshot);
  return OBJC3_RUNTIME_EXECUTABLE_CONTRACT_STATUS_OK;
}

extern "C" int objc3_runtime_executable_contract_support_status_for_surface(
    int surface_kind) {
  const objc3c::runtime::RuntimeExecutableContractSurfaceRecord *const record =
      objc3c::runtime::FindRuntimeExecutableContractSurfaceByKind(
          surface_kind);
  if (record == nullptr) {
    return OBJC3_RUNTIME_EXECUTABLE_CONTRACT_SUPPORT_REJECTED;
  }
  return record->support_status;
}
