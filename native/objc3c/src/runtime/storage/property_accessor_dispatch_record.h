#pragma once

#include <cstdint>

namespace objc3c::runtime {

struct RealizedClassNode;
struct SlowPathResolution;

bool TryResolveRuntimePropertyAccessorOnNode(
    const RealizedClassNode &node,
    std::uint64_t normalized_receiver_identity,
    std::uint64_t selector_stable_id,
    const char *selector_spelling,
    SlowPathResolution &resolution);

}  // namespace objc3c::runtime
