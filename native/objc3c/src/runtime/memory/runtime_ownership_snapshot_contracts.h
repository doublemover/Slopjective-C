#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

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

typedef struct objc3_runtime_block_arc_runtime_abi_snapshot {
  uint64_t private_runtime_abi_ready;
  uint64_t public_runtime_header_unchanged;
  uint64_t deterministic;
  uint64_t live_runtime_block_handle_count;
  uint64_t block_promote_call_count;
  uint64_t block_invoke_call_count;
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
  int last_promoted_block_handle;
  int last_promote_has_pointer_capture_storage;
  int last_invoked_block_handle;
  int last_block_invoke_result;
  int last_retain_value;
  int last_release_value;
  int last_autorelease_value;
  const char *block_promote_symbol;
  const char *block_invoke_symbol;
  const char *retain_symbol;
  const char *release_symbol;
  const char *autorelease_symbol;
  const char *autoreleasepool_push_symbol;
  const char *autoreleasepool_pop_symbol;
  const char *current_property_read_symbol;
  const char *current_property_write_symbol;
  const char *current_property_exchange_symbol;
  const char *bind_current_property_context_symbol;
  const char *clear_current_property_context_symbol;
  const char *weak_current_property_load_symbol;
  const char *weak_current_property_store_symbol;
  const char *arc_debug_state_snapshot_symbol;
  const char *runtime_abi_boundary_model;
  const char *block_runtime_model;
  const char *arc_runtime_model;
  const char *fail_closed_model;
} objc3_runtime_block_arc_runtime_abi_snapshot;

// ownership runtime hook emission anchor: lowering-generated
// synthesized accessors target these private runtime helpers so retain/release,
// autorelease, and weak property paths execute against realized runtime-backed
// storage without widening the stable public runtime header yet.
// runtime-memory-management-api anchor: this private bootstrap
// internal header is the canonical home for lowered ownership helper
// entrypoints until later runtime work decides whether any part of the memory
// management surface should become public.
// runtime-arc-helper-api-surface anchor: the same private header now
// truthfully freezes the ARC helper ABI consumed by ARC lowering, including
// weak/current-property helpers plus private autoreleasepool push/pop hooks.
int objc3_runtime_read_current_property_i32(void);
void objc3_runtime_write_current_property_i32(int value);
int objc3_runtime_exchange_current_property_i32(int value);
// ownership-debug/runtime-validation anchor: private testing hooks
// may bind one live runtime property context at a time so probes can exercise
// the existing current-property helpers directly without widening the public
// runtime ABI.
int objc3_runtime_bind_current_property_context_for_testing(
    int receiver, const char *class_name, const char *property_name);
void objc3_runtime_clear_current_property_context_for_testing(void);

// runtime ARC helper implementation anchor: these helpers are not
// just a frozen private ABI surface anymore; they are the live runtime-owned
// entrypoints that the supported ARC property/weak/autorelease-return slice
// links and executes through.
// cleanup-unwind integration anchor: the current runnable Part 5
// cleanup/unwind proof still reuses these same private autoreleasepool hooks
// plus the memory-management snapshot surface instead of widening the public
// runtime ABI with a standalone cleanup stack API.
// system-helper/runtime-contract anchor: Part 8 cleanup execution,
// resource invalidation proof, and retainable-family helper integration now
// freeze this same private ARC/autorelease helper cluster plus the paired
// memory-management and ARC-debug snapshots. Runtime integration does not add a dedicated
// borrowed-pointer runtime helper or widen the public runtime header.
// live cleanup/runtime integration anchor: the supported Part 8
// fixture path now links and executes through this same private helper slice,
// with emitted cleanup calls and retainable-family stubs proving live helper
// traffic rather than a contract-only boundary.
int objc3_runtime_load_weak_current_property_i32(void);
void objc3_runtime_store_weak_current_property_i32(int value);
int objc3_runtime_retain_i32(int value);
int objc3_runtime_release_i32(int value);
int objc3_runtime_autorelease_i32(int value);
// block-runtime API/object-layout freeze anchor: block
// promotion/invoke helpers are now explicitly frozen as private
// lowering/runtime entrypoints in this internal header; they are not public
// runtime ABI and later lane-D issues must preserve that boundary unless they
// deliberately widen it.
// block-runtime allocation/copy-dispose/invoke anchor: helper-backed
// promotion now supports pointer-capture block records with runtime-managed
// copy/dispose and invoke behavior, but the helper ABI remains private to this
// internal header.
// byref-forwarding/heap-promotion/ownership-interop anchor:
// escaping pointer-capture promotion now also rewrites capture slots onto
// runtime-owned forwarding cells before helper execution, while the helper ABI
// still remains private to this internal header.
int objc3_runtime_promote_block_i32(const void *storage,
                                    uint64_t storage_size_bytes,
                                    int has_pointer_capture_storage);
int objc3_runtime_invoke_block_i32(int block_handle, int a0, int a1, int a2,
                                   int a3);
// runtime-memory-management implementation anchor: autoreleasepool
// scopes, refcount draining, and weak zeroing remain private runtime/lowering
// mechanics until a later milestone makes a deliberate public-ABI decision.
// runtime cleanup/unwind implementation anchor: wider runnable
// cleanup execution still uses this same private helper cluster rather than
// widening a public cleanup/unwind ABI surface.
// system-helper/runtime-contract anchor: the current Part 8 runtime
// proof keeps cleanup/resource behavior on these same private autoreleasepool
// hooks and snapshot helpers rather than introducing a second resource-runtime
// stack or public helper surface.
void objc3_runtime_push_autoreleasepool_scope(void);
void objc3_runtime_pop_autoreleasepool_scope(void);
int objc3_runtime_copy_memory_management_state_for_testing(
    objc3_runtime_memory_management_state_snapshot *snapshot);
// ownership-debug/runtime-validation anchor: ARC ownership-debug
// counters and last-value/property context remain a private runtime-testing
// surface so lane-D can validate ARC helper traffic without widening the
// public runtime ABI.
int objc3_runtime_copy_arc_debug_state_for_testing(
    objc3_runtime_arc_debug_state_snapshot *snapshot);
// block-arc-runtime-abi anchor: the supported block promotion/invoke
// entrypoints plus ARC helper cluster now publish one authoritative private
// ABI/testing snapshot rather than relying on probe-local symbol inventories.
int objc3_runtime_copy_block_arc_runtime_abi_snapshot_for_testing(
    objc3_runtime_block_arc_runtime_abi_snapshot *snapshot);

#ifdef __cplusplus
}
#endif
