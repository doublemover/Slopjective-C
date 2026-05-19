#include "runtime/selectors/selector_records.h"

#include "runtime/state/runtime_state_records.h"

#include <cstddef>
#include <utility>

namespace objc3c::runtime {

SelectorSlot *FindSelectorSlotByCanonicalSpellingUnlocked(
    RuntimeState &state,
    const char *canonical_selector) {
  const auto found = state.selector_index_by_name.find(canonical_selector);
  if (found == state.selector_index_by_name.end()) {
    return nullptr;
  }
  return &state.selector_slots[found->second];
}

const SelectorSlot *FindSelectorSlotByCanonicalSpellingUnlocked(
    const RuntimeState &state,
    const char *canonical_selector) {
  const auto found = state.selector_index_by_name.find(canonical_selector);
  if (found == state.selector_index_by_name.end()) {
    return nullptr;
  }
  return &state.selector_slots[found->second];
}

namespace {

SelectorSlot &AppendSelectorSlotUnlocked(RuntimeState &state,
                                         const char *canonical_selector) {
  SelectorSlot slot;
  slot.spelling_storage = canonical_selector;
  state.selector_slots.push_back(std::move(slot));

  SelectorSlot &stored = state.selector_slots.back();
  stored.handle.selector = stored.spelling_storage.c_str();
  stored.handle.stable_id =
      static_cast<std::uint64_t>(state.selector_slots.size());

  const std::size_t index = state.selector_slots.size() - 1u;
  state.selector_index_by_name.emplace(stored.spelling_storage, index);
  return stored;
}

}  // namespace

SelectorSlot &AppendDynamicSelectorSlotUnlocked(
    RuntimeState &state,
    const char *canonical_selector) {
  SelectorSlot &stored =
      AppendSelectorSlotUnlocked(state, canonical_selector);
  ++state.dynamic_selector_count;
  RecordSelectorMaterializationUnlocked(state, stored, 0, 0, false);
  return stored;
}

SelectorSlot &AppendMetadataSelectorSlotUnlocked(
    RuntimeState &state,
    const char *canonical_selector,
    std::uint64_t registration_order_ordinal,
    std::uint64_t selector_pool_index) {
  SelectorSlot &stored =
      AppendSelectorSlotUnlocked(state, canonical_selector);
  stored.metadata_backed = true;
  stored.metadata_provider_count = 1;
  stored.first_registration_order_ordinal = registration_order_ordinal;
  stored.last_registration_order_ordinal = registration_order_ordinal;
  stored.first_selector_pool_index = selector_pool_index;
  stored.last_selector_pool_index = selector_pool_index;
  ++state.metadata_backed_selector_count;
  ++state.metadata_provider_edge_count;
  RecordSelectorMaterializationUnlocked(state, stored,
                                        registration_order_ordinal,
                                        selector_pool_index, true);
  return stored;
}

void PromoteSelectorSlotToMetadataBackedUnlocked(
    RuntimeState &state,
    SelectorSlot &slot,
    std::uint64_t registration_order_ordinal,
    std::uint64_t selector_pool_index) {
  if (!slot.metadata_backed) {
    slot.metadata_backed = true;
    slot.first_registration_order_ordinal = registration_order_ordinal;
    slot.first_selector_pool_index = selector_pool_index;
    ++state.metadata_backed_selector_count;
    if (state.dynamic_selector_count > 0) {
      --state.dynamic_selector_count;
    }
  }

  ++slot.metadata_provider_count;
  slot.last_registration_order_ordinal = registration_order_ordinal;
  slot.last_selector_pool_index = selector_pool_index;
  ++state.metadata_provider_edge_count;
}

void RecordSelectorMaterializationUnlocked(
    RuntimeState &state,
    const SelectorSlot &slot,
    std::uint64_t registration_order_ordinal,
    std::uint64_t selector_pool_index,
    bool from_metadata) {
  state.last_materialized_selector = slot.spelling_storage;
  state.last_materialized_stable_id = slot.handle.stable_id;
  state.last_materialized_registration_order_ordinal =
      registration_order_ordinal;
  state.last_materialized_selector_pool_index = selector_pool_index;
  state.last_materialized_from_metadata = from_metadata;
}

}  // namespace objc3c::runtime
