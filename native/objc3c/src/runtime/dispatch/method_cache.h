#pragma once

#include <cstddef>
#include <cstdint>

namespace objc3c::runtime {

struct MethodCacheKey {
  std::uint64_t normalized_receiver_identity = 0;
  std::uint64_t selector_stable_id = 0;

  bool operator==(const MethodCacheKey &other) const;
};

struct MethodCacheKeyHash {
  std::size_t operator()(const MethodCacheKey &key) const;
};

}  // namespace objc3c::runtime
