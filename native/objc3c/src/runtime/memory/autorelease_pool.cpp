#include "runtime/memory/autorelease_pool.h"

#include <algorithm>
#include <utility>

namespace objc3c::runtime {

namespace {

thread_local std::vector<RuntimeDispatchFrame> g_runtime_dispatch_frames;
thread_local RuntimeDispatchFrame g_runtime_testing_dispatch_frame;
thread_local bool g_runtime_has_testing_dispatch_frame = false;
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

RuntimeDispatchFrame *CurrentRuntimeDispatchFrame() {
  if (!g_runtime_dispatch_frames.empty()) {
    return &g_runtime_dispatch_frames.back();
  }
  return g_runtime_has_testing_dispatch_frame ? &g_runtime_testing_dispatch_frame
                                              : nullptr;
}

void PushRuntimeDispatchFrame(int receiver, std::uint64_t base_identity,
                              const RealizedPropertyAccessor *accessor) {
  RuntimeDispatchFrame frame;
  frame.receiver = receiver;
  frame.base_identity = base_identity;
  frame.runtime_property_accessor = accessor;
  g_runtime_dispatch_frames.push_back(std::move(frame));
}

std::vector<int> PopRuntimeDispatchFrameAutoreleaseValues() {
  if (g_runtime_dispatch_frames.empty()) {
    return {};
  }
  RuntimeDispatchFrame frame = std::move(g_runtime_dispatch_frames.back());
  g_runtime_dispatch_frames.pop_back();
  return std::move(frame.autorelease_values);
}

RuntimeDispatchFrame *SetRuntimeTestingDispatchFrame(
    int receiver, std::uint64_t base_identity,
    const RealizedPropertyAccessor *accessor) {
  g_runtime_testing_dispatch_frame = RuntimeDispatchFrame{};
  g_runtime_testing_dispatch_frame.receiver = receiver;
  g_runtime_testing_dispatch_frame.base_identity = base_identity;
  g_runtime_testing_dispatch_frame.runtime_property_accessor = accessor;
  g_runtime_has_testing_dispatch_frame = true;
  return &g_runtime_testing_dispatch_frame;
}

void ClearRuntimeTestingDispatchFrame() {
  g_runtime_testing_dispatch_frame = RuntimeDispatchFrame{};
  g_runtime_has_testing_dispatch_frame = false;
}

void ResetRuntimeAutoreleasepoolStateForTesting() {
  g_runtime_dispatch_frames.clear();
  ClearRuntimeTestingDispatchFrame();
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
  if (g_runtime_dispatch_frames.size() >= 2u) {
    g_runtime_dispatch_frames[g_runtime_dispatch_frames.size() - 2u]
        .autorelease_values.push_back(value);
    return;
  }
  if (RuntimeDispatchFrame *frame = CurrentRuntimeDispatchFrame()) {
    frame->autorelease_values.push_back(value);
  }
}

}  // namespace objc3c::runtime
