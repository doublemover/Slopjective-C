#include "runtime/dispatch/method_cache.h"

#include <functional>

namespace objc3c::runtime {

bool MethodCacheKey::operator==(const MethodCacheKey &other) const {
  return normalized_receiver_identity == other.normalized_receiver_identity &&
         selector_stable_id == other.selector_stable_id;
}

std::size_t MethodCacheKeyHash::operator()(const MethodCacheKey &key) const {
  return std::hash<std::uint64_t>{}(key.normalized_receiver_identity) ^
         (std::hash<std::uint64_t>{}(key.selector_stable_id) << 1u);
}

}  // namespace objc3c::runtime
