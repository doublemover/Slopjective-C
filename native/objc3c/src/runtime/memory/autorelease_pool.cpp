#include "runtime/memory/autorelease_pool.h"

#include <algorithm>

namespace objc3c::runtime {

namespace {

thread_local std::vector<RuntimeAutoreleasePoolFrame>
    g_runtime_autoreleasepool_frames;
thread_local std::uint64_t g_runtime_autoreleasepool_max_depth = 0;
thread_local std::uint64_t g_runtime_autoreleasepool_drained_value_count = 0;
thread_local int g_runtime_last_autoreleased_value = 0;
thread_local int g_runtime_last_drained_autorelease_value = 0;

}  // namespace

bool RuntimeAutoreleasePoolCanEnqueue(int value) {
  return value != 0;
}

void ResetRuntimeAutoreleasepoolStateForTesting() {
  ResetRuntimeDispatchFrameStateForTesting();
  g_runtime_autoreleasepool_frames.clear();
  g_runtime_autoreleasepool_max_depth = 0;
  g_runtime_autoreleasepool_drained_value_count = 0;
  g_runtime_last_autoreleased_value = 0;
  g_runtime_last_drained_autorelease_value = 0;
}

void PushRuntimeAutoreleasePoolFrame() {
  g_runtime_autoreleasepool_frames.push_back({});
  g_runtime_autoreleasepool_max_depth = std::max<std::uint64_t>(
      g_runtime_autoreleasepool_max_depth,
      static_cast<std::uint64_t>(g_runtime_autoreleasepool_frames.size()));
}

std::vector<int> PopRuntimeAutoreleasePoolFrameValues() {
  if (g_runtime_autoreleasepool_frames.empty()) {
    return {};
  }
  RuntimeAutoreleasePoolFrame frame =
      std::move(g_runtime_autoreleasepool_frames.back());
  g_runtime_autoreleasepool_frames.pop_back();
  return std::move(frame.values);
}

std::uint64_t RuntimeAutoreleasePoolDepth() {
  return static_cast<std::uint64_t>(g_runtime_autoreleasepool_frames.size());
}

std::uint64_t RuntimeAutoreleasePoolMaxDepth() {
  return g_runtime_autoreleasepool_max_depth;
}

std::uint64_t CountQueuedAutoreleaseValues() {
  std::uint64_t total = 0;
  for (const RuntimeAutoreleasePoolFrame &frame :
       g_runtime_autoreleasepool_frames) {
    total += static_cast<std::uint64_t>(frame.values.size());
  }
  return total;
}

std::uint64_t RuntimeAutoreleasePoolDrainedValueCount() {
  return g_runtime_autoreleasepool_drained_value_count;
}

void RecordRuntimeAutoreleasePoolDrainedValue(int value) {
  g_runtime_last_drained_autorelease_value = value;
  ++g_runtime_autoreleasepool_drained_value_count;
}

int RuntimeLastAutoreleasedValue() {
  return g_runtime_last_autoreleased_value;
}

int RuntimeLastDrainedAutoreleaseValue() {
  return g_runtime_last_drained_autorelease_value;
}

void EnqueueAutoreleaseValue(int value) {
  if (!RuntimeAutoreleasePoolCanEnqueue(value)) {
    return;
  }
  g_runtime_last_autoreleased_value = value;
  if (!g_runtime_autoreleasepool_frames.empty()) {
    g_runtime_autoreleasepool_frames.back().values.push_back(value);
    return;
  }
  (void)EnqueueRuntimeDispatchFrameAutoreleaseValue(value);
}

}  // namespace objc3c::runtime
