#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

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
// memory-management and ARC-debug snapshots. Runtime integration does not add a
// dedicated borrowed-pointer runtime helper or widen the public runtime header.
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

#ifdef __cplusplus
}
#endif
