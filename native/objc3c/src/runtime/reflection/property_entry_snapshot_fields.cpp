#include "runtime/reflection/property_entry_snapshot_fields.h"

#include "runtime/metadata/runtime_emitted_records.h"
#include "runtime/metadata/runtime_realized_records.h"
#include "runtime/state/runtime_state_records.h"
#include "runtime/reflection/property_entry_descriptor_snapshot_fields.h"
#include "runtime/reflection/property_entry_layout_snapshot_fields.h"
#include "runtime/strings/borrowed_string.h"

namespace objc3c::runtime {

void ResetRuntimePropertyEntrySnapshot(
    objc3_runtime_property_entry_snapshot &snapshot) {
  snapshot.found = 0;
  snapshot.inherited = 0;
  snapshot.setter_available = 0;
  snapshot.has_runtime_getter = 0;
  snapshot.has_runtime_setter = 0;
  snapshot.attribute_count = 0;
  snapshot.is_readonly = 0;
  snapshot.is_nonatomic = 0;
  snapshot.is_strong = 0;
  snapshot.is_assign = 0;
  snapshot.is_weak = 0;
  snapshot.is_copy = 0;
  snapshot.has_custom_getter = 0;
  snapshot.has_custom_setter = 0;
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
  snapshot.property_behavior_name = nullptr;
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
  snapshot.resolved_class_name =
      BorrowRuntimeCString(state.last_reflected_property_class_name);
  PopulateRuntimePropertyEntryLayoutSnapshotFields(
      resolved_node, accessor, descriptor, snapshot);
  PopulateRuntimePropertyEntryDescriptorSnapshotFields(
      accessor, descriptor, snapshot);
}

}  // namespace objc3c::runtime
