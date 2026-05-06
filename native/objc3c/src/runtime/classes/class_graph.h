#pragma once

#include <cstddef>
#include <cstdint>

namespace objc3c::runtime {

std::uint64_t RuntimeReceiverIdentityBase();
std::uint64_t RuntimeReceiverIdentityStride();
std::uint64_t BuildReceiverBaseIdentity(std::size_t ordinal);
bool IsRuntimeReceiverBaseIdentity(std::uint64_t base_identity);

}  // namespace objc3c::runtime
