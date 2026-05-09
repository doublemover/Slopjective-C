#include "runtime/memory/dispatch_frame_state.h"

#include "runtime/memory/dispatch_frame_store.h"

#include <utility>

namespace objc3c::runtime {

RuntimeDispatchFrame *CurrentRuntimeDispatchFrame() {
  RuntimeDispatchFrameState &state = RuntimeDispatchFrameStateForCurrentThread();
  if (!state.frames.empty()) {
    return &state.frames.back();
  }
  return state.has_testing_frame ? &state.testing_frame : nullptr;
}

void PushRuntimeDispatchFrame(int receiver, std::uint64_t base_identity,
                              const RealizedPropertyAccessor *accessor) {
  RuntimeDispatchFrameState &state = RuntimeDispatchFrameStateForCurrentThread();
  RuntimeDispatchFrame frame;
  frame.receiver = receiver;
  frame.base_identity = base_identity;
  frame.runtime_property_accessor = accessor;
  state.ownership_explicit = RuntimeOwnerSplitContractIsReady();
  state.fallback_path_allowed = RuntimeFallbackPathsAreAllowed();
  state.frames.push_back(std::move(frame));
}

std::vector<int> PopRuntimeDispatchFrameAutoreleaseValues() {
  RuntimeDispatchFrameState &state = RuntimeDispatchFrameStateForCurrentThread();
  if (state.frames.empty()) {
    return {};
  }
  RuntimeDispatchFrame frame = std::move(state.frames.back());
  state.frames.pop_back();
  return std::move(frame.autorelease_values);
}

RuntimeDispatchFrame *SetRuntimeTestingDispatchFrame(
    int receiver, std::uint64_t base_identity,
    const RealizedPropertyAccessor *accessor) {
  RuntimeDispatchFrameState &state = RuntimeDispatchFrameStateForCurrentThread();
  state.testing_frame = RuntimeDispatchFrame{};
  state.ownership_explicit = RuntimeOwnerSplitContractIsReady();
  state.fallback_path_allowed = RuntimeFallbackPathsAreAllowed();
  state.testing_frame.receiver = receiver;
  state.testing_frame.base_identity = base_identity;
  state.testing_frame.runtime_property_accessor = accessor;
  state.has_testing_frame = true;
  return &state.testing_frame;
}

void ClearRuntimeTestingDispatchFrame() {
  RuntimeDispatchFrameState &state = RuntimeDispatchFrameStateForCurrentThread();
  state.testing_frame = RuntimeDispatchFrame{};
  state.has_testing_frame = false;
}

void ResetRuntimeDispatchFrameStateForTesting() {
  RuntimeDispatchFrameState &state = RuntimeDispatchFrameStateForCurrentThread();
  state.frames.clear();
  state.ownership_explicit = RuntimeOwnerSplitContractIsReady();
  state.fallback_path_allowed = RuntimeFallbackPathsAreAllowed();
  ClearRuntimeTestingDispatchFrame();
}

bool EnqueueRuntimeDispatchFrameAutoreleaseValue(int value) {
  RuntimeDispatchFrameState &state = RuntimeDispatchFrameStateForCurrentThread();
  if (state.frames.size() >= 2u) {
    state.frames[state.frames.size() - 2u].autorelease_values.push_back(value);
    return true;
  }
  if (RuntimeDispatchFrame *frame = CurrentRuntimeDispatchFrame()) {
    frame->autorelease_values.push_back(value);
    return true;
  }
  return false;
}

}  // namespace objc3c::runtime
