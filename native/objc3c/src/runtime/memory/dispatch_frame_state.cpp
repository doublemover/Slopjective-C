#include "runtime/memory/dispatch_frame_state.h"

#include <utility>

namespace objc3c::runtime {
namespace {

thread_local std::vector<RuntimeDispatchFrame> g_runtime_dispatch_frames;
thread_local RuntimeDispatchFrame g_runtime_testing_dispatch_frame;
thread_local bool g_runtime_has_testing_dispatch_frame = false;

}  // namespace

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

void ResetRuntimeDispatchFrameStateForTesting() {
  g_runtime_dispatch_frames.clear();
  ClearRuntimeTestingDispatchFrame();
}

bool EnqueueRuntimeDispatchFrameAutoreleaseValue(int value) {
  if (g_runtime_dispatch_frames.size() >= 2u) {
    g_runtime_dispatch_frames[g_runtime_dispatch_frames.size() - 2u]
        .autorelease_values.push_back(value);
    return true;
  }
  if (RuntimeDispatchFrame *frame = CurrentRuntimeDispatchFrame()) {
    frame->autorelease_values.push_back(value);
    return true;
  }
  return false;
}

}  // namespace objc3c::runtime
