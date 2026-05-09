#include "runtime/selectors/selector_table.h"

#include "runtime/selectors/selector_records.h"
#include "runtime/selectors/selector_spelling.h"
#include "runtime/state/runtime_state_records.h"

namespace objc3c::runtime {

bool MaterializeSelectorLookupEntryUnlocked(
    RuntimeState &state,
    const char *selector,
    std::uint64_t registration_order_ordinal,
    std::uint64_t selector_pool_index) {
  const char *canonical_selector = NormalizeRuntimeSelectorSpelling(selector);
  if (!RuntimeSelectorTableAcceptsMetadataSelector(canonical_selector)) {
    return false;
  }

  SelectorSlot *slot =
      FindSelectorSlotByCanonicalSpellingUnlocked(state, canonical_selector);
  if (slot == nullptr) {
    AppendMetadataSelectorSlotUnlocked(state, canonical_selector,
                                       registration_order_ordinal,
                                       selector_pool_index);
    return true;
  }

  PromoteSelectorSlotToMetadataBackedUnlocked(
      state, *slot, registration_order_ordinal, selector_pool_index);
  RecordSelectorMaterializationUnlocked(state, *slot,
                                        registration_order_ordinal,
                                        selector_pool_index, true);
  return true;
}

}  // namespace objc3c::runtime
