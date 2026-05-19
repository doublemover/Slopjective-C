#pragma once

#include "runtime/memory/dispatch_frame_ownership.h"
#include "runtime/state/runtime_thread_records.h"

#include <vector>

namespace objc3c::runtime {

struct RuntimeDispatchFrameState {
  RuntimeDispatchFrameOwnership ownership =
      RuntimeDispatchFrameOwnershipForHardCutover();
  std::vector<RuntimeDispatchFrame> frames;
  RuntimeDispatchFrame testing_frame;
  bool has_testing_frame = false;
};

RuntimeDispatchFrameState &RuntimeDispatchFrameStateForCurrentThread();

}  // namespace objc3c::runtime
