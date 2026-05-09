#include "runtime/reflection/property_registry_snapshot_fields.h"

#include "runtime/metadata/runtime_realized_records.h"
#include "runtime/state/runtime_state_records.h"
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

}  // namespace objc3c::runtime
