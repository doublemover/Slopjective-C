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

typedef struct objc3_runtime_method_cache_state_snapshot {
  uint64_t cache_entry_count;
  uint64_t cache_hit_count;
  uint64_t cache_miss_count;
  uint64_t slow_path_lookup_count;
  uint64_t live_dispatch_count;
  uint64_t fallback_dispatch_count;
  uint64_t last_selector_stable_id;
  uint64_t last_normalized_receiver_identity;
  uint64_t last_category_probe_count;
  uint64_t last_protocol_probe_count;
  int last_dispatch_used_cache;
  int last_dispatch_resolved_live_method;
  int last_dispatch_fell_back;
  const char *last_selector;
  const char *last_resolved_class_name;
  const char *last_resolved_owner_identity;
} objc3_runtime_method_cache_state_snapshot;

typedef struct objc3_runtime_method_cache_entry_snapshot {
  int found;
  int resolved;
  int dispatch_family_is_class;
  uint64_t normalized_receiver_identity;
  uint64_t selector_stable_id;
  uint64_t parameter_count;
  uint64_t category_probe_count;
  uint64_t protocol_probe_count;
  const char *selector;
  const char *resolved_class_name;
  const char *resolved_owner_identity;
} objc3_runtime_method_cache_entry_snapshot;

typedef struct objc3_runtime_realized_class_graph_state_snapshot {
  uint64_t realized_class_count;
  uint64_t root_class_count;
  uint64_t metaclass_edge_count;
  uint64_t receiver_class_binding_count;
  uint64_t attached_category_count;
  uint64_t protocol_conformance_edge_count;
  const char *last_realized_class_name;
  const char *last_realized_class_owner_identity;
  const char *last_realized_metaclass_owner_identity;
  const char *last_attached_category_owner_identity;
  const char *last_attached_category_name;
} objc3_runtime_realized_class_graph_state_snapshot;

typedef struct objc3_runtime_realized_class_entry_snapshot {
  int found;
  uint64_t base_identity;
  uint64_t registration_order_ordinal;
  int is_root_class;
  int implementation_backed;
  uint64_t attached_category_count;
  uint64_t direct_protocol_count;
  uint64_t attached_protocol_count;
  const char *module_name;
  const char *translation_unit_identity_key;
  const char *class_name;
  const char *class_owner_identity;
  const char *metaclass_owner_identity;
  const char *super_class_owner_identity;
  const char *super_metaclass_owner_identity;
  const char *last_attached_category_owner_identity;
  const char *last_attached_category_name;
} objc3_runtime_realized_class_entry_snapshot;

typedef struct objc3_runtime_protocol_conformance_query_snapshot {
  int class_found;
  int protocol_found;
  int conforms;
  uint64_t visited_protocol_count;
  uint64_t attached_category_count;
  const char *class_name;
  const char *protocol_name;
  const char *matched_protocol_owner_identity;
  const char *matched_attachment_owner_identity;
} objc3_runtime_protocol_conformance_query_snapshot;

// M254-D002 runtime-registrar anchor: this private bootstrap surface carries
// emitted registration tables into the frozen D001 public API without widening
// the public header or archive contract.
// M263-D001 runtime-bootstrap-table-consumption anchor: the next public
// `objc3_runtime_register_image` call consumes the staged registration table at
// most once, duplicate identity rejection happens before live state advances,
// and `objc3_runtime_copy_image_walk_state_for_testing` remains the canonical
// bootstrap-visible image-state publication surface for runtime probes.
void objc3_runtime_stage_registration_table_for_bootstrap(
    const objc3_runtime_registration_table *registration_table);
int objc3_runtime_copy_image_walk_state_for_testing(
    objc3_runtime_image_walk_state_snapshot *snapshot);
// M263-D002 live-registration-discovery-replay anchor: the retained bootstrap
// catalog, reset/replay snapshot, and replay entrypoint are the canonical
// runtime-owned proof surface for live discovery tracking and deterministic
// replay over emitted metadata images.
// M263-D003 live-restart-hardening anchor: repeated reset/replay cycles must
// stay idempotent, preserve the retained bootstrap catalog across teardown, and
// keep restart evidence observable through the same replay/reset snapshot APIs.
int objc3_runtime_replay_registered_images_for_testing(void);
int objc3_runtime_copy_reset_replay_state_for_testing(
    objc3_runtime_reset_replay_state_snapshot *snapshot);
int objc3_runtime_copy_selector_lookup_table_state_for_testing(
    objc3_runtime_selector_lookup_table_state_snapshot *snapshot);
int objc3_runtime_copy_selector_lookup_entry_for_testing(
    const char *selector,
    objc3_runtime_selector_lookup_entry_snapshot *snapshot);
// M256-D001 class-realization-runtime anchor: the private method-cache
// snapshots remain the canonical proof surface for realized class/metaclass
// resolution, category attachment, and protocol-aware negative runtime checks
// without widening the public runtime header.
int objc3_runtime_copy_method_cache_state_for_testing(
    objc3_runtime_method_cache_state_snapshot *snapshot);
int objc3_runtime_copy_method_cache_entry_for_testing(
    int receiver, const char *selector,
    objc3_runtime_method_cache_entry_snapshot *snapshot);
// M256-D002 metaclass-graph-root-class anchor: the runtime now owns a realized
// class/metaclass graph with explicit root-class publication, and the
// canonical proof surface for that graph stays behind private testing
// snapshots instead of widening the public ABI.
// M256-D003 category-attachment-protocol-conformance anchor: runtime-owned
// category attachment and protocol conformance queries also remain on this
// private testing surface so live graph proofs stay observable without
// expanding the public runtime ABI.
// M256-D004 canonical-runnable-object-sample anchor: canonical object-sample
// proofs likewise stay on these same realized-graph and method-cache snapshots
// so builtin alloc/new/init ownership can be observed without widening the
// public ABI.
int objc3_runtime_copy_realized_class_graph_state_for_testing(
    objc3_runtime_realized_class_graph_state_snapshot *snapshot);
int objc3_runtime_copy_realized_class_entry_for_testing(
    const char *class_name,
    objc3_runtime_realized_class_entry_snapshot *snapshot);
int objc3_runtime_copy_protocol_conformance_query_for_testing(
    const char *class_name, const char *protocol_name,
    objc3_runtime_protocol_conformance_query_snapshot *snapshot);

#ifdef __cplusplus
}
#endif
