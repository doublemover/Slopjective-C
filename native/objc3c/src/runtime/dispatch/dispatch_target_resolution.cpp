#include "runtime/dispatch/dispatch_target_resolution.h"

#include "runtime/classes/class_graph.h"
#include "runtime/dispatch/dispatch_api.h"
#include "runtime/dispatch/dispatch_family.h"
#include "runtime/dispatch/dispatch_result_state.h"
#include "runtime/dispatch/method_cache.h"
#include "runtime/dispatch/runtime_resolution_records.h"
#include "runtime/selectors/selector_table.h"
#include "runtime/state/runtime_state_records.h"

#include <utility>

namespace objc3c::runtime {

namespace {

void ResetLastDispatchStateUnlocked(
    RuntimeState &state, const char *selector,
    const objc3_runtime_selector_handle *selector_handle,
    objc3_runtime_dispatch_status_code initial_status) {
  state.last_dispatch_selector = selector != nullptr ? selector : "";
  state.last_dispatch_selector_stable_id =
      selector_handle != nullptr ? selector_handle->stable_id : 0;
  state.last_dispatch_normalized_receiver_identity = 0;
  state.last_category_probe_count = 0;
  state.last_protocol_probe_count = 0;
  state.last_dispatch_used_cache = false;
  state.last_dispatch_used_fast_path = false;
  state.last_dispatch_resolved_live_method = false;
  state.last_dispatch_strict_error = false;
  state.last_dispatch_effective_direct_dispatch = false;
  state.last_dispatch_used_builtin = false;
  state.last_dispatch_status_code = initial_status;
  state.last_fast_path_reason.clear();
  state.last_dispatch_path.clear();
  state.last_dispatch_implementation_kind.clear();
  state.last_dispatch_return_kind.clear();
  state.last_dispatch_diagnostic_code.clear();
  state.last_dispatch_diagnostic_message.clear();
  state.last_dispatch_result_contract.clear();
  state.last_dispatch_property_name.clear();
  state.last_resolved_class_name.clear();
  state.last_resolved_owner_identity.clear();
  state.last_dispatch_parameter_count = 0;
  state.last_dispatch_property_base_identity = 0;
  state.last_dispatch_property_slot_index = 0;
  StoreDispatchResultContractUnlocked(
      state, initial_status, RuntimeMethodReturnKind::Unsupported);
}

void PublishRuntimePropertyAccessorStateUnlocked(
    RuntimeState &state,
    const RealizedPropertyAccessor *runtime_property_accessor,
    std::uint64_t receiver_base_identity) {
  if (runtime_property_accessor == nullptr ||
      runtime_property_accessor->property_descriptor == nullptr) {
    return;
  }
  state.last_dispatch_property_name =
      runtime_property_accessor->property_descriptor->property_name != nullptr
          ? runtime_property_accessor->property_descriptor->property_name
          : "";
  state.last_dispatch_property_base_identity = receiver_base_identity;
  state.last_dispatch_property_slot_index =
      runtime_property_accessor->ivar_descriptor != nullptr
          ? runtime_property_accessor->ivar_descriptor->slot_index
          : runtime_property_accessor->property_descriptor
                ->ivar_layout_slot_index;
}

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

void PublishMethodCacheEntryStateUnlocked(
    RuntimeState &state, const MethodCacheEntry &entry,
    std::uint64_t receiver_base_identity) {
  state.last_dispatch_used_cache = true;
  state.last_dispatch_used_fast_path = entry.fast_path_seeded;
  state.last_dispatch_resolved_live_method = entry.resolved;
  state.last_dispatch_strict_error = !entry.resolved;
  state.last_dispatch_effective_direct_dispatch =
      entry.effective_direct_dispatch;
  state.last_category_probe_count = entry.category_probe_count;
  state.last_protocol_probe_count = entry.protocol_probe_count;
  state.last_fast_path_reason = entry.fast_path_reason;
  state.last_resolved_class_name = entry.class_name;
  state.last_resolved_owner_identity = entry.owner_identity;
  state.last_dispatch_parameter_count = entry.parameter_count;
  PublishRuntimePropertyAccessorStateUnlocked(
      state, entry.runtime_property_accessor, receiver_base_identity);
}

void PublishSlowPathResolutionStateUnlocked(
    RuntimeState &state, const SlowPathResolution &resolution,
    std::uint64_t receiver_base_identity) {
  state.last_dispatch_resolved_live_method = resolution.resolved;
  state.last_dispatch_strict_error = !resolution.resolved;
  state.last_dispatch_effective_direct_dispatch =
      resolution.effective_direct_dispatch;
  state.last_category_probe_count = resolution.category_probe_count;
  state.last_protocol_probe_count = resolution.protocol_probe_count;
  state.last_fast_path_reason = resolution.fast_path_reason;
  state.last_resolved_class_name = resolution.class_name;
  state.last_resolved_owner_identity = resolution.owner_identity;
  state.last_dispatch_parameter_count = resolution.parameter_count;
  PublishRuntimePropertyAccessorStateUnlocked(
      state, resolution.runtime_property_accessor, receiver_base_identity);
}

MethodCacheEntry BuildMethodCacheEntry(
    const SlowPathResolution &resolution,
    std::uint64_t normalized_receiver_identity,
    std::uint64_t selector_stable_id) {
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
  cache_entry.strict_error_status =
      RuntimeStrictDispatchStatus(resolution.resolved, resolution.ambiguous,
                                  resolution.strict_error_status);
  cache_entry.implementation = resolution.implementation;
  cache_entry.builtin_kind = resolution.builtin_kind;
  cache_entry.runtime_property_accessor =
      resolution.runtime_property_accessor;
  return cache_entry;
}

RuntimeDispatchTarget ResolveMethodCacheHitUnlocked(
    RuntimeState &state, const MethodCacheEntry &entry,
    std::uint64_t receiver_base_identity) {
  RuntimeDispatchTarget target;
  ++state.method_cache_hit_count;
  PublishMethodCacheEntryStateUnlocked(state, entry, receiver_base_identity);
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
  target.dispatch_status = entry.strict_error_status;
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
      resolution, normalized_receiver_identity, selector_handle.stable_id);
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

RuntimeDispatchTarget PublishStrictDispatchErrorUnlocked(
    RuntimeState &state, objc3_runtime_dispatch_status_code status_code,
    const char *dispatch_path) {
  RuntimeDispatchTarget target;
  target.dispatch_status = status_code;
  ++state.strict_dispatch_error_count;
  state.last_dispatch_strict_error = true;
  state.last_dispatch_resolved_live_method = false;
  state.last_dispatch_path = dispatch_path != nullptr ? dispatch_path : "";
  state.last_dispatch_implementation_kind = "strict-dispatch-error";
  StoreDispatchResultContractUnlocked(
      state, status_code, RuntimeMethodReturnKind::Unsupported);
  return target;
}

}  // namespace

RuntimeDispatchTarget ResolveRuntimeDispatchTargetUnlocked(
    RuntimeState &state, int receiver, const char *selector) {
  const objc3_runtime_selector_handle *selector_handle =
      LookupSelectorUnlocked(selector);
  ResetLastDispatchStateUnlocked(
      state, selector, selector_handle,
      OBJC3_RUNTIME_DISPATCH_STATUS_UNKNOWN_SELECTOR);

  if (receiver == 0) {
    return PublishStrictDispatchErrorUnlocked(
        state, OBJC3_RUNTIME_DISPATCH_STATUS_NIL_RECEIVER,
        "nil-receiver-error");
  }
  if (selector_handle == nullptr) {
    return PublishStrictDispatchErrorUnlocked(
        state, OBJC3_RUNTIME_DISPATCH_STATUS_UNKNOWN_SELECTOR,
        "unknown-selector-error");
  }

  std::uint64_t base_identity = 0;
  std::uint64_t normalized_receiver_identity = 0;
  DispatchFamily family = DispatchFamily::Invalid;
  if (!DecodeReceiverIdentity(state, receiver, base_identity, family,
                              normalized_receiver_identity)) {
    return PublishStrictDispatchErrorUnlocked(
        state, OBJC3_RUNTIME_DISPATCH_STATUS_UNKNOWN_RECEIVER_CLASS,
        "invalid-receiver-error");
  }

  state.last_dispatch_normalized_receiver_identity =
      normalized_receiver_identity;
  const MethodCacheKey cache_key{normalized_receiver_identity,
                                 selector_handle->stable_id};
  const auto cache_it = state.method_cache.find(cache_key);
  if (cache_it != state.method_cache.end()) {
    return ResolveMethodCacheHitUnlocked(state, cache_it->second,
                                         base_identity);
  }
  return ResolveMethodCacheMissUnlocked(
      state, base_identity, normalized_receiver_identity, family,
      *selector_handle, base_identity, cache_key);
}

}  // namespace objc3c::runtime
