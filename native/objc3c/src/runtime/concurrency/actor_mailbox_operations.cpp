#include "runtime/concurrency/actor_mailbox_operations.h"

#include "runtime/concurrency/actor.h"
#include "runtime/concurrency/actor_state_store.h"
#include "runtime/concurrency/executor.h"

#include <deque>

namespace objc3c::runtime {
namespace {

int CurrentMailboxDepth(const RuntimeActorState &state, int actor_handle) {
  const auto found = state.mailboxes.find(actor_handle);
  return found == state.mailboxes.end()
             ? 0
             : static_cast<int>(found->second.size());
}

bool RecordRuntimeActorMailboxBindingGuard(RuntimeActorState &state,
                                           int actor_handle,
                                           int executor_tag) {
  state.mailbox_identity_guard_passed = RuntimeActorHandleIsValid(actor_handle)
                                            ? 1
                                            : 0;
  if (state.mailbox_identity_guard_passed == 0) {
    state.last_expected_executor_tag = 0;
    state.executor_binding_guard_passed = 0;
    return false;
  }

  const auto found = state.actor_executor_bindings.find(actor_handle);
  if (found == state.actor_executor_bindings.end()) {
    state.last_expected_executor_tag = 0;
    state.executor_binding_guard_passed = 0;
    RecordRuntimeActorFailure(state,
                              OBJC3_RUNTIME_ACTOR_FAILURE_UNBOUND_ACTOR);
    return false;
  }

  state.last_expected_executor_tag = found->second;
  state.executor_binding_guard_passed =
      found->second == executor_tag ? 1 : 0;
  if (state.executor_binding_guard_passed == 0) {
    RecordRuntimeActorFailure(state,
                              OBJC3_RUNTIME_ACTOR_FAILURE_EXECUTOR_MISMATCH);
    return false;
  }
  return true;
}

}  // namespace

int BindRuntimeActorMailboxExecutor(RuntimeActorState &state,
                                    int actor_handle,
                                    int executor_tag) {
  ++state.bind_executor_call_count;
  state.last_bound_actor_handle = actor_handle;
  state.last_bound_executor_tag = executor_tag;
  state.last_mailbox_actor_handle = actor_handle;
  state.last_mailbox_executor_tag = executor_tag;
  state.last_expected_executor_tag = executor_tag;
  if (!RuntimeActorHandleIsValid(actor_handle)) {
    state.last_mailbox_depth = 0;
    state.mailbox_identity_guard_passed = 0;
    state.executor_binding_guard_passed = 0;
    RecordRuntimeActorFailure(
        state, OBJC3_RUNTIME_ACTOR_FAILURE_INVALID_ACTOR_HANDLE);
    return 0;
  }
  if (!RuntimeExecutorTagIsValid(executor_tag)) {
    state.last_mailbox_depth = 0;
    state.mailbox_identity_guard_passed = 1;
    state.executor_binding_guard_passed = 0;
    RecordRuntimeActorFailure(state,
                              OBJC3_RUNTIME_ACTOR_FAILURE_INVALID_EXECUTOR);
    return 0;
  }
  state.actor_executor_bindings[actor_handle] = executor_tag;
  state.actor_executor_binding_count =
      static_cast<std::uint64_t>(state.actor_executor_bindings.size());
  state.mailbox_identity_guard_passed = 1;
  state.executor_binding_guard_passed = 1;
  state.last_mailbox_depth =
      CurrentMailboxDepth(state, actor_handle);
  RecordRuntimeActorSuccess(state);
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
  state.last_expected_executor_tag = 0;
  if (!RuntimeActorHandleIsValid(actor_handle)) {
    state.last_mailbox_depth = 0;
    state.mailbox_identity_guard_passed = 0;
    state.executor_binding_guard_passed = 0;
    RecordRuntimeActorFailure(
        state, OBJC3_RUNTIME_ACTOR_FAILURE_INVALID_ACTOR_HANDLE);
    return 0;
  }
  if (!RuntimeExecutorTagIsValid(executor_tag)) {
    state.last_mailbox_depth = CurrentMailboxDepth(state, actor_handle);
    state.mailbox_identity_guard_passed = 1;
    state.executor_binding_guard_passed = 0;
    RecordRuntimeActorFailure(state,
                              OBJC3_RUNTIME_ACTOR_FAILURE_INVALID_EXECUTOR);
    return 0;
  }
  if (!RecordRuntimeActorMailboxBindingGuard(state, actor_handle,
                                             executor_tag)) {
    state.last_mailbox_depth = CurrentMailboxDepth(state, actor_handle);
    return 0;
  }
  std::deque<int> &mailbox = state.mailboxes[actor_handle];
  mailbox.push_back(value);
  state.last_mailbox_depth = static_cast<int>(mailbox.size());
  RecordRuntimeActorSuccess(state);
  return value;
}

int DrainRuntimeActorMailboxNextValue(RuntimeActorState &state,
                                      int actor_handle,
                                      int executor_tag) {
  ++state.mailbox_drain_call_count;
  state.last_mailbox_actor_handle = actor_handle;
  state.last_mailbox_executor_tag = executor_tag;
  state.last_expected_executor_tag = 0;
  if (!RuntimeActorHandleIsValid(actor_handle)) {
    state.last_mailbox_depth = 0;
    state.last_mailbox_drained_value = 0;
    state.mailbox_identity_guard_passed = 0;
    state.executor_binding_guard_passed = 0;
    RecordRuntimeActorFailure(
        state, OBJC3_RUNTIME_ACTOR_FAILURE_INVALID_ACTOR_HANDLE);
    return 0;
  }
  if (!RuntimeExecutorTagIsValid(executor_tag)) {
    state.last_mailbox_depth = CurrentMailboxDepth(state, actor_handle);
    state.last_mailbox_drained_value = 0;
    state.mailbox_identity_guard_passed = 1;
    state.executor_binding_guard_passed = 0;
    RecordRuntimeActorFailure(state,
                              OBJC3_RUNTIME_ACTOR_FAILURE_INVALID_EXECUTOR);
    return 0;
  }
  if (!RecordRuntimeActorMailboxBindingGuard(state, actor_handle,
                                             executor_tag)) {
    state.last_mailbox_depth = CurrentMailboxDepth(state, actor_handle);
    state.last_mailbox_drained_value = 0;
    return 0;
  }
  std::deque<int> &mailbox = state.mailboxes[actor_handle];
  if (mailbox.empty()) {
    state.last_mailbox_depth = 0;
    state.last_mailbox_drained_value = 0;
    RecordRuntimeActorFailure(state,
                              OBJC3_RUNTIME_ACTOR_FAILURE_EMPTY_MAILBOX);
    return 0;
  }
  const int value = mailbox.front();
  mailbox.pop_front();
  state.last_mailbox_depth = static_cast<int>(mailbox.size());
  state.last_mailbox_drained_value = value;
  RecordRuntimeActorSuccess(state);
  return value;
}

}  // namespace objc3c::runtime
