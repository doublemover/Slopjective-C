#pragma once

#include <stdint.h>

#include "runtime/objc3_runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct objc3_runtime_pointer_aggregate {
  uint64_t count;
  const void *entries[1];
} objc3_runtime_pointer_aggregate;

typedef struct objc3_runtime_registration_table {
  uint64_t abi_version;
  uint64_t pointer_field_count;
  const objc3_runtime_image_descriptor *image_descriptor;
  const objc3_runtime_pointer_aggregate *discovery_root;
  const void *linker_anchor;
  const objc3_runtime_pointer_aggregate *class_descriptor_root;
  const objc3_runtime_pointer_aggregate *protocol_descriptor_root;
  const objc3_runtime_pointer_aggregate *category_descriptor_root;
  const objc3_runtime_pointer_aggregate *property_descriptor_root;
  const objc3_runtime_pointer_aggregate *ivar_descriptor_root;
  const objc3_runtime_pointer_aggregate *selector_pool_root;
  const objc3_runtime_pointer_aggregate *string_pool_root;
  unsigned char *image_local_init_state;
} objc3_runtime_registration_table;

typedef struct objc3_runtime_image_walk_state_snapshot {
  uint64_t walked_image_count;
  uint64_t last_discovery_root_entry_count;
  uint64_t last_walked_class_descriptor_count;
  uint64_t last_walked_protocol_descriptor_count;
  uint64_t last_walked_category_descriptor_count;
  uint64_t last_walked_property_descriptor_count;
  uint64_t last_walked_ivar_descriptor_count;
  uint64_t last_walked_selector_pool_count;
  uint64_t last_walked_string_pool_count;
  int last_linker_anchor_matches_discovery_root;
  int last_registration_used_staged_table;
  const char *last_walked_module_name;
  const char *last_walked_translation_unit_identity_key;
} objc3_runtime_image_walk_state_snapshot;

typedef struct objc3_runtime_reset_replay_state_snapshot {
  uint64_t retained_bootstrap_image_count;
  uint64_t last_reset_cleared_image_local_init_state_count;
  uint64_t last_replayed_image_count;
  uint64_t reset_generation;
  uint64_t replay_generation;
  int last_replay_status;
  const char *last_replayed_module_name;
  const char *last_replayed_translation_unit_identity_key;
} objc3_runtime_reset_replay_state_snapshot;

// M254-D002 runtime-registrar anchor: this private bootstrap surface carries
// emitted registration tables into the frozen D001 public API without widening
// the public header or archive contract.
void objc3_runtime_stage_registration_table_for_bootstrap(
    const objc3_runtime_registration_table *registration_table);
int objc3_runtime_copy_image_walk_state_for_testing(
    objc3_runtime_image_walk_state_snapshot *snapshot);
int objc3_runtime_replay_registered_images_for_testing(void);
int objc3_runtime_copy_reset_replay_state_for_testing(
    objc3_runtime_reset_replay_state_snapshot *snapshot);

#ifdef __cplusplus
}
#endif
