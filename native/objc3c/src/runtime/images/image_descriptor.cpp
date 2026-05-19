#include "runtime/images/image_descriptor.h"

#include <string>

namespace objc3c::runtime {

std::uint64_t RuntimeDescriptorTotal(
    const objc3_runtime_image_descriptor *image) {
  if (image == nullptr) {
    return 0;
  }
  return image->class_descriptor_count + image->protocol_descriptor_count +
         image->category_descriptor_count + image->property_descriptor_count +
         image->ivar_descriptor_count;
}

bool RuntimeImageDescriptorHasRequiredIdentity(
    const objc3_runtime_image_descriptor *image) {
  return image != nullptr && image->module_name != nullptr &&
         image->module_name[0] != '\0' &&
         image->translation_unit_identity_key != nullptr &&
         image->translation_unit_identity_key[0] != '\0' &&
         image->registration_order_ordinal != 0;
}

std::uint64_t RuntimeAggregateCount(
    const objc3_runtime_pointer_aggregate *aggregate) {
  return aggregate == nullptr ? 0 : aggregate->count;
}

const void *RuntimeAggregateEntry(
    const objc3_runtime_pointer_aggregate *aggregate, std::uint64_t index) {
  if (aggregate == nullptr || index >= aggregate->count) {
    return nullptr;
  }
  return aggregate->entries[index];
}

bool RuntimeAggregateContainsPointer(
    const objc3_runtime_pointer_aggregate *aggregate, const void *target) {
  if (aggregate == nullptr || target == nullptr) {
    return false;
  }
  for (std::uint64_t index = 0; index < aggregate->count; ++index) {
    if (aggregate->entries[index] == target) {
      return true;
    }
  }
  return false;
}

bool RuntimeImageDescriptorsMatch(
    const objc3_runtime_image_descriptor *lhs,
    const objc3_runtime_image_descriptor *rhs) {
  if (lhs == nullptr || rhs == nullptr) {
    return false;
  }
  const std::string lhs_module =
      lhs->module_name != nullptr ? lhs->module_name : "";
  const std::string rhs_module =
      rhs->module_name != nullptr ? rhs->module_name : "";
  const std::string lhs_identity =
      lhs->translation_unit_identity_key != nullptr
          ? lhs->translation_unit_identity_key
          : "";
  const std::string rhs_identity =
      rhs->translation_unit_identity_key != nullptr
          ? rhs->translation_unit_identity_key
          : "";
  return lhs_module == rhs_module && lhs_identity == rhs_identity &&
         lhs->registration_order_ordinal == rhs->registration_order_ordinal &&
         lhs->class_descriptor_count == rhs->class_descriptor_count &&
         lhs->protocol_descriptor_count == rhs->protocol_descriptor_count &&
         lhs->category_descriptor_count == rhs->category_descriptor_count &&
         lhs->property_descriptor_count == rhs->property_descriptor_count &&
         lhs->ivar_descriptor_count == rhs->ivar_descriptor_count;
}

const char *RuntimeImageDescriptorOwnershipModel() {
  return "public-image-descriptor-strings-borrowed-during-call-then-copied-"
         "into-runtime-owned-registration-state";
}

}  // namespace objc3c::runtime
