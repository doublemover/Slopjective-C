#include "runtime/storage/property_accessors.h"

#include "runtime/metadata/runtime_emitted_records.h"
#include "runtime/metadata/runtime_realized_records.h"
#include "runtime/objc3_runtime_bootstrap_internal.h"
#include "runtime/state/runtime_state_records.h"
#include "runtime/state/runtime_state_store.h"
#include "runtime/strings/borrowed_string.h"
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

extern "C" int objc3_runtime_copy_property_registry_state_for_testing(
    objc3_runtime_property_registry_state_snapshot *snapshot) {
  // property-metadata-reflection anchor: the private registry-state snapshot is
  // the canonical diagnostic/testing surface for aggregate reflectable
  // property metadata counts and last-query evidence.
  if (snapshot == nullptr) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR;
  }

  snapshot->layout_ready_class_count = 0;
  snapshot->reflectable_property_count = 0;
  snapshot->writable_property_count = 0;
  snapshot->slot_backed_property_count = 0;
  snapshot->property_lookup_cache_entry_count = 0;
  snapshot->property_lookup_cache_hit_count = 0;
  snapshot->property_lookup_cache_miss_count = 0;
  snapshot->last_query_found = 0;
  snapshot->last_query_inherited = 0;
  snapshot->last_query_used_cache = 0;
  snapshot->last_queried_class_name = nullptr;
  snapshot->last_queried_property_name = nullptr;
  snapshot->last_resolved_class_name = nullptr;
  snapshot->last_resolved_owner_identity = nullptr;

  objc3c::runtime::RuntimeState &state = objc3c::runtime::ProcessRuntimeState();
  std::lock_guard<std::mutex> lock(state.mutex);
  for (const objc3c::runtime::RealizedClassNode &node :
       state.realized_class_nodes) {
    if (!node.runtime_layout_ready) {
      continue;
    }
    snapshot->layout_ready_class_count += 1;
    snapshot->reflectable_property_count +=
        static_cast<std::uint64_t>(node.runtime_property_accessors.size());
    for (const objc3c::runtime::RealizedPropertyAccessor &accessor :
         node.runtime_property_accessors) {
      snapshot->writable_property_count +=
          !accessor.setter_owner_identity.empty() ? 1u : 0u;
      snapshot->slot_backed_property_count +=
          accessor.ivar_descriptor != nullptr ? 1u : 0u;
    }
  }
  snapshot->property_lookup_cache_entry_count =
      static_cast<std::uint64_t>(state.property_lookup_cache.size());
  snapshot->property_lookup_cache_hit_count =
      state.property_lookup_cache_hit_count;
  snapshot->property_lookup_cache_miss_count =
      state.property_lookup_cache_miss_count;
  snapshot->last_query_found = state.last_property_query_found ? 1 : 0;
  snapshot->last_query_inherited =
      state.last_property_query_inherited ? 1 : 0;
  snapshot->last_query_used_cache =
      state.last_property_query_used_cache ? 1 : 0;
  snapshot->last_queried_class_name =
      objc3c::runtime::BorrowRuntimeCString(
          state.last_queried_property_class_name);
  snapshot->last_queried_property_name =
      objc3c::runtime::BorrowRuntimeCString(state.last_queried_property_name);
  snapshot->last_resolved_class_name =
      objc3c::runtime::BorrowRuntimeCString(
          state.last_reflected_property_class_name);
  snapshot->last_resolved_owner_identity =
      objc3c::runtime::BorrowRuntimeCString(
          state.last_reflected_property_owner_identity);
  return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
}

extern "C" int objc3_runtime_copy_property_entry_for_testing(
    const char *class_name,
    const char *property_name,
    objc3_runtime_property_entry_snapshot *snapshot) {
  // property-metadata-reflection anchor: the private per-property snapshot
  // exposes runtime-owned accessor/layout facts by class/property name without
  // widening the public ABI or rederiving metadata from source.
  if (snapshot == nullptr) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR;
  }

  snapshot->found = 0;
  snapshot->inherited = 0;
  snapshot->setter_available = 0;
  snapshot->has_runtime_getter = 0;
  snapshot->has_runtime_setter = 0;
  snapshot->base_identity = 0;
  snapshot->slot_index = 0;
  snapshot->offset_bytes = 0;
  snapshot->size_bytes = 0;
  snapshot->alignment_bytes = 0;
  snapshot->padding_bytes = 0;
  snapshot->inherited_slot_count = 0;
  snapshot->inherited_size_bytes = 0;
  snapshot->owner_size_bytes = 0;
  snapshot->init_order_index = 0;
  snapshot->destroy_order_index = 0;
  snapshot->layout_valid = 0;
  snapshot->instance_size_bytes = 0;
  snapshot->queried_class_name = nullptr;
  snapshot->resolved_class_name = nullptr;
  snapshot->property_name = nullptr;
  snapshot->declaration_owner_identity = nullptr;
  snapshot->export_owner_identity = nullptr;
  snapshot->getter_selector = nullptr;
  snapshot->setter_selector = nullptr;
  snapshot->effective_getter_selector = nullptr;
  snapshot->effective_setter_selector = nullptr;
  snapshot->ivar_binding_symbol = nullptr;
  snapshot->synthesized_binding_symbol = nullptr;
  snapshot->ivar_layout_symbol = nullptr;
  snapshot->ivar_layout_replay_key = nullptr;
  snapshot->property_attribute_profile = nullptr;
  snapshot->ownership_lifetime_profile = nullptr;
  snapshot->ownership_runtime_hook_profile = nullptr;
  snapshot->accessor_ownership_profile = nullptr;
  snapshot->getter_owner_identity = nullptr;
  snapshot->setter_owner_identity = nullptr;

  objc3c::runtime::RuntimeState &state = objc3c::runtime::ProcessRuntimeState();
  std::lock_guard<std::mutex> lock(state.mutex);
  state.last_queried_property_class_name =
      class_name != nullptr ? class_name : "";
  state.last_queried_property_name =
      property_name != nullptr ? property_name : "";
  state.last_reflected_property_class_name.clear();
  state.last_reflected_property_owner_identity.clear();
  state.last_property_query_found = false;
  state.last_property_query_inherited = false;

  snapshot->queried_class_name =
      objc3c::runtime::BorrowRuntimeCString(
          state.last_queried_property_class_name);
  if (class_name == nullptr || class_name[0] == '\0' ||
      property_name == nullptr || property_name[0] == '\0') {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
  }

  const auto found = state.realized_class_node_indices_by_name.find(class_name);
  if (found == state.realized_class_node_indices_by_name.end() ||
      found->second.empty()) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
  }
  const std::size_t node_index = found->second.front();
  if (node_index >= state.realized_class_nodes.size()) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
  }

  const objc3c::runtime::RealizedClassNode &start_node =
      state.realized_class_nodes[node_index];
  const objc3c::runtime::RealizedClassNode *resolved_node = nullptr;
  bool inherited = false;
  bool used_cache = false;
  const objc3c::runtime::RealizedPropertyAccessor *accessor =
      objc3c::runtime::FindRuntimePropertyAccessorByNameUnlocked(
          state, start_node, property_name, resolved_node, inherited,
          used_cache);
  state.last_property_query_used_cache = used_cache;
  if (accessor == nullptr || resolved_node == nullptr ||
      accessor->property_descriptor == nullptr) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
  }

  state.last_property_query_found = true;
  state.last_property_query_inherited = inherited;
  state.last_reflected_property_class_name = resolved_node->class_name;
  state.last_reflected_property_owner_identity =
      accessor->property_descriptor->declaration_owner_identity != nullptr
          ? accessor->property_descriptor->declaration_owner_identity
          : "";

  const objc3c::runtime::EmittedPropertyDescriptor &descriptor =
      *accessor->property_descriptor;
  snapshot->found = 1;
  snapshot->inherited = inherited ? 1 : 0;
  snapshot->setter_available = descriptor.effective_setter_available ? 1 : 0;
  snapshot->has_runtime_getter = 1;
  snapshot->has_runtime_setter =
      !accessor->setter_owner_identity.empty() ? 1 : 0;
  snapshot->base_identity = resolved_node->base_identity;
  snapshot->slot_index =
      accessor->ivar_descriptor != nullptr
          ? accessor->ivar_descriptor->slot_index
          : descriptor.ivar_layout_slot_index;
  snapshot->offset_bytes = objc3c::runtime::EffectiveIvarOffset(*accessor);
  snapshot->size_bytes = objc3c::runtime::EffectiveIvarSize(*accessor);
  snapshot->alignment_bytes =
      accessor->ivar_descriptor != nullptr
          ? accessor->ivar_descriptor->alignment_bytes
          : descriptor.ivar_layout_alignment_bytes;
  snapshot->padding_bytes =
      accessor->ivar_descriptor != nullptr
          ? accessor->ivar_descriptor->padding_bytes
          : descriptor.ivar_layout_padding_bytes;
  snapshot->inherited_slot_count =
      accessor->ivar_descriptor != nullptr
          ? accessor->ivar_descriptor->inherited_slot_count
          : descriptor.ivar_layout_inherited_slot_count;
  snapshot->inherited_size_bytes =
      accessor->ivar_descriptor != nullptr
          ? accessor->ivar_descriptor->inherited_size_bytes
          : descriptor.ivar_layout_inherited_size_bytes;
  snapshot->owner_size_bytes =
      accessor->ivar_descriptor != nullptr
          ? accessor->ivar_descriptor->owner_size_bytes
          : descriptor.ivar_layout_owner_size_bytes;
  snapshot->init_order_index =
      accessor->ivar_descriptor != nullptr
          ? accessor->ivar_descriptor->init_order_index
          : descriptor.ivar_init_order_index;
  snapshot->destroy_order_index =
      accessor->ivar_descriptor != nullptr
          ? accessor->ivar_descriptor->destroy_order_index
          : descriptor.ivar_destroy_order_index;
  snapshot->layout_valid =
      accessor->ivar_descriptor != nullptr
          ? (accessor->ivar_descriptor->layout_valid ? 1 : 0)
          : (descriptor.ivar_layout_valid ? 1 : 0);
  snapshot->instance_size_bytes =
      static_cast<std::uint64_t>(resolved_node->runtime_instance_size_bytes);
  snapshot->resolved_class_name =
      objc3c::runtime::BorrowRuntimeCString(
          state.last_reflected_property_class_name);
  snapshot->property_name = descriptor.property_name;
  snapshot->declaration_owner_identity =
      descriptor.declaration_owner_identity != nullptr
          ? descriptor.declaration_owner_identity
          : nullptr;
  snapshot->export_owner_identity =
      descriptor.export_owner_identity != nullptr
          ? descriptor.export_owner_identity
          : nullptr;
  snapshot->getter_selector =
      descriptor.getter_selector != nullptr ? descriptor.getter_selector
                                            : nullptr;
  snapshot->setter_selector =
      descriptor.setter_selector != nullptr ? descriptor.setter_selector
                                            : nullptr;
  snapshot->effective_getter_selector =
      descriptor.effective_getter_selector != nullptr
          ? descriptor.effective_getter_selector
          : nullptr;
  snapshot->effective_setter_selector =
      descriptor.effective_setter_selector != nullptr
          ? descriptor.effective_setter_selector
          : nullptr;
  snapshot->ivar_binding_symbol =
      descriptor.ivar_binding_symbol != nullptr ? descriptor.ivar_binding_symbol
                                                : nullptr;
  snapshot->synthesized_binding_symbol =
      descriptor.synthesized_binding_symbol != nullptr
          ? descriptor.synthesized_binding_symbol
          : nullptr;
  snapshot->ivar_layout_symbol =
      descriptor.ivar_layout_symbol != nullptr ? descriptor.ivar_layout_symbol
                                               : nullptr;
  snapshot->ivar_layout_replay_key =
      accessor->ivar_descriptor != nullptr &&
              accessor->ivar_descriptor->layout_replay_key != nullptr
          ? accessor->ivar_descriptor->layout_replay_key
          : descriptor.ivar_layout_replay_key;
  snapshot->property_attribute_profile =
      descriptor.property_attribute_profile != nullptr
          ? descriptor.property_attribute_profile
          : nullptr;
  snapshot->ownership_lifetime_profile =
      descriptor.ownership_lifetime_profile != nullptr
          ? descriptor.ownership_lifetime_profile
          : nullptr;
  snapshot->ownership_runtime_hook_profile =
      descriptor.ownership_runtime_hook_profile != nullptr
          ? descriptor.ownership_runtime_hook_profile
          : nullptr;
  snapshot->accessor_ownership_profile =
      descriptor.accessor_ownership_profile != nullptr
          ? descriptor.accessor_ownership_profile
          : nullptr;
  snapshot->getter_owner_identity =
      objc3c::runtime::BorrowRuntimeCString(accessor->getter_owner_identity);
  snapshot->setter_owner_identity =
      objc3c::runtime::BorrowRuntimeCString(accessor->setter_owner_identity);
  return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
}
