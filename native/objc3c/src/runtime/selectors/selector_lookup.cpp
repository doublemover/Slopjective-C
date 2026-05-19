#include "runtime/selectors/selector_table.h"

#include "runtime/selectors/selector_records.h"
#include "runtime/selectors/selector_spelling.h"
#include "runtime/state/runtime_state_records.h"
#include "runtime/state/runtime_state_store.h"

namespace objc3c::runtime {

const objc3_runtime_selector_handle *LookupSelectorUnlocked(
    const char *selector) {
  const char *canonical_selector = NormalizeRuntimeSelectorSpelling(selector);
  if (canonical_selector == nullptr) {
    return nullptr;
  }

  RuntimeState &state = ProcessRuntimeState();
  SelectorSlot *slot =
      FindSelectorSlotByCanonicalSpellingUnlocked(state, canonical_selector);
  if (slot != nullptr) {
    return &slot->handle;
  }

  SelectorSlot &stored =
      AppendDynamicSelectorSlotUnlocked(state, canonical_selector);
  return &stored.handle;
}

}  // namespace objc3c::runtime
