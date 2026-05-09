#include "runtime/images/registration_table_walk.h"

#include "runtime/images/image_descriptor.h"
#include "runtime/metadata/runtime_emitted_records.h"
#include "runtime/metadata/runtime_registration_records.h"
#include "runtime/selectors/keypath_table.h"
#include "runtime/selectors/selector_table.h"
#include "runtime/state/runtime_state_records.h"

#include <cstddef>
#include <cstdint>
#include <string>
#include <unordered_set>
#include <vector>

namespace objc3c::runtime {

bool RuntimeRegistrationTableShapeIsSupported(
    const objc3_runtime_registration_table *registration_table,
    const objc3_runtime_image_descriptor *image) {
  return registration_table != nullptr &&
         registration_table->abi_version == 2 &&
         registration_table->pointer_field_count == 12 &&
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

bool TryWalkRegistrationTableUnlocked(
    RuntimeState &state,
    const objc3_runtime_registration_table *registration_table,
    const objc3_runtime_image_descriptor *image,
    RegisteredImageMetadata &record) {
  // runtime-bootstrap-table-consumption anchor: staged registration tables
  // must match the image descriptor exactly and discovery-root membership must
  // close over every descriptor family before image state is published.
  if (!RuntimeRegistrationTableShapeIsSupported(registration_table, image)) {
    return false;
  }

  const void *const linker_anchor_target =
      *reinterpret_cast<const void *const *>(registration_table->linker_anchor);
  const std::uint64_t discovery_root_entry_count =
      RuntimeAggregateCount(registration_table->discovery_root);
  if (discovery_root_entry_count < 6) {
    return false;
  }

  const std::uint64_t class_descriptor_count =
      RuntimeAggregateCount(registration_table->class_descriptor_root);
  const std::uint64_t protocol_descriptor_count =
      RuntimeAggregateCount(registration_table->protocol_descriptor_root);
  const std::uint64_t category_descriptor_count =
      RuntimeAggregateCount(registration_table->category_descriptor_root);
  const std::uint64_t property_descriptor_count =
      RuntimeAggregateCount(registration_table->property_descriptor_root);
  const std::uint64_t ivar_descriptor_count =
      RuntimeAggregateCount(registration_table->ivar_descriptor_root);
  const std::uint64_t selector_pool_count =
      RuntimeAggregateCount(registration_table->selector_pool_root);
  const std::uint64_t string_pool_count =
      RuntimeAggregateCount(registration_table->string_pool_root);
  const std::uint64_t keypath_descriptor_count =
      RuntimeAggregateCount(registration_table->keypath_descriptor_root);
  state.selector_index_by_name.reserve(
      state.selector_index_by_name.size() +
      static_cast<std::size_t>(selector_pool_count));
  state.keypath_slots.reserve(
      state.keypath_slots.size() +
      static_cast<std::size_t>(keypath_descriptor_count));
  const bool linker_anchor_matches_discovery_root =
      linker_anchor_target == registration_table->discovery_root;

  if (class_descriptor_count != image->class_descriptor_count ||
      protocol_descriptor_count != image->protocol_descriptor_count ||
      category_descriptor_count != image->category_descriptor_count ||
      property_descriptor_count != image->property_descriptor_count ||
      ivar_descriptor_count != image->ivar_descriptor_count ||
      !linker_anchor_matches_discovery_root ||
      !RuntimeAggregateContainsPointer(registration_table->discovery_root,
                                       registration_table->class_descriptor_root) ||
      !RuntimeAggregateContainsPointer(
          registration_table->discovery_root,
          registration_table->protocol_descriptor_root) ||
      !RuntimeAggregateContainsPointer(
          registration_table->discovery_root,
          registration_table->category_descriptor_root) ||
      !RuntimeAggregateContainsPointer(
          registration_table->discovery_root,
          registration_table->property_descriptor_root) ||
      !RuntimeAggregateContainsPointer(registration_table->discovery_root,
                                       registration_table->ivar_descriptor_root)) {
    return false;
  }

  if (registration_table->selector_pool_root != nullptr &&
      !RuntimeAggregateContainsPointer(registration_table->discovery_root,
                                       registration_table->selector_pool_root)) {
    return false;
  }
  if (registration_table->string_pool_root != nullptr &&
      !RuntimeAggregateContainsPointer(registration_table->discovery_root,
                                       registration_table->string_pool_root)) {
    return false;
  }
  if (registration_table->keypath_descriptor_root != nullptr &&
      !RuntimeAggregateContainsPointer(
          registration_table->discovery_root,
          registration_table->keypath_descriptor_root)) {
    return false;
  }

  std::unordered_set<std::string> selector_pool_spelling_set;
  selector_pool_spelling_set.reserve(
      static_cast<std::size_t>(selector_pool_count));
  std::vector<std::string> selector_pool_spellings;
  selector_pool_spellings.reserve(static_cast<std::size_t>(selector_pool_count));
  for (std::uint64_t index = 0; index < selector_pool_count; ++index) {
    const char *selector = reinterpret_cast<const char *>(
        RuntimeAggregateEntry(registration_table->selector_pool_root, index));
    if (selector == nullptr || selector[0] == '\0') {
      return false;
    }
    const auto inserted = selector_pool_spelling_set.emplace(selector);
    if (!inserted.second) {
      return false;
    }
    selector_pool_spellings.emplace_back(selector);
  }
  for (std::uint64_t index = 0; index < string_pool_count; ++index) {
    const char *value = reinterpret_cast<const char *>(
        RuntimeAggregateEntry(registration_table->string_pool_root, index));
    if (value == nullptr) {
      return false;
    }
  }
  for (std::uint64_t index = 0; index < keypath_descriptor_count; ++index) {
    const auto *descriptor =
        reinterpret_cast<const EmittedKeyPathDescriptor *>(
            RuntimeAggregateEntry(registration_table->keypath_descriptor_root,
                                  index));
    if (descriptor == nullptr ||
        !MaterializeKeyPathDescriptorUnlocked(
            state, *descriptor, image->registration_order_ordinal)) {
      return false;
    }
  }

  record.module_name = image->module_name;
  record.translation_unit_identity_key = image->translation_unit_identity_key;
  record.registration_order_ordinal = image->registration_order_ordinal;
  record.registration_table = registration_table;
  record.discovery_root = registration_table->discovery_root;
  record.class_descriptor_root = registration_table->class_descriptor_root;
  record.protocol_descriptor_root = registration_table->protocol_descriptor_root;
  record.category_descriptor_root = registration_table->category_descriptor_root;
  record.property_descriptor_root = registration_table->property_descriptor_root;
  record.ivar_descriptor_root = registration_table->ivar_descriptor_root;
  record.selector_pool_root = registration_table->selector_pool_root;
  record.string_pool_root = registration_table->string_pool_root;
  record.keypath_descriptor_root = registration_table->keypath_descriptor_root;
  record.discovery_root_entry_count = discovery_root_entry_count;
  record.class_descriptor_count = class_descriptor_count;
  record.protocol_descriptor_count = protocol_descriptor_count;
  record.category_descriptor_count = category_descriptor_count;
  record.property_descriptor_count = property_descriptor_count;
  record.ivar_descriptor_count = ivar_descriptor_count;
  record.selector_pool_count = selector_pool_count;
  record.string_pool_count = string_pool_count;
  record.keypath_descriptor_count = keypath_descriptor_count;
  record.linker_anchor_matches_discovery_root =
      linker_anchor_matches_discovery_root;
  record.used_staged_registration_table = true;

  for (std::size_t index = 0; index < selector_pool_spellings.size(); ++index) {
    if (!MaterializeSelectorLookupEntryUnlocked(
            state, selector_pool_spellings[index].c_str(),
            image->registration_order_ordinal,
            static_cast<std::uint64_t>(index + 1u))) {
      return false;
    }
  }
  return true;
}

}  // namespace objc3c::runtime
