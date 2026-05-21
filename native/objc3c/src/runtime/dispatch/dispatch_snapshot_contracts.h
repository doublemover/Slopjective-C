#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

enum objc3_runtime_method_cache_invalidation_reason {
  OBJC3_RUNTIME_METHOD_CACHE_INVALIDATION_NONE = 0,
  OBJC3_RUNTIME_METHOD_CACHE_INVALIDATION_STALE_GENERATION = 1,
  OBJC3_RUNTIME_METHOD_CACHE_INVALIDATION_RESET = 2,
  OBJC3_RUNTIME_METHOD_CACHE_INVALIDATION_PROPERTY_ACCESSOR_REBIND = 3
};

enum {
  OBJC3_RUNTIME_METHOD_CACHE_ABI_VERSION = 1
};

// runtime-fast-path-integration anchor: Part 9 freezes the
// existing private method-cache snapshot and entry-query helpers as the
// truthful runtime proof surface for mixed direct-call bypass, dynamic opt-out,
// and strict dispatch error behavior. D002 widens that same private
// runtime proof surface with one dedicated realized-dispatch snapshot rather
// than opening a second public dispatch ABI.
// live-dispatch-fast-path anchor: Part 9 now widens the same private
// proof surface with seeded-fast-path counters, seeded-entry flags, and the
// last fast-path reason so first-call cache-hit behavior remains observable
// without reopening the public runtime ABI.
typedef struct objc3_runtime_method_cache_state_snapshot {
  uint64_t cache_entry_count;
  uint64_t cache_hit_count;
  uint64_t cache_miss_count;
  uint64_t slow_path_lookup_count;
  uint64_t stale_method_cache_entry_count;
  uint64_t live_dispatch_count;
  uint64_t strict_dispatch_error_count;
  uint64_t fast_path_seed_count;
  uint64_t fast_path_hit_count;
  uint64_t class_graph_generation;
  uint64_t category_attachment_generation;
  uint64_t protocol_declaration_generation;
  uint64_t storage_surface_generation;
  uint64_t method_surface_generation;
  uint64_t last_selector_stable_id;
  uint64_t last_normalized_receiver_identity;
  uint64_t last_category_probe_count;
  uint64_t last_protocol_probe_count;
  int last_dispatch_used_cache;
  int last_dispatch_used_fast_path;
  int last_dispatch_resolved_live_method;
  int last_dispatch_strict_error;
  const char *last_selector;
  const char *last_fast_path_reason;
  const char *last_resolved_class_name;
  const char *last_resolved_owner_identity;
  uint32_t abi_version;
  uint64_t next_cache_entry_generation;
  int last_invalidation_reason;
} objc3_runtime_method_cache_state_snapshot;

typedef struct objc3_runtime_method_cache_entry_snapshot {
  int found;
  int resolved;
  int dispatch_family_is_class;
  uint64_t lookup_start_base_identity;
  uint64_t normalized_receiver_identity;
  uint64_t selector_stable_id;
  uint64_t parameter_count;
  uint64_t category_probe_count;
  uint64_t protocol_probe_count;
  uint64_t cache_class_graph_generation;
  uint64_t cache_category_attachment_generation;
  uint64_t cache_protocol_declaration_generation;
  uint64_t cache_storage_surface_generation;
  uint64_t cache_method_surface_generation;
  int fast_path_seeded;
  int effective_direct_dispatch;
  int objc_final_declared;
  int objc_sealed_declared;
  const char *selector;
  const char *fast_path_reason;
  const char *resolved_class_name;
  const char *resolved_owner_identity;
  uint32_t abi_version;
  uint64_t cache_entry_generation;
  int miss_status;
} objc3_runtime_method_cache_entry_snapshot;

// realized-dispatch-runtime anchor: lane-D now widens the same
// private runtime testing surface with one dispatch-state snapshot so
// executable probes can read the authoritative realized dispatch path and
// implementation kind from the live runtime rather than inferring them from
// cache counters alone.
typedef struct objc3_runtime_dispatch_state_snapshot {
  uint64_t cache_entry_count;
  uint64_t fast_path_seed_count;
  uint64_t fast_path_hit_count;
  uint64_t live_dispatch_count;
  uint64_t strict_dispatch_error_count;
  uint64_t last_selector_stable_id;
  uint64_t last_normalized_receiver_identity;
  uint64_t last_resolved_parameter_count;
  uint64_t last_property_base_identity;
  uint64_t last_property_slot_index;
  int last_dispatch_used_cache;
  int last_dispatch_used_fast_path;
  int last_dispatch_resolved_live_method;
  int last_dispatch_strict_error;
  int last_effective_direct_dispatch;
  int last_used_builtin;
  int last_dispatch_status_code;
  const char *last_selector;
  const char *last_fast_path_reason;
  const char *last_dispatch_path;
  const char *last_implementation_kind;
  const char *last_return_kind;
  const char *last_diagnostic_code;
  const char *last_diagnostic_message;
  const char *last_result_contract;
  const char *last_property_name;
  const char *last_resolved_class_name;
  const char *last_resolved_owner_identity;
} objc3_runtime_dispatch_state_snapshot;

// class-realization-runtime anchor: the private method-cache
// snapshots remain the canonical proof surface for realized class/metaclass
// resolution, category attachment, and protocol-aware negative runtime checks
// without widening the public runtime header.
// runtime-fast-path-integration anchor: Part 9 explicitly reuses this
// same cache snapshot surface for direct-bypass/cache/strict-error proof rather
// than inventing a second runtime query API.
int objc3_runtime_copy_method_cache_state_for_testing(
    objc3_runtime_method_cache_state_snapshot *snapshot);
int objc3_runtime_copy_method_cache_entry_for_testing(
    int receiver, const char *selector,
    objc3_runtime_method_cache_entry_snapshot *snapshot);
int objc3_runtime_copy_dispatch_state_for_testing(
    objc3_runtime_dispatch_state_snapshot *snapshot);

#ifdef __cplusplus
}
#endif
