#include "support/objc3_concurrency_symbol_profiles.h"

#include "support/objc3_profile_token_match.h"
#include "support/objc3_string_predicates.h"

namespace objc3c::support {

std::string BuildConcurrencyLowercaseProfileToken(std::string_view token) {
  return LowercaseAscii(token);
}

bool IsConcurrencyTaskCreationSymbol(std::string_view symbol) {
  return !symbol.empty() &&
         LowercaseProfileContainsAnyToken(
             symbol,
             {"task_spawn", "spawn_task", "detached_task", "task_detach"});
}

bool IsConcurrencyDetachedTaskCreationSymbol(std::string_view symbol) {
  return !symbol.empty() &&
         LowercaseProfileContainsAnyToken(symbol,
                                          {"detached_task", "task_detach"});
}

bool IsConcurrencyTaskGroupScopeSymbol(std::string_view symbol) {
  return !symbol.empty() &&
         LowercaseProfileContainsAnyToken(symbol,
                                          {"with_task_group",
                                           "task_group_scope"});
}

bool IsConcurrencyTaskGroupAddTaskSymbol(std::string_view symbol) {
  return !symbol.empty() &&
         LowercaseProfileContainsAnyToken(symbol,
                                          {"task_group_add_task",
                                           "group_add_task"});
}

bool IsConcurrencyTaskGroupWaitNextSymbol(std::string_view symbol) {
  return !symbol.empty() &&
         LowercaseProfileContainsAnyToken(symbol,
                                          {"task_group_wait_next",
                                           "group_wait_next", "wait_next"});
}

bool IsConcurrencyTaskGroupCancelAllSymbol(std::string_view symbol) {
  return !symbol.empty() &&
         LowercaseProfileContainsAnyToken(
             symbol,
             {"task_group_cancel_all", "group_cancel_all", "cancel_all"});
}

bool IsConcurrencyCancellationCheckSymbol(std::string_view symbol) {
  return !symbol.empty() &&
         LowercaseProfileContainsAnyToken(
             symbol, {"cancelled", "is_cancelled", "cancellation"});
}

bool IsConcurrencyCancellationHandlerSymbol(std::string_view symbol) {
  return !symbol.empty() &&
         LowercaseProfileContainsAnyToken(
             symbol,
             {"on_cancel", "cancel_handler",
              "with_cancellation_handler"});
}

}  // namespace objc3c::support
