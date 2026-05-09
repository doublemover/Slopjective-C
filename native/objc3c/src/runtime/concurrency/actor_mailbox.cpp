#include "runtime/concurrency/actor_state_store.h"

#include "runtime/concurrency/actor_mailbox_operations.h"

extern "C" int objc3_runtime_actor_bind_executor_i32(int actor_handle,
                                                     int executor_tag) {
  objc3c::runtime::RuntimeActorState &state =
      objc3c::runtime::RuntimeActorStateForCurrentThread();
  return objc3c::runtime::BindRuntimeActorMailboxExecutor(
      state, actor_handle, executor_tag);
}

extern "C" int objc3_runtime_actor_mailbox_enqueue_i32(int actor_handle,
                                                       int value,
                                                       int executor_tag) {
  objc3c::runtime::RuntimeActorState &state =
      objc3c::runtime::RuntimeActorStateForCurrentThread();
  return objc3c::runtime::EnqueueRuntimeActorMailboxValue(
      state, actor_handle, value, executor_tag);
}

extern "C" int objc3_runtime_actor_mailbox_drain_next_i32(int actor_handle,
                                                          int executor_tag) {
  objc3c::runtime::RuntimeActorState &state =
      objc3c::runtime::RuntimeActorStateForCurrentThread();
  return objc3c::runtime::DrainRuntimeActorMailboxNextValue(
      state, actor_handle, executor_tag);
}
