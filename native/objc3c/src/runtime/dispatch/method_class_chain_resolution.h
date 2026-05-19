#pragma once

#include <cstdint>
#include <string>

namespace objc3c::runtime {

enum class DispatchFamily;
struct RealizedClassNode;
struct RuntimeState;
struct SlowPathResolution;

bool ResolveReceiverClassNameUnlocked(
    const RuntimeState &state,
    std::uint64_t base_identity,
    std::string &class_name,
    bool &ambiguous);
bool TryResolveMethodFromRealizedClassChainUnlocked(
    RuntimeState &state,
    const RealizedClassNode *start_node,
    DispatchFamily family,
    std::uint64_t normalized_receiver_identity,
    std::uint64_t selector_stable_id,
    const char *selector_spelling,
    SlowPathResolution &resolution,
    bool &ambiguous,
    std::uint64_t &category_probe_count,
    std::uint64_t &protocol_probe_count);

}  // namespace objc3c::runtime
