#include "runtime/storage/property_layout_rules.h"

#include "runtime/metadata/runtime_emitted_records.h"
#include "runtime/storage/ivar_layout.h"

#include <algorithm>
#include <limits>

namespace objc3c::runtime {

std::size_t AlignRuntimePropertyStorageSize(std::size_t value,
                                            std::size_t alignment) {
  const std::size_t effective_alignment = std::max<std::size_t>(alignment, 1u);
  const std::size_t remainder = value % effective_alignment;
  if (remainder == 0u) {
    return value;
  }
  const std::size_t padding = effective_alignment - remainder;
  if (value > std::numeric_limits<std::size_t>::max() - padding) {
    return value;
  }
  return value + padding;
}

bool RuntimePropertyIvarDescriptorHasStrictPublishedLayout(
    const EmittedIvarDescriptor &descriptor,
    std::size_t effective_offset,
    std::size_t effective_alignment) {
  return descriptor.layout_record != nullptr && descriptor.layout_valid &&
         descriptor.layout_record->layout_valid &&
         descriptor.layout_record->layout_replay_key != nullptr &&
         descriptor.layout_record->layout_replay_key[0] != '\0' &&
         descriptor.layout_replay_key != nullptr &&
         descriptor.layout_replay_key[0] != '\0' &&
         descriptor.layout_record->offset_bytes == descriptor.offset_bytes &&
         descriptor.layout_record->size_bytes == descriptor.size_bytes &&
         descriptor.layout_record->alignment_bytes ==
             descriptor.alignment_bytes &&
         descriptor.layout_record->padding_bytes == descriptor.padding_bytes &&
         descriptor.layout_record->inherited_slot_count ==
             descriptor.inherited_slot_count &&
         descriptor.layout_record->inherited_size_bytes ==
             descriptor.inherited_size_bytes &&
         descriptor.layout_record->owner_size_bytes ==
             descriptor.owner_size_bytes &&
         descriptor.layout_record->init_order_index ==
             descriptor.init_order_index &&
         descriptor.layout_record->destroy_order_index ==
             descriptor.destroy_order_index &&
         effective_offset == static_cast<std::size_t>(descriptor.offset_bytes) &&
         effective_alignment != 0u &&
         effective_offset % effective_alignment == 0u;
}

bool RuntimePropertyIvarStorageExtentIsAddressable(std::size_t offset,
                                                   std::size_t size) {
  return RuntimeIvarLayoutSlotIsAddressable(
      offset, size, std::numeric_limits<std::size_t>::max());
}

}  // namespace objc3c::runtime
