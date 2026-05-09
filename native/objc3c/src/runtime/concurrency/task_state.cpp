#include "runtime/concurrency/task_state.h"

#include "runtime/concurrency/task_state_store.h"

namespace objc3c::runtime {

RuntimeTaskState &RuntimeTaskStateForCurrentThread() {
  thread_local RuntimeTaskState state;
  return state;
}

void ResetRuntimeTaskStateForTesting() {
  RuntimeTaskStateForCurrentThread() = RuntimeTaskState{};
}

}  // namespace objc3c::runtime
