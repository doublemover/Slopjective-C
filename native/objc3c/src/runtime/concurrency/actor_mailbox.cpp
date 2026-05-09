#include "runtime/concurrency/actor_state_store.h"

#include "runtime/concurrency/actor.h"
#include "runtime/concurrency/executor.h"

#include <deque>

extern "C" int objc3_runtime_actor_bind_executor_i32(int actor_handle,
                                                     int executor_tag) {
  objc3c::runtime::RuntimeActorState &state =
      objc3c::runtime::RuntimeActorStateForCurrentThread();
  ++state.bind_executor_call_count;
  state.last_bound_actor_handle = actor_handle;
  state.last_bound_executor_tag = executor_tag;
  state.last_mailbox_actor_handle = actor_handle;
  state.last_mailbox_executor_tag = executor_tag;
  if (!objc3c::runtime::RuntimeActorHandleIsValid(actor_handle) ||
      !objc3c::runtime::RuntimeExecutorTagIsValid(executor_tag)) {
    state.last_mailbox_depth = 0;
    return 0;
  }
  state.last_mailbox_depth =
      static_cast<int>(state.mailboxes[actor_handle].size());
  return executor_tag;
}

extern "C" int objc3_runtime_actor_mailbox_enqueue_i32(int actor_handle,
                                                       int value,
                                                       int executor_tag) {
  objc3c::runtime::RuntimeActorState &state =
      objc3c::runtime::RuntimeActorStateForCurrentThread();
  ++state.mailbox_enqueue_call_count;
  state.last_mailbox_actor_handle = actor_handle;
  state.last_mailbox_enqueued_value = value;
  state.last_mailbox_executor_tag = executor_tag;
  if (!objc3c::runtime::RuntimeActorHandleIsValid(actor_handle) ||
      !objc3c::runtime::RuntimeExecutorTagIsValid(executor_tag)) {
    state.last_mailbox_depth = 0;
    return 0;
  }
  std::deque<int> &mailbox = state.mailboxes[actor_handle];
  mailbox.push_back(value);
  state.last_mailbox_depth = static_cast<int>(mailbox.size());
  return value;
}

extern "C" int objc3_runtime_actor_mailbox_drain_next_i32(int actor_handle,
                                                          int executor_tag) {
  objc3c::runtime::RuntimeActorState &state =
      objc3c::runtime::RuntimeActorStateForCurrentThread();
  ++state.mailbox_drain_call_count;
  state.last_mailbox_actor_handle = actor_handle;
  state.last_mailbox_executor_tag = executor_tag;
  if (!objc3c::runtime::RuntimeActorHandleIsValid(actor_handle) ||
      !objc3c::runtime::RuntimeExecutorTagIsValid(executor_tag)) {
    state.last_mailbox_depth = 0;
    state.last_mailbox_drained_value = 0;
    return 0;
  }
  std::deque<int> &mailbox = state.mailboxes[actor_handle];
  if (mailbox.empty()) {
    state.last_mailbox_depth = 0;
    state.last_mailbox_drained_value = 0;
    return 0;
  }
  const int value = mailbox.front();
  mailbox.pop_front();
  state.last_mailbox_depth = static_cast<int>(mailbox.size());
  state.last_mailbox_drained_value = value;
  return value;
}
