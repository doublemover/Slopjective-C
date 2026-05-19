#include "runtime/reflection/property_entry_layout_snapshot_fields.h"

#include "runtime/metadata/runtime_emitted_records.h"
#include "runtime/metadata/runtime_realized_records.h"
#include "runtime/storage/ivar_storage_span.h"

#include <cstdint>

namespace objc3c::runtime {

void PopulateRuntimePropertyEntryLayoutSnapshotFields(
    const RealizedClassNode &resolved_node,
    const RealizedPropertyAccessor &accessor,
    const EmittedPropertyDescriptor &descriptor,
    objc3_runtime_property_entry_snapshot &snapshot) {
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
}

}  // namespace objc3c::runtime
