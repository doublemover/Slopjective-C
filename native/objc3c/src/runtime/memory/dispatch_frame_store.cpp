#include "runtime/memory/dispatch_frame_store.h"

namespace objc3c::runtime {

RuntimeDispatchFrameState &RuntimeDispatchFrameStateForCurrentThread() {
  thread_local RuntimeDispatchFrameState state;
  return state;
}

}  // namespace objc3c::runtime
