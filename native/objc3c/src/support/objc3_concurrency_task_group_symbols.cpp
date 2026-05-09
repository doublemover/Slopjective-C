#include "support/objc3_concurrency_task_group_symbols.h"

#include "support/objc3_profile_token_match.h"

namespace objc3c::support {

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

}  // namespace objc3c::support
