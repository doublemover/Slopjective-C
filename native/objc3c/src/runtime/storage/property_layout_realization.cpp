#include "runtime/storage/property_layout_realization.h"

#include "runtime/dispatch/runtime_method_return.h"
#include "runtime/images/image_descriptor.h"
#include "runtime/metadata/runtime_emitted_records.h"
#include "runtime/metadata/runtime_realized_records.h"
#include "runtime/state/runtime_state_records.h"
#include "runtime/storage/ivar_layout.h"
#include "runtime/storage/property_accessors.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <string>
#include <tuple>
#include <unordered_map>
#include <utility>

namespace objc3c::runtime {
namespace {

std::size_t AlignPropertyStorageSize(std::size_t value,
                                     std::size_t alignment) {
  const std::size_t effective_alignment = std::max<std::size_t>(alignment, 1u);
  const std::size_t remainder = value % effective_alignment;
  return remainder == 0u ? value : value + (effective_alignment - remainder);
}

std::string BuildSynthesizedInstanceMethodOwnerIdentity(
    const char *declaration_owner_identity, const char *selector) {
  const std::string owner =
      declaration_owner_identity != nullptr ? declaration_owner_identity : "";
  const std::string selector_text = selector != nullptr ? selector : "";
  return owner + "::instance_method:" + selector_text;
}

RuntimeMethodReturnKind ClassifySynthesizedPropertyReturnType(
    const EmittedPropertyDescriptor &descriptor) {
  return ClassifyRuntimeReturnType(descriptor.type_name);
}

}  // namespace

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
    if (descriptor->layout_record == nullptr || !descriptor->layout_valid ||
        !descriptor->layout_record->layout_valid ||
        descriptor->layout_record->layout_replay_key == nullptr ||
        descriptor->layout_record->layout_replay_key[0] == '\0' ||
        descriptor->layout_replay_key == nullptr ||
        descriptor->layout_replay_key[0] == '\0') {
      return false;
    }
    const std::size_t offset =
        descriptor->offset_global != nullptr
            ? static_cast<std::size_t>(*descriptor->offset_global)
            : static_cast<std::size_t>(descriptor->offset_bytes);
    const std::size_t size = static_cast<std::size_t>(descriptor->size_bytes);
    const std::size_t alignment = std::max<std::size_t>(
        static_cast<std::size_t>(descriptor->alignment_bytes), 1u);
    if (descriptor->layout_record->offset_bytes != descriptor->offset_bytes ||
        descriptor->layout_record->size_bytes != descriptor->size_bytes ||
        descriptor->layout_record->alignment_bytes !=
            descriptor->alignment_bytes ||
        descriptor->layout_record->padding_bytes != descriptor->padding_bytes ||
        descriptor->layout_record->inherited_slot_count !=
            descriptor->inherited_slot_count ||
        descriptor->layout_record->inherited_size_bytes !=
            descriptor->inherited_size_bytes ||
        descriptor->layout_record->owner_size_bytes !=
            descriptor->owner_size_bytes ||
        descriptor->layout_record->init_order_index !=
            descriptor->init_order_index ||
        descriptor->layout_record->destroy_order_index !=
            descriptor->destroy_order_index ||
        offset != static_cast<std::size_t>(descriptor->offset_bytes) ||
        (alignment != 0u && offset % alignment != 0u)) {
      return false;
    }
    if (size != 0u &&
        !RuntimeIvarLayoutSlotIsAddressable(
            offset, size, std::numeric_limits<std::size_t>::max())) {
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
                            AlignPropertyStorageSize(max_end, max_alignment));

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
        ClassifySynthesizedPropertyReturnType(*descriptor);
    accessor.getter_owner_identity = BuildSynthesizedInstanceMethodOwnerIdentity(
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
      accessor.setter_owner_identity = BuildSynthesizedInstanceMethodOwnerIdentity(
          descriptor->declaration_owner_identity,
          descriptor->effective_setter_selector);
    }
    node.runtime_property_accessors.push_back(std::move(accessor));
  }

  std::sort(node.runtime_property_accessors.begin(),
            node.runtime_property_accessors.end(),
            [](const RealizedPropertyAccessor &lhs,
               const RealizedPropertyAccessor &rhs) {
              const std::string lhs_owner =
                  lhs.property_descriptor != nullptr &&
                          lhs.property_descriptor->declaration_owner_identity !=
                              nullptr
                      ? lhs.property_descriptor->declaration_owner_identity
                      : "";
              const std::string rhs_owner =
                  rhs.property_descriptor != nullptr &&
                          rhs.property_descriptor->declaration_owner_identity !=
                              nullptr
                      ? rhs.property_descriptor->declaration_owner_identity
                      : "";
              const std::string lhs_name =
                  lhs.property_descriptor != nullptr &&
                          lhs.property_descriptor->property_name != nullptr
                      ? lhs.property_descriptor->property_name
                      : "";
              const std::string rhs_name =
                  rhs.property_descriptor != nullptr &&
                          rhs.property_descriptor->property_name != nullptr
                      ? rhs.property_descriptor->property_name
                      : "";
              return std::tie(lhs_owner, lhs_name, lhs.getter_owner_identity,
                              lhs.setter_owner_identity) <
                     std::tie(rhs_owner, rhs_name, rhs.getter_owner_identity,
                              rhs.setter_owner_identity);
            });
  node.runtime_layout_ready = !node.runtime_property_accessors.empty() ||
                              node.runtime_instance_size_bytes != 0u;
  return true;
}

}  // namespace objc3c::runtime
