#include "runtime/concurrency/actor_mailbox_operations.h"

#include "runtime/concurrency/actor.h"
#include "runtime/concurrency/actor_state_store.h"
#include "runtime/concurrency/executor.h"

#include <deque>

namespace objc3c::runtime {

int BindRuntimeActorMailboxExecutor(RuntimeActorState &state,
                                    int actor_handle,
                                    int executor_tag) {
  ++state.bind_executor_call_count;
  state.last_bound_actor_handle = actor_handle;
  state.last_bound_executor_tag = executor_tag;
  state.last_mailbox_actor_handle = actor_handle;
  state.last_mailbox_executor_tag = executor_tag;
  if (!RuntimeActorHandleIsValid(actor_handle) ||
      !RuntimeExecutorTagIsValid(executor_tag)) {
    state.last_mailbox_depth = 0;
    return 0;
  }
  state.last_mailbox_depth =
      static_cast<int>(state.mailboxes[actor_handle].size());
  return executor_tag;
}

int EnqueueRuntimeActorMailboxValue(RuntimeActorState &state,
                                    int actor_handle,
                                    int value,
                                    int executor_tag) {
  ++state.mailbox_enqueue_call_count;
  state.last_mailbox_actor_handle = actor_handle;
  state.last_mailbox_enqueued_value = value;
  state.last_mailbox_executor_tag = executor_tag;
  if (!RuntimeActorHandleIsValid(actor_handle) ||
      !RuntimeExecutorTagIsValid(executor_tag)) {
    state.last_mailbox_depth = 0;
    return 0;
  }
  std::deque<int> &mailbox = state.mailboxes[actor_handle];
  mailbox.push_back(value);
  state.last_mailbox_depth = static_cast<int>(mailbox.size());
  return value;
}

int DrainRuntimeActorMailboxNextValue(RuntimeActorState &state,
                                      int actor_handle,
                                      int executor_tag) {
  ++state.mailbox_drain_call_count;
  state.last_mailbox_actor_handle = actor_handle;
  state.last_mailbox_executor_tag = executor_tag;
  if (!RuntimeActorHandleIsValid(actor_handle) ||
      !RuntimeExecutorTagIsValid(executor_tag)) {
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

}  // namespace objc3c::runtime
