#include "runtime/images/registration_table_walk.h"

#include "runtime/classes/class_metadata_tables.h"
#include "runtime/classes/protocol_conformance.h"
#include "runtime/images/image_descriptor.h"
#include "runtime/images/registration_table_record.h"
#include "runtime/metadata/runtime_emitted_records.h"
#include "runtime/metadata/runtime_registration_records.h"
#include "runtime/selectors/keypath_table.h"
#include "runtime/selectors/selector_table.h"
#include "runtime/state/runtime_state_records.h"

#include <cstddef>
#include <cstdint>
#include <deque>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace objc3c::runtime {

namespace {

struct RuntimeRegistrationTableWalkMutationCheckpoint {
  std::unordered_map<std::string, std::size_t> selector_index_by_name;
  std::deque<SelectorSlot> selector_slots;
  std::unordered_map<std::uint64_t, KeyPathSlot> keypath_slots;
  std::uint64_t metadata_backed_selector_count = 0;
  std::uint64_t dynamic_selector_count = 0;
  std::uint64_t metadata_provider_edge_count = 0;
  std::uint64_t image_backed_keypath_count = 0;
  std::uint64_t ambiguous_keypath_handle_count = 0;
  std::string last_materialized_selector;
  std::uint64_t last_materialized_stable_id = 0;
  std::uint64_t last_materialized_registration_order_ordinal = 0;
  std::uint64_t last_materialized_selector_pool_index = 0;
  bool last_materialized_from_metadata = false;
  std::uint64_t last_materialized_keypath_handle = 0;
  std::uint64_t last_materialized_keypath_registration_order_ordinal = 0;
  std::string last_materialized_keypath_profile;

  explicit RuntimeRegistrationTableWalkMutationCheckpoint(
      const RuntimeState &state)
      : selector_index_by_name(state.selector_index_by_name),
        selector_slots(state.selector_slots),
        keypath_slots(state.keypath_slots),
        metadata_backed_selector_count(state.metadata_backed_selector_count),
        dynamic_selector_count(state.dynamic_selector_count),
        metadata_provider_edge_count(state.metadata_provider_edge_count),
        image_backed_keypath_count(state.image_backed_keypath_count),
        ambiguous_keypath_handle_count(state.ambiguous_keypath_handle_count),
        last_materialized_selector(state.last_materialized_selector),
        last_materialized_stable_id(state.last_materialized_stable_id),
        last_materialized_registration_order_ordinal(
            state.last_materialized_registration_order_ordinal),
        last_materialized_selector_pool_index(
            state.last_materialized_selector_pool_index),
        last_materialized_from_metadata(state.last_materialized_from_metadata),
        last_materialized_keypath_handle(
            state.last_materialized_keypath_handle),
        last_materialized_keypath_registration_order_ordinal(
            state.last_materialized_keypath_registration_order_ordinal),
        last_materialized_keypath_profile(
            state.last_materialized_keypath_profile) {}

  void Restore(RuntimeState &state) const {
    state.selector_index_by_name = selector_index_by_name;
    state.selector_slots = selector_slots;
    for (SelectorSlot &slot : state.selector_slots) {
      slot.handle.selector = slot.spelling_storage.c_str();
    }
    state.keypath_slots = keypath_slots;
    state.metadata_backed_selector_count = metadata_backed_selector_count;
    state.dynamic_selector_count = dynamic_selector_count;
    state.metadata_provider_edge_count = metadata_provider_edge_count;
    state.image_backed_keypath_count = image_backed_keypath_count;
    state.ambiguous_keypath_handle_count = ambiguous_keypath_handle_count;
    state.last_materialized_selector = last_materialized_selector;
    state.last_materialized_stable_id = last_materialized_stable_id;
    state.last_materialized_registration_order_ordinal =
        last_materialized_registration_order_ordinal;
    state.last_materialized_selector_pool_index =
        last_materialized_selector_pool_index;
    state.last_materialized_from_metadata = last_materialized_from_metadata;
    state.last_materialized_keypath_handle = last_materialized_keypath_handle;
    state.last_materialized_keypath_registration_order_ordinal =
        last_materialized_keypath_registration_order_ordinal;
    state.last_materialized_keypath_profile = last_materialized_keypath_profile;
  }
};

} // namespace

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

  const RuntimeRegistrationTableDescriptorCounts counts =
      ReadRuntimeRegistrationTableDescriptorCounts(registration_table);
  state.selector_index_by_name.reserve(
      state.selector_index_by_name.size() +
      static_cast<std::size_t>(counts.selector_pool_count));
  state.keypath_slots.reserve(
      state.keypath_slots.size() +
      static_cast<std::size_t>(counts.keypath_descriptor_count));
  bool linker_anchor_matches_discovery_root = false;
  if (!RuntimeRegistrationTableDescriptorCountsMatchImage(counts, image) ||
      !RuntimeRegistrationTableDiscoveryRootsAreClosed(
          registration_table, linker_anchor_matches_discovery_root)) {
    return false;
  }
  std::string class_metadata_diagnostic_reason;
  if (!RuntimeClassMetadataTableIsSupported(state, registration_table,
                                            class_metadata_diagnostic_reason)) {
    ++state.malformed_class_metadata_rejection_count;
    state.last_malformed_class_graph_reason =
        std::move(class_metadata_diagnostic_reason);
    return false;
  }
  std::string protocol_metadata_diagnostic_reason;
  if (!RuntimeProtocolCategoryMetadataTableIsSupported(
          state, registration_table, protocol_metadata_diagnostic_reason)) {
    ++state.malformed_class_metadata_rejection_count;
    state.last_malformed_class_graph_reason =
        std::move(protocol_metadata_diagnostic_reason);
    return false;
  }

  std::unordered_set<std::string> selector_pool_spelling_set;
  selector_pool_spelling_set.reserve(
      static_cast<std::size_t>(counts.selector_pool_count));
  std::vector<std::string> selector_pool_spellings;
  selector_pool_spellings.reserve(
      static_cast<std::size_t>(counts.selector_pool_count));
  for (std::uint64_t index = 0; index < counts.selector_pool_count; ++index) {
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
  for (std::uint64_t index = 0; index < counts.string_pool_count; ++index) {
    const char *value = reinterpret_cast<const char *>(
        RuntimeAggregateEntry(registration_table->string_pool_root, index));
    if (value == nullptr) {
      return false;
    }
  }

  const RuntimeRegistrationTableWalkMutationCheckpoint mutation_checkpoint(
      state);
  for (std::uint64_t index = 0; index < counts.keypath_descriptor_count;
       ++index) {
    const auto *descriptor = reinterpret_cast<const EmittedKeyPathDescriptor *>(
        RuntimeAggregateEntry(registration_table->keypath_descriptor_root,
                              index));
    if (descriptor == nullptr ||
        !MaterializeKeyPathDescriptorUnlocked(
            state, *descriptor, image->registration_order_ordinal)) {
      mutation_checkpoint.Restore(state);
      return false;
    }
  }

  PublishRegistrationTableRecord(record, registration_table, image, counts,
                                 linker_anchor_matches_discovery_root);

  for (std::size_t index = 0; index < selector_pool_spellings.size(); ++index) {
    if (!MaterializeSelectorLookupEntryUnlocked(
            state, selector_pool_spellings[index].c_str(),
            image->registration_order_ordinal,
            static_cast<std::uint64_t>(index + 1u))) {
      mutation_checkpoint.Restore(state);
      return false;
    }
  }
  return true;
}

} // namespace objc3c::runtime
