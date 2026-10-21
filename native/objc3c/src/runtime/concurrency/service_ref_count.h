#ifndef OBJC3_RUNTIME_SERVICE_REF_COUNT_H
#define OBJC3_RUNTIME_SERVICE_REF_COUNT_H

#include <atomic>
#include <cstdint>
#include <limits>

namespace objc3::services {

inline bool try_retain(std::atomic<uint32_t> &references) noexcept {
  uint32_t count = references.load(std::memory_order_relaxed);
  for (;;) {
    if (count == 0 || count == std::numeric_limits<uint32_t>::max())
      return false;
    if (references.compare_exchange_weak(
            count, count + 1, std::memory_order_relaxed,
            std::memory_order_relaxed))
      return true;
  }
}

} // namespace objc3::services

#endif
