#pragma once

#include <cstdint>

namespace objc3c::runtime {

struct RealizedClassNode;
struct RuntimeState;
struct SlowPathResolution;

bool TryResolveRuntimeManagedPropertyAccessorUnlocked(
    const RuntimeState &state, const RealizedClassNode &start_node,
    std::uint64_t normalized_receiver_identity,
    std::uint64_t selector_stable_id, const char *selector_spelling,
    SlowPathResolution &resolution);

}  // namespace objc3c::runtime
