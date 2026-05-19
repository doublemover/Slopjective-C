#include "runtime/dispatch/method_cache_resolution.h"

#include "runtime/dispatch/dispatch_resolution_state.h"
#include "runtime/dispatch/dispatch_result_state.h"
#include "runtime/dispatch/dispatch_status.h"
#include "runtime/dispatch/runtime_resolution_records.h"
#include "runtime/dispatch/runtime_method_return.h"
#include "runtime/state/runtime_cache_invalidation.h"
#include "runtime/state/runtime_state_records.h"

#include <utility>

namespace objc3c::runtime {
namespace {

RuntimeDispatchTarget BuildResolvedDispatchTarget(
    std::uint64_t receiver_base_identity, const MethodCacheEntry &entry) {
  RuntimeDispatchTarget target;
  target.implementation = entry.implementation;
  target.runtime_property_accessor = entry.runtime_property_accessor;
  target.parameter_count = entry.parameter_count;
  target.return_kind = entry.return_kind;
  target.builtin_kind = entry.builtin_kind;
  target.resolved_live_method = true;
  target.receiver_base_identity = receiver_base_identity;
  target.dispatch_status = OBJC3_RUNTIME_DISPATCH_STATUS_OK;
  return target;
}

RuntimeDispatchTarget BuildResolvedDispatchTarget(
    std::uint64_t receiver_base_identity, const SlowPathResolution &resolution) {
  RuntimeDispatchTarget target;
  target.implementation = resolution.implementation;
  target.runtime_property_accessor = resolution.runtime_property_accessor;
  target.parameter_count = resolution.parameter_count;
  target.return_kind = resolution.return_kind;
  target.builtin_kind = resolution.builtin_kind;
  target.resolved_live_method = true;
  target.receiver_base_identity = receiver_base_identity;
  target.dispatch_status = OBJC3_RUNTIME_DISPATCH_STATUS_OK;
  return target;
}

MethodCacheEntry BuildMethodCacheEntry(
    const SlowPathResolution &resolution,
    std::uint64_t normalized_receiver_identity,
    std::uint64_t selector_stable_id,
    const RuntimeState &state) {
  MethodCacheEntry cache_entry;
  cache_entry.resolved = resolution.resolved;
  cache_entry.dispatch_family_is_class = resolution.dispatch_family_is_class;
  cache_entry.effective_direct_dispatch =
      resolution.effective_direct_dispatch;
  cache_entry.objc_final_declared = resolution.objc_final_declared;
  cache_entry.objc_sealed_declared = resolution.objc_sealed_declared;
  cache_entry.selector_storage = resolution.selector_storage;
  cache_entry.fast_path_reason = resolution.fast_path_reason;
  cache_entry.class_name = resolution.class_name;
  cache_entry.owner_identity = resolution.owner_identity;
  cache_entry.normalized_receiver_identity = normalized_receiver_identity;
  cache_entry.selector_stable_id = selector_stable_id;
  cache_entry.parameter_count = resolution.parameter_count;
  cache_entry.return_kind = resolution.return_kind;
  cache_entry.category_probe_count = resolution.category_probe_count;
  cache_entry.protocol_probe_count = resolution.protocol_probe_count;
  cache_entry.cache_registered_image_count = state.registered_image_count;
  cache_entry.cache_last_successful_registration_order_ordinal =
      state.last_successful_registration_order_ordinal;
  cache_entry.cache_reset_generation = state.reset_generation;
  cache_entry.cache_replay_generation = state.replay_generation;
  cache_entry.cache_realized_class_node_count =
      static_cast<std::uint64_t>(state.realized_class_nodes.size());
  StampMethodCacheMutationGenerationsUnlocked(cache_entry, state);
  cache_entry.strict_error_status =
      RuntimeStrictDispatchStatus(resolution.resolved, resolution.ambiguous,
                                  resolution.strict_error_status);
  cache_entry.implementation = resolution.implementation;
  cache_entry.builtin_kind = resolution.builtin_kind;
  cache_entry.runtime_property_accessor =
      resolution.runtime_property_accessor;
  return cache_entry;
}

objc3_runtime_dispatch_status_code ValidateMethodCacheEntryForDispatch(
    const RuntimeState &state, const MethodCacheEntry &entry,
    std::uint64_t expected_normalized_receiver_identity,
    std::uint64_t expected_selector_stable_id) {
  if (entry.normalized_receiver_identity !=
          expected_normalized_receiver_identity ||
      entry.selector_stable_id != expected_selector_stable_id ||
      entry.cache_registered_image_count != state.registered_image_count ||
      entry.cache_last_successful_registration_order_ordinal !=
          state.last_successful_registration_order_ordinal ||
      entry.cache_reset_generation != state.reset_generation ||
      entry.cache_replay_generation != state.replay_generation ||
      entry.cache_realized_class_node_count !=
          static_cast<std::uint64_t>(state.realized_class_nodes.size()) ||
      !MethodCacheMutationGenerationsMatchUnlocked(state, entry)) {
    return OBJC3_RUNTIME_DISPATCH_STATUS_STALE_METHOD_CACHE;
  }
  if (!entry.resolved) {
    return entry.strict_error_status;
  }
  if (!RuntimeMethodReturnKindIsDispatchResultSupported(entry.return_kind)) {
    return OBJC3_RUNTIME_DISPATCH_STATUS_UNSUPPORTED_RETURN_TYPE;
  }
  if (entry.parameter_count > 4) {
    return OBJC3_RUNTIME_DISPATCH_STATUS_UNSUPPORTED_ARGUMENT_LAYOUT;
  }
  if (entry.implementation == nullptr &&
      entry.builtin_kind == RuntimeBuiltinKind::None &&
      entry.runtime_property_accessor == nullptr) {
    return OBJC3_RUNTIME_DISPATCH_STATUS_MALFORMED_METADATA;
  }
  return OBJC3_RUNTIME_DISPATCH_STATUS_OK;
}

}  // namespace

RuntimeDispatchTarget ResolveMethodCacheHitUnlocked(
    RuntimeState &state, const MethodCacheKey &cache_key,
    const MethodCacheEntry &entry, std::uint64_t receiver_base_identity,
    std::uint64_t expected_normalized_receiver_identity,
    std::uint64_t expected_selector_stable_id) {
  RuntimeDispatchTarget target;
  ++state.method_cache_hit_count;
  PublishMethodCacheEntryStateUnlocked(state, entry, receiver_base_identity);
  const objc3_runtime_dispatch_status_code cache_status =
      ValidateMethodCacheEntryForDispatch(
          state, entry, expected_normalized_receiver_identity,
          expected_selector_stable_id);
  if (cache_status == OBJC3_RUNTIME_DISPATCH_STATUS_STALE_METHOD_CACHE) {
    state.method_cache.erase(cache_key);
    state.last_dispatch_resolved_live_method = false;
    state.last_dispatch_strict_error = true;
    state.last_dispatch_path = "cache-hit-stale";
    state.last_dispatch_implementation_kind = "strict-dispatch-error";
    target.dispatch_status = cache_status;
    StoreDispatchResultContractUnlocked(
        state, target.dispatch_status, RuntimeMethodReturnKind::Unsupported);
    ++state.strict_dispatch_error_count;
    return target;
  }
  if (cache_status != OBJC3_RUNTIME_DISPATCH_STATUS_OK && entry.resolved) {
    state.last_dispatch_resolved_live_method = false;
    state.last_dispatch_strict_error = true;
    state.last_dispatch_path = "cache-hit-invalid-abi";
    state.last_dispatch_implementation_kind = "strict-dispatch-error";
    target.dispatch_status = cache_status;
    StoreDispatchResultContractUnlocked(
        state, target.dispatch_status, entry.return_kind);
    ++state.strict_dispatch_error_count;
    return target;
  }
  if (entry.resolved) {
    state.last_dispatch_used_builtin =
        entry.builtin_kind != RuntimeBuiltinKind::None;
    state.last_dispatch_path =
        entry.fast_path_seeded ? "cache-hit-fast-path" : "cache-hit-live";
    state.last_dispatch_implementation_kind =
        DescribeResolvedImplementationKind(entry.builtin_kind,
                                           entry.implementation);
    if (entry.fast_path_seeded) {
      ++state.fast_path_hit_count;
    }
    return BuildResolvedDispatchTarget(receiver_base_identity, entry);
  }

  state.last_dispatch_path = "cache-hit-error";
  state.last_dispatch_implementation_kind = "strict-dispatch-error";
  target.dispatch_status = cache_status;
  StoreDispatchResultContractUnlocked(
      state, target.dispatch_status, RuntimeMethodReturnKind::Unsupported);
  ++state.strict_dispatch_error_count;
  return target;
}

RuntimeDispatchTarget ResolveMethodCacheMissUnlocked(
    RuntimeState &state, std::uint64_t base_identity,
    std::uint64_t normalized_receiver_identity, DispatchFamily family,
    const objc3_runtime_selector_handle &selector_handle,
    std::uint64_t receiver_base_identity, const MethodCacheKey &cache_key) {
  RuntimeDispatchTarget target;
  ++state.method_cache_miss_count;
  ++state.slow_path_lookup_count;
  SlowPathResolution resolution = ResolveMethodSlowPathUnlocked(
      state, base_identity, normalized_receiver_identity, family,
      selector_handle.stable_id, selector_handle.selector);
  MethodCacheEntry cache_entry = BuildMethodCacheEntry(
      resolution, normalized_receiver_identity, selector_handle.stable_id,
      state);
  const objc3_runtime_dispatch_status_code strict_error_status =
      cache_entry.strict_error_status;
  state.method_cache.emplace(cache_key, std::move(cache_entry));
  PublishSlowPathResolutionStateUnlocked(state, resolution,
                                         receiver_base_identity);
  if (resolution.resolved) {
    state.last_dispatch_used_builtin =
        resolution.builtin_kind != RuntimeBuiltinKind::None;
    state.last_dispatch_path = "slow-path-live";
    state.last_dispatch_implementation_kind =
        DescribeResolvedImplementationKind(resolution.builtin_kind,
                                           resolution.implementation);
    return BuildResolvedDispatchTarget(receiver_base_identity, resolution);
  }

  state.last_dispatch_path = "slow-path-error";
  state.last_dispatch_implementation_kind = "strict-dispatch-error";
  target.dispatch_status = strict_error_status;
  StoreDispatchResultContractUnlocked(
      state, target.dispatch_status, RuntimeMethodReturnKind::Unsupported);
  ++state.strict_dispatch_error_count;
  return target;
}

}  // namespace objc3c::runtime
