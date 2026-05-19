#include "runtime/concurrency/actor_state_store.h"

#include "runtime/concurrency/actor_guard_operations.h"
#include "runtime/concurrency/actor_isolation_operations.h"

extern "C" int objc3_runtime_actor_enter_isolation_thunk_i32(int executor_tag) {
  objc3c::runtime::RuntimeActorState &state =
      objc3c::runtime::RuntimeActorStateForCurrentThread();
  return objc3c::runtime::EnterRuntimeActorIsolationThunk(state, executor_tag);
}

extern "C" int objc3_runtime_actor_enter_nonisolated_i32(int value,
                                                         int executor_tag) {
  objc3c::runtime::RuntimeActorState &state =
      objc3c::runtime::RuntimeActorStateForCurrentThread();
  return objc3c::runtime::EnterRuntimeActorNonisolated(
      state, value, executor_tag);
}

extern "C" int objc3_runtime_actor_hop_to_executor_i32(int value,
                                                       int executor_tag) {
  objc3c::runtime::RuntimeActorState &state =
      objc3c::runtime::RuntimeActorStateForCurrentThread();
  return objc3c::runtime::HopRuntimeActorToExecutor(
      state, value, executor_tag);
}

extern "C" int objc3_runtime_actor_record_replay_proof_i32(int executor_tag) {
  objc3c::runtime::RuntimeActorState &state =
      objc3c::runtime::RuntimeActorStateForCurrentThread();
  return objc3c::runtime::RecordRuntimeActorReplayProof(state, executor_tag);
}

extern "C" int objc3_runtime_actor_record_race_guard_i32(int executor_tag) {
  objc3c::runtime::RuntimeActorState &state =
      objc3c::runtime::RuntimeActorStateForCurrentThread();
  return objc3c::runtime::RecordRuntimeActorRaceGuard(state, executor_tag);
}
