#include "runtime/images/registration.h"

#include "runtime/classes/class_graph.h"
#include "runtime/dispatch/method_cache.h"
#include "runtime/images/image_descriptor.h"
#include "runtime/images/registration_snapshots.h"
#include "runtime/metadata/runtime_emitted_records.h"
#include "runtime/metadata/runtime_registration_records.h"
#include "runtime/selectors/keypath_table.h"
#include "runtime/selectors/selector_table.h"
#include "runtime/state/runtime_reset.h"
#include "runtime/state/runtime_state_clear.h"
#include "runtime/state/runtime_state_records.h"
#include "runtime/state/runtime_state_store.h"

#include <cstdint>
#include <mutex>
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

void MarkRejectedRegistrationUnlocked(
    RuntimeState &state, const objc3_runtime_image_descriptor *image,
    int status) {
  state.last_registration_status = status;
  state.last_rejected_module_name =
      image != nullptr && image->module_name != nullptr ? image->module_name : "";
  state.last_rejected_translation_unit_identity_key =
      image != nullptr && image->translation_unit_identity_key != nullptr
          ? image->translation_unit_identity_key
          : "";
  state.last_rejected_registration_order_ordinal =
      image != nullptr ? image->registration_order_ordinal : 0;
}

void ClearRejectedRegistrationUnlocked(RuntimeState &state) {
  state.last_rejected_module_name.clear();
  state.last_rejected_translation_unit_identity_key.clear();
  state.last_rejected_registration_order_ordinal = 0;
}

void ApplyImageWalkRecordUnlocked(RuntimeState &state,
                                  const RegisteredImageMetadata &record) {
  state.walked_image_count = static_cast<std::uint64_t>(
      state.registered_image_metadata_by_identity_key.size());
  state.last_discovery_root_entry_count = record.discovery_root_entry_count;
  state.last_walked_class_descriptor_count = record.class_descriptor_count;
  state.last_walked_protocol_descriptor_count = record.protocol_descriptor_count;
  state.last_walked_category_descriptor_count = record.category_descriptor_count;
  state.last_walked_property_descriptor_count = record.property_descriptor_count;
  state.last_walked_ivar_descriptor_count = record.ivar_descriptor_count;
  state.last_walked_selector_pool_count = record.selector_pool_count;
  state.last_walked_string_pool_count = record.string_pool_count;
  state.last_walked_keypath_descriptor_count = record.keypath_descriptor_count;
  state.last_linker_anchor_matches_discovery_root =
      record.linker_anchor_matches_discovery_root;
  state.last_registration_used_staged_table =
      record.used_staged_registration_table;
  state.last_walked_module_name = record.module_name;
  state.last_walked_translation_unit_identity_key =
      record.translation_unit_identity_key;
}

void RetainBootstrapRecordUnlocked(RuntimeState &state,
                                   const RegisteredImageMetadata &record) {
  const auto found = state.retained_bootstrap_metadata_by_identity_key.find(
      record.translation_unit_identity_key);
  if (found == state.retained_bootstrap_metadata_by_identity_key.end()) {
    state.retained_bootstrap_identity_order.push_back(
        record.translation_unit_identity_key);
  }
  state.retained_bootstrap_metadata_by_identity_key
      [record.translation_unit_identity_key] = record;
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

int RegisterImageUnlocked(
    RuntimeState &state, const objc3_runtime_image_descriptor *image,
    const objc3_runtime_registration_table *staged_registration_table,
    bool retain_bootstrap_record, bool mark_image_local_init_state) {
  // runtime-bootstrap-table-consumption anchor: duplicate identity rejection
  // and out-of-order rejection happen before live counters advance, while
  // successful staged-table consumption is the only path allowed to publish
  // bootstrap-visible image-walk state.
  (void)RuntimeImageDescriptorOwnershipModel();
  if (!RuntimeImageDescriptorHasRequiredIdentity(image)) {
    MarkRejectedRegistrationUnlocked(
        state, image, OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR);
    return OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR;
  }

  if (state.registration_order_by_identity_key.find(
          image->translation_unit_identity_key) !=
      state.registration_order_by_identity_key.end()) {
    MarkRejectedRegistrationUnlocked(
        state, image,
        OBJC3_RUNTIME_REGISTRATION_STATUS_DUPLICATE_TRANSLATION_UNIT_IDENTITY_KEY);
    return OBJC3_RUNTIME_REGISTRATION_STATUS_DUPLICATE_TRANSLATION_UNIT_IDENTITY_KEY;
  }

  if (image->registration_order_ordinal !=
      state.next_expected_registration_order_ordinal) {
    MarkRejectedRegistrationUnlocked(
        state, image,
        OBJC3_RUNTIME_REGISTRATION_STATUS_OUT_OF_ORDER_REGISTRATION);
    return OBJC3_RUNTIME_REGISTRATION_STATUS_OUT_OF_ORDER_REGISTRATION;
  }

  std::uint64_t descriptor_total = RuntimeDescriptorTotal(image);
  if (staged_registration_table != nullptr) {
    state.registration_order_by_identity_key.reserve(
        state.registration_order_by_identity_key.size() + 1u);
    state.registered_image_metadata_by_identity_key.reserve(
        state.registered_image_metadata_by_identity_key.size() + 1u);
    RegisteredImageMetadata record;
    if (!TryWalkRegistrationTableUnlocked(state, staged_registration_table,
                                          image, record)) {
      ClearImageWalkSnapshotUnlocked(state);
      MarkRejectedRegistrationUnlocked(
          state, image,
          OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_REGISTRATION_ROOTS);
      return OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_REGISTRATION_ROOTS;
    }
    descriptor_total = record.class_descriptor_count +
                       record.protocol_descriptor_count +
                       record.category_descriptor_count +
                       record.property_descriptor_count +
                       record.ivar_descriptor_count;
    if (retain_bootstrap_record) {
      RetainBootstrapRecordUnlocked(state, record);
    }
    state.registered_image_metadata_by_identity_key
        [record.translation_unit_identity_key] = record;
    ApplyImageWalkRecordUnlocked(
        state,
        state.registered_image_metadata_by_identity_key.at(
            record.translation_unit_identity_key));
    if (mark_image_local_init_state &&
        staged_registration_table->image_local_init_state != nullptr) {
      *staged_registration_table->image_local_init_state = 1;
    }
  } else {
    ClearImageWalkSnapshotUnlocked(state);
  }

  ++state.registered_image_count;
  state.registered_descriptor_total += descriptor_total;
  state.next_expected_registration_order_ordinal =
      image->registration_order_ordinal + 1;
  state.last_successful_registration_order_ordinal =
      image->registration_order_ordinal;
  state.last_registration_status = OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
  state.last_registered_module_name = image->module_name;
  state.last_registered_translation_unit_identity_key =
      image->translation_unit_identity_key;
  state.registration_order_by_identity_key.emplace(
      image->translation_unit_identity_key, image->registration_order_ordinal);
  RebuildRealizedClassGraphUnlocked(state);
  ClearRejectedRegistrationUnlocked(state);
  ClearMethodCacheStateUnlocked(state);
  SeedDispatchIntentFastPathCacheUnlocked(state);
  return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
}

}  // namespace objc3c::runtime

extern "C" void objc3_runtime_stage_registration_table_for_bootstrap(
    const objc3_runtime_registration_table *registration_table) {
  objc3c::runtime::RuntimeState &state =
      objc3c::runtime::ProcessRuntimeState();
  std::lock_guard<std::mutex> lock(state.mutex);
  state.staged_registration_table = registration_table;
}

extern "C" int objc3_runtime_register_image(
    const objc3_runtime_image_descriptor *image) {
  objc3c::runtime::RuntimeState &state =
      objc3c::runtime::ProcessRuntimeState();
  std::lock_guard<std::mutex> lock(state.mutex);
  const objc3_runtime_registration_table *const staged_registration_table =
      state.staged_registration_table;
  // runtime-bootstrap-table-consumption anchor: staging is one-shot and is
  // consumed by the next public registration call only.
  state.staged_registration_table = nullptr;
  return objc3c::runtime::RegisterImageUnlocked(
      state, image, staged_registration_table, true, false);
}

extern "C" int objc3_runtime_replay_registered_images_for_testing(void) {
  objc3c::runtime::RuntimeState &state =
      objc3c::runtime::ProcessRuntimeState();
  std::lock_guard<std::mutex> lock(state.mutex);
  return objc3c::runtime::ReplayRegisteredImagesForTestingUnlocked(
      state, objc3c::runtime::RegisterImageUnlocked);
}
