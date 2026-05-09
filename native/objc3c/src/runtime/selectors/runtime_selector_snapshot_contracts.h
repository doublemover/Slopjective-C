#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct objc3_runtime_selector_lookup_table_state_snapshot {
  uint64_t selector_table_entry_count;
  uint64_t metadata_backed_selector_count;
  uint64_t dynamic_selector_count;
  uint64_t metadata_provider_edge_count;
  const char *last_materialized_selector;
  uint64_t last_materialized_stable_id;
  uint64_t last_materialized_registration_order_ordinal;
  uint64_t last_materialized_selector_pool_index;
  int last_materialized_from_metadata;
} objc3_runtime_selector_lookup_table_state_snapshot;

typedef struct objc3_runtime_selector_lookup_entry_snapshot {
  int found;
  int metadata_backed;
  uint64_t stable_id;
  uint64_t metadata_provider_count;
  uint64_t first_registration_order_ordinal;
  uint64_t last_registration_order_ordinal;
  uint64_t first_selector_pool_index;
  uint64_t last_selector_pool_index;
  const char *canonical_selector;
} objc3_runtime_selector_lookup_entry_snapshot;

typedef struct objc3_runtime_keypath_registry_state_snapshot {
  uint64_t keypath_table_entry_count;
  uint64_t image_backed_keypath_count;
  uint64_t ambiguous_keypath_handle_count;
  uint64_t last_materialized_handle;
  uint64_t last_materialized_registration_order_ordinal;
  uint64_t last_queried_handle;
  int last_query_found;
  int last_query_ambiguous;
  const char *last_materialized_profile;
  const char *last_resolved_profile;
} objc3_runtime_keypath_registry_state_snapshot;

typedef struct objc3_runtime_keypath_entry_snapshot {
  int found;
  int ambiguous;
  int root_is_self;
  uint64_t stable_id;
  uint64_t component_count;
  uint64_t metadata_provider_count;
  uint64_t first_registration_order_ordinal;
  uint64_t last_registration_order_ordinal;
  const char *root_name;
  const char *component_path;
  const char *profile;
  const char *generic_metadata_replay_key;
} objc3_runtime_keypath_entry_snapshot;

int objc3_runtime_copy_selector_lookup_table_state_for_testing(
    objc3_runtime_selector_lookup_table_state_snapshot *snapshot);
int objc3_runtime_copy_selector_lookup_entry_for_testing(
    const char *selector,
    objc3_runtime_selector_lookup_entry_snapshot *snapshot);
int objc3_runtime_copy_keypath_registry_state_for_testing(
    objc3_runtime_keypath_registry_state_snapshot *snapshot);
int objc3_runtime_copy_keypath_entry_for_testing(
    uint64_t stable_id, objc3_runtime_keypath_entry_snapshot *snapshot);

// optional/key-path runtime-helper freeze anchor: the current Part 3
// runtime boundary does not add a new public helper API. Optional sends remain
// routed through the public selector lookup/dispatch entrypoints, while the
// validated typed key-path slice currently exposes retained descriptor handles
// and sections as the runtime-facing input boundary. Full runtime key-path
// evaluation helpers remain deferred to the next runtime step.
// live-optional-send-and-keypath-runtime-support anchor: the first
// live typed key-path runtime support stays on this private runtime header.
// The runtime consumes emitted key-path descriptor roots into an image-backed
// registry, publishes query/state snapshots for probes, and exposes narrow
// helper entrypoints for validated single-component handle execution without
// widening the stable public runtime header yet.
// cross-module type-surface preservation anchor: imported runtime
// surfaces must preserve the same typed key-path/runtime-helper packets so
// multi-image registration keeps provider metadata truthful without inventing a
// second key-path registry model.
int objc3_runtime_keypath_component_count_for_testing(int keypath_handle);
int objc3_runtime_keypath_root_is_self_for_testing(int keypath_handle);

#ifdef __cplusplus
}
#endif
