#include "runtime/storage/property_layout_realization.h"

#include "runtime/images/image_descriptor.h"
#include "runtime/metadata/runtime_emitted_records.h"
#include "runtime/metadata/runtime_realized_records.h"
#include "runtime/state/runtime_cache_invalidation.h"
#include "runtime/state/runtime_state_records.h"
#include "runtime/storage/property_accessor_records.h"
#include "runtime/storage/property_ivar_layout_index.h"

#include <algorithm>
#include <cstdint>
#include <string>
#include <unordered_set>
#include <utility>

namespace objc3c::runtime {

bool AttachRealizedPropertyLayoutRecordsUnlocked(RuntimeState &state,
                                                 RealizedClassNode &node) {
  // instance-allocation-layout-runtime anchor: realized classes now
  // eagerly consume emitted property and ivar metadata into a runtime-owned
  // layout/accessor view so alloc/new and synthesized accessors can execute
  // against per-instance storage owned by runtime records.
  node.runtime_property_accessors.clear();
  node.runtime_layout_ready = false;
  node.runtime_instance_size_bytes = 0;
  if (node.image == nullptr || node.image->property_descriptor_root == nullptr ||
      node.image->ivar_descriptor_root == nullptr ||
      node.bundle_owner_identity.empty()) {
    return false;
  }
  std::unordered_set<std::string> ivar_owner_identities;
  std::unordered_set<std::string> descriptor_owner_identities;
  const std::string class_storage_owner_identity =
      node.interface_owner_identity.empty() ? node.bundle_owner_identity
                                            : node.interface_owner_identity;
  ivar_owner_identities.insert(class_storage_owner_identity);
  descriptor_owner_identities.insert(node.bundle_owner_identity);
  descriptor_owner_identities.insert(class_storage_owner_identity);
  for (const EmittedCategoryRecord *category_record :
       node.attached_category_records) {
    if (category_record == nullptr || category_record->owner_identity == nullptr ||
        category_record->owner_identity[0] == '\0') {
      return false;
    }
    ivar_owner_identities.insert(category_record->owner_identity);
    descriptor_owner_identities.insert(category_record->owner_identity);
  }
  RuntimePropertyIvarLayoutIndex ivar_layout_index;
  if (!BuildRuntimePropertyIvarLayoutIndexForOwners(*node.image,
                                                   ivar_owner_identities,
                                                   ivar_layout_index)) {
    return false;
  }
  node.runtime_instance_size_bytes =
      RuntimePropertyIvarLayoutInstanceSize(ivar_layout_index);

  for (std::uint64_t index = 0; index < node.image->property_descriptor_count;
       ++index) {
    const auto *descriptor = static_cast<const EmittedPropertyDescriptor *>(
        RuntimeAggregateEntry(node.image->property_descriptor_root, index));
    if (descriptor == nullptr ||
        !RuntimePropertyDescriptorHasRealizableAccessorShape(*descriptor)) {
      return false;
    }
    if (descriptor->declaration_owner_identity == nullptr ||
        descriptor_owner_identities.find(
            descriptor->declaration_owner_identity) ==
            descriptor_owner_identities.end()) {
      continue;
    }
    if (!RuntimePropertyDescriptorDeclaresSynthesizedStorageBinding(
            *descriptor)) {
      continue;
    }
    const EmittedIvarDescriptor *ivar_descriptor =
        FindRuntimePropertyIvarDescriptorForProperty(ivar_layout_index,
                                                     *descriptor);
    if (ivar_descriptor == nullptr) {
      continue;
    }
    RealizedPropertyAccessor accessor;
    if (!BuildRuntimePropertyAccessorRecord(*descriptor, *ivar_descriptor,
                                            accessor)) {
      return false;
    }
    node.runtime_property_accessors.push_back(std::move(accessor));
  }

  std::sort(node.runtime_property_accessors.begin(),
            node.runtime_property_accessors.end(),
            RuntimePropertyAccessorSortsBefore);
  node.runtime_layout_ready = !node.runtime_property_accessors.empty() ||
                              node.runtime_instance_size_bytes != 0u;
  if (node.runtime_layout_ready) {
    BumpRuntimeStorageSurfaceGenerationUnlocked(state);
    BumpRuntimeMethodSurfaceGenerationUnlocked(state);
  }
  return true;
}

}  // namespace objc3c::runtime
