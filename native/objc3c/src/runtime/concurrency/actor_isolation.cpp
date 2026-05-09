#include "runtime/concurrency/actor_state_store.h"

#include "runtime/concurrency/executor.h"

extern "C" int objc3_runtime_actor_enter_isolation_thunk_i32(int executor_tag) {
  objc3c::runtime::RuntimeActorState &state =
      objc3c::runtime::RuntimeActorStateForCurrentThread();
  ++state.isolation_thunk_call_count;
  state.last_isolation_executor_tag = executor_tag;
  if (!objc3c::runtime::RuntimeExecutorTagIsValid(executor_tag)) {
    return 0;
  }
  return executor_tag;
}

extern "C" int objc3_runtime_actor_enter_nonisolated_i32(int value,
                                                         int executor_tag) {
  objc3c::runtime::RuntimeActorState &state =
      objc3c::runtime::RuntimeActorStateForCurrentThread();
  ++state.nonisolated_entry_call_count;
  state.last_nonisolated_value = value;
  state.last_nonisolated_executor_tag = executor_tag;
  if (!objc3c::runtime::RuntimeExecutorTagIsValid(executor_tag)) {
    return 0;
  }
  return value;
}

extern "C" int objc3_runtime_actor_hop_to_executor_i32(int value,
                                                       int executor_tag) {
  objc3c::runtime::RuntimeActorState &state =
      objc3c::runtime::RuntimeActorStateForCurrentThread();
  ++state.hop_to_executor_call_count;
  state.last_hop_value = value;
  state.last_hop_executor_tag = executor_tag;
  if (!objc3c::runtime::RuntimeExecutorTagIsValid(executor_tag)) {
    state.last_hop_result = 0;
    return 0;
  }
  state.last_hop_result = value;
  return value;
}

extern "C" int objc3_runtime_actor_record_replay_proof_i32(int executor_tag) {
  objc3c::runtime::RuntimeActorState &state =
      objc3c::runtime::RuntimeActorStateForCurrentThread();
  ++state.replay_proof_call_count;
  state.last_replay_proof_executor_tag = executor_tag;
  if (!objc3c::runtime::RuntimeExecutorTagIsValid(executor_tag)) {
    return 0;
  }
  return executor_tag;
}

extern "C" int objc3_runtime_actor_record_race_guard_i32(int executor_tag) {
  objc3c::runtime::RuntimeActorState &state =
      objc3c::runtime::RuntimeActorStateForCurrentThread();
  ++state.race_guard_call_count;
  state.last_race_guard_executor_tag = executor_tag;
  if (!objc3c::runtime::RuntimeExecutorTagIsValid(executor_tag)) {
    return 0;
  }
  return executor_tag;
}
