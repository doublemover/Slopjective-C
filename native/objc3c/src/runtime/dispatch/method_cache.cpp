#include "runtime/dispatch/method_cache.h"

#include "runtime/dispatch/builtin_method_lookup.h"
#include "runtime/dispatch/dispatch_family.h"
#include "runtime/dispatch/method_class_chain_resolution.h"
#include "runtime/dispatch/runtime_resolution_records.h"
#include "runtime/objc3_runtime_bootstrap_internal.h"
#include "runtime/state/runtime_state_records.h"
#include "runtime/storage/property_accessor_resolution.h"

#include <functional>
#include <string>

namespace objc3c::runtime {

bool MethodCacheKey::operator==(const MethodCacheKey &other) const {
  return lookup_start_base_identity == other.lookup_start_base_identity &&
         normalized_receiver_identity == other.normalized_receiver_identity &&
         selector_stable_id == other.selector_stable_id;
}

std::size_t MethodCacheKeyHash::operator()(const MethodCacheKey &key) const {
  return std::hash<std::uint64_t>{}(key.lookup_start_base_identity) ^
         (std::hash<std::uint64_t>{}(key.normalized_receiver_identity) << 1u) ^
         (std::hash<std::uint64_t>{}(key.selector_stable_id) << 2u);
}

SlowPathResolution ResolveMethodSlowPathUnlocked(
    RuntimeState &state, std::uint64_t lookup_start_base_identity,
    std::uint64_t normalized_receiver_identity, DispatchFamily family,
    std::uint64_t selector_stable_id, const char *selector_spelling) {
  // Live lookup walks the emitted class/metaclass graph that came through
  // startup registration, resolves one deterministic class family for the
  // receiver identity, and records the outcome in the method cache.
  SlowPathResolution resolution;
  resolution.selector_storage = selector_spelling != nullptr ? selector_spelling
                                                            : "";
  resolution.lookup_start_base_identity = lookup_start_base_identity;
  resolution.normalized_receiver_identity = normalized_receiver_identity;
  resolution.selector_stable_id = selector_stable_id;
  std::uint64_t category_probe_count = 0;
  std::uint64_t protocol_probe_count = 0;

  bool receiver_ambiguous = false;
  std::string resolved_class_name;
  if (!ResolveReceiverClassNameUnlocked(state, lookup_start_base_identity,
                                        resolved_class_name,
                                        receiver_ambiguous)) {
    resolution.ambiguous = receiver_ambiguous;
    resolution.strict_error_status =
        receiver_ambiguous ? OBJC3_RUNTIME_DISPATCH_STATUS_CATEGORY_CONFLICT
                           : OBJC3_RUNTIME_DISPATCH_STATUS_MISSING_CLASS_GRAPH;
    return resolution;
  }

  const auto realized_nodes_it =
      state.realized_class_node_indices_by_name.find(resolved_class_name);
  if (realized_nodes_it == state.realized_class_node_indices_by_name.end()) {
    resolution.strict_error_status =
        OBJC3_RUNTIME_DISPATCH_STATUS_MISSING_CLASS_GRAPH;
    return resolution;
  }

  for (const std::size_t node_index : realized_nodes_it->second) {
    if (node_index >= state.realized_class_nodes.size()) {
      continue;
    }
    const RealizedClassNode &node = state.realized_class_nodes[node_index];
    SlowPathResolution image_resolution;
    image_resolution.selector_storage =
        selector_spelling != nullptr ? selector_spelling : "";
    image_resolution.lookup_start_base_identity = lookup_start_base_identity;
    image_resolution.normalized_receiver_identity =
        normalized_receiver_identity;
    image_resolution.selector_stable_id = selector_stable_id;
    std::uint64_t image_category_probe_count = 0;
    std::uint64_t image_protocol_probe_count = 0;
    bool method_ambiguous = false;
    if (!TryResolveMethodFromRealizedClassChainUnlocked(
            state, &node, family, normalized_receiver_identity,
            selector_stable_id, selector_spelling, image_resolution,
            method_ambiguous, image_category_probe_count,
            image_protocol_probe_count)) {
      if (image_resolution.strict_error_status !=
          OBJC3_RUNTIME_DISPATCH_STATUS_UNKNOWN_SELECTOR) {
        return image_resolution;
      }
      SlowPathResolution malformed_resolution;
      malformed_resolution.selector_storage =
          selector_spelling != nullptr ? selector_spelling : "";
      malformed_resolution.lookup_start_base_identity =
          lookup_start_base_identity;
      malformed_resolution.normalized_receiver_identity =
          normalized_receiver_identity;
      malformed_resolution.selector_stable_id = selector_stable_id;
      malformed_resolution.strict_error_status =
          OBJC3_RUNTIME_DISPATCH_STATUS_MALFORMED_METADATA;
      return malformed_resolution;
    }
    if (method_ambiguous) {
      resolution = SlowPathResolution{};
      resolution.ambiguous = true;
      resolution.strict_error_status =
          OBJC3_RUNTIME_DISPATCH_STATUS_CATEGORY_CONFLICT;
      resolution.selector_storage =
          selector_spelling != nullptr ? selector_spelling : "";
      resolution.lookup_start_base_identity = lookup_start_base_identity;
      resolution.normalized_receiver_identity = normalized_receiver_identity;
      resolution.selector_stable_id = selector_stable_id;
      resolution.category_probe_count =
          category_probe_count + image_category_probe_count;
      resolution.protocol_probe_count =
          protocol_probe_count + image_protocol_probe_count;
      return resolution;
    }
    if (HasTerminalStrictDispatchError(image_resolution)) {
      image_resolution.category_probe_count =
          category_probe_count + image_category_probe_count;
      image_resolution.protocol_probe_count =
          protocol_probe_count + image_protocol_probe_count;
      return image_resolution;
    }
    category_probe_count += image_category_probe_count;
    protocol_probe_count += image_protocol_probe_count;
    if (family == DispatchFamily::Instance &&
        TryResolveRuntimeManagedPropertyAccessorUnlocked(
            state, node, normalized_receiver_identity, selector_stable_id,
            selector_spelling, image_resolution)) {
      image_resolution.category_probe_count =
          category_probe_count + image_category_probe_count;
      image_resolution.protocol_probe_count =
          protocol_probe_count + image_protocol_probe_count;
    }
    if (HasTerminalStrictDispatchError(image_resolution)) {
      return image_resolution;
    }
    if (image_resolution.resolved) {
      image_resolution.objc_final_declared =
          image_resolution.objc_final_declared || node.objc_final_declared;
      image_resolution.objc_sealed_declared =
          image_resolution.objc_sealed_declared || node.objc_sealed_declared;
      if (image_resolution.fast_path_reason.empty()) {
        if (image_resolution.objc_final_declared) {
          image_resolution.fast_path_reason = "class-final";
        } else if (image_resolution.objc_sealed_declared) {
          image_resolution.fast_path_reason = "class-sealed";
        }
      }
      if (resolution.resolved &&
          (resolution.implementation != image_resolution.implementation ||
           resolution.parameter_count != image_resolution.parameter_count ||
           resolution.owner_identity != image_resolution.owner_identity ||
           resolution.class_name != image_resolution.class_name)) {
        resolution = SlowPathResolution{};
        resolution.ambiguous = true;
        resolution.strict_error_status =
            OBJC3_RUNTIME_DISPATCH_STATUS_CATEGORY_CONFLICT;
        resolution.selector_storage =
            selector_spelling != nullptr ? selector_spelling : "";
        resolution.lookup_start_base_identity = lookup_start_base_identity;
        resolution.normalized_receiver_identity = normalized_receiver_identity;
        resolution.selector_stable_id = selector_stable_id;
        resolution.category_probe_count = category_probe_count;
        resolution.protocol_probe_count = protocol_probe_count;
        return resolution;
      }
      if (!resolution.resolved) {
        resolution = image_resolution;
      }
    }
  }
  resolution.category_probe_count = category_probe_count;
  resolution.protocol_probe_count = protocol_probe_count;
  if (!resolution.resolved && !resolution.ambiguous &&
      TryResolveRuntimeBuiltinObjectMethod(resolved_class_name, family,
                                           selector_spelling, resolution)) {
    resolution.normalized_receiver_identity = normalized_receiver_identity;
    resolution.lookup_start_base_identity = lookup_start_base_identity;
    resolution.selector_stable_id = selector_stable_id;
    resolution.category_probe_count = category_probe_count;
    resolution.protocol_probe_count = protocol_probe_count;
  }
  return resolution;
}

}  // namespace objc3c::runtime
