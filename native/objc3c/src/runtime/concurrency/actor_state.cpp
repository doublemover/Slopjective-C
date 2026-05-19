#include "runtime/concurrency/actor_state.h"

#include "runtime/concurrency/actor_state_store.h"

namespace objc3c::runtime {

RuntimeActorState &RuntimeActorStateForCurrentThread() {
  thread_local RuntimeActorState state;
  return state;
}

void ResetRuntimeActorStateForTesting() {
  RuntimeActorStateForCurrentThread() = RuntimeActorState{};
}

}  // namespace objc3c::runtime
