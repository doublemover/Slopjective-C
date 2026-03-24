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
  const objc3_runtime_pointer_aggregate *keypath_descriptor_root;
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
  uint64_t last_walked_keypath_descriptor_count;
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

// M272-D001 runtime-fast-path-integration anchor: Part 9 freezes the
// existing private method-cache snapshot and entry-query helpers as the
// truthful runtime proof surface for mixed direct-call bypass, dynamic opt-out,
// and deterministic fallback dispatch behavior before D002 widens the live
// fast-path runtime itself.
// M272-D002 live-dispatch-fast-path anchor: Part 9 now widens the same private
// proof surface with seeded-fast-path counters, seeded-entry flags, and the
// last fast-path reason so first-call cache-hit behavior remains observable
// without reopening the public runtime ABI.
typedef struct objc3_runtime_method_cache_state_snapshot {
  uint64_t cache_entry_count;
  uint64_t cache_hit_count;
  uint64_t cache_miss_count;
  uint64_t slow_path_lookup_count;
  uint64_t live_dispatch_count;
  uint64_t fallback_dispatch_count;
  uint64_t fast_path_seed_count;
  uint64_t fast_path_hit_count;
  uint64_t last_selector_stable_id;
  uint64_t last_normalized_receiver_identity;
  uint64_t last_category_probe_count;
  uint64_t last_protocol_probe_count;
  int last_dispatch_used_cache;
  int last_dispatch_used_fast_path;
  int last_dispatch_resolved_live_method;
  int last_dispatch_fell_back;
  const char *last_selector;
  const char *last_fast_path_reason;
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
  int fast_path_seeded;
  int effective_direct_dispatch;
  int objc_final_declared;
  int objc_sealed_declared;
  const char *selector;
  const char *fast_path_reason;
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
  uint64_t live_instance_count;
  uint64_t last_allocated_receiver_identity;
  uint64_t last_allocated_base_identity;
  uint64_t last_allocated_instance_size_bytes;
  const char *last_realized_class_name;
  const char *last_realized_class_owner_identity;
  const char *last_realized_metaclass_owner_identity;
  const char *last_attached_category_owner_identity;
  const char *last_attached_category_name;
  const char *last_allocated_class_name;
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
  uint64_t runtime_property_accessor_count;
  uint64_t runtime_instance_size_bytes;
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

typedef struct objc3_runtime_property_registry_state_snapshot {
  uint64_t layout_ready_class_count;
  uint64_t reflectable_property_count;
  uint64_t writable_property_count;
  uint64_t slot_backed_property_count;
  int last_query_found;
  int last_query_inherited;
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
  uint64_t base_identity;
  uint64_t slot_index;
  uint64_t offset_bytes;
  uint64_t size_bytes;
  uint64_t alignment_bytes;
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
  const char *property_attribute_profile;
  const char *ownership_lifetime_profile;
  const char *ownership_runtime_hook_profile;
  const char *accessor_ownership_profile;
  const char *getter_owner_identity;
  const char *setter_owner_identity;
} objc3_runtime_property_entry_snapshot;

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

typedef struct objc3_runtime_memory_management_state_snapshot {
  uint64_t live_runtime_instance_count;
  uint64_t weak_target_count;
  uint64_t weak_slot_ref_count;
  uint64_t autoreleasepool_depth;
  uint64_t autoreleasepool_max_depth;
  uint64_t queued_autorelease_value_count;
  uint64_t drained_autorelease_value_count;
  int last_autoreleased_value;
  int last_drained_autorelease_value;
} objc3_runtime_memory_management_state_snapshot;

typedef struct objc3_runtime_arc_debug_state_snapshot {
  uint64_t retain_call_count;
  uint64_t release_call_count;
  uint64_t autorelease_call_count;
  uint64_t autoreleasepool_push_count;
  uint64_t autoreleasepool_pop_count;
  uint64_t current_property_read_count;
  uint64_t current_property_write_count;
  uint64_t current_property_exchange_count;
  uint64_t weak_current_property_load_count;
  uint64_t weak_current_property_store_count;
  int last_retain_value;
  int last_release_value;
  int last_autorelease_value;
  int last_property_read_value;
  int last_property_written_value;
  int last_property_exchange_previous_value;
  int last_property_exchange_new_value;
  int last_weak_loaded_value;
  int last_weak_stored_value;
  int last_property_receiver;
  const char *last_property_name;
  const char *last_property_owner_identity;
} objc3_runtime_arc_debug_state_snapshot;

typedef struct objc3_runtime_error_bridge_state_snapshot {
  uint64_t store_call_count;
  uint64_t load_call_count;
  uint64_t status_bridge_call_count;
  uint64_t nserror_bridge_call_count;
  uint64_t catch_match_call_count;
  int last_stored_error_value;
  int last_loaded_error_value;
  int last_status_bridge_status_value;
  int last_status_bridge_error_value;
  int last_nserror_bridge_error_value;
  int last_catch_match_error_value;
  int last_catch_match_kind;
  int last_catch_match_is_catch_all;
  int last_catch_match_result;
  const char *last_catch_kind_name;
} objc3_runtime_error_bridge_state_snapshot;

typedef struct objc3_runtime_async_continuation_state_snapshot {
  uint64_t allocation_call_count;
  uint64_t handoff_call_count;
  uint64_t resume_call_count;
  uint64_t live_continuation_handle_count;
  int last_allocated_continuation_handle;
  int last_allocated_resume_entry_tag;
  int last_allocated_executor_tag;
  int last_handoff_continuation_handle;
  int last_handoff_executor_tag;
  int last_resume_continuation_handle;
  int last_resume_result_value;
  int last_resume_return_value;
} objc3_runtime_async_continuation_state_snapshot;

typedef struct objc3_runtime_task_runtime_state_snapshot {
  uint64_t spawn_call_count;
  uint64_t scope_call_count;
  uint64_t add_task_call_count;
  uint64_t wait_next_call_count;
  uint64_t cancel_all_call_count;
  uint64_t cancellation_poll_call_count;
  uint64_t on_cancel_call_count;
  uint64_t executor_hop_call_count;
  int last_spawn_kind;
  int last_spawn_executor_tag;
  int last_scope_executor_tag;
  int last_add_task_executor_tag;
  int last_wait_next_executor_tag;
  int last_cancel_all_executor_tag;
  int last_cancellation_poll_executor_tag;
  int last_on_cancel_executor_tag;
  int last_executor_hop_executor_tag;
  int last_executor_hop_value;
  int last_wait_next_result;
  int last_cancel_all_result;
  int last_cancellation_poll_result;
} objc3_runtime_task_runtime_state_snapshot;

typedef struct objc3_runtime_actor_runtime_state_snapshot {
  uint64_t isolation_thunk_call_count;
  uint64_t nonisolated_entry_call_count;
  uint64_t hop_to_executor_call_count;
  uint64_t replay_proof_call_count;
  uint64_t race_guard_call_count;
  uint64_t bind_executor_call_count;
  uint64_t mailbox_enqueue_call_count;
  uint64_t mailbox_drain_call_count;
  int last_isolation_executor_tag;
  int last_nonisolated_value;
  int last_nonisolated_executor_tag;
  int last_hop_value;
  int last_hop_executor_tag;
  int last_hop_result;
  int last_replay_proof_executor_tag;
  int last_race_guard_executor_tag;
  int last_bound_actor_handle;
  int last_bound_executor_tag;
  int last_mailbox_actor_handle;
  int last_mailbox_enqueued_value;
  int last_mailbox_executor_tag;
  int last_mailbox_depth;
  int last_mailbox_drained_value;
} objc3_runtime_actor_runtime_state_snapshot;

// M270-C002 actor lowering/runtime anchor: actor thunk, nonisolated entry,
// and executor-hop lowering remain private runtime helpers with a private
// testing snapshot rather than a public actor runtime ABI.
// M270-D001 actor-runtime/executor-binding anchor: the same private actor
// helper cluster plus `objc3_runtime_actor_runtime_state_snapshot` is now the
// canonical lane-D runtime contract for actor-state, mailbox-ownership, and
// executor-binding proof without widening the public runtime header.
// M270-D002 actor-mailbox/isolation-runtime anchor: live mailbox binding,
// enqueue, and drain helpers also remain inside that same private snapshot-
// backed runtime slice rather than claiming a public actor runtime ABI.
// M270-D003 cross-module isolation-metadata hardening anchor: imported modules
// now preserve the replay facts that describe this same private actor mailbox
// runtime slice across runtime-import surfaces and mixed-module link plans.

// M264-D002 conformance-claim operations anchor: the runtime/bootstrap layer
// still does not own profile selection, but the driver/toolchain now consume
// the emitted `module.objc3-conformance-report.json` plus the sibling
// `module.objc3-conformance-publication.json` through an explicit validation
// operation that publishes `module.objc3-conformance-validation.json`.
// Strictness and strict concurrency remain fail-closed and unclaimed until a
// later executable lane ships them end to end.

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
int objc3_runtime_copy_keypath_registry_state_for_testing(
    objc3_runtime_keypath_registry_state_snapshot *snapshot);
int objc3_runtime_copy_keypath_entry_for_testing(
    uint64_t stable_id, objc3_runtime_keypath_entry_snapshot *snapshot);
// M256-D001 class-realization-runtime anchor: the private method-cache
// snapshots remain the canonical proof surface for realized class/metaclass
// resolution, category attachment, and protocol-aware negative runtime checks
// without widening the public runtime header.
// M272-D001 runtime-fast-path-integration anchor: Part 9 explicitly reuses this
// same cache snapshot surface for direct-bypass/cache/fallback proof rather
// than inventing a second runtime query API.
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
// M257-D001 property-layout-runtime anchor: repeated alloc/new observations,
// synthesized accessor cache entries, and registration/selector snapshots also
// remain the canonical proof surface for the current single-instance
// property/layout runtime boundary.
// M257-D002 instance-allocation-layout-runtime anchor: the same private
// realized-graph snapshots now also publish live instance-allocation and
// runtime layout-consumption evidence for synthesized property access without
// widening the public runtime header.
// M257-D003 property-metadata-reflection anchor: property metadata
// registration state and per-property reflective helper lookups stay on this
// same private testing surface so diagnostics and probes can consume the live
// runtime-owned metadata without widening the public ABI.
int objc3_runtime_copy_realized_class_graph_state_for_testing(
    objc3_runtime_realized_class_graph_state_snapshot *snapshot);
int objc3_runtime_copy_realized_class_entry_for_testing(
    const char *class_name,
    objc3_runtime_realized_class_entry_snapshot *snapshot);
int objc3_runtime_copy_property_registry_state_for_testing(
    objc3_runtime_property_registry_state_snapshot *snapshot);
int objc3_runtime_copy_property_entry_for_testing(
    const char *class_name, const char *property_name,
    objc3_runtime_property_entry_snapshot *snapshot);
int objc3_runtime_copy_protocol_conformance_query_for_testing(
    const char *class_name, const char *protocol_name,
    objc3_runtime_protocol_conformance_query_snapshot *snapshot);
// M265-D001 optional/key-path runtime-helper freeze anchor: the current Part 3
// runtime boundary does not add a new public helper API. Optional sends remain
// routed through the public selector lookup/dispatch entrypoints, while the
// validated typed key-path slice currently exposes retained descriptor handles
// and sections as the runtime-facing input boundary. Full runtime key-path
// evaluation helpers remain deferred to M265-D002.
// M265-D002 live-optional-send-and-keypath-runtime-support anchor: the first
// live typed key-path runtime support stays on this private runtime header.
// The runtime consumes emitted key-path descriptor roots into an image-backed
// registry, publishes query/state snapshots for probes, and exposes narrow
// helper entrypoints for validated single-component handle execution without
// widening the stable public runtime header yet.
// M265-D003 cross-module type-surface preservation anchor: imported runtime
// surfaces must preserve the same typed key-path/runtime-helper packets so
// multi-image registration keeps provider metadata truthful without inventing a
// second key-path registry model.
int objc3_runtime_keypath_component_count_for_testing(int keypath_handle);
int objc3_runtime_keypath_root_is_self_for_testing(int keypath_handle);
// M260-C002 ownership runtime hook emission anchor: lowering-generated
// synthesized accessors target these private runtime helpers so retain/release,
// autorelease, and weak property paths execute against realized runtime-backed
// storage without widening the stable public runtime header yet.
// M260-D001 runtime-memory-management-api anchor: this private bootstrap
// internal header is the canonical home for lowered ownership helper
// entrypoints until later runtime work decides whether any part of the memory
// management surface should become public.
// M262-D001 runtime-arc-helper-api-surface anchor: the same private header now
// truthfully freezes the ARC helper ABI consumed by ARC lowering, including
// weak/current-property helpers plus private autoreleasepool push/pop hooks.
int objc3_runtime_read_current_property_i32(void);
void objc3_runtime_write_current_property_i32(int value);
int objc3_runtime_exchange_current_property_i32(int value);
// M262-D003 ownership-debug/runtime-validation anchor: private testing hooks
// may bind one live runtime property context at a time so probes can exercise
// the existing current-property helpers directly without widening the public
// runtime ABI.
int objc3_runtime_bind_current_property_context_for_testing(
    int receiver, const char *class_name, const char *property_name);
void objc3_runtime_clear_current_property_context_for_testing(void);
// M273-D001 expansion-host/runtime-boundary anchor: the same private property
// accessor/current-property helper slice now defines the truthful Part 10
// property-behavior runtime boundary while macro host execution and runtime
// package loading remain explicitly disabled.
typedef struct objc3_runtime_part10_expansion_host_boundary_snapshot {
  uint64_t property_runtime_ready;
  uint64_t macro_host_execution_ready;
  uint64_t macro_host_process_launch_ready;
  uint64_t runtime_package_loader_ready;
  uint64_t deterministic;
  const char *runtime_support_library_archive_relative_path;
  const char *property_behavior_runtime_model;
  const char *macro_expansion_host_model;
  const char *packaging_model;
  const char *fail_closed_model;
} objc3_runtime_part10_expansion_host_boundary_snapshot;
int objc3_runtime_copy_part10_expansion_host_boundary_snapshot_for_testing(
    objc3_runtime_part10_expansion_host_boundary_snapshot *snapshot);
typedef struct objc3_runtime_part10_macro_host_process_cache_integration_snapshot {
  uint64_t property_runtime_ready;
  uint64_t macro_host_execution_ready;
  uint64_t macro_host_process_launch_ready;
  uint64_t runtime_package_loader_ready;
  uint64_t deterministic;
  const char *host_executable_relative_path;
  const char *cache_root_relative_path;
  const char *host_model;
  const char *toolchain_model;
  const char *cache_model;
  const char *fail_closed_model;
} objc3_runtime_part10_macro_host_process_cache_integration_snapshot;
int objc3_runtime_copy_part10_macro_host_process_cache_integration_snapshot_for_testing(
    objc3_runtime_part10_macro_host_process_cache_integration_snapshot *snapshot);
// M262-D002 runtime ARC helper implementation anchor: these helpers are not
// just a frozen private ABI surface anymore; they are the live runtime-owned
// entrypoints that the supported ARC property/weak/autorelease-return slice
// links and executes through.
// M266-D001 cleanup-unwind integration anchor: the current runnable Part 5
// cleanup/unwind proof still reuses these same private autoreleasepool hooks
// plus the memory-management snapshot surface instead of widening the public
// runtime ABI with a standalone cleanup stack API.
// M271-D001 system-helper/runtime-contract anchor: Part 8 cleanup execution,
// resource invalidation proof, and retainable-family helper integration now
// freeze this same private ARC/autorelease helper cluster plus the paired
// memory-management and ARC-debug snapshots. Lane-D does not add a dedicated
// borrowed-pointer runtime helper or widen the public runtime header.
// M271-D002 live cleanup/runtime integration anchor: the supported Part 8
// fixture path now links and executes through this same private helper slice,
// with emitted cleanup calls and retainable-family stubs proving live helper
// traffic rather than a contract-only boundary.
int objc3_runtime_load_weak_current_property_i32(void);
void objc3_runtime_store_weak_current_property_i32(int value);
// M267-D001 error-runtime/bridge-helper anchor: the supported runnable Part 6
// slice now uses one narrow private helper ABI for thrown-error slot traffic,
// bridge normalization, and catch-kind matching without widening the public
// runtime header or claiming generalized foreign exception support.
// M267-D002 live catch/bridge/runtime integration anchor: linked native Part 6
// probes now execute through this same helper cluster and validate the helper
// traffic through the retained private snapshot surface below.
// M267-D003 cross-module preservation anchor: imported modules preserve this
// same Part 6 helper cluster indirectly through replay sidecars and cross-image
// link-plan validation rather than by widening the runtime helper ABI again.
void objc3_runtime_store_thrown_error_i32(int *slot, int value);
int objc3_runtime_load_thrown_error_i32(const int *slot);
int objc3_runtime_bridge_status_error_i32(int status_value,
                                          int mapped_error_value);
int objc3_runtime_bridge_nserror_error_i32(int error_value);
int objc3_runtime_catch_matches_error_i32(int error_value, int catch_kind,
                                          int catch_all);
// M268-D001 continuation/runtime-helper anchor: lane-D now freezes the first
// truthful private Part 7 helper ABI for logical continuation-handle
// allocation, scheduler handoff, and resume traffic. The current direct-call
// async slice still does not consume this helper cluster yet, but the helper
// runtime surface itself is now real and probeable without widening the public
// runtime header.
int objc3_runtime_allocate_async_continuation_i32(int resume_entry_tag,
                                                  int executor_tag);
int objc3_runtime_handoff_async_continuation_to_executor_i32(
    int continuation_handle, int executor_tag);
int objc3_runtime_resume_async_continuation_i32(int continuation_handle,
                                                int result_value);
// M269-C002 task-runtime lowering anchor: the IR emitter now rewrites the
// supported task/executor/cancellation symbol-profile family onto this private
// helper cluster so task creation, task-group operations, cancellation polls,
// and executor-handoff proof points become real runnable runtime traffic
// without widening the public runtime header.
// M269-D001 scheduler/executor runtime anchor: lane-D now freezes this same
// private helper cluster plus `objc3_runtime_copy_task_runtime_state_for_testing`
// as the canonical scheduler/executor/task-state runtime contract for the
// current supported Part 7 slice.
// M269-D002 live task runtime anchor: the same private helper cluster now also
// serves as the executable Part 7 runtime boundary for the supported task
// slice, with live probe coverage proving helper traffic and snapshot state.
int objc3_runtime_spawn_task_i32(int task_kind, int executor_tag);
int objc3_runtime_enter_task_group_scope_i32(int executor_tag);
int objc3_runtime_add_task_group_task_i32(int executor_tag);
int objc3_runtime_wait_task_group_next_i32(int executor_tag);
int objc3_runtime_cancel_task_group_i32(int executor_tag);
int objc3_runtime_task_is_cancelled_i32(int executor_tag);
int objc3_runtime_task_on_cancel_i32(int executor_tag);
int objc3_runtime_executor_hop_i32(int value, int executor_tag);
int objc3_runtime_actor_enter_isolation_thunk_i32(int executor_tag);
int objc3_runtime_actor_enter_nonisolated_i32(int value, int executor_tag);
int objc3_runtime_actor_hop_to_executor_i32(int value, int executor_tag);
int objc3_runtime_actor_record_replay_proof_i32(int executor_tag);
int objc3_runtime_actor_record_race_guard_i32(int executor_tag);
int objc3_runtime_actor_bind_executor_i32(int actor_handle, int executor_tag);
int objc3_runtime_actor_mailbox_enqueue_i32(int actor_handle, int value,
                                            int executor_tag);
int objc3_runtime_actor_mailbox_drain_next_i32(int actor_handle,
                                               int executor_tag);
int objc3_runtime_retain_i32(int value);
int objc3_runtime_release_i32(int value);
int objc3_runtime_autorelease_i32(int value);
// M261-D001 block-runtime API/object-layout freeze anchor: block
// promotion/invoke helpers are now explicitly frozen as private
// lowering/runtime entrypoints in this internal header; they are not public
// runtime ABI and later lane-D issues must preserve that boundary unless they
// deliberately widen it.
// M261-D002 block-runtime allocation/copy-dispose/invoke anchor: helper-backed
// promotion now supports pointer-capture block records with runtime-managed
// copy/dispose and invoke behavior, but the helper ABI remains private to this
// internal header.
// M261-D003 byref-forwarding/heap-promotion/ownership-interop anchor:
// escaping pointer-capture promotion now also rewrites capture slots onto
// runtime-owned forwarding cells before helper execution, while the helper ABI
// still remains private to this internal header.
int objc3_runtime_promote_block_i32(const void *storage,
                                    uint64_t storage_size_bytes,
                                    int has_pointer_capture_storage);
int objc3_runtime_invoke_block_i32(int block_handle, int a0, int a1, int a2,
                                   int a3);
// M260-D002 runtime-memory-management implementation anchor: autoreleasepool
// scopes, refcount draining, and weak zeroing remain private runtime/lowering
// mechanics until a later milestone makes a deliberate public-ABI decision.
// M266-D002 runtime cleanup/unwind implementation anchor: wider runnable
// cleanup execution still uses this same private helper cluster rather than
// widening a public cleanup/unwind ABI surface.
// M271-D001 system-helper/runtime-contract anchor: the current Part 8 runtime
// proof keeps cleanup/resource behavior on these same private autoreleasepool
// hooks and snapshot helpers rather than introducing a second resource-runtime
// stack or public helper surface.
void objc3_runtime_push_autoreleasepool_scope(void);
void objc3_runtime_pop_autoreleasepool_scope(void);
int objc3_runtime_copy_memory_management_state_for_testing(
    objc3_runtime_memory_management_state_snapshot *snapshot);
// M262-D003 ownership-debug/runtime-validation anchor: ARC ownership-debug
// counters and last-value/property context remain a private runtime-testing
// surface so lane-D can validate ARC helper traffic without widening the
// public runtime ABI.
int objc3_runtime_copy_arc_debug_state_for_testing(
    objc3_runtime_arc_debug_state_snapshot *snapshot);
// M267-D002 live catch/bridge/runtime integration anchor: lane-D continues to
// prove executable helper traffic without widening the public runtime ABI.
// M269-D002 live task runtime anchor: task-runtime snapshot publication stays
// private and is consumed by the linked runtime probe rather than a widened
// public scheduler ABI.
// M269-D003 hardening anchor: cancellation cleanup, autoreleasepool scopes,
// arc-debug counters, and explicit runtime resets are validated against the
// same private snapshot/testing surface rather than widening any public task
// scheduler ABI.
int objc3_runtime_copy_error_bridge_state_for_testing(
    objc3_runtime_error_bridge_state_snapshot *snapshot);
int objc3_runtime_copy_async_continuation_state_for_testing(
    objc3_runtime_async_continuation_state_snapshot *snapshot);
int objc3_runtime_copy_task_runtime_state_for_testing(
    objc3_runtime_task_runtime_state_snapshot *snapshot);
int objc3_runtime_copy_actor_runtime_state_for_testing(
    objc3_runtime_actor_runtime_state_snapshot *snapshot);

#ifdef __cplusplus
}
#endif
