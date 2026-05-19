#include "runtime/images/registration_table_shape.h"

#include "runtime/images/image_descriptor.h"
#include "runtime/metadata/runtime_ownership_contracts.h"

namespace objc3c::runtime {

bool RuntimeRegistrationTableShapeIsSupported(
    const objc3_runtime_registration_table *registration_table,
    const objc3_runtime_image_descriptor *image) {
  return registration_table != nullptr &&
         registration_table->abi_version ==
             kObjc3RuntimeRegistrationTableAbiVersion &&
         registration_table->pointer_field_count ==
             kObjc3RuntimeRegistrationTablePointerFieldCount &&
         RuntimeOwnerSplitContractIsReady() &&
         registration_table->image_descriptor != nullptr &&
         RuntimeImageDescriptorsMatch(registration_table->image_descriptor,
                                      image) &&
         registration_table->discovery_root != nullptr &&
         registration_table->linker_anchor != nullptr &&
         registration_table->class_descriptor_root != nullptr &&
         registration_table->protocol_descriptor_root != nullptr &&
         registration_table->category_descriptor_root != nullptr &&
         registration_table->property_descriptor_root != nullptr &&
         registration_table->ivar_descriptor_root != nullptr &&
         registration_table->image_local_init_state != nullptr;
}

RuntimeRegistrationTableDescriptorCounts
ReadRuntimeRegistrationTableDescriptorCounts(
    const objc3_runtime_registration_table *registration_table) {
  RuntimeRegistrationTableDescriptorCounts counts;
  counts.discovery_root_entry_count =
      RuntimeAggregateCount(registration_table->discovery_root);
  counts.class_descriptor_count =
      RuntimeAggregateCount(registration_table->class_descriptor_root);
  counts.protocol_descriptor_count =
      RuntimeAggregateCount(registration_table->protocol_descriptor_root);
  counts.category_descriptor_count =
      RuntimeAggregateCount(registration_table->category_descriptor_root);
  counts.property_descriptor_count =
      RuntimeAggregateCount(registration_table->property_descriptor_root);
  counts.ivar_descriptor_count =
      RuntimeAggregateCount(registration_table->ivar_descriptor_root);
  counts.selector_pool_count =
      RuntimeAggregateCount(registration_table->selector_pool_root);
  counts.string_pool_count =
      RuntimeAggregateCount(registration_table->string_pool_root);
  counts.keypath_descriptor_count =
      RuntimeAggregateCount(registration_table->keypath_descriptor_root);
  return counts;
}

bool RuntimeRegistrationTableDescriptorCountsMatchImage(
    const RuntimeRegistrationTableDescriptorCounts &counts,
    const objc3_runtime_image_descriptor *image) {
  return image != nullptr &&
         counts.class_descriptor_count == image->class_descriptor_count &&
         counts.protocol_descriptor_count ==
             image->protocol_descriptor_count &&
         counts.category_descriptor_count == image->category_descriptor_count &&
         counts.property_descriptor_count == image->property_descriptor_count &&
         counts.ivar_descriptor_count == image->ivar_descriptor_count;
}

bool RuntimeRegistrationTableDiscoveryRootsAreClosed(
    const objc3_runtime_registration_table *registration_table,
    bool &linker_anchor_matches_discovery_root) {
  linker_anchor_matches_discovery_root = false;
  if (registration_table == nullptr ||
      registration_table->discovery_root == nullptr ||
      registration_table->linker_anchor == nullptr ||
      registration_table->class_descriptor_root == nullptr ||
      registration_table->protocol_descriptor_root == nullptr ||
      registration_table->category_descriptor_root == nullptr ||
      registration_table->property_descriptor_root == nullptr ||
      registration_table->ivar_descriptor_root == nullptr) {
    return false;
  }
  const void *const linker_anchor_target =
      *reinterpret_cast<const void *const *>(registration_table->linker_anchor);
  linker_anchor_matches_discovery_root =
      linker_anchor_target == registration_table->discovery_root;
  return RuntimeAggregateCount(registration_table->discovery_root) >= 6 &&
         linker_anchor_matches_discovery_root &&
         RuntimeAggregateContainsPointer(
             registration_table->discovery_root,
             registration_table->class_descriptor_root) &&
         RuntimeAggregateContainsPointer(
             registration_table->discovery_root,
             registration_table->protocol_descriptor_root) &&
         RuntimeAggregateContainsPointer(
             registration_table->discovery_root,
             registration_table->category_descriptor_root) &&
         RuntimeAggregateContainsPointer(
             registration_table->discovery_root,
             registration_table->property_descriptor_root) &&
         RuntimeAggregateContainsPointer(
             registration_table->discovery_root,
             registration_table->ivar_descriptor_root) &&
         (registration_table->selector_pool_root == nullptr ||
          RuntimeAggregateContainsPointer(
              registration_table->discovery_root,
              registration_table->selector_pool_root)) &&
         (registration_table->string_pool_root == nullptr ||
          RuntimeAggregateContainsPointer(
              registration_table->discovery_root,
              registration_table->string_pool_root)) &&
         (registration_table->keypath_descriptor_root == nullptr ||
          RuntimeAggregateContainsPointer(
              registration_table->discovery_root,
              registration_table->keypath_descriptor_root));
}

}  // namespace objc3c::runtime
