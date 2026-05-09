#pragma once

#include <cstddef>
#include <cstdint>

namespace objc3c::runtime {

enum class DispatchFamily;
struct RuntimeState;
struct SlowPathResolution;

struct MethodCacheKey {
  std::uint64_t normalized_receiver_identity = 0;
  std::uint64_t selector_stable_id = 0;

  bool operator==(const MethodCacheKey &other) const;
};

struct MethodCacheKeyHash {
  std::size_t operator()(const MethodCacheKey &key) const;
};

SlowPathResolution ResolveMethodSlowPathUnlocked(
    RuntimeState &state,
    std::uint64_t base_identity,
    std::uint64_t normalized_receiver_identity,
    DispatchFamily family,
    std::uint64_t selector_stable_id,
    const char *selector_spelling);

}  // namespace objc3c::runtime
