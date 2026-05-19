#pragma once

#include <cstddef>
#include <cstdint>

namespace objc3c::runtime {

enum class DispatchFamily;
struct RealizedClassNode;
struct RuntimeState;

std::uint64_t RuntimeReceiverIdentityBase();
std::uint64_t RuntimeReceiverIdentityStride();
std::uint64_t BuildReceiverBaseIdentity(std::size_t ordinal);
bool IsRuntimeReceiverBaseIdentity(std::uint64_t base_identity);
bool DecodeReceiverIdentity(const RuntimeState &state, int receiver,
                            std::uint64_t &base_identity,
                            DispatchFamily &family,
                            std::uint64_t &normalized_receiver_identity);
const RealizedClassNode *FindRealizedClassNodeByBaseIdentityUnlocked(
    const RuntimeState &state, std::uint64_t base_identity);

}  // namespace objc3c::runtime
