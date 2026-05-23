#pragma once

#include "runtime/public/objc3_runtime_dispatch_result.h"
#include "runtime/public/objc3_runtime_registration.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define OBJC3_RUNTIME_EXECUTABLE_CONTRACT_ABI_VERSION 1u

typedef enum objc3_runtime_executable_contract_status_code {
  OBJC3_RUNTIME_EXECUTABLE_CONTRACT_STATUS_OK = 0,
  OBJC3_RUNTIME_EXECUTABLE_CONTRACT_STATUS_NOT_FOUND = 1,
  OBJC3_RUNTIME_EXECUTABLE_CONTRACT_STATUS_INVALID_OUTPUT = -1,
  OBJC3_RUNTIME_EXECUTABLE_CONTRACT_STATUS_INVALID_QUERY = -2
} objc3_runtime_executable_contract_status_code;

typedef enum objc3_runtime_executable_contract_surface_kind {
  OBJC3_RUNTIME_EXECUTABLE_CONTRACT_SURFACE_INVALID = 0,
  OBJC3_RUNTIME_EXECUTABLE_CONTRACT_SURFACE_SCHEDULER_TASK_RUNTIME = 1,
  OBJC3_RUNTIME_EXECUTABLE_CONTRACT_SURFACE_FOREIGN_ABI_RUNTIME = 2
} objc3_runtime_executable_contract_surface_kind;

typedef enum objc3_runtime_executable_contract_support_status {
  OBJC3_RUNTIME_EXECUTABLE_CONTRACT_SUPPORT_INVALID = 0,
  OBJC3_RUNTIME_EXECUTABLE_CONTRACT_SUPPORT_SUPPORTED = 1,
  OBJC3_RUNTIME_EXECUTABLE_CONTRACT_SUPPORT_RESERVED = 2,
  OBJC3_RUNTIME_EXECUTABLE_CONTRACT_SUPPORT_REJECTED = 3
} objc3_runtime_executable_contract_support_status;

typedef struct objc3_runtime_executable_contract_surface_snapshot {
  uint32_t abi_version;
  uint32_t snapshot_size;
  int query_status;
  int surface_kind;
  int issue_ref;
  int dependency_issue_ref;
  int support_status;
  int typed_dispatch_required;
  int typed_dispatch_result_abi_version;
  int native_registration_required;
  int registration_snapshot_abi_version;
  int executable_runtime_required;
  int fail_closed_required;
  int public_abi_widened;
  int compatibility_shim_allowed;
  int task_record_required;
  int continuation_record_required;
  int executor_hop_record_required;
  int actor_mailbox_event_required;
  int deterministic_replay_required;
  int scheduler_fairness_supported;
  int scheduler_priority_supported;
  int distributed_scheduler_supported;
  int foreign_c_abi_supported;
  int foreign_cxx_abi_supported;
  int swift_runtime_abi_supported;
  int foreign_metadata_preserved;
  int foreign_runtime_mirroring_supported;
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
} objc3_runtime_executable_contract_surface_snapshot;

uint32_t objc3_runtime_executable_contract_api_abi_version(void);
uint64_t objc3_runtime_executable_contract_surface_count(void);
int objc3_runtime_copy_executable_contract_surface(
    uint64_t index,
    objc3_runtime_executable_contract_surface_snapshot *snapshot);
int objc3_runtime_copy_executable_contract_surface_by_kind(
    int surface_kind,
    objc3_runtime_executable_contract_surface_snapshot *snapshot);
int objc3_runtime_executable_contract_support_status_for_surface(
    int surface_kind);

#ifdef __cplusplus
}
#endif
