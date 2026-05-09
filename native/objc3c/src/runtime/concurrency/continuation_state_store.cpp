#include "runtime/concurrency/continuation_state_store.h"

#include "runtime/concurrency/continuation_state.h"

namespace objc3c::runtime {

RuntimeContinuationState &RuntimeContinuationStateForCurrentThread() {
  thread_local RuntimeContinuationState state;
  return state;
}

void ResetRuntimeContinuationStateForTesting() {
  RuntimeContinuationStateForCurrentThread() = RuntimeContinuationState{};
}

}  // namespace objc3c::runtime
