#include "runtime/classes/class_graph.h"

namespace objc3c::runtime {

namespace {

constexpr std::uint64_t kReceiverIdentityBase = 1024;
constexpr std::uint64_t kReceiverIdentityStride = 17;

}  // namespace

std::uint64_t RuntimeReceiverIdentityBase() {
  return kReceiverIdentityBase;
}

std::uint64_t RuntimeReceiverIdentityStride() {
  return kReceiverIdentityStride;
}

std::uint64_t BuildReceiverBaseIdentity(std::size_t ordinal) {
  return kReceiverIdentityBase +
         static_cast<std::uint64_t>(ordinal) * kReceiverIdentityStride;
}

bool IsRuntimeReceiverBaseIdentity(std::uint64_t base_identity) {
  return base_identity >= kReceiverIdentityBase &&
         ((base_identity - kReceiverIdentityBase) %
          kReceiverIdentityStride) == 0;
}

}  // namespace objc3c::runtime
