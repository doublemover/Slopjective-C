#include "runtime/storage/property_layout_realization.h"

#include "runtime/images/image_descriptor.h"
#include "runtime/metadata/runtime_emitted_records.h"
#include "runtime/metadata/runtime_realized_records.h"
#include "runtime/state/runtime_state_records.h"
#include "runtime/storage/property_accessors.h"
#include "runtime/storage/property_layout_rules.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <utility>

namespace objc3c::runtime {

bool AttachRealizedPropertyLayoutRecordsUnlocked(RuntimeState &state,
                                                 RealizedClassNode &node) {
  // instance-allocation-layout-runtime anchor: realized classes now
  // eagerly consume emitted property and ivar metadata into a runtime-owned
  // layout/accessor view so alloc/new and synthesized accessors can execute
  // against per-instance storage instead of lane-C globals.
  (void)state;
  node.runtime_property_accessors.clear();
  node.runtime_layout_ready = false;
  node.runtime_instance_size_bytes = 0;
  if (node.image == nullptr || node.image->property_descriptor_root == nullptr ||
      node.image->ivar_descriptor_root == nullptr ||
      node.bundle_owner_identity.empty()) {
    return false;
  }
  const std::string ivar_owner_identity =
      node.interface_owner_identity.empty() ? node.bundle_owner_identity
                                            : node.interface_owner_identity;
  std::unordered_map<std::string, const EmittedIvarDescriptor *> ivar_by_binding;
  std::unordered_map<std::string, const EmittedIvarDescriptor *> ivar_by_property;
  std::size_t max_end = 0u;
  std::size_t max_alignment = 1u;
  std::size_t published_owner_size = 0u;
  for (std::uint64_t index = 0; index < node.image->ivar_descriptor_count;
       ++index) {
    const auto *descriptor = static_cast<const EmittedIvarDescriptor *>(
        RuntimeAggregateEntry(node.image->ivar_descriptor_root, index));
    if (descriptor == nullptr ||
        descriptor->declaration_owner_identity == nullptr ||
        descriptor->property_name == nullptr ||
        descriptor->ivar_binding_symbol == nullptr) {
      return false;
    }
    if (ivar_owner_identity != descriptor->declaration_owner_identity) {
      continue;
    }
    const std::size_t offset =
        descriptor->offset_global != nullptr
            ? static_cast<std::size_t>(*descriptor->offset_global)
            : static_cast<std::size_t>(descriptor->offset_bytes);
    const std::size_t size = static_cast<std::size_t>(descriptor->size_bytes);
    const std::size_t alignment = std::max<std::size_t>(
        static_cast<std::size_t>(descriptor->alignment_bytes), 1u);
    if (!RuntimePropertyIvarDescriptorHasStrictPublishedLayout(
            *descriptor, offset, alignment)) {
      return false;
    }
    if (size != 0u &&
        !RuntimePropertyIvarStorageExtentIsAddressable(offset, size)) {
      return false;
    }
    if (size != 0u) {
      max_end = std::max(max_end, offset + size);
      max_alignment = std::max(max_alignment, alignment);
    }
    published_owner_size =
        std::max<std::size_t>(published_owner_size,
                              static_cast<std::size_t>(
                                  descriptor->owner_size_bytes));
    ivar_by_binding.emplace(descriptor->ivar_binding_symbol, descriptor);
    ivar_by_property.emplace(descriptor->property_name, descriptor);
  }
  node.runtime_instance_size_bytes =
      std::max<std::size_t>(published_owner_size,
                            AlignRuntimePropertyStorageSize(max_end,
                                                            max_alignment));

  for (std::uint64_t index = 0; index < node.image->property_descriptor_count;
       ++index) {
    const auto *descriptor = static_cast<const EmittedPropertyDescriptor *>(
        RuntimeAggregateEntry(node.image->property_descriptor_root, index));
    if (descriptor == nullptr ||
        descriptor->declaration_owner_identity == nullptr ||
        descriptor->property_name == nullptr ||
        descriptor->effective_getter_selector == nullptr ||
        !RuntimePropertyAccessorSelectorIsMaterializable(
            descriptor->effective_getter_selector)) {
      return false;
    }
    if (node.bundle_owner_identity != descriptor->declaration_owner_identity) {
      continue;
    }
    if (descriptor->synthesized_binding_symbol == nullptr ||
        descriptor->synthesized_binding_symbol[0] == '\0') {
      continue;
    }
    const EmittedIvarDescriptor *ivar_descriptor = nullptr;
    if (descriptor->ivar_binding_symbol != nullptr &&
        descriptor->ivar_binding_symbol[0] != '\0') {
      const auto ivar_by_binding_it =
          ivar_by_binding.find(descriptor->ivar_binding_symbol);
      if (ivar_by_binding_it != ivar_by_binding.end()) {
        ivar_descriptor = ivar_by_binding_it->second;
      }
    }
    if (ivar_descriptor == nullptr) {
      const auto ivar_by_property_it =
          ivar_by_property.find(descriptor->property_name);
      if (ivar_by_property_it != ivar_by_property.end()) {
        ivar_descriptor = ivar_by_property_it->second;
      }
    }
    if (ivar_descriptor == nullptr) {
      continue;
    }
    RealizedPropertyAccessor accessor;
    accessor.property_descriptor = descriptor;
    accessor.ivar_descriptor = ivar_descriptor;
    accessor.getter_return_kind =
        ClassifyRuntimePropertyAccessorReturnType(*descriptor);
    accessor.getter_owner_identity = BuildRuntimePropertyAccessorOwnerIdentity(
        descriptor->declaration_owner_identity,
        descriptor->effective_getter_selector);
    if (descriptor->effective_setter_available &&
        descriptor->effective_setter_selector != nullptr &&
        descriptor->effective_setter_selector[0] != '\0') {
      if (!RuntimePropertyAccessorSelectorIsMaterializable(
              descriptor->effective_setter_selector) ||
          !RuntimePropertySetterHasSupportedArity(1)) {
        return false;
      }
      accessor.setter_owner_identity = BuildRuntimePropertyAccessorOwnerIdentity(
          descriptor->declaration_owner_identity,
          descriptor->effective_setter_selector);
    }
    node.runtime_property_accessors.push_back(std::move(accessor));
  }

  std::sort(node.runtime_property_accessors.begin(),
            node.runtime_property_accessors.end(),
            RuntimePropertyAccessorSortsBefore);
  node.runtime_layout_ready = !node.runtime_property_accessors.empty() ||
                              node.runtime_instance_size_bytes != 0u;
  return true;
}

}  // namespace objc3c::runtime
