#include "runtime/dispatch/dispatch_target_resolution.h"

#include "runtime/classes/receiver_identity.h"
#include "runtime/dispatch/dispatch_family.h"
#include "runtime/dispatch/dispatch_resolution_state.h"
#include "runtime/dispatch/method_cache.h"
#include "runtime/dispatch/method_cache_resolution.h"
#include "runtime/selectors/selector_table.h"
#include "runtime/state/runtime_state_records.h"

#include <cstddef>

namespace objc3c::runtime {
namespace {

bool LookupStartBaseIdentityIsReachableFromReceiverUnlocked(
    const RuntimeState &state, std::uint64_t receiver_base_identity,
    std::uint64_t lookup_start_base_identity) {
  const RealizedClassNode *node =
      FindRealizedClassNodeByBaseIdentityUnlocked(state, receiver_base_identity);
  std::size_t visited_count = 0;
  while (node != nullptr && visited_count <= state.realized_class_nodes.size()) {
    if (node->base_identity == lookup_start_base_identity) {
      return true;
    }
    ++visited_count;
    if (!node->has_super_node ||
        node->super_node_index >= state.realized_class_nodes.size()) {
      return false;
    }
    node = &state.realized_class_nodes[node->super_node_index];
  }
  return false;
}

RuntimeDispatchTarget ResolveRuntimeDispatchTargetFromStartIdentityUnlocked(
    RuntimeState &state, int receiver, const char *selector,
    std::uint64_t lookup_start_base_identity, const char *start_error_path) {
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
  if (lookup_start_base_identity == 0) {
    lookup_start_base_identity = base_identity;
  }
  if (state.realized_class_name_by_base_identity.find(
          lookup_start_base_identity) ==
      state.realized_class_name_by_base_identity.end()) {
    return PublishStrictDispatchErrorTargetUnlocked(
        state, OBJC3_RUNTIME_DISPATCH_STATUS_MISSING_CLASS_GRAPH,
        start_error_path);
  }
  if (!LookupStartBaseIdentityIsReachableFromReceiverUnlocked(
          state, base_identity, lookup_start_base_identity)) {
    return PublishStrictDispatchErrorTargetUnlocked(
        state, OBJC3_RUNTIME_DISPATCH_STATUS_MISSING_CLASS_GRAPH,
        start_error_path);
  }

  state.last_dispatch_normalized_receiver_identity =
      normalized_receiver_identity;
  const MethodCacheKey cache_key{lookup_start_base_identity,
                                 normalized_receiver_identity,
                                 selector_handle->stable_id};
  const auto cache_it = state.method_cache.find(cache_key);
  if (cache_it != state.method_cache.end()) {
    return ResolveMethodCacheHitUnlocked(
        state, cache_key, cache_it->second, base_identity,
        lookup_start_base_identity, normalized_receiver_identity,
        selector_handle->stable_id);
  }
  return ResolveMethodCacheMissUnlocked(
      state, lookup_start_base_identity, normalized_receiver_identity, family,
      *selector_handle, base_identity, cache_key);
}

}  // namespace

RuntimeDispatchTarget ResolveRuntimeDispatchTargetUnlocked(
    RuntimeState &state, int receiver, const char *selector) {
  return ResolveRuntimeDispatchTargetFromStartIdentityUnlocked(
      state, receiver, selector, 0, "missing-lookup-start-class-error");
}

RuntimeDispatchTarget ResolveRuntimeDispatchTargetFromClassUnlocked(
    RuntimeState &state, int receiver, const char *lookup_start_class_name,
    const char *selector) {
  if (lookup_start_class_name == nullptr ||
      lookup_start_class_name[0] == '\0') {
    const objc3_runtime_selector_handle *selector_handle =
        LookupSelectorUnlocked(selector);
    ResetRuntimeDispatchStateUnlocked(
        state, selector, selector_handle,
        OBJC3_RUNTIME_DISPATCH_STATUS_MISSING_CLASS_GRAPH);
    return PublishStrictDispatchErrorTargetUnlocked(
        state, OBJC3_RUNTIME_DISPATCH_STATUS_MISSING_CLASS_GRAPH,
        "missing-lookup-start-class-error");
  }

  const auto node_indices_it =
      state.realized_class_node_indices_by_name.find(lookup_start_class_name);
  if (node_indices_it == state.realized_class_node_indices_by_name.end() ||
      node_indices_it->second.size() != 1u) {
    const objc3_runtime_selector_handle *selector_handle =
        LookupSelectorUnlocked(selector);
    ResetRuntimeDispatchStateUnlocked(
        state, selector, selector_handle,
        OBJC3_RUNTIME_DISPATCH_STATUS_MISSING_CLASS_GRAPH);
    return PublishStrictDispatchErrorTargetUnlocked(
        state, OBJC3_RUNTIME_DISPATCH_STATUS_MISSING_CLASS_GRAPH,
        "missing-lookup-start-class-error");
  }

  for (const std::size_t node_index : node_indices_it->second) {
    if (node_index >= state.realized_class_nodes.size()) {
      continue;
    }
    return ResolveRuntimeDispatchTargetFromStartIdentityUnlocked(
        state, receiver, selector,
        state.realized_class_nodes[node_index].base_identity,
        "missing-lookup-start-class-error");
  }

  const objc3_runtime_selector_handle *selector_handle =
      LookupSelectorUnlocked(selector);
  ResetRuntimeDispatchStateUnlocked(
      state, selector, selector_handle,
      OBJC3_RUNTIME_DISPATCH_STATUS_MISSING_CLASS_GRAPH);
  return PublishStrictDispatchErrorTargetUnlocked(
      state, OBJC3_RUNTIME_DISPATCH_STATUS_MISSING_CLASS_GRAPH,
      "missing-lookup-start-class-error");
}

}  // namespace objc3c::runtime
