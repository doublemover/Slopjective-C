#include "runtime/concurrency/actor_isolation_operations.h"

#include "runtime/concurrency/actor_state_store.h"
#include "runtime/concurrency/executor.h"

namespace objc3c::runtime {

int EnterRuntimeActorIsolationThunk(RuntimeActorState &state,
                                    int executor_tag) {
  ++state.isolation_thunk_call_count;
  state.last_isolation_executor_tag = executor_tag;
  if (!RuntimeExecutorTagIsValid(executor_tag)) {
    return 0;
  }
  return executor_tag;
}

int EnterRuntimeActorNonisolated(RuntimeActorState &state,
                                 int value,
                                 int executor_tag) {
  ++state.nonisolated_entry_call_count;
  state.last_nonisolated_value = value;
  state.last_nonisolated_executor_tag = executor_tag;
  if (!RuntimeExecutorTagIsValid(executor_tag)) {
    return 0;
  }
  return value;
}

int HopRuntimeActorToExecutor(RuntimeActorState &state,
                              int value,
                              int executor_tag) {
  ++state.hop_to_executor_call_count;
  state.last_hop_value = value;
  state.last_hop_executor_tag = executor_tag;
  if (!RuntimeExecutorTagIsValid(executor_tag)) {
    state.last_hop_result = 0;
    return 0;
  }
  state.last_hop_result = value;
  return value;
}

}  // namespace objc3c::runtime
