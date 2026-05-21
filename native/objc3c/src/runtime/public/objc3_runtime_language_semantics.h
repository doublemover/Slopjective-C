#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define OBJC3_RUNTIME_LANGUAGE_SEMANTICS_ABI_VERSION 3u

typedef enum objc3_runtime_language_semantics_status_code {
  OBJC3_RUNTIME_LANGUAGE_SEMANTICS_STATUS_OK = 0,
  OBJC3_RUNTIME_LANGUAGE_SEMANTICS_STATUS_NOT_FOUND = 1,
  OBJC3_RUNTIME_LANGUAGE_SEMANTICS_STATUS_INVALID_OUTPUT = -1,
  OBJC3_RUNTIME_LANGUAGE_SEMANTICS_STATUS_INVALID_QUERY = -2,
} objc3_runtime_language_semantics_status_code;

typedef enum objc3_runtime_language_semantics_surface_kind {
  OBJC3_RUNTIME_LANGUAGE_SEMANTICS_SURFACE_INVALID = 0,
  OBJC3_RUNTIME_LANGUAGE_SEMANTICS_SURFACE_GENERIC_RUNTIME_IDENTITY = 1,
  OBJC3_RUNTIME_LANGUAGE_SEMANTICS_SURFACE_PROTOCOL_EXISTENTIAL_WITNESS = 2,
  OBJC3_RUNTIME_LANGUAGE_SEMANTICS_SURFACE_OWNERSHIP_MEMORY_EDGE = 3,
  OBJC3_RUNTIME_LANGUAGE_SEMANTICS_SURFACE_CONCURRENCY_PUBLIC_API = 4,
  OBJC3_RUNTIME_LANGUAGE_SEMANTICS_SURFACE_ADVANCED_RUNTIME_CLOSURE = 5,
} objc3_runtime_language_semantics_surface_kind;

typedef struct objc3_runtime_language_semantics_surface_snapshot {
  uint32_t abi_version;
  uint32_t snapshot_size;
  int status;
  int surface_kind;
  int issue_ref;
  int supported;
  int runtime_metadata_evidence;
  int sema_lowering_evidence;
  int executable_fixture_evidence;
  int fail_closed;
  int public_api_surface;
  int associated_type_support;
  int dynamic_existential_dispatch_support;
  int combined_runtime_evidence;
  int negative_combination_evidence;
  int source_identity_evidence;
  int umbrella_closure_support;
  const char *support_claim;
  const char *semantic_surface;
  const char *metadata_key;
  const char *runtime_anchor;
  const char *witness_metadata_key;
  const char *conformance_metadata_key;
  const char *positive_fixture;
  const char *negative_fixture;
  const char *diagnostic_code;
  const char *unsupported_associated_type_diagnostic;
  const char *unsupported_dynamic_dispatch_diagnostic;
  const char *unsupported_combination_diagnostic;
  const char *unsupported_policy;
  const char *combined_fixture;
  const char *combined_contract;
  const char *public_command;
  const char *replay_key;
} objc3_runtime_language_semantics_surface_snapshot;

uint32_t objc3_runtime_language_semantics_api_abi_version(void);
uint64_t objc3_runtime_language_semantics_surface_count(void);
int objc3_runtime_copy_language_semantics_surface(
    uint64_t index,
    objc3_runtime_language_semantics_surface_snapshot *snapshot);
int objc3_runtime_copy_language_semantics_surface_by_kind(
    int surface_kind,
    objc3_runtime_language_semantics_surface_snapshot *snapshot);

#ifdef __cplusplus
}
#endif
