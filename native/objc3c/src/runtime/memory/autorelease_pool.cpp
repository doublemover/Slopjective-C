#include "runtime/memory/autorelease_pool.h"

#include "runtime/memory/autorelease_pool_state.h"

#include <algorithm>
#include <utility>

namespace objc3c::runtime {

bool RuntimeAutoreleasePoolCanEnqueue(int value) {
  return value != 0;
}

void ResetRuntimeAutoreleasepoolStateForTesting() {
  ResetRuntimeDispatchFrameStateForTesting();
  RuntimeAutoreleasePoolStateForCurrentThread() = RuntimeAutoreleasePoolState{};
}

void PushRuntimeAutoreleasePoolFrame() {
  RuntimeAutoreleasePoolState &state =
      RuntimeAutoreleasePoolStateForCurrentThread();
  state.frames.push_back({});
  state.max_depth = std::max<std::uint64_t>(
      state.max_depth, static_cast<std::uint64_t>(state.frames.size()));
}

std::vector<int> PopRuntimeAutoreleasePoolFrameValuesInDrainOrder() {
  RuntimeAutoreleasePoolState &state =
      RuntimeAutoreleasePoolStateForCurrentThread();
  if (state.frames.empty()) {
    return {};
  }
  RuntimeAutoreleasePoolFrame frame =
      std::move(state.frames.back());
  state.frames.pop_back();
  std::reverse(frame.values.begin(), frame.values.end());
  return std::move(frame.values);
}

std::uint64_t RuntimeAutoreleasePoolDepth() {
  const RuntimeAutoreleasePoolState &state =
      RuntimeAutoreleasePoolStateForCurrentThread();
  return static_cast<std::uint64_t>(state.frames.size());
}

std::uint64_t RuntimeAutoreleasePoolMaxDepth() {
  return RuntimeAutoreleasePoolStateForCurrentThread().max_depth;
}

std::uint64_t CountQueuedAutoreleaseValues() {
  const RuntimeAutoreleasePoolState &state =
      RuntimeAutoreleasePoolStateForCurrentThread();
  std::uint64_t total = 0;
  for (const RuntimeAutoreleasePoolFrame &frame : state.frames) {
    total += static_cast<std::uint64_t>(frame.values.size());
  }
  return total;
}

std::uint64_t RuntimeAutoreleasePoolDrainedValueCount() {
  return RuntimeAutoreleasePoolStateForCurrentThread().drained_value_count;
}

void RecordRuntimeAutoreleasePoolDrainedValue(int value) {
  RuntimeAutoreleasePoolState &state =
      RuntimeAutoreleasePoolStateForCurrentThread();
  state.last_drained_autorelease_value = value;
  ++state.drained_value_count;
}

int RuntimeLastAutoreleasedValue() {
  return RuntimeAutoreleasePoolStateForCurrentThread().last_autoreleased_value;
}

int RuntimeLastDrainedAutoreleaseValue() {
  return RuntimeAutoreleasePoolStateForCurrentThread()
      .last_drained_autorelease_value;
}

void EnqueueAutoreleaseValue(int value) {
  if (!RuntimeAutoreleasePoolCanEnqueue(value)) {
    return;
  }
  RuntimeAutoreleasePoolState &state =
      RuntimeAutoreleasePoolStateForCurrentThread();
  state.last_autoreleased_value = value;
  if (!state.frames.empty()) {
    state.frames.back().values.push_back(value);
    return;
  }
  (void)EnqueueRuntimeDispatchFrameAutoreleaseValue(value);
}

}  // namespace objc3c::runtime
