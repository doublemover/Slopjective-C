#include "runtime/concurrency/runtime_debug.h"

#include "runtime/concurrency/actor_state.h"
#include "runtime/concurrency/continuation_state.h"
#include "runtime/concurrency/task_state.h"

namespace objc3c::runtime {

void ResetRuntimeConcurrencyDebugStateForTesting() {
  ResetRuntimeContinuationStateForTesting();
  ResetRuntimeTaskStateForTesting();
  ResetRuntimeActorStateForTesting();
}

}  // namespace objc3c::runtime
