#include "runtime/concurrency/actor_guard_operations.h"

#include "runtime/concurrency/actor_state_store.h"
#include "runtime/concurrency/executor.h"

namespace objc3c::runtime {

int RecordRuntimeActorReplayProof(RuntimeActorState &state,
                                  int executor_tag) {
  ++state.replay_proof_call_count;
  state.last_replay_proof_executor_tag = executor_tag;
  if (!RuntimeExecutorTagIsValid(executor_tag)) {
    RecordRuntimeActorFailure(state,
                              OBJC3_RUNTIME_ACTOR_FAILURE_INVALID_EXECUTOR);
    return 0;
  }
  RecordRuntimeActorSuccess(state);
  return executor_tag;
}

int RecordRuntimeActorRaceGuard(RuntimeActorState &state,
                                int executor_tag) {
  ++state.race_guard_call_count;
  state.last_race_guard_executor_tag = executor_tag;
  if (!RuntimeExecutorTagIsValid(executor_tag)) {
    RecordRuntimeActorFailure(state,
                              OBJC3_RUNTIME_ACTOR_FAILURE_INVALID_EXECUTOR);
    return 0;
  }
  RecordRuntimeActorSuccess(state);
  return executor_tag;
}

}  // namespace objc3c::runtime
