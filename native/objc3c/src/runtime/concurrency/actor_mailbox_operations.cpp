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

bool RuntimeActorMailboxIsShutdown(const RuntimeActorState &state,
                                   int actor_handle) {
  const auto found = state.actor_mailbox_shutdowns.find(actor_handle);
  return found != state.actor_mailbox_shutdowns.end() && found->second != 0;
}

void RecordRuntimeActorMailboxOrdering(RuntimeActorState &state,
                                       int actor_handle,
                                       std::uint64_t message_id) {
  const auto found = state.actor_last_completed_sequence.find(actor_handle);
  state.mailbox_ordering_guard_passed =
      found == state.actor_last_completed_sequence.end() ||
              message_id > found->second
          ? 1
          : 0;
  if (state.mailbox_ordering_guard_passed == 0) {
    RecordRuntimeActorFailure(
        state, OBJC3_RUNTIME_ACTOR_FAILURE_MAILBOX_ORDERING_DRIFT);
    return;
  }
  state.actor_last_completed_sequence[actor_handle] = message_id;
  state.last_mailbox_completion_sequence = message_id;
}

bool RecordRuntimeActorMailboxShutdownGuard(RuntimeActorState &state,
                                            int actor_handle) {
  state.mailbox_shutdown_guard_passed =
      RuntimeActorMailboxIsShutdown(state, actor_handle) ? 0 : 1;
  if (state.mailbox_shutdown_guard_passed == 0) {
    RecordRuntimeActorFailure(state,
                              OBJC3_RUNTIME_ACTOR_FAILURE_MAILBOX_SHUTDOWN);
    return false;
  }
  return true;
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
    state.mailbox_shutdown_guard_passed = 0;
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
  if (!RecordRuntimeActorMailboxShutdownGuard(state, actor_handle)) {
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
  if (RuntimeActorMailboxIsShutdown(state, actor_handle)) {
    state.last_mailbox_depth = 0;
    state.mailbox_identity_guard_passed = 1;
    state.executor_binding_guard_passed = 0;
    state.mailbox_shutdown_guard_passed = 0;
    RecordRuntimeActorFailure(
        state, OBJC3_RUNTIME_ACTOR_FAILURE_STALE_ACTOR_IDENTITY);
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
  state.mailbox_shutdown_guard_passed = 1;
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
  const std::uint64_t message_id = ++state.mailbox_message_sequence;
  RuntimeActorMailboxMessage message;
  message.message_id = message_id;
  message.value = value;
  message.executor_tag = executor_tag;
  std::deque<RuntimeActorMailboxMessage> &mailbox =
      state.mailboxes[actor_handle];
  mailbox.push_back(message);
  state.last_mailbox_message_id = message_id;
  state.last_mailbox_enqueue_sequence = message_id;
  state.last_mailbox_depth = static_cast<int>(mailbox.size());
  state.mailbox_ordering_guard_passed = 1;
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
  std::deque<RuntimeActorMailboxMessage> &mailbox =
      state.mailboxes[actor_handle];
  if (mailbox.empty()) {
    state.last_mailbox_depth = 0;
    state.last_mailbox_drained_value = 0;
    RecordRuntimeActorFailure(state,
                              OBJC3_RUNTIME_ACTOR_FAILURE_EMPTY_MAILBOX);
    return 0;
  }
  const RuntimeActorMailboxMessage message = mailbox.front();
  RecordRuntimeActorMailboxOrdering(state, actor_handle, message.message_id);
  if (state.mailbox_ordering_guard_passed == 0) {
    state.last_mailbox_depth = static_cast<int>(mailbox.size());
    state.last_mailbox_drained_value = 0;
    return 0;
  }
  mailbox.pop_front();
  const int value = message.value;
  state.last_mailbox_message_id = message.message_id;
  state.last_mailbox_dequeue_sequence = message.message_id;
  state.last_mailbox_depth = static_cast<int>(mailbox.size());
  state.last_mailbox_drained_value = value;
  RecordRuntimeActorSuccess(state);
  return value;
}

int CancelRuntimeActorMailbox(RuntimeActorState &state,
                              int actor_handle,
                              int executor_tag) {
  ++state.mailbox_cancel_call_count;
  state.last_mailbox_actor_handle = actor_handle;
  state.last_mailbox_executor_tag = executor_tag;
  state.last_mailbox_cancelled_count = 0;
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
  std::deque<RuntimeActorMailboxMessage> &mailbox =
      state.mailboxes[actor_handle];
  const std::uint64_t cancelled_count =
      static_cast<std::uint64_t>(mailbox.size());
  if (!mailbox.empty()) {
    state.last_mailbox_completion_sequence = mailbox.back().message_id;
    state.actor_last_completed_sequence[actor_handle] =
        state.last_mailbox_completion_sequence;
  }
  mailbox.clear();
  state.last_mailbox_cancelled_count = cancelled_count;
  state.last_mailbox_depth = 0;
  state.mailbox_ordering_guard_passed = 1;
  RecordRuntimeActorSuccess(state);
  return static_cast<int>(cancelled_count);
}

int RecordRuntimeActorMailboxError(RuntimeActorState &state,
                                   int actor_handle,
                                   int error_code,
                                   int executor_tag) {
  ++state.mailbox_error_call_count;
  state.last_mailbox_actor_handle = actor_handle;
  state.last_mailbox_executor_tag = executor_tag;
  state.last_mailbox_error_code = error_code;
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
  state.last_mailbox_depth = CurrentMailboxDepth(state, actor_handle);
  RecordRuntimeActorFailure(state,
                            OBJC3_RUNTIME_ACTOR_FAILURE_ACTOR_METHOD_ERROR);
  return error_code;
}

int ShutdownRuntimeActorMailbox(RuntimeActorState &state,
                                int actor_handle,
                                int executor_tag) {
  ++state.mailbox_shutdown_call_count;
  state.last_mailbox_actor_handle = actor_handle;
  state.last_mailbox_executor_tag = executor_tag;
  state.last_mailbox_shutdown_pending_count = 0;
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
  std::deque<RuntimeActorMailboxMessage> &mailbox =
      state.mailboxes[actor_handle];
  const std::uint64_t pending_count =
      static_cast<std::uint64_t>(mailbox.size());
  if (!mailbox.empty()) {
    state.last_mailbox_completion_sequence = mailbox.back().message_id;
    state.actor_last_completed_sequence[actor_handle] =
        state.last_mailbox_completion_sequence;
  }
  mailbox.clear();
  state.actor_mailbox_shutdowns[actor_handle] = 1;
  state.last_mailbox_shutdown_pending_count = pending_count;
  state.last_mailbox_depth = 0;
  state.mailbox_shutdown_guard_passed = 1;
  RecordRuntimeActorSuccess(state);
  return static_cast<int>(pending_count);
}

}  // namespace objc3c::runtime
