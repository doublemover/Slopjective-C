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

namespace {

std::size_t RuntimeSuperclassStorageFloor(
    const RuntimeState &state,
    const RealizedClassNode &node) {
  std::size_t inherited_size_bytes = 0;
  const RealizedClassNode *cursor = &node;
  std::size_t visited_count = 0;
  while (cursor->has_super_node &&
         cursor->super_node_index < state.realized_class_nodes.size() &&
         visited_count < state.realized_class_nodes.size()) {
    const RealizedClassNode &super_node =
        state.realized_class_nodes[cursor->super_node_index];
    inherited_size_bytes =
        std::max(inherited_size_bytes,
                 super_node.runtime_instance_size_bytes);
    cursor = &super_node;
    ++visited_count;
  }
  return inherited_size_bytes;
}

void PublishInheritedOnlyRuntimeLayoutIfNeeded(
    RuntimeState &state,
    RealizedClassNode &node,
    std::size_t inherited_size_bytes) {
  if (inherited_size_bytes == 0u) {
    return;
  }
  node.runtime_instance_size_bytes = inherited_size_bytes;
  node.runtime_layout_ready = true;
  BumpRuntimeStorageSurfaceGenerationUnlocked(state);
}

}  // namespace

bool AttachRealizedPropertyLayoutRecordsUnlocked(RuntimeState &state,
                                                 RealizedClassNode &node) {
  // instance-allocation-layout-runtime anchor: realized classes now
  // eagerly consume emitted property and ivar metadata into a runtime-owned
  // layout/accessor view so alloc/new and synthesized accessors can execute
  // against per-instance storage owned by runtime records.
  node.runtime_property_accessors.clear();
  node.runtime_layout_ready = false;
  node.runtime_instance_size_bytes = 0;
  const std::size_t inherited_size_bytes =
      RuntimeSuperclassStorageFloor(state, node);
  if (node.image == nullptr || node.image->property_descriptor_root == nullptr ||
      node.image->ivar_descriptor_root == nullptr ||
      node.bundle_owner_identity.empty()) {
    PublishInheritedOnlyRuntimeLayoutIfNeeded(state, node,
                                             inherited_size_bytes);
    return node.runtime_layout_ready;
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
    PublishInheritedOnlyRuntimeLayoutIfNeeded(state, node,
                                             inherited_size_bytes);
    return false;
  }
  node.runtime_instance_size_bytes =
      std::max(RuntimePropertyIvarLayoutInstanceSize(ivar_layout_index),
               inherited_size_bytes);

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
