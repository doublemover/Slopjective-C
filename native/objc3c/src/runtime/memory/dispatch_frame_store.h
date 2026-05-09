#pragma once

#include "runtime/state/runtime_thread_records.h"

#include <vector>

namespace objc3c::runtime {

struct RuntimeDispatchFrameState {
  std::vector<RuntimeDispatchFrame> frames;
  RuntimeDispatchFrame testing_frame;
  bool has_testing_frame = false;
};

RuntimeDispatchFrameState &RuntimeDispatchFrameStateForCurrentThread();

}  // namespace objc3c::runtime
