#include "runtime/storage/property_snapshot_fields.h"

#include "runtime/metadata/runtime_emitted_records.h"
#include "runtime/metadata/runtime_realized_records.h"
#include "runtime/state/runtime_state_records.h"
#include "runtime/storage/ivar_storage_span.h"
#include "runtime/strings/borrowed_string.h"

#include <cstdint>

namespace objc3c::runtime {

void ResetRuntimePropertyRegistryStateSnapshot(
    objc3_runtime_property_registry_state_snapshot &snapshot) {
  snapshot.layout_ready_class_count = 0;
  snapshot.reflectable_property_count = 0;
  snapshot.writable_property_count = 0;
  snapshot.slot_backed_property_count = 0;
  snapshot.property_lookup_cache_entry_count = 0;
  snapshot.property_lookup_cache_hit_count = 0;
  snapshot.property_lookup_cache_miss_count = 0;
  snapshot.last_query_found = 0;
  snapshot.last_query_inherited = 0;
  snapshot.last_query_used_cache = 0;
  snapshot.last_queried_class_name = nullptr;
  snapshot.last_queried_property_name = nullptr;
  snapshot.last_resolved_class_name = nullptr;
  snapshot.last_resolved_owner_identity = nullptr;
}

void PopulateRuntimePropertyRegistryStateSnapshotUnlocked(
    const RuntimeState &state,
    objc3_runtime_property_registry_state_snapshot &snapshot) {
  for (const RealizedClassNode &node : state.realized_class_nodes) {
    if (!node.runtime_layout_ready) {
      continue;
    }
    snapshot.layout_ready_class_count += 1;
    snapshot.reflectable_property_count +=
        static_cast<std::uint64_t>(node.runtime_property_accessors.size());
    for (const RealizedPropertyAccessor &accessor :
         node.runtime_property_accessors) {
      snapshot.writable_property_count +=
          !accessor.setter_owner_identity.empty() ? 1u : 0u;
      snapshot.slot_backed_property_count +=
          accessor.ivar_descriptor != nullptr ? 1u : 0u;
    }
  }
  snapshot.property_lookup_cache_entry_count =
      static_cast<std::uint64_t>(state.property_lookup_cache.size());
  snapshot.property_lookup_cache_hit_count =
      state.property_lookup_cache_hit_count;
  snapshot.property_lookup_cache_miss_count =
      state.property_lookup_cache_miss_count;
  snapshot.last_query_found = state.last_property_query_found ? 1 : 0;
  snapshot.last_query_inherited =
      state.last_property_query_inherited ? 1 : 0;
  snapshot.last_query_used_cache =
      state.last_property_query_used_cache ? 1 : 0;
  snapshot.last_queried_class_name =
      BorrowRuntimeCString(state.last_queried_property_class_name);
  snapshot.last_queried_property_name =
      BorrowRuntimeCString(state.last_queried_property_name);
  snapshot.last_resolved_class_name =
      BorrowRuntimeCString(state.last_reflected_property_class_name);
  snapshot.last_resolved_owner_identity =
      BorrowRuntimeCString(state.last_reflected_property_owner_identity);
}

void ResetRuntimePropertyEntrySnapshot(
    objc3_runtime_property_entry_snapshot &snapshot) {
  snapshot.found = 0;
  snapshot.inherited = 0;
  snapshot.setter_available = 0;
  snapshot.has_runtime_getter = 0;
  snapshot.has_runtime_setter = 0;
  snapshot.base_identity = 0;
  snapshot.slot_index = 0;
  snapshot.offset_bytes = 0;
  snapshot.size_bytes = 0;
  snapshot.alignment_bytes = 0;
  snapshot.padding_bytes = 0;
  snapshot.inherited_slot_count = 0;
  snapshot.inherited_size_bytes = 0;
  snapshot.owner_size_bytes = 0;
  snapshot.init_order_index = 0;
  snapshot.destroy_order_index = 0;
  snapshot.layout_valid = 0;
  snapshot.instance_size_bytes = 0;
  snapshot.queried_class_name = nullptr;
  snapshot.resolved_class_name = nullptr;
  snapshot.property_name = nullptr;
  snapshot.declaration_owner_identity = nullptr;
  snapshot.export_owner_identity = nullptr;
  snapshot.getter_selector = nullptr;
  snapshot.setter_selector = nullptr;
  snapshot.effective_getter_selector = nullptr;
  snapshot.effective_setter_selector = nullptr;
  snapshot.ivar_binding_symbol = nullptr;
  snapshot.synthesized_binding_symbol = nullptr;
  snapshot.ivar_layout_symbol = nullptr;
  snapshot.ivar_layout_replay_key = nullptr;
  snapshot.property_attribute_profile = nullptr;
  snapshot.ownership_lifetime_profile = nullptr;
  snapshot.ownership_runtime_hook_profile = nullptr;
  snapshot.accessor_ownership_profile = nullptr;
  snapshot.getter_owner_identity = nullptr;
  snapshot.setter_owner_identity = nullptr;
}

void PopulateRuntimePropertyEntrySnapshotUnlocked(
    const RuntimeState &state,
    const RealizedClassNode &resolved_node,
    const RealizedPropertyAccessor &accessor,
    bool inherited,
    objc3_runtime_property_entry_snapshot &snapshot) {
  const EmittedPropertyDescriptor &descriptor = *accessor.property_descriptor;
  snapshot.found = 1;
  snapshot.inherited = inherited ? 1 : 0;
  snapshot.setter_available = descriptor.effective_setter_available ? 1 : 0;
  snapshot.has_runtime_getter = 1;
  snapshot.has_runtime_setter =
      !accessor.setter_owner_identity.empty() ? 1 : 0;
  snapshot.base_identity = resolved_node.base_identity;
  snapshot.slot_index =
      accessor.ivar_descriptor != nullptr
          ? accessor.ivar_descriptor->slot_index
          : descriptor.ivar_layout_slot_index;
  snapshot.offset_bytes = EffectiveIvarOffset(accessor);
  snapshot.size_bytes = EffectiveIvarSize(accessor);
  snapshot.alignment_bytes =
      accessor.ivar_descriptor != nullptr
          ? accessor.ivar_descriptor->alignment_bytes
          : descriptor.ivar_layout_alignment_bytes;
  snapshot.padding_bytes =
      accessor.ivar_descriptor != nullptr
          ? accessor.ivar_descriptor->padding_bytes
          : descriptor.ivar_layout_padding_bytes;
  snapshot.inherited_slot_count =
      accessor.ivar_descriptor != nullptr
          ? accessor.ivar_descriptor->inherited_slot_count
          : descriptor.ivar_layout_inherited_slot_count;
  snapshot.inherited_size_bytes =
      accessor.ivar_descriptor != nullptr
          ? accessor.ivar_descriptor->inherited_size_bytes
          : descriptor.ivar_layout_inherited_size_bytes;
  snapshot.owner_size_bytes =
      accessor.ivar_descriptor != nullptr
          ? accessor.ivar_descriptor->owner_size_bytes
          : descriptor.ivar_layout_owner_size_bytes;
  snapshot.init_order_index =
      accessor.ivar_descriptor != nullptr
          ? accessor.ivar_descriptor->init_order_index
          : descriptor.ivar_init_order_index;
  snapshot.destroy_order_index =
      accessor.ivar_descriptor != nullptr
          ? accessor.ivar_descriptor->destroy_order_index
          : descriptor.ivar_destroy_order_index;
  snapshot.layout_valid =
      accessor.ivar_descriptor != nullptr
          ? (accessor.ivar_descriptor->layout_valid ? 1 : 0)
          : (descriptor.ivar_layout_valid ? 1 : 0);
  snapshot.instance_size_bytes =
      static_cast<std::uint64_t>(resolved_node.runtime_instance_size_bytes);
  snapshot.resolved_class_name =
      BorrowRuntimeCString(state.last_reflected_property_class_name);
  snapshot.property_name = descriptor.property_name;
  snapshot.declaration_owner_identity =
      descriptor.declaration_owner_identity != nullptr
          ? descriptor.declaration_owner_identity
          : nullptr;
  snapshot.export_owner_identity =
      descriptor.export_owner_identity != nullptr
          ? descriptor.export_owner_identity
          : nullptr;
  snapshot.getter_selector =
      descriptor.getter_selector != nullptr ? descriptor.getter_selector
                                            : nullptr;
  snapshot.setter_selector =
      descriptor.setter_selector != nullptr ? descriptor.setter_selector
                                            : nullptr;
  snapshot.effective_getter_selector =
      descriptor.effective_getter_selector != nullptr
          ? descriptor.effective_getter_selector
          : nullptr;
  snapshot.effective_setter_selector =
      descriptor.effective_setter_selector != nullptr
          ? descriptor.effective_setter_selector
          : nullptr;
  snapshot.ivar_binding_symbol =
      descriptor.ivar_binding_symbol != nullptr ? descriptor.ivar_binding_symbol
                                                : nullptr;
  snapshot.synthesized_binding_symbol =
      descriptor.synthesized_binding_symbol != nullptr
          ? descriptor.synthesized_binding_symbol
          : nullptr;
  snapshot.ivar_layout_symbol =
      descriptor.ivar_layout_symbol != nullptr ? descriptor.ivar_layout_symbol
                                               : nullptr;
  snapshot.ivar_layout_replay_key =
      accessor.ivar_descriptor != nullptr &&
              accessor.ivar_descriptor->layout_replay_key != nullptr
          ? accessor.ivar_descriptor->layout_replay_key
          : descriptor.ivar_layout_replay_key;
  snapshot.property_attribute_profile =
      descriptor.property_attribute_profile != nullptr
          ? descriptor.property_attribute_profile
          : nullptr;
  snapshot.ownership_lifetime_profile =
      descriptor.ownership_lifetime_profile != nullptr
          ? descriptor.ownership_lifetime_profile
          : nullptr;
  snapshot.ownership_runtime_hook_profile =
      descriptor.ownership_runtime_hook_profile != nullptr
          ? descriptor.ownership_runtime_hook_profile
          : nullptr;
  snapshot.accessor_ownership_profile =
      descriptor.accessor_ownership_profile != nullptr
          ? descriptor.accessor_ownership_profile
          : nullptr;
  snapshot.getter_owner_identity =
      BorrowRuntimeCString(accessor.getter_owner_identity);
  snapshot.setter_owner_identity =
      BorrowRuntimeCString(accessor.setter_owner_identity);
}

}  // namespace objc3c::runtime
