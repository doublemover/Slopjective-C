#pragma once

#include "runtime/public/objc3_runtime_api.h"

#include <cstdint>

namespace objc3c::runtime {

struct RuntimeState;

bool RuntimeSelectorTableAcceptsDynamicSelector(const char *selector);
bool RuntimeSelectorTableAcceptsMetadataSelector(const char *selector);
const objc3_runtime_selector_handle *LookupSelectorUnlocked(
    const char *selector);
bool MaterializeSelectorLookupEntryUnlocked(
    RuntimeState &state,
    const char *selector,
    std::uint64_t registration_order_ordinal,
    std::uint64_t selector_pool_index);

}  // namespace objc3c::runtime
