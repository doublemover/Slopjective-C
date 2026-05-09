#include "runtime/storage/property_layout_realization.h"

#include "runtime/images/image_descriptor.h"
#include "runtime/metadata/runtime_emitted_records.h"
#include "runtime/metadata/runtime_realized_records.h"
#include "runtime/state/runtime_state_records.h"
#include "runtime/storage/property_accessors.h"
#include "runtime/storage/property_ivar_layout_index.h"
#include "runtime/storage/property_layout_rules.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <string>
#include <utility>

namespace objc3c::runtime {

bool AttachRealizedPropertyLayoutRecordsUnlocked(RuntimeState &state,
                                                 RealizedClassNode &node) {
  // instance-allocation-layout-runtime anchor: realized classes now
  // eagerly consume emitted property and ivar metadata into a runtime-owned
  // layout/accessor view so alloc/new and synthesized accessors can execute
  // against per-instance storage owned by runtime records.
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
  RuntimePropertyIvarLayoutIndex ivar_layout_index;
  if (!BuildRuntimePropertyIvarLayoutIndex(*node.image, ivar_owner_identity,
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
    const EmittedIvarDescriptor *ivar_descriptor =
        FindRuntimePropertyIvarDescriptorByBinding(ivar_layout_index,
                                                   *descriptor);
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
