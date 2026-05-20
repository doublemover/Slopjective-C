#include "runtime/storage/property_lookup.h"

#include "runtime/metadata/runtime_realized_records.h"
#include "runtime/state/runtime_cache_invalidation.h"
#include "runtime/state/runtime_state_records.h"

#include <cstddef>
#include <cstring>
#include <unordered_set>
#include <utility>

namespace objc3c::runtime {

const RealizedPropertyAccessor *FindRuntimePropertyAccessorByNameUnlocked(
    RuntimeState &state,
    const RealizedClassNode &start_node,
    const char *property_name,
    const RealizedClassNode *&resolved_node,
    bool &inherited,
    bool &used_cache) {
  resolved_node = nullptr;
  inherited = false;
  used_cache = false;
  if (property_name == nullptr || property_name[0] == '\0') {
    return nullptr;
  }
  PropertyLookupCacheKey cache_key;
  cache_key.start_base_identity = start_node.base_identity;
  cache_key.property_name = property_name;
  const auto cache_it = state.property_lookup_cache.find(cache_key);
  if (cache_it != state.property_lookup_cache.end()) {
    used_cache = true;
    ++state.property_lookup_cache_hit_count;
    if (!PropertyLookupCacheMutationGenerationsMatchUnlocked(state,
                                                             cache_it->second)) {
      state.property_lookup_cache.erase(cache_it);
      used_cache = false;
    } else {
      if (!cache_it->second.found) {
        inherited = false;
        resolved_node = nullptr;
        return nullptr;
      }
      if (cache_it->second.resolved_node_index <
          state.realized_class_nodes.size()) {
        const RealizedClassNode &cached_node =
            state.realized_class_nodes[cache_it->second.resolved_node_index];
        if (cache_it->second.accessor_index <
            cached_node.runtime_property_accessors.size()) {
          resolved_node = &cached_node;
          inherited = cache_it->second.inherited;
          return &cached_node.runtime_property_accessors[
              cache_it->second.accessor_index];
        }
      }
      state.property_lookup_cache.erase(cache_it);
    }
  }
  ++state.property_lookup_cache_miss_count;
  std::unordered_set<const RealizedClassNode *> visited;
  const RealizedClassNode *node = &start_node;
  while (node != nullptr && visited.insert(node).second) {
    if (node->runtime_layout_ready) {
      for (const RealizedPropertyAccessor &accessor :
           node->runtime_property_accessors) {
        if (accessor.property_descriptor == nullptr ||
            accessor.property_descriptor->property_name == nullptr) {
          continue;
        }
        if (std::strcmp(accessor.property_descriptor->property_name,
                        property_name) == 0) {
          resolved_node = node;
          inherited = node != &start_node;
          const std::size_t resolved_node_index = static_cast<std::size_t>(
              node - state.realized_class_nodes.data());
          const std::size_t accessor_index = static_cast<std::size_t>(
              &accessor - node->runtime_property_accessors.data());
          PropertyLookupCacheEntry entry{true, inherited, resolved_node_index,
                                         accessor_index};
          StampPropertyLookupCacheMutationGenerationsUnlocked(entry, state);
          state.property_lookup_cache.emplace(std::move(cache_key), entry);
          return &accessor;
        }
      }
    }
    if (!node->has_super_node) {
      node = nullptr;
    } else if (node->super_node_index < state.realized_class_nodes.size()) {
      node = &state.realized_class_nodes[node->super_node_index];
    } else {
      node = nullptr;
    }
  }
  PropertyLookupCacheEntry entry;
  StampPropertyLookupCacheMutationGenerationsUnlocked(entry, state);
  state.property_lookup_cache.emplace(std::move(cache_key), entry);
  return nullptr;
}

}  // namespace objc3c::runtime
