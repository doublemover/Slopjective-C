#include "runtime/dispatch/dispatch_target_resolution.h"

#include "runtime/classes/receiver_identity.h"
#include "runtime/dispatch/dispatch_family.h"
#include "runtime/dispatch/dispatch_resolution_state.h"
#include "runtime/dispatch/method_cache.h"
#include "runtime/dispatch/method_cache_resolution.h"
#include "runtime/selectors/selector_table.h"
#include "runtime/state/runtime_state_records.h"

namespace objc3c::runtime {

RuntimeDispatchTarget ResolveRuntimeDispatchTargetUnlocked(
    RuntimeState &state, int receiver, const char *selector) {
  const objc3_runtime_selector_handle *selector_handle =
      LookupSelectorUnlocked(selector);
  ResetRuntimeDispatchStateUnlocked(
      state, selector, selector_handle,
      OBJC3_RUNTIME_DISPATCH_STATUS_UNKNOWN_SELECTOR);

  if (receiver == 0) {
    return PublishStrictDispatchErrorTargetUnlocked(
        state, OBJC3_RUNTIME_DISPATCH_STATUS_NIL_RECEIVER,
        "nil-receiver-error");
  }
  if (selector_handle == nullptr) {
    return PublishStrictDispatchErrorTargetUnlocked(
        state, OBJC3_RUNTIME_DISPATCH_STATUS_UNKNOWN_SELECTOR,
        "unknown-selector-error");
  }

  std::uint64_t base_identity = 0;
  std::uint64_t normalized_receiver_identity = 0;
  DispatchFamily family = DispatchFamily::Invalid;
  if (!DecodeReceiverIdentity(state, receiver, base_identity, family,
                              normalized_receiver_identity)) {
    return PublishStrictDispatchErrorTargetUnlocked(
        state, OBJC3_RUNTIME_DISPATCH_STATUS_UNKNOWN_RECEIVER_CLASS,
        "invalid-receiver-error");
  }

  state.last_dispatch_normalized_receiver_identity =
      normalized_receiver_identity;
  const MethodCacheKey cache_key{normalized_receiver_identity,
                                 selector_handle->stable_id};
  const auto cache_it = state.method_cache.find(cache_key);
  if (cache_it != state.method_cache.end()) {
    return ResolveMethodCacheHitUnlocked(
        state, cache_key, cache_it->second, base_identity,
        normalized_receiver_identity, selector_handle->stable_id);
  }
  return ResolveMethodCacheMissUnlocked(
      state, base_identity, normalized_receiver_identity, family,
      *selector_handle, base_identity, cache_key);
}

}  // namespace objc3c::runtime
