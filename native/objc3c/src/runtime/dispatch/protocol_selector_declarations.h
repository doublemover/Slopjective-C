#pragma once

#include "runtime/state/runtime_bootstrap_contracts.h"

#include <cstdint>

namespace objc3c::runtime {

enum class DispatchFamily;
struct RuntimeState;

bool ProbeProtocolSelectorDeclarationsFromAggregateUnlocked(
    RuntimeState &state,
    const objc3_runtime_pointer_aggregate *protocol_refs,
    DispatchFamily family,
    std::uint64_t selector_stable_id,
    const char *selector_spelling,
    std::uint64_t &protocol_probe_count,
    bool &ambiguous);

}  // namespace objc3c::runtime
