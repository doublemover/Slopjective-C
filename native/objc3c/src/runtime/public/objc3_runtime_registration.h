#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct objc3_runtime_image_descriptor {
  /* Borrowed by the registration call and copied into runtime-owned storage. */
  const char *module_name;
  const char *translation_unit_identity_key;
  uint64_t registration_order_ordinal;
  uint64_t class_descriptor_count;
  uint64_t protocol_descriptor_count;
  uint64_t category_descriptor_count;
  uint64_t property_descriptor_count;
  uint64_t ivar_descriptor_count;
} objc3_runtime_image_descriptor;

typedef struct objc3_runtime_registration_state_snapshot {
  uint64_t registered_image_count;
  uint64_t registered_descriptor_total;
  uint64_t next_expected_registration_order_ordinal;
  uint64_t last_successful_registration_order_ordinal;
  int last_registration_status;
  /* Runtime-owned borrowed snapshot strings; callers must not free them. */
  const char *last_registered_module_name;
  const char *last_registered_translation_unit_identity_key;
  const char *last_rejected_module_name;
  const char *last_rejected_translation_unit_identity_key;
  uint64_t last_rejected_registration_order_ordinal;
  const char *owner_split_contract_id;
  const char *metadata_model_owner;
  const char *registration_table_owner;
  const char *manifest_descriptor_artifact_owner;
  const char *bootstrap_replay_owner;
  const char *public_registration_api_owner;
  const char *public_dispatch_diagnostics_owner;
  const char *fail_closed_ownership_model;
  int runtime_owner_split_explicit;
  int retired_route_path_allowed;
} objc3_runtime_registration_state_snapshot;

#ifdef __cplusplus
}
#endif
