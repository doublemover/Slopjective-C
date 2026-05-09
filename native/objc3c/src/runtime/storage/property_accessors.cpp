#include "runtime/storage/property_accessors.h"

#include "runtime/metadata/runtime_emitted_records.h"
#include "runtime/metadata/runtime_realized_records.h"
#include "runtime/state/runtime_state_records.h"
#include "runtime/storage/instance_storage.h"
#include "runtime/storage/runtime_instance_records.h"
#include "runtime/storage/weak_slots.h"
#include "support/selectors/selector_normalization.h"

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <unordered_set>
#include <utility>

namespace objc3c::runtime {

bool RuntimePropertyAccessorSelectorIsMaterializable(const char *selector) {
  return objc3c::support::selectors::IsValidMetadataSelectorSpelling(
      objc3c::support::selectors::NormalizeSelectorSpelling(selector));
}

bool RuntimePropertySetterHasSupportedArity(unsigned long long parameter_count) {
  return parameter_count == 1;
}

std::size_t EffectiveIvarOffset(const RealizedPropertyAccessor &accessor) {
  if (accessor.ivar_descriptor == nullptr) {
    return 0u;
  }
  if (accessor.ivar_descriptor->offset_global != nullptr) {
    return static_cast<std::size_t>(*accessor.ivar_descriptor->offset_global);
  }
  return static_cast<std::size_t>(accessor.ivar_descriptor->offset_bytes);
}

std::size_t EffectiveIvarSize(const RealizedPropertyAccessor &accessor) {
  return accessor.ivar_descriptor != nullptr
             ? static_cast<std::size_t>(accessor.ivar_descriptor->size_bytes)
             : 0u;
}

namespace {

const char *PropertyOwnershipLifetimeProfile(
    const RealizedPropertyAccessor &accessor) {
  return accessor.property_descriptor != nullptr
             ? accessor.property_descriptor->ownership_lifetime_profile
             : nullptr;
}

const char *PropertyOwnershipRuntimeHookProfile(
    const RealizedPropertyAccessor &accessor) {
  return accessor.property_descriptor != nullptr
             ? accessor.property_descriptor->ownership_runtime_hook_profile
             : nullptr;
}

const char *PropertyAccessorOwnershipProfile(
    const RealizedPropertyAccessor &accessor) {
  return accessor.property_descriptor != nullptr
             ? accessor.property_descriptor->accessor_ownership_profile
             : nullptr;
}

bool AccessorProfileContains(const char *profile, const char *needle) {
  return profile != nullptr && needle != nullptr &&
         std::strstr(profile, needle) != nullptr;
}

bool IsRuntimeManagedReceiverValueUnlocked(const RuntimeState &state,
                                           int value) {
  return RuntimeInstanceReceiverIsManaged(value) &&
         state.runtime_instances_by_receiver.find(value) !=
             state.runtime_instances_by_receiver.end();
}

bool WriteWeakRuntimeManagedPropertyValueUnlocked(
    RuntimeState &state,
    RuntimeInstanceRecord &instance,
    const RealizedPropertyAccessor &accessor,
    int value) {
  const std::size_t offset = EffectiveIvarOffset(accessor);
  const std::size_t size = EffectiveIvarSize(accessor);
  if (size == 0u || offset + size > instance.storage_bytes.size()) {
    return false;
  }
  int previous = 0;
  (void)ReadRuntimeManagedPropertyValueRaw(instance, accessor, previous);
  RemoveWeakSlotRefUnlocked(state, previous,
                            static_cast<int>(instance.receiver_identity),
                            offset, size);
  if (!WriteRuntimeManagedPropertyValueRaw(instance, accessor, value)) {
    return false;
  }
  if (IsRuntimeManagedReceiverValueUnlocked(state, value)) {
    RegisterWeakSlotRefUnlocked(state, value,
                                static_cast<int>(instance.receiver_identity),
                                offset, size);
  }
  return true;
}

}  // namespace

bool UsesStrongOwnedRuntimeHooks(const RealizedPropertyAccessor &accessor) {
  const char *lifetime_profile = PropertyOwnershipLifetimeProfile(accessor);
  if (lifetime_profile != nullptr &&
      std::strcmp(lifetime_profile, "strong-owned") == 0) {
    return true;
  }
  return AccessorProfileContains(PropertyAccessorOwnershipProfile(accessor),
                                 "ownership_lifetime=strong-owned");
}

bool UsesWeakRuntimeHooks(const RealizedPropertyAccessor &accessor) {
  const char *hook_profile = PropertyOwnershipRuntimeHookProfile(accessor);
  return hook_profile != nullptr &&
         std::strcmp(hook_profile, "objc-weak-side-table") == 0;
}

bool UsesSafeUnownedRuntimeHooks(const RealizedPropertyAccessor &accessor) {
  const char *hook_profile = PropertyOwnershipRuntimeHookProfile(accessor);
  return hook_profile != nullptr &&
         std::strcmp(hook_profile, "objc-unowned-safe-guard") == 0;
}

bool ReadRuntimeManagedPropertyValueRaw(
    const RuntimeInstanceRecord &instance,
    const RealizedPropertyAccessor &accessor,
    int &value) {
  const std::size_t offset = EffectiveIvarOffset(accessor);
  const std::size_t size = EffectiveIvarSize(accessor);
  if (size == 0u || offset + size > instance.storage_bytes.size()) {
    return false;
  }
  std::uint64_t raw = 0;
  std::memcpy(&raw, instance.storage_bytes.data() + offset,
              std::min<std::size_t>(size, sizeof(raw)));
  if (accessor.getter_return_kind == RuntimeMethodReturnKind::Bool) {
    value = raw != 0u ? 1 : 0;
    return true;
  }
  value = static_cast<int>(raw & 0xffffffffu);
  return true;
}

bool WriteRuntimeManagedPropertyValueRaw(
    RuntimeInstanceRecord &instance,
    const RealizedPropertyAccessor &accessor,
    int value) {
  const std::size_t offset = EffectiveIvarOffset(accessor);
  const std::size_t size = EffectiveIvarSize(accessor);
  if (size == 0u || offset + size > instance.storage_bytes.size()) {
    return false;
  }
  std::uint64_t raw = accessor.getter_return_kind == RuntimeMethodReturnKind::Bool
                          ? static_cast<std::uint64_t>(value != 0 ? 1 : 0)
                          : static_cast<std::uint64_t>(
                                static_cast<std::uint32_t>(value));
  std::memcpy(instance.storage_bytes.data() + offset, &raw,
              std::min<std::size_t>(size, sizeof(raw)));
  if (size > sizeof(raw)) {
    std::fill(
        instance.storage_bytes.begin() +
            static_cast<std::ptrdiff_t>(offset + sizeof(raw)),
        instance.storage_bytes.begin() +
            static_cast<std::ptrdiff_t>(offset + size),
        0);
  }
  return true;
}

bool ReadRuntimeManagedPropertyValueUnlocked(
    const RuntimeState &state,
    const RuntimeInstanceRecord &instance,
    const RealizedPropertyAccessor &accessor,
    int &value) {
  if (!ReadRuntimeManagedPropertyValueRaw(instance, accessor, value)) {
    return false;
  }
  if (!UsesSafeUnownedRuntimeHooks(accessor) || value == 0) {
    return true;
  }
  if (!IsRuntimeManagedReceiverValueUnlocked(state, value)) {
    value = 0;
  }
  return true;
}

bool WriteRuntimeManagedPropertyValueUnlocked(
    RuntimeState &state,
    RuntimeInstanceRecord &instance,
    const RealizedPropertyAccessor &accessor,
    int value) {
  if (UsesWeakRuntimeHooks(accessor)) {
    return WriteWeakRuntimeManagedPropertyValueUnlocked(state, instance,
                                                        accessor, value);
  }
  return WriteRuntimeManagedPropertyValueRaw(instance, accessor, value);
}

bool ExchangeRuntimeManagedPropertyValueUnlocked(
    RuntimeState &state,
    RuntimeInstanceRecord &instance,
    const RealizedPropertyAccessor &accessor,
    int value,
    int &previous_value) {
  if (!ReadRuntimeManagedPropertyValueUnlocked(state, instance, accessor,
                                               previous_value)) {
    previous_value = 0;
    return false;
  }
  return WriteRuntimeManagedPropertyValueUnlocked(state, instance, accessor,
                                                  value);
}

const RealizedPropertyAccessor *FindRuntimePropertyAccessorByNameUnlocked(
    RuntimeState &state,
    const RealizedClassNode &start_node,
    const char *property_name,
    const RealizedClassNode *&resolved_node,
    bool &inherited,
    bool &used_cache) {
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
        return &cached_node
                    .runtime_property_accessors[cache_it->second.accessor_index];
      }
    }
    state.property_lookup_cache.erase(cache_it);
  }
  used_cache = false;
  ++state.property_lookup_cache_miss_count;
  std::unordered_set<const RealizedClassNode *> visited;
  const RealizedClassNode *node = &start_node;
  inherited = false;
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
          state.property_lookup_cache.emplace(
              std::move(cache_key),
              PropertyLookupCacheEntry{true, inherited, resolved_node_index,
                                       accessor_index});
          return &accessor;
        }
      }
    }
    node = node->has_super_node ? &state.realized_class_nodes[node->super_node_index]
                                : nullptr;
  }
  state.property_lookup_cache.emplace(std::move(cache_key),
                                      PropertyLookupCacheEntry{});
  return nullptr;
}

}  // namespace objc3c::runtime
