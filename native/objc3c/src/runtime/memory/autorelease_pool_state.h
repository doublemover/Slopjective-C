#pragma once

#include "runtime/state/runtime_thread_records.h"

#include <cstdint>
#include <vector>

namespace objc3c::runtime {

struct RuntimeAutoreleasePoolState {
  std::vector<RuntimeAutoreleasePoolFrame> frames;
  std::uint64_t max_depth = 0;
  std::uint64_t drained_value_count = 0;
  int last_autoreleased_value = 0;
  int last_drained_autorelease_value = 0;
};

RuntimeAutoreleasePoolState &RuntimeAutoreleasePoolStateForCurrentThread();

}  // namespace objc3c::runtime
