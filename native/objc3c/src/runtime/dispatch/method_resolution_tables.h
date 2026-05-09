#pragma once

#include "runtime/dispatch/method_list_resolution.h"
#include "runtime/dispatch/protocol_selector_declarations.h"

#include <cstdint>

namespace objc3c::runtime {

enum class DispatchFamily;
struct RealizedClassNode;
struct RuntimeState;
struct SlowPathResolution;

bool TryResolveMethodFromAttachedCategoriesUnlocked(
    RuntimeState &state,
    const RealizedClassNode &node,
    DispatchFamily family,
    std::uint64_t normalized_receiver_identity,
    std::uint64_t selector_stable_id,
    const char *selector_spelling,
    SlowPathResolution &resolution,
    bool &ambiguous,
    std::uint64_t &category_probe_count,
    std::uint64_t &protocol_probe_count);

}  // namespace objc3c::runtime
