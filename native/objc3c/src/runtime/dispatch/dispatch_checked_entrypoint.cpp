#include "runtime/dispatch/dispatch_checked_entrypoint.h"

#include "runtime/dispatch/dispatch_resolution_state.h"
#include "runtime/dispatch/dispatch_result_state.h"
#include "runtime/dispatch/dispatch_target_resolution.h"
#include "runtime/dispatch/runtime_method_return.h"
#include "runtime/dispatch/strict_dispatch_execution.h"
#include "runtime/dispatch/typed_dispatch_result.h"
#include "runtime/public/objc3_runtime_result_materialization_contract.h"
#include "runtime/selectors/selector_table.h"
#include "runtime/state/runtime_state_records.h"
#include "runtime/state/runtime_state_store.h"

#include <cstdint>
#include <mutex>

namespace objc3c::runtime {
namespace {

objc3_runtime_dispatch_typed_result ExecuteRuntimeDispatchTargetChecked(
    int receiver, const RuntimeDispatchTarget &dispatch_target, int a0, int a1,
    int a2, int a3) {
  RuntimeState &state = ProcessRuntimeState();
  if (dispatch_target.resolved_live_method) {
    const RuntimeTypedDispatchResult result =
        ExecuteResolvedRuntimeDispatchTargetStrict(
            state, receiver, dispatch_target, a0, a1, a2, a3);
    return MakeRuntimeDispatchTypedResult(
        result.status_code, result.value,
        RuntimeMethodReturnKindDispatchAbiCode(result.return_kind));
  }
  {
    std::lock_guard<std::mutex> lock(state.mutex);
    StoreDispatchResultContractUnlocked(
        state, dispatch_target.dispatch_status,
        RuntimeMethodReturnKind::Unsupported);
  }
  return MakeRuntimeDispatchTypedResult(
      dispatch_target.dispatch_status, 0,
      OBJC3_RUNTIME_DISPATCH_RETURN_KIND_UNSUPPORTED);
}

bool CacheAwareDescriptorRequires(
    const objc3_runtime_cache_aware_dispatch_descriptor &descriptor,
    std::uint32_t flag) {
  return (descriptor.flags & flag) != 0;
}

bool CacheAwareGenerationsMatchUnlocked(
    const RuntimeState &state,
    const objc3_runtime_cache_aware_dispatch_descriptor &descriptor) {
  return descriptor.class_graph_generation == state.class_graph_generation &&
         descriptor.category_attachment_generation ==
             state.category_attachment_generation &&
         descriptor.protocol_declaration_generation ==
             state.protocol_declaration_generation &&
         descriptor.storage_surface_generation ==
             state.storage_surface_generation &&
         descriptor.method_surface_generation == state.method_surface_generation;
}

std::uintptr_t HashCacheAwareIdentityPart(std::uintptr_t seed,
                                          const std::string &value) {
  constexpr std::uintptr_t kPrime = 1099511628211ull;
  for (const unsigned char byte : value) {
    seed ^= static_cast<std::uintptr_t>(byte);
    seed *= kPrime;
  }
  return seed;
}

std::uintptr_t CacheAwareMethodTargetIdentityUnlocked(
    const RuntimeState &state) {
  constexpr std::uintptr_t kOffset = 1469598103934665603ull;
  std::uintptr_t identity = kOffset;
  identity =
      HashCacheAwareIdentityPart(identity, state.last_resolved_owner_identity);
  identity = HashCacheAwareIdentityPart(identity, state.last_dispatch_selector);
  identity = HashCacheAwareIdentityPart(
      identity, state.last_dispatch_implementation_kind);
  if (identity != kOffset) {
    return identity;
  }
  return state.last_dispatch_resolved_live_method ? kOffset : 0u;
}

std::uint64_t CacheAwareCacheEntryGenerationUnlocked(
    const RuntimeState &state) {
  if (!state.last_dispatch_used_cache) {
    return 0;
  }
  for (const auto &entry_pair : state.method_cache) {
    const MethodCacheEntry &entry = entry_pair.second;
    if (entry.selector_stable_id == state.last_dispatch_selector_stable_id &&
        entry.normalized_receiver_identity ==
            state.last_dispatch_normalized_receiver_identity &&
        entry.resolved == state.last_dispatch_resolved_live_method) {
      return entry.cache_entry_generation;
    }
  }
  return 0;
}

void RecordCacheAwareDispatchUnlocked(
    RuntimeState &state,
    const objc3_runtime_cache_aware_dispatch_descriptor *descriptor,
    bool descriptor_valid,
    bool fallback_used,
    int invalidation_reason,
    objc3_runtime_dispatch_status_code status_code) {
  state.last_cache_aware_descriptor_valid = descriptor_valid;
  state.last_cache_aware_fallback_used = fallback_used;
  state.last_cache_aware_descriptor_flags =
      descriptor != nullptr ? descriptor->flags : 0;
  state.last_cache_aware_invalidation_reason = invalidation_reason;
  state.last_cache_aware_cache_entry_generation =
      CacheAwareCacheEntryGenerationUnlocked(state);
  state.last_cache_aware_method_target_identity =
      CacheAwareMethodTargetIdentityUnlocked(state);
  state.last_cache_aware_source_path =
      descriptor != nullptr && descriptor->source_path != nullptr
          ? descriptor->source_path
          : "";
  state.last_cache_aware_source_line =
      descriptor != nullptr ? descriptor->source_line : 0;
  state.last_cache_aware_source_column =
      descriptor != nullptr ? descriptor->source_column : 0;
}

objc3_runtime_dispatch_i32_result CacheAwareMalformedDispatchResult(
    RuntimeState &state,
    const objc3_runtime_cache_aware_dispatch_descriptor *descriptor) {
  std::lock_guard<std::mutex> lock(state.mutex);
  ResetRuntimeDispatchStateUnlocked(
      state, descriptor != nullptr ? descriptor->selector : nullptr, nullptr,
      OBJC3_RUNTIME_DISPATCH_STATUS_MALFORMED_METADATA);
  state.last_dispatch_path = "cache-aware-descriptor-error";
  state.last_dispatch_implementation_kind = "strict-dispatch-error";
  RecordCacheAwareDispatchUnlocked(
      state, descriptor, false, false,
      OBJC3_RUNTIME_METHOD_CACHE_INVALIDATION_NONE,
      OBJC3_RUNTIME_DISPATCH_STATUS_MALFORMED_METADATA);
  return MakeRuntimeDispatchI32Result(
      OBJC3_RUNTIME_DISPATCH_STATUS_MALFORMED_METADATA, 0);
}

}  // namespace

objc3_runtime_dispatch_i32_result ExecuteRuntimeDispatchI32Checked(
    int receiver, const char *selector, int a0, int a1, int a2, int a3) {
  return MakeRuntimeDispatchI32ResultFromTypedResult(
      ExecuteRuntimeDispatchTypedChecked(receiver, selector, a0, a1, a2, a3));
}

objc3_runtime_dispatch_typed_result ExecuteRuntimeDispatchTypedChecked(
    int receiver, const char *selector, int a0, int a1, int a2, int a3) {
  RuntimeState &state = ProcessRuntimeState();
  RuntimeDispatchTarget dispatch_target;
  {
    std::lock_guard<std::mutex> lock(state.mutex);
    dispatch_target =
        ResolveRuntimeDispatchTargetUnlocked(state, receiver, selector);
  }
  return ExecuteRuntimeDispatchTargetChecked(
      receiver, dispatch_target, a0, a1, a2, a3);
}

objc3_runtime_dispatch_i32_result ExecuteRuntimeDispatchI32FromClassChecked(
    int receiver, const char *lookup_start_class_name, const char *selector,
    int a0, int a1, int a2, int a3) {
  return MakeRuntimeDispatchI32ResultFromTypedResult(
      ExecuteRuntimeDispatchTypedFromClassChecked(
          receiver, lookup_start_class_name, selector, a0, a1, a2, a3));
}

objc3_runtime_dispatch_typed_result
ExecuteRuntimeDispatchTypedFromClassChecked(
    int receiver, const char *lookup_start_class_name, const char *selector,
    int a0, int a1, int a2, int a3) {
  RuntimeState &state = ProcessRuntimeState();
  RuntimeDispatchTarget dispatch_target;
  {
    std::lock_guard<std::mutex> lock(state.mutex);
    dispatch_target = ResolveRuntimeDispatchTargetFromClassUnlocked(
        state, receiver, lookup_start_class_name, selector);
  }
  return ExecuteRuntimeDispatchTargetChecked(
      receiver, dispatch_target, a0, a1, a2, a3);
}

objc3_runtime_dispatch_i32_result ExecuteRuntimeCacheAwareDispatchI32Checked(
    int receiver,
    const objc3_runtime_cache_aware_dispatch_descriptor *descriptor,
    int a0, int a1, int a2, int a3) {
  RuntimeState &state = ProcessRuntimeState();
  if (descriptor == nullptr ||
      descriptor->abi_version !=
          OBJC3_RUNTIME_CACHE_AWARE_DISPATCH_ABI_VERSION ||
      descriptor->selector == nullptr || descriptor->selector[0] == '\0') {
    return CacheAwareMalformedDispatchResult(state, descriptor);
  }

  bool descriptor_valid = true;
  bool fallback_used = false;
  int invalidation_reason = OBJC3_RUNTIME_METHOD_CACHE_INVALIDATION_NONE;
  {
    std::lock_guard<std::mutex> lock(state.mutex);
    const objc3_runtime_selector_handle *selector_handle =
        LookupSelectorUnlocked(descriptor->selector);
    if (CacheAwareDescriptorRequires(
            *descriptor,
            OBJC3_RUNTIME_CACHE_AWARE_DISPATCH_REQUIRE_SELECTOR_STABLE_ID) &&
        (selector_handle == nullptr ||
         descriptor->selector_stable_id != selector_handle->stable_id)) {
      descriptor_valid = false;
      fallback_used = true;
      invalidation_reason =
          OBJC3_RUNTIME_METHOD_CACHE_INVALIDATION_STALE_GENERATION;
    }
    if (descriptor_valid &&
        CacheAwareDescriptorRequires(
            *descriptor,
            OBJC3_RUNTIME_CACHE_AWARE_DISPATCH_REQUIRE_GENERATIONS) &&
        !CacheAwareGenerationsMatchUnlocked(state, *descriptor)) {
      descriptor_valid = false;
      fallback_used = true;
      invalidation_reason =
          OBJC3_RUNTIME_METHOD_CACHE_INVALIDATION_STALE_GENERATION;
    }
  }

  objc3_runtime_dispatch_i32_result result =
      ExecuteRuntimeDispatchI32Checked(receiver, descriptor->selector, a0, a1,
                                       a2, a3);
  {
    std::lock_guard<std::mutex> lock(state.mutex);
    RecordCacheAwareDispatchUnlocked(
        state, descriptor, descriptor_valid, fallback_used,
        invalidation_reason, result.status_code);
  }
  return result;
}

}  // namespace objc3c::runtime
