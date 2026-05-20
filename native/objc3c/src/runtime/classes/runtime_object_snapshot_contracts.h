#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct objc3_runtime_realized_class_graph_state_snapshot {
  uint64_t realized_class_count;
  uint64_t root_class_count;
  uint64_t metaclass_edge_count;
  uint64_t receiver_class_binding_count;
  uint64_t attached_category_count;
  uint64_t protocol_conformance_edge_count;
  uint64_t class_graph_generation;
  uint64_t category_attachment_generation;
  uint64_t protocol_declaration_generation;
  uint64_t storage_surface_generation;
  uint64_t method_surface_generation;
  uint64_t live_instance_count;
  uint64_t malformed_class_metadata_rejection_count;
  uint64_t last_allocated_receiver_identity;
  uint64_t last_allocated_base_identity;
  uint64_t last_allocated_instance_size_bytes;
  uint64_t last_allocated_allocation_ordinal;
  const char *last_realized_class_name;
  const char *last_realized_class_owner_identity;
  const char *last_realized_metaclass_owner_identity;
  const char *last_attached_category_owner_identity;
  const char *last_attached_category_name;
  const char *last_allocated_class_name;
  const char *last_malformed_class_graph_reason;
} objc3_runtime_realized_class_graph_state_snapshot;

typedef struct objc3_runtime_realized_class_entry_snapshot {
  int found;
  uint64_t base_identity;
  uint64_t instance_receiver_identity;
  uint64_t class_receiver_identity;
  uint64_t registration_order_ordinal;
  int is_root_class;
  int has_super_node;
  int implementation_backed;
  uint64_t attached_category_count;
  uint64_t direct_protocol_count;
  uint64_t attached_protocol_count;
  uint64_t runtime_property_accessor_count;
  uint64_t runtime_instance_size_bytes;
  uint64_t super_base_identity;
  const char *module_name;
  const char *translation_unit_identity_key;
  const char *class_name;
  const char *super_class_name;
  const char *class_owner_identity;
  const char *metaclass_owner_identity;
  const char *super_class_owner_identity;
  const char *super_metaclass_owner_identity;
  const char *instance_isa_owner_identity;
  const char *class_object_isa_owner_identity;
  const char *metaclass_object_isa_owner_identity;
  const char *root_class_owner_identity;
  const char *root_metaclass_owner_identity;
  const char *last_attached_category_owner_identity;
  const char *last_attached_category_name;
} objc3_runtime_realized_class_entry_snapshot;

typedef struct objc3_runtime_instance_entry_snapshot {
  int found;
  uint64_t receiver_identity;
  uint64_t base_identity;
  uint64_t allocation_ordinal;
  uint64_t instance_size_bytes;
  uint64_t storage_size_bytes;
  uint64_t zero_initialized_storage_byte_count;
  uint64_t retain_count;
  const char *class_name;
} objc3_runtime_instance_entry_snapshot;

typedef struct objc3_runtime_property_registry_state_snapshot {
  uint64_t layout_ready_class_count;
  uint64_t reflectable_property_count;
  uint64_t writable_property_count;
  uint64_t slot_backed_property_count;
  uint64_t property_lookup_cache_entry_count;
  uint64_t property_lookup_cache_hit_count;
  uint64_t property_lookup_cache_miss_count;
  int last_query_found;
  int last_query_inherited;
  int last_query_used_cache;
  const char *last_queried_class_name;
  const char *last_queried_property_name;
  const char *last_resolved_class_name;
  const char *last_resolved_owner_identity;
} objc3_runtime_property_registry_state_snapshot;

typedef struct objc3_runtime_property_entry_snapshot {
  int found;
  int inherited;
  int setter_available;
  int has_runtime_getter;
  int has_runtime_setter;
  uint64_t attribute_count;
  int is_readonly;
  int is_nonatomic;
  int is_strong;
  int is_assign;
  int is_weak;
  int is_copy;
  int has_custom_getter;
  int has_custom_setter;
  uint64_t base_identity;
  uint64_t slot_index;
  uint64_t offset_bytes;
  uint64_t size_bytes;
  uint64_t alignment_bytes;
  uint64_t padding_bytes;
  uint64_t inherited_slot_count;
  uint64_t inherited_size_bytes;
  uint64_t owner_size_bytes;
  uint64_t init_order_index;
  uint64_t destroy_order_index;
  int layout_valid;
  uint64_t instance_size_bytes;
  const char *queried_class_name;
  const char *resolved_class_name;
  const char *property_name;
  const char *declaration_owner_identity;
  const char *export_owner_identity;
  const char *getter_selector;
  const char *setter_selector;
  const char *effective_getter_selector;
  const char *effective_setter_selector;
  const char *ivar_binding_symbol;
  const char *synthesized_binding_symbol;
  const char *ivar_layout_symbol;
  const char *ivar_layout_replay_key;
  const char *property_attribute_profile;
  const char *ownership_lifetime_profile;
  const char *ownership_runtime_hook_profile;
  const char *accessor_ownership_profile;
  const char *getter_owner_identity;
  const char *setter_owner_identity;
} objc3_runtime_property_entry_snapshot;

typedef struct objc3_runtime_storage_accessor_implementation_snapshot {
  uint64_t property_registry_ready;
  uint64_t runtime_accessor_dispatch_ready;
  uint64_t runtime_layout_ready;
  uint64_t reflection_query_ready;
  uint64_t deterministic;
  const char *property_registry_state_snapshot_symbol;
  const char *property_entry_snapshot_symbol;
  const char *current_property_read_symbol;
  const char *current_property_write_symbol;
  const char *current_property_exchange_symbol;
  const char *bind_current_property_context_symbol;
  const char *clear_current_property_context_symbol;
  const char *weak_current_property_load_symbol;
  const char *weak_current_property_store_symbol;
  const char *implementation_model;
  const char *reflection_model;
  const char *fail_closed_model;
} objc3_runtime_storage_accessor_implementation_snapshot;

typedef struct objc3_runtime_protocol_conformance_query_snapshot {
  int class_found;
  int protocol_found;
  int conforms;
  uint64_t visited_protocol_count;
  uint64_t attached_category_count;
  uint64_t matched_protocol_depth;
  int matched_from_category;
  int matched_from_superclass;
  int matched_via_inherited_protocol;
  int malformed_metadata;
  const char *class_name;
  const char *protocol_name;
  const char *matched_protocol_owner_identity;
  const char *matched_attachment_owner_identity;
  const char *matched_class_name;
  const char *matched_class_owner_identity;
  const char *failure_reason;
} objc3_runtime_protocol_conformance_query_snapshot;

typedef struct objc3_runtime_object_model_query_state_snapshot {
  uint64_t realized_class_count;
  uint64_t reflectable_property_count;
  uint64_t attached_category_count;
  uint64_t protocol_conformance_edge_count;
  uint64_t method_cache_entry_count;
  int last_class_query_found;
  int last_property_query_found;
  int last_property_query_inherited;
  int last_protocol_query_class_found;
  int last_protocol_query_protocol_found;
  int last_protocol_query_conforms;
  const char *last_queried_class_name;
  const char *last_resolved_class_name;
  const char *last_resolved_class_owner_identity;
  const char *last_queried_property_name;
  const char *last_resolved_property_class_name;
  const char *last_resolved_property_owner_identity;
  const char *last_queried_protocol_class_name;
  const char *last_queried_protocol_name;
  const char *last_matched_protocol_owner_identity;
  const char *last_matched_attachment_owner_identity;
} objc3_runtime_object_model_query_state_snapshot;

// metaclass-graph-root-class anchor: the runtime now owns a realized
// class/metaclass graph with explicit root-class publication, and the
// canonical proof surface for that graph stays behind private testing
// snapshots instead of widening the public ABI.
// category-attachment-protocol-conformance anchor: runtime-owned
// category attachment and protocol conformance queries also remain on this
// private testing surface so live graph proofs stay observable without
// expanding the public runtime ABI.
// canonical-runnable-object-sample anchor: canonical object-sample
// proofs likewise stay on these same realized-graph and method-cache snapshots
// so builtin alloc/new/init ownership can be observed without widening the
// public ABI.
// property-layout-runtime anchor: repeated alloc/new observations,
// synthesized accessor cache entries, and registration/selector snapshots also
// remain the canonical proof surface for the current single-instance
// property/layout runtime boundary.
// instance-allocation-layout-runtime anchor: the same private
// realized-graph snapshots now also publish live instance-allocation and
// runtime layout-consumption evidence for synthesized property access without
// widening the public runtime header.
// property-metadata-reflection anchor: property metadata
// registration state and per-property reflective helper lookups stay on this
// same private testing surface so diagnostics and probes can consume the live
// runtime-owned metadata without widening the public ABI.
int objc3_runtime_copy_realized_class_graph_state_for_testing(
    objc3_runtime_realized_class_graph_state_snapshot *snapshot);
int objc3_runtime_copy_realized_class_entry_for_testing(
    const char *class_name,
    objc3_runtime_realized_class_entry_snapshot *snapshot);
int objc3_runtime_copy_instance_entry_for_testing(
    int receiver_identity, objc3_runtime_instance_entry_snapshot *snapshot);
int objc3_runtime_copy_property_registry_state_for_testing(
    objc3_runtime_property_registry_state_snapshot *snapshot);
int objc3_runtime_copy_property_entry_for_testing(
    const char *class_name, const char *property_name,
    objc3_runtime_property_entry_snapshot *snapshot);
int objc3_runtime_copy_storage_accessor_implementation_snapshot_for_testing(
    objc3_runtime_storage_accessor_implementation_snapshot *snapshot);
int objc3_runtime_copy_protocol_conformance_query_for_testing(
    const char *class_name, const char *protocol_name,
    objc3_runtime_protocol_conformance_query_snapshot *snapshot);
int objc3_runtime_copy_object_model_query_state_for_testing(
    objc3_runtime_object_model_query_state_snapshot *snapshot);

#ifdef __cplusplus
}
#endif
