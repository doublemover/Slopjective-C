#include "runtime/selectors/selector_snapshot.h"

#include "runtime/metadata/runtime_registration_records.h"
#include "runtime/selectors/selector_records.h"
#include "runtime/selectors/selector_spelling.h"
#include "runtime/state/runtime_state_records.h"
#include "runtime/strings/borrowed_string.h"

namespace objc3c::runtime {

void CopySelectorLookupTableStateSnapshotUnlocked(
    const RuntimeState &state,
    objc3_runtime_selector_lookup_table_state_snapshot *snapshot) {
  snapshot->selector_table_entry_count =
      static_cast<std::uint64_t>(state.selector_slots.size());
  snapshot->metadata_backed_selector_count =
      state.metadata_backed_selector_count;
  snapshot->dynamic_selector_count = state.dynamic_selector_count;
  snapshot->metadata_provider_edge_count = state.metadata_provider_edge_count;
  snapshot->last_materialized_selector =
      BorrowRuntimeCString(state.last_materialized_selector);
  snapshot->last_materialized_stable_id = state.last_materialized_stable_id;
  snapshot->last_materialized_registration_order_ordinal =
      state.last_materialized_registration_order_ordinal;
  snapshot->last_materialized_selector_pool_index =
      state.last_materialized_selector_pool_index;
  snapshot->last_materialized_from_metadata =
      state.last_materialized_from_metadata ? 1 : 0;
}

void InitializeSelectorLookupEntrySnapshot(
    objc3_runtime_selector_lookup_entry_snapshot *snapshot) {
  snapshot->found = 0;
  snapshot->metadata_backed = 0;
  snapshot->stable_id = 0;
  snapshot->metadata_provider_count = 0;
  snapshot->first_registration_order_ordinal = 0;
  snapshot->last_registration_order_ordinal = 0;
  snapshot->first_selector_pool_index = 0;
  snapshot->last_selector_pool_index = 0;
  snapshot->canonical_selector = nullptr;
}

void CopySelectorLookupEntrySnapshotUnlocked(
    const RuntimeState &state,
    const char *selector,
    objc3_runtime_selector_lookup_entry_snapshot *snapshot) {
  InitializeSelectorLookupEntrySnapshot(snapshot);

  const char *canonical_selector = NormalizeRuntimeSelectorSpelling(selector);
  if (canonical_selector == nullptr) {
    return;
  }

  const SelectorSlot *slot =
      FindSelectorSlotByCanonicalSpellingUnlocked(state, canonical_selector);
  if (slot == nullptr) {
    return;
  }

  snapshot->found = 1;
  snapshot->metadata_backed = slot->metadata_backed ? 1 : 0;
  snapshot->stable_id = slot->handle.stable_id;
  snapshot->metadata_provider_count = slot->metadata_provider_count;
  snapshot->first_registration_order_ordinal =
      slot->first_registration_order_ordinal;
  snapshot->last_registration_order_ordinal =
      slot->last_registration_order_ordinal;
  snapshot->first_selector_pool_index = slot->first_selector_pool_index;
  snapshot->last_selector_pool_index = slot->last_selector_pool_index;
  snapshot->canonical_selector = slot->handle.selector;
}

}  // namespace objc3c::runtime
