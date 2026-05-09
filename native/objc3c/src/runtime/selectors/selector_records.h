#pragma once

#include "runtime/metadata/runtime_registration_records.h"

#include <cstdint>

namespace objc3c::runtime {

struct RuntimeState;

SelectorSlot *FindSelectorSlotByCanonicalSpellingUnlocked(
    RuntimeState &state,
    const char *canonical_selector);
const SelectorSlot *FindSelectorSlotByCanonicalSpellingUnlocked(
    const RuntimeState &state,
    const char *canonical_selector);
SelectorSlot &AppendDynamicSelectorSlotUnlocked(
    RuntimeState &state,
    const char *canonical_selector);
SelectorSlot &AppendMetadataSelectorSlotUnlocked(
    RuntimeState &state,
    const char *canonical_selector,
    std::uint64_t registration_order_ordinal,
    std::uint64_t selector_pool_index);
void PromoteSelectorSlotToMetadataBackedUnlocked(
    RuntimeState &state,
    SelectorSlot &slot,
    std::uint64_t registration_order_ordinal,
    std::uint64_t selector_pool_index);
void RecordSelectorMaterializationUnlocked(
    RuntimeState &state,
    const SelectorSlot &slot,
    std::uint64_t registration_order_ordinal,
    std::uint64_t selector_pool_index,
    bool from_metadata);

}  // namespace objc3c::runtime
